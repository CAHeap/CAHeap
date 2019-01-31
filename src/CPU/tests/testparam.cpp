/*
 * test how d affect the performance
 */

#include <stdio.h>
#include <stdlib.h>
#include "../AHeap/AHeap.h"
#include "../common/IO.h"
#include "../common/metric.h"
#include "../tasks/AHeap_CountingTasks.h"

#define MAX_FLOW_NUM 16000

/* 
 * 1. range d1 from 1 to largest size
 *    d2 is fixed to 4
 * 2. range d2 from 1 to largest size
 *    d1 is fixed to 4
 * 
 * parameters:
 *	  M: size of 1st layer
 *    tuples: input file containing packets
 *    dstFile: output file to store the performance
 */
void test(int M, vector<string>& tuples, int N = MAX_FLOW_NUM){
	int m;
	int d1 = 2, d2 = 2;
	// for(int d1 = 1; d1 <= 4; ++d1){
	// 	for(int d2 = 1; d2 <= 10; ++d2){
			m = M / (pow(2, d1 + d2) - 1);
			// m = 10660;
			printf("d1=%d, d2=%d, m=%d\n", d1, d2, m);


			AHeap* aheap = new AHeap(m, 21, -1);
			aheap->set_keylen(13);
			FlowsizePerform fsp = accTest(aheap, tuples, N);
			delete aheap;
			printf("accuracy: %lf, AAE: %lf, ARE: %lf, load_factor: %lf\n\n",
				fsp.accuracy, fsp.aae, fsp.are, fsp.load_factor);

			AHeap* aheap = new AHeap(d1, d2, m);
			fsp = accTest(aheap, tuples, N);
			delete aheap;
			printf("accuracy: %lf, AAE: %lf, ARE: %lf, load_factor: %lf\n\n",
				fsp.accuracy, fsp.aae, fsp.are, fsp.load_factor);

	// 	}
	// }
}

/*
 * ./testparam srcFile M dstFile
 * M is the size of first layer
 */
int main(int argc, const char* argv[]){
	if(argc != 4){
		printf("\t./testparam srcFile M N\n\t\"srcFile\": file containing 5-tuples\n\t\"M\": maximum buckets number\n\t\"N\": maximum number of distinct flows\n");
		exit(1);
	}

	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);

	int M = atoi(argv[2]);
	int N = atoi(argv[3]);
	test(M, tuples, N);
}

