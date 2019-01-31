/*
 * entropy test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AHeap/AHeap.h"
#include "../common/IO.h"
#include "../common/metric.h"
#include "../FlowRadar/insertable_iblt.h"
#include "../tasks/FlowRadar_CountingTasks.h"
#include "../tasks/AHeap_CountingTasks.h"

#define MAX_FLOW_NUM 16000
	

/* 
 * parameters:
 *	  M: total number of buckets
 *    tuples: input file containing packets
 *    dstFile: output file to store the performance
 */
void test(int M, vector<string>& tuples, FILE* fout, int N = MAX_FLOW_NUM){
	int total_mem_in_bytes = M * (KEY_LEN + 4);
	int packet_cnt;
	EntropyPerform ep;

  /* for AHeap test */
	int d1 = 2, d2 = 2, m = M / (pow(2, d1 + d2) - 1);
	AHeap* aheap = new AHeap(m, KEY_LEN + 4, -1);
	aheap->set_keylen(13);
	ep = entropyTest(aheap, tuples, N, &packet_cnt);
	delete aheap;
	printf("AHeap\t%d\t%d\t%d\t%lf\n",
		total_mem_in_bytes, N, packet_cnt, ep.re);
	fprintf(fout, "AHeap\tAHeap%d\t%d\t%d\t%lf\n",
		total_mem_in_bytes, N, packet_cnt, ep.re);

  /* for FlowRadar test */
	InsertableIBLT *iblt = new InsertableIBLT(total_mem_in_bytes);
	ep = entropyTest(iblt, tuples, N);
	delete iblt;
	printf("FlowRadar\t%d\t%d\t%d\t%lf\n",
		total_mem_in_bytes, N, packet_cnt, ep.re);
	fprintf(fout, "FlowRadar\tFlowRadar%d\t%d\t%d\t%lf\n",
		total_mem_in_bytes, N, packet_cnt, ep.re);

		/* for FlowRadar test */
	InsertableIBLT *iblt2 = new InsertableIBLT(total_mem_in_bytes);
	ep = entropyTest2(iblt2, tuples, N);
	delete iblt;
	printf("NFR\t%d\t%d\t%d\t%lf\n",
		total_mem_in_bytes, N, packet_cnt, ep.re);
	fprintf(fout, "NFR\tNFR%d\t%d\t%d\t%lf\n",
		total_mem_in_bytes, N, packet_cnt, ep.re);

  /* for AHeap with encode test */
	AHeap* ah[3];
	ah[0] = new AHeap(m*2, KEY_LEN + 4, 0);
	ah[1] = new AHeap(m*2, KEY_LEN + 4, 3);
	ah[2] = new AHeap(m*2, KEY_LEN + 4, 4);


	ah[0]->set_keylen(5);	
	ah[1]->set_keylen(5);	
	ah[2]->set_keylen(5);
	ep = entropyTest(ah, tuples, N, 3);
	delete ah[0];	delete ah[1];	delete ah[2];
	printf("AHE\t%d\t%d\t%d\t%lf\n",
		total_mem_in_bytes, N, packet_cnt, ep.re);
	fprintf(fout, "AHE\tAHeap%d\t%d\t%d\t%lf\n",
		total_mem_in_bytes, N, packet_cnt, ep.re);
}

/*
 * ./testparam srcFile N Mb Mu step
 * srcFile: 5-tuple file
 * N: maximum number of distinct flows
 * M1: bucket number lower bound
 * M2: bucket number upper bound
 * step: bucket number step size
 * dstFile: for storing performance
 */
int main(int argc, const char* argv[]){
	if(argc != 8){
		printf("./testflowentropy srcFile N Mb Mu step dstFile mode\n\tsrcFile: 5-tuple file\n\tN: maximum number of distinct flows\n\tM1: bucket number lower bound\n\tM2: bucket number upper bound\n\tstep: bucket number step size\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' continue test\n");
		exit(1);
	}

	int N = atoi(argv[2]);
	int Mb = atoi(argv[3]);
	int Mu = atoi(argv[4]);
	int step = atoi(argv[5]);


	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);

	FILE* fout;
	if(strcmp(argv[7], "w") == 0){
		fout = fopen(argv[6], "w");		// Weighted Mean Relative Error
		fprintf(fout, "algo\tmem\tflownum\tpacketnum\tre\n");
	}
	else if(strcmp(argv[7], "a") == 0){
		fout = fopen(argv[6], "a");
	}
	else{
		printf("./testflowsizeentropy srcFile N Mb Mu step dstFile mode\n\tsrcFile: 5-tuple file\n\tN: maximum number of distinct flows\n\tM1: bucket number lower bound\n\tM2: bucket number upper bound\n\tstep: bucket number step size\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' continue test\n");
		exit(1);
	}

	for(int M = Mb; M <= Mu; M += step)
		test(M, tuples, fout, N);
	fclose(fout);
}

