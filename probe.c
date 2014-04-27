#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
//#include <ammintrin.h>

int main(int argc, char *argv[]){

	FILE *file = fopen(argv[1], "r");
	if (file == 0){
		fprintf(stderr, "Could not open dump file.\n");
		exit(1);
	}

	//close file
	fclose(file);
}
