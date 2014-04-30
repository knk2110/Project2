#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "probe.h"
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <math.h>


int main(int argc, char *argv[]){

	//read in the dump file
	FILE *dumpFile = fopen(argv[1], "r");
	if (dumpFile == 0){
		fprintf(stderr, "Could not open dump file.\n");
		exit(1);
	}

	int numVals = 0;
	int B = 0;
	int S = 0;
	int h = 0;
	int N = 0;
	long n = 0;
	int numRows = 0;
	int numCols = 0;
	int tableSize = 0; 
	int hashMults[2];
	int **bucketKeys;
	int **bucketPayloads;
	int bucketNumber = 0;
	int slotNumber = 0;
	int numBitsToShift = 0;
	int shiftNum = 0;

	while ( fscanf(dumpFile, "%lu", &n) > 0){

		if (numVals == 0){
			B = n;	
			numVals++;
		}
		else if (numVals == 1){
			S = (int)n;
			tableSize = power(2,S);
			numRows = tableSize/B;
			numBitsToShift = (int)log2(numRows);
			shiftNum = power(2,(32-numBitsToShift));
			numCols = B;
			bucketKeys = malloc(numRows * sizeof(int *));
			bucketPayloads = malloc(numRows * sizeof(int *));
			if (bucketKeys == NULL || bucketPayloads == NULL){
				fprintf(stderr, "Malloc failed.\n");
				exit(1);
			}

			int i = 0; 
			for (i = 0; i < numRows; i++){
				bucketKeys[i] = malloc(numCols * sizeof(int));
				bucketPayloads[i] = malloc(numCols * sizeof(int));
				if (bucketKeys[i] == 0 || bucketPayloads[i] == 0){
					fprintf(stderr, "Malloc failed.\n");
					exit(1);
				}

			}
			numVals++;
		}
		else if (numVals == 2){
			h = (int)n;	//we know this should be 2
			numVals++;	
		}
		else if (numVals == 3){
			N = (int)n;
			numVals++;
		}
		else if (numVals <= 5){
			hashMults[numVals-4] = (int)n;
			numVals++;
		}
		else if (numVals > 5){ //key or value, insert into table
			if (numVals % 2 == 0) {//if even, this is a key
				bucketKeys[bucketNumber][slotNumber] = (int)n;			
			}
			else { //this is a payload
				bucketPayloads[bucketNumber][slotNumber] = (int)n;
				slotNumber = slotNumber + 1;
				if (slotNumber == B){
					slotNumber = 0;
					bucketNumber = bucketNumber + 1;
				}
			}
			numVals++;
		}	
	}

	//now, read in list of probe values
	FILE *probeFile = fopen(argv[2], "r");
	if (probeFile == 0){
		fprintf(stderr, "Could not open probe file.\n");
		exit(1);
	}

	n = 0;
	numVals = 0;
	while (fscanf(probeFile, "%lu", &n) > 0){
		numVals++;
	}
	int *probeKeys = malloc(numVals*sizeof(int));
	n = 0;
	rewind(probeFile);
	int i = 0;
	while (fscanf(probeFile, "%lu", &n) > 0){
		probeKeys[i] = (int)n;
		i++;
	}

	//close files
	fclose(dumpFile);
	fclose(probeFile);

	//call to probe goes here
	FILE *results = fopen(argv[3], "w");
	if(results == NULL)
	{
		printf("Error opening results file.\n");
		exit(1);
	}

	int j = 0;
	for (j = 0; j < numVals; j++){
	
		//probe
		int payLoad = probe(bucketKeys, bucketPayloads, hashMults, tableSize, probeKeys[j], shiftNum);
		if(payLoad != 0){
			/* print key and payload to file */
			fprintf(results, "%d %d\n", probeKeys[j], payLoad);

		}
	}

	fclose(results);

	// got two possible buckets, now going to perform logic - check to see if inside of buckets


	//before program ends, free bucketKeys and bucketPayloads
	for (i = 0; i < numRows; i++){
		free(bucketKeys[i]);
		free(bucketPayloads[i]);
	}
	free(bucketKeys);
	free(bucketPayloads);
	free(probeKeys);

	return 0;
}


int probe(int **bucketKeys, int **bucketPayloads, int hashMults[], int tableSize, int searchKey, int shiftNum){
	
	//store search key and hash mults
	__m128i sk = _mm_set_epi32(searchKey, searchKey, searchKey, searchKey);
	__m128i hm = _mm_set_epi32(0,hashMults[1], 0, hashMults[0]);
	
	//multiply search key and hash mults
	__m128i sh = _mm_set_epi32(shiftNum, shiftNum, shiftNum, shiftNum);
	__m128i hv = _mm_mullo_epi32(sk,hm);
	
	//get slot
	int32_t *val3 = (int32_t*)&hv;
	uint32_t *shv = (uint32_t*)&sh;
	int slot1 = val3[0]/shv[0];
	int slot2 = val3[2]/shv[2];
	
	//load in payloads and keys for each slot
	__m128i slot1keys = _mm_set_epi32(bucketKeys[slot1][3], bucketKeys[slot1][2], bucketKeys[slot1][1], bucketKeys[slot1][0]);
	__m128i slot1payloads = _mm_set_epi32(bucketPayloads[slot1][3], bucketPayloads[slot1][2], bucketPayloads[slot1][1], bucketPayloads[slot1][0]);
	__m128i slot2keys = _mm_set_epi32(bucketKeys[slot2][3], bucketKeys[slot2][2], bucketKeys[slot2][1], bucketKeys[slot2][0]);
	__m128i slot2payloads = _mm_set_epi32(bucketPayloads[slot2][3], bucketPayloads[slot2][2], bucketPayloads[slot2][1], bucketPayloads[slot2][0]);
	
	//cmp-eq
	__m128i cmpSlot1 = _mm_cmpeq_epi32(slot1keys,sk);
	__m128i cmpSlot2 = _mm_cmpeq_epi32(slot2keys, sk);
	
	//and of payloads and cmp-eqs
	__m128i andS1 = _mm_and_si128(cmpSlot1, slot1payloads);
	__m128i andS2 = _mm_and_si128(cmpSlot2, slot2payloads);
	
	//or
	__m128i orAnds = _mm_or_si128(andS1, andS2);
	
	//or-across
	orAnds = _mm_max_epi32(orAnds, _mm_shuffle_epi32(orAnds, _MM_SHUFFLE(2,1,0,3)));
	orAnds = _mm_max_epi32(orAnds, _mm_shuffle_epi32(orAnds, _MM_SHUFFLE(1,0,3,2)));
	uint32_t *oA = (uint32_t*)&orAnds;

	int val = oA[0];

	return val;
	
	
}


long power(int base, int exp){
	int i = 0;
	long result = 1;
	for (i = 0; i < exp; i++){
		result *= base;
	}
	//printf("power = %d\n", result);
	return result;
}

