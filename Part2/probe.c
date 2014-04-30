#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "probe.h"
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
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
	int n = 0;
	int numRows = 0;
	int numCols = 0;
//	int numProbeVals = 0;
	int tableSize = 0; 
	int hashMults[2];
	int **bucketKeys;
	int **bucketPayloads;
	int bucketNumber = 0;
	int slotNumber = 0;

	while ( fscanf(dumpFile, "%d", &n) > 0){

		if (numVals == 0){
			B = n;	
			numVals++;
		}
		else if (numVals == 1){
			S = n;
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
			h = n;	//we know this should be 2
			numVals++;	
		}
		else if (numVals == 3){
			N = n;
			numVals++;
		}
		else if (numVals <= 5){
			hashMults[numVals-4] = n;
			numVals++;
		}
		else if (numVals > 5){ //key or value, insert into table
			//printf("numVals > 5, n = %d\n", n);
			if (numVals % 2 == 0) {//if even, this is a key
//				printf("numVals is even (%d),%d is a key for bucket %d, slot %d\n", numVals, n, bucketNumber, slotNumber);
				bucketKeys[bucketNumber][slotNumber] = n;			
			}
			else { //this is a payload
//				printf("numVals is odd (%d), %d is a value for bucket %d, slot %d\n", numVals, n, bucketNumber, slotNumber);
				bucketPayloads[bucketNumber][slotNumber] = n;
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
	while (fscanf(probeFile, "%d", &n) > 0){
		numVals++;
	}
	int *probeKeys = malloc(numVals*sizeof(int));
	n = 0;
	rewind(probeFile);
	int i = 0;
	while (fscanf(probeFile, "%d", &n) > 0){
		//printf("n = %d\n", n);
		probeKeys[i] = n;
		i++;
	}

	//print new table
	for (i = 0; i < numRows; i++){
		int j = 0;
		for (j = 0; j < numCols; j++){
//			printf("Bucket %d, Slot %d: (%d, %d)\n", i, j, bucketKeys[i][j], bucketPayloads[i][j]);
			}
	}

	//print list of proble values
	for (i = 0; i < numVals; i++){
		printf("%d ", probeKeys[i]);
	}

	printf("\n");

	//close files
	fclose(dumpFile);
	fclose(probeFile);

	for (i = 0; i < numVals; i++){
//		int result = probe(bucketKeys, bucketPayloads, hashMults, tableSize, probeKeys[i]);
		//printf("Payload for key %d: %d", probeKeys[i], result);
	}
	
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


int probe(int **bucketKeys, int **bucketPayloads, int hashMults[], int tableSize, int searchKey){

	// make 4 copies of search key into copiesOfSearchKey	
	// const float sK = searchKey;
	// __m128 copiesOfSearchKey = _mm_load1_ps(&sK);
	// __m128i copiesOfSearchKeyInt = _mm_cvtps_epi32(copiesOfSearchKey);

	// int result [4];
	// _mm_store_ps (result, copiesOfSearchKeyInt);
	// __m128i searchKeyI = _mm_cvtsi32_si128(searchKey);
	// __m128i searchKeyI = _mm_load_si128(&searchKey[0]);
	// __m128i searchKeyI = _mm_set_epi32 (searchKey, searchKey, searchKey, searchKey);
	// printf("I am in here %i", (int*) &copiesOfSearchKeyInt[0]);
	// __m128i copiesOfSearchKeyInt = _mm_cvtps_epi32(searchKeyI); 
	// __m128i copiesOfSearchKeyInt = _mm_load_si128(&searchKeyI);
	uint32_t *aPtr = &searchKey;
	const __m128i a = _mm_storeu_si128((const __m128i*) aPtr, (const __m128i*) aPtr);

	// printf("Search key %i, right now!!! ", searchKey);
	// printf("I am in here %i, right now!!! ", _mm_cvtsi128_si32(copiesOfSearchKeyInt));

	// take hash multiplier array and convert to __128i
	// float hMs[4] = {hashMults[0], 0, hashMults[1], 0};
	// __m128 hMults = _mm_load_ps(&hMs[0]);
	// __m128i hMultsI = _mm_cvtps_epi32(hMults);
	// printf("I am in here %i, right now!!! ", _mm_cvtsi128_si32(copiesOfSearchKeyInt));

	//multiply by 2^S/B

	//make vector containing hash multipliers
	// int32_t hMs[4] = {hashMults[0], 0, hashMults[1], 0};
	// __m128i hMults = _mm_load_si128(&hMs[0]);	
	// float hMs[4] = {hashMults[0], 0, hashMults[1], 0};
	// __m128 hMults = _mm_load_ps(&hMs[0]);


	//multiply hash multipliers times copies of search key
	// __m128 hashValues = _mm_mul_ps(copiesOfSearchKey, hMults);

	//to get slots, multiply hash values by table size
	// float tableSizes[4] = {tableSize, 0, tableSize, 0};
	// __m128 tS = _mm_load_ps(&tableSizes[0]);
	// __m128 slots = _mm_mul_ss(hashValues, tS);

	// float result [4];
	// _mm_store_ps (result, slots);
	// printf("I am in here %f", result[3]);



	//HERE IS THE PROBLEM: slots should contain possible buckets, but instead it contains huge numbers.

	/*

	//for each slot, create mask between copiesOfSearchKey and the keys for that slot
	float slot1Keys[4] = {bucketKeys[slot1][0], bucketKeys[slot1][1], bucketKeys[slot1][2], bucketKeys[slot1][3]};
	__m128 s1Keys = _mm_load_ps(&slot1Keys[0]);
	__m128 s1Mask = _mm_cmpeq_ps(copiesOfSearchKey, s1Keys);
	float slot2Keys[4] = {bucketKeys[slot2][0], bucketKeys[slot2][0], bucketKeys[slot2][2], bucketKeys[slot2][3]};
	__m128 s2Keys = _mm_load_ps(&slot2Keys[0]);
	__m128 s2Mask = _mm_cmpeq_ps(copiesOfSearchKey, s2Keys);
	
	//now, perform AND with masks and payloads
	float slot1Payloads[4] = {bucketPayloads[slot1][0], bucketPayloads[slot1][1], bucketPayloads[slot1][2], bucketPayloads[slot2][3]};
	__m128 s1Payloads = _mm_load_ps(&slot1Payloads[0]);
	__m128 s1AND = _mm_and_ps(s1Payloads, s1Mask);
	float slot2Payloads[4] = {bucketPayloads[slot2][0], bucketPayloads[slot2][1], bucketPayloads[slot2][2], bucketPayloads[slot2][3]};
	__m128 s2Payloads = _mm_load_ps(&slot2Payloads[0]);
	__m128 s2AND = _mm_and_ps(s2Payloads, s2Mask);

	//perform OR of the two ANDs
	
	__m128 slotsOR = _mm_or_ps(s1AND, s2AND);
	int j = 0;
	for (j = 0; j < 4; j++){
		printf("slotsOR[%d]: %f", j, slotsOR[j]);
	}

	//perform OR-ACROSS

	*/
	//if not foumd, return 0
	return 0;
	
}


int power(int base, int exp){
	int i = 0;
	int result = 1;
	for (i = 0; i < exp; i++){
		result *= base;
	}
	//printf("power = %d\n", result);
	return result;
}
