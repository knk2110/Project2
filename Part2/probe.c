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
		int possibleBucket1 = possibleBucket(hashMults[0], probeKeys[j], numRows);
		int possibleBucket2 = possibleBucket(hashMults[1], probeKeys[j], numRows);
		printf("possible buckets for key %d: for h[0]: %d, for h[1]: %d\n", probeKeys[i], possibleBucket1, possibleBucket2);
		//call probe here
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


int probe(int **bucketKeys, int **bucketPayloads, int hashMults[], int tableSize, int searchKey, int B1, int S1){

	 __m128i s;//= _mm_setzero_si128();
	// printf("test with zeros: %lld %lld %lld %lld\n", s[0], s[1], s[2], s[3]);
	//test = _mm_set_epi32(searchKey, searchKey, searchKey, searchKey);
	// s[0] = (int32_t)searchKey;
	// s[1] = (int32_t)searchKey;
	// s[2] = (int32_t)searchKey;
	// s[3] = (int32_t)searchKey;


	__m128i hms = _mm_setzero_si128();
	long andOp = (long) pow(2, 32)-1;
	long h = hashMults[0] & andOp;
	h = hashMults[1] & andOp;
	

	long hk = h*((long)searchKey) % (long)pow(2,32);

	// hms[0] = (int32_t)hashMults[0];
	// hms[1] = (int32_t)0;
	// hms[2] = (int32_t)hashMults[1];
	// hms[3] = (int32_t)0;


	__m128i hvs = _mm_mullo_epi32(s, hms);


	// hms[0] = (int32_t)hashMults[1];
	__m128i hvs2 = _mm_mullo_epi32(s,hms);

	//bit-shift to get table slot
	int shift = (int)log2(power(2,S1)/B1);

	// unsigned int h1 = hvs[0];

	// int shift1 = numDigitsInBinary(hvs[0])-(shift-(32-numDigitsInBinary(hvs[0]))); 		
	// int slot1 = (int32_t)(hvs[0]) >> shift1;
	// int slot2 = (int32_t)hvs2[0] >> (32-shift);//__m128i slot2 = _mm_srli_si128(hvs2, numBuckets);
	//printf("slot2: %lld %lld %lld %lld\n", slot2[0], slot2[1], slot2[2], slot2[3]);


	/*
	__m128i h[4];
	h[0] = _mm_cvtsi32_si128(searchKey); 
	h[1] = _mm_cvtsi32_si128(searchKey); 
	h[2] = _mm_cvtsi32_si128(searchKey); 
	h[3] = _mm_cvtsi32_si128(searchKey); 
	__m128i *p = &h[0];
	const __m128i t = _mm_loadu_si128(p);
	_mm_storeu_si128(p, t);

	printf("t: %lld %lld %lld %lld\n", t[0], t[1], t[2], t[3]);	
	
	__m128i res = _mm_mullo_epi32(t,t);
	printf("res: %lld %lld %lld %lld\n", res[0], res[1], res[2], res[3])*/
	//for now, return 0
	return 0;
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
