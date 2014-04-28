#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>

int main(){

	float **keys;
	int numRows = 4;
	int numCols = 4;
	keys = malloc(numRows * sizeof(float *));
	int i = 0;
	for (i = 0; i < numRows; i++){
		keys[i] = malloc(numCols * sizeof(float));
	}
	int j = 0;
	for (i = 0; i < numRows; i++){
		for (j = 0; j < numCols; j++){
			keys[i][j] = i*j;
		}
	}
	
	const float KEY0 = keys[0][0];
	const float KEY1 = keys[0][1];
	const float KEY2 = keys[0][2];
	const float KEY3 = keys[0][3];
	const float subkeys[4] = {KEY0, KEY1, KEY2, KEY3};
	__m128 X = _mm_load_ps(&subkeys[0]);

	for (i = 0; i < numRows; i++){
		free(keys[i]);
	}
	free(keys);
}
