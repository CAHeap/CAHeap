/*
 * flow size test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AHeap/AHeap.h"
#include "../common/IO.h"
#include "../common/metric.h"
#include "../FlowRadar/insertable_iblt.h"
#include "../tasks/FlowRadar_CountingTasks.h"
#include "../tasks/FlowRadar_KVTasks.h"
#include "../tasks/AHeap_CountingTasks.h"
#include "../tasks/AHeap_KVTasks.h"

#define PACKET_PER_W 60000
	

/* 
 * parameters:
 *	  M: size of 1st layer
 *    tuples: input file containing packets
 *    dstFile: output file to store the performance
 */
void test(int M, vector<string>& tuples, FILE* fout, int total_w, int N = PACKET_PER_W, int thre = 1){
	int total_mem_in_bytes = M * (KEY_LEN + 8);
	FlowletPerform fp;

  /* for AHeap test */
	int d1 = 2, d2 = 2, m = M / (pow(2, d1 + d2) - 1);
	AHeap* aheap = new AHeap(m, KEY_LEN + 8, -1);
	aheap->set_keylen(13);
	fp = flowletTest(aheap, tuples, N, total_w, thre);
	delete aheap;
	
	printf("AHeap\t\t%d\t%d\t%d\t%lf\t%lf\t%lf\n", 
		total_mem_in_bytes, total_w, N, fp.precision, fp.recall, fp.f1);
	fprintf(fout, "AHeap\tAHeap%d\t%d\t%d\t%lf\t%lf\t%lf\n", 
		total_mem_in_bytes, total_w, N, fp.precision, fp.recall, fp.f1);

  /* for FlowRadar test */
	InsertableIBLT *iblt = new InsertableIBLT(total_mem_in_bytes);
	fp = flowletTest(iblt, tuples, N, total_w, thre);
	delete iblt;

	printf("FlowRadar\t%d\t%d\t%d\t%lf\t%lf\t%lf\n", 
		total_mem_in_bytes, total_w, N, fp.precision, fp.recall, fp.f1);
	fprintf(fout, "FlowRadar\tFlowRadar%d\t%d\t%d\t%lf\t%lf\t%lf\n", 
		total_mem_in_bytes, total_w, N, fp.precision, fp.recall, fp.f1);

  /* for AHeap test */
	AHeap* ah[3];
	ah[0] = new AHeap(m*2, KEY_LEN + 4, 3);
	ah[1] = new AHeap(m*2, KEY_LEN + 4, 4);
	ah[2] = new AHeap(m*2, KEY_LEN + 4, 0);


	ah[0]->set_keylen(5);	 
	ah[1]->set_keylen(5);	
	ah[2]->set_keylen(5);


	fp = flowletTest(ah, tuples, N, total_w, 3, thre);
	delete ah[0];	delete ah[1];	delete ah[2];

	printf("AHE\t\t%d\t%d\t%d\t%lf\t%lf\t%lf\n", 
		total_mem_in_bytes, total_w, N, fp.precision, fp.recall, fp.f1);
	fprintf(fout, "AHE\tAHE%d\t%d\t%d\t%lf\t%lf\t%lf\n", 
		total_mem_in_bytes, total_w, N, fp.precision, fp.recall, fp.f1);
}

/*
 * ./testparam srcFile N Mb Mu step
 * srcFile: 5-tuple file
 * N: number of packet per window
 * dstFile: for storing performance
 */
int main(int argc, const char* argv[]){
	if(argc != 8){
		printf("./testflowlet srcFile N M total_w thre dstFile mode\n\tsrcFile: 5-tuple file\n\tN: number of packet per window\n\tM: number of buckets for AHeap\n\ttotal_w: total number of time windows\n\tthre: number of time windows to split two flowlets\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' for continue test\n");
		exit(1);
	}

	int N = atoi(argv[2]);
	int M = atoi(argv[3]);
	int total_w = atoi(argv[4]);
	int thre = atoi(argv[5]);


	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);

	FILE* fout;
	if(strcmp(argv[7], "w") == 0){
		fout = fopen(argv[6], "w");
		fprintf(fout, "algo\tmem\ttot_w\tp_per_w\tprec\trecall\tf1-score\n");
		printf("algo\tmem\ttot_w\tp_per_w\tprec\trecall\tf1-score\n");
	}
	else if(strcmp(argv[7], "a") == 0){
		fout = fopen(argv[6], "a");
	}
	else{
		printf("./testflowlet srcFile N M total_w thre dstFile mode\n\tsrcFile: 5-tuple file\n\tN: number of packet per window\n\tM: number of buckets for AHeap\n\ttotal_w: total number of time windows\n\tthre: number of time windows to split two flowlets\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' for continue test\n");
		exit(1);
	}

	test(M, tuples, fout, total_w, N, thre);
	fclose(fout);
}

