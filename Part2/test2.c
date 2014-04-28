#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>

int main(){

	float *hashMults = malloc(4*sizeof(float));
	hashMults[0] = 10;
	hashMults[1] = 0;
	hashMults[2] = 10;
	hashMults[3] - 0;

	__m128 X, Y, Z;
	const float key = 123;
	X = _mm_load1_ps(&key);
	Y = _mm_load_ps(&hashMults[0]);
	Z = _mm_mul_ps(X, Y);

	float prod;
	int i = 0;
	for (i =0; i < 4; i++){
		prod += Z[i];	
	}

	printf("prod = %f", prod);
}	
