#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
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

	//for (i = 0; i < numVals; i++){
		int result = probe(bucketKeys, bucketPayloads, hashMults, tableSize, probeKeys[0], B, S);
		//printf("Payload for key %d: %d", probeKeys[i], result);
	//}
	
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


int probe(int **bucketKeys, int **bucketPayloads, int hashMults[], int tableSize, int searchKey, int B1, int S1){

	//make four copies of search key
	//__m128i searchKeyII = _mm_cvtps_epi32(searchKey);
	//_m128i searchKeyI;
	//earchKeyI[0] = _mm_cvtps_epi32(searchKeyII);
	//__m128i searchKeyI = _mm_set_epi32(searchKey,searchKey,searchKey,searchKey);//, (int32_t)searchKey, (int32_t)searchKey, (int32_t)searchKey);
	//printf("searchKeyI: %lld %lld %lld %lld\n", searchKeyI[0], searchKeyI[4], searchKeyI[8], searchKeyI[12]);	
	//t i = 0;
	//or (i = 0; i < 128; i++){
	//	printf("sK[%d]:%lld\n", i, searchKeyI[i]);
	// printf("I am in here %i", (int*) &copiesOfSearchKeyInt[0]);
	 //__m128i copiesOfSearchKeyInt = _mm_cvtps_epi32(searchKeyI); 
	//__m128i copiesOfSearchKeyInt = _mm_load_si128(&searchKeyI);
//	__m128i copiesOfSearchKey = _mm_broadcast_epi32(&searchKeyI);
//	printf("Search key %i, right now!!!\n", searchKey);
//printf("I am in here %i, right now!!\n", _mm_cvtsi128_si32(copiesOfSearchKeyInt));
	//_m128i hMults = _mm_loadu_si128(&hashMults[0]);
//	printf("test for search key: %lld %lld %lld %lld\n", copiesOfSearchKeyInt[0], copiesOfSearchKeyInt[1], copiesOfSearchKeyInt[2], copiesOfSearchKeyInt[3]);

	__m128i sk = _mm_set_epi32(searchKey, searchKey, searchKey, searchKey);
	uint32_t *val = (uint32_t*)&sk;
	printf("searchkey array: %i %i %i %i\n", val[0], val[1], val[2], val[3]);
	__m128i hm = _mm_set_epi32(0,hashMults[1], 0, hashMults[0]);
	uint32_t *val2 = (uint32_t*)&hm;
	printf("hm array: %i %i %i %i\n", val2[0], val2[1], val2[2], val2[3]);

	int numBuckets = power(2,S1)/B1;
	int numBitsToShift = (int)log2(numBuckets);
	int shiftNum = power(2,(32-numBitsToShift));
	printf("bitstoshift %d ; shift num %d\n", numBitsToShift, shiftNum);
	__m128i sh = _mm_set_epi32(shiftNum, shiftNum, shiftNum, shiftNum);
	__m128i hv = _mm_mullo_epi32(sk,hm);
	int32_t *val3 = (int32_t*)&hv;
	printf("hv array: %i %i %i %i\n", val3[0], val3[1], val3[2], val3[3]);

	//get slots
	uint32_t *shv = (uint32_t*)&sh;
	printf("shift array: %i %i %i %i\n", shv[0], shv[1], shv[2], shv[3]);
	int slot1 = val3[0]/shv[0];
	int slot2 = val3[2]/shv[2];
	printf("slots: %d %d\n", slot1, slot2);
	__m128i slot1keys = _mm_set_epi32(bucketKeys[slot1][3], bucketKeys[slot1][2], bucketKeys[slot1][1], bucketKeys[slot1][0]);
	uint32_t *s1p = (uint32_t*)&slot1keys;
	printf("slot 1 keys: %i %i %i %i\n", s1p[0], s1p[1], s1p[2], s1p[3]);
	__m128i slot1payloads = _mm_set_epi32(bucketPayloads[slot1][3], bucketPayloads[slot1][2], bucketPayloads[slot1][1], bucketPayloads[slot1][0]);
	__m128i cmpSlot1 = _mm_cmpeq_epi32(slot1keys,sk);
	uint32_t *cs1 = (uint32_t*)&cmpSlot1;
	printf("compslot1: %i %i %i %i\n", cs1[0], cs1[1], cs1[2], cs1[3]);
	__m128i slot2keys = _mm_set_epi32(bucketKeys[slot2][3], bucketKeys[slot2][2], bucketKeys[slot2][1], bucketKeys[slot2][0]);
	__m128i slot2payloads = _mm_set_epi32(bucketPayloads[slot2][3], bucketPayloads[slot2][2], bucketPayloads[slot2][1], bucketPayloads[slot2][0]);
	__m128i cmpSlot2 = _mm_cmpeq_epi32(slot2keys, sk);
	uint32_t *cs2 = (uint32_t*)&cmpSlot2;
	printf("compslot2: %i %i %i %i\n", cs2[0], cs2[1], cs2[2], cs2[3]);
//	__m128 hvi = _mm_castsi128_ps(hv);
//	uint32_t *hvip = (uint32_t*)&hvi;

//	printf("hvi: %i %i %i %i\n", hvip[0], hvip[1], hvip[2], hvip[3]);
//	__m128 shi = _mm_castsi128_ps(sh);
//	uint32_t *ship = (uint32_t*)&shi;
//	printf("shi: %i %i %i %i\n", ship[0], ship[1], ship[2],ship[3]);
//	__m128 slots = _mm_div_ps(hvi,shi);
	//slots = _mm_udiv_epi32(hv,sh);
//	uint32_t *val4 = (int32_t*)&slots;
//	printf("slot array: %i %i %i %i\n", val4[0], val4[1], val4[2], val4[3]);
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

long power(int base, int exp){
	long i = 0;
	int result = 1;
	for (i = 0; i < exp; i++){
		result *= base;
	}
	//printf("power = %d\n", result);
	return result;
}
