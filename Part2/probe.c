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
//#include <ammintrin.h>

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

	while ( fscanf(dumpFile, "%lu", &n) > 0){

		if (numVals == 0){
			B = n;	
			numVals++;
		}
		else if (numVals == 1){
			S = (int)n;
			tableSize = power(2,S);
//			printf("tablesize: %d", tableSize);
			numRows = tableSize/B;
			numCols = B;
//			printf("numrows: %d, numcols: %d", numRows, numCols);
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

	printf("B = %d, S = %d, h = %d, N = %d, h[0] = %d, h[1] = %d\n", B, S, h, N, hashMults[0], hashMults[1]); 

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

	//print probe vals
	int j = 0;
	for (j = 0; j < numVals; j++){
		printf("%d ", probeKeys[j]);
	}
	printf("\n");


	//close files
	fclose(dumpFile);
	fclose(probeFile);

	//call to probe goes here
	for (j = 0; j < numVals; j++){
		//call probe here
		
		//call probe here
		int payLoad = probe(bucketKeys, bucketPayloads, hashMults, tableSize, probeKeys[j], B, S, numRows);
		printf("payload %d\n", payLoad);
		if(payLoad != 0){
			FILE *f = fopen(argv[3], "w");
			if (f == NULL)
			{
			    printf("Error opening file!\n");
			    exit(1);
			}

			/* print key and payload to file */
			fprintf(f, "%d %d\n", probeKeys[j], payLoad);

			fclose(f);
		}
	}

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


int probe(int **bucketKeys, int **bucketPayloads, int hashMults[], int tableSize, int searchKey, int B1, int S1, int numRows){
	int possibleBucket1 = possibleBucket(hashMults[0], searchKey, numRows);
	int possibleBucket2 = possibleBucket(hashMults[1], searchKey, numRows);
	printf("possible buckets for key %d: for h[0]: %d, for h[1]: %d\n", searchKey, possibleBucket1, possibleBucket2);

	// Got possible buckets now check each slot of buckets to see if matches key
	int payload = 0;
	int mask = (bucketKeys[possibleBucket1][0]==searchKey) ?  1 : 0;
	payload |= mask * bucketPayloads[possibleBucket1][0];
	mask = (bucketKeys[possibleBucket1][1]==searchKey) ?  1 : 0;
	payload |= mask * bucketPayloads[possibleBucket1][1];
	mask = (bucketKeys[possibleBucket1][2]==searchKey) ?  1 : 0;
	payload |= mask * bucketPayloads[possibleBucket1][2];
	mask = (bucketKeys[possibleBucket1][03]==searchKey) ?  1 : 0;
	payload |= mask * bucketPayloads[possibleBucket1][3];

	mask = (bucketKeys[possibleBucket2][0]==searchKey) ?  1 : 0;
	payload |= mask * bucketPayloads[possibleBucket2][0];
	mask = (bucketKeys[possibleBucket2][1]==searchKey) ?  1 : 0;
	payload |= mask * bucketPayloads[possibleBucket2][1];
	mask = (bucketKeys[possibleBucket2][2]==searchKey) ?  1 : 0;
	payload |= mask * bucketPayloads[possibleBucket2][2];
	mask = (bucketKeys[possibleBucket2][03]==searchKey) ?  1 : 0;
	payload |= mask * bucketPayloads[possibleBucket2][3];


	//   __m128i s;//= _mm_setzero_si128();
	// printf("test with zeros: %lld %lld %lld %lld\n", s[0], s[1], s[2], s[3]);
	// //test = _mm_set_epi32(searchKey, searchKey, searchKey, searchKey);
	// s[0] = (int32_t)searchKey;
	// s[1] = (int32_t)searchKey;
	// s[2] = (int32_t)searchKey;
	// s[3] = (int32_t)searchKey;
	// printf("test with search key: %lld %lld %lld %lld\n", s[0], s[1], s[2], s[3]);
	// __m128i hms = _mm_setzero_si128();
	// printf("hms with zeros: %lld %lld %lld %lld\n", hms[0], hms[1], hms[2], hms[3]);
	// hms[0] = (int32_t)hashMults[0];
	// hms[1] = (int32_t)0;
	// hms[2] = (int32_t)hashMults[1];
	// hms[3] = (int32_t)0;
	// printf("hms with hash multipliers: %lld %lld %lld %lld\n", hms[0], hms[1], hms[2], hms[3]);

	// __m128i hvs = _mm_mullo_epi32(s, hms);
	// printf("hvs: %lld %lld %lld %lld\n", hvs[0], hvs[1], hvs[2], hvs[3]);

	// hms[0] = (int32_t)hashMults[1];
	// __m128i hvs2 = _mm_mullo_epi32(s,hms);
	// printf("hvs2: %lld %lld %lld %lld\n", hvs2[0], hvs2[1], hvs2[2], hvs2[3]);
	// //bit-shift to get table slot
	// int shift = (int)log2(power(2,S1)/B1);
	// printf("bitshift: %d\n", shift);
	// unsigned int h1 = hvs[0];
	// printf("h1: %d\n", h1);
	// int shift1 = numDigitsInBinary(hvs[0])-(shift-(32-numDigitsInBinary(hvs[0]))); 		
	// int slot1 = (int32_t)(hvs[0]) >> shift1;
	// int slot2 = (int32_t)hvs2[0] >> (32-shift);//__m128i slot2 = _mm_srli_si128(hvs2, numBuckets);
	// printf("slots: %d %d\n", slot1, slot2);
	return payload;
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

int numDigitsInBinary(int num){
	int digits = 0;
//	printf("num at start: %d", num);
	for (digits = 0; num > 0; num >>=1){
//		printf("num: %d\n", num);
		digits++;
	}
//	printf("total digits: %d\n", digits);
	return digits;
}

int possibleBucket(int hashValue, int key, int numBuckets){
//	printf("in method...\n");
	long andOp = (long)(power(2,32)-1);
	long h = hashValue & andOp;
//	printf("%lu\n", h);
//	printf("key: %lu\n", (long)key);
//	printf("pow: %lu\n", (long)(power(2,32)));
	long hk = h*((long)key) %(long)power(2,32);
	int num = (int)log2(numBuckets);
	long numDigits = numDigitsInBinary(key);
	int pB = (int)(hk >> (numDigits-(num-(32-numDigits))));
	//int pB = 0;	
	return pB;
}

