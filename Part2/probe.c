#include <stdio.h>
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
			int numRows = tableSize/B;
			int numCols = B;
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
/*			int i1 = 0;
			int j = 0;
			for (i1 = 0; i1 < numRows; i1++){
				for (j = 0; j < numCols; j++){
					bucketKeys[i1][j] = 0;
				}
			}
*/
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

	printf("B = %d, S = %d, h = %d, N = %d, h[0] = %d, h[1] = %d", B, S, h, N, hashMults[0], hashMults[1]); 

	//now, read in list of probe values
	FILE *probeFile = fopen(argv[2], "r");
	if (probeFile == 0){
		fprintf(stderr, "Could not open probe file.\n");
		exit(1);
	}

	n = 0;


	//close files
	fclose(dumpFile);
	fclose(probeFile);

	//before program ends, free bucketKeys and bucketPayloads
	free(bucketKeys);
	free(bucketPayloads);

	return 0;
}

int power(int base, int exp){
	int i = 0;
	int result = 1;
	for (i = 0; i < exp; i++){
		result *= base;
	}
	printf("power = %d\n", result);
	return result;
}
