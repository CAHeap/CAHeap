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
void test(int M, vector<string>& tuples, FILE* fout, int total_w, int thre, int N = PACKET_PER_W){
	int total_mem_in_bytes = M * (KEY_LEN + 8);
	int packet_cnt = 0;
	PersistPerform pp;

  /* for AHeap test */
	int d1 = 2, d2 = 2, m = M / (pow(2, d1 + d2) - 1);
	AHeap* aheap = new AHeap(m, KEY_LEN + 8, -1);
	aheap->set_keylen(13);
	pp = persistentTest(aheap, tuples, N, total_w, thre);
	delete aheap;

	double f1 = NONAN(2 * pp.precision * pp.recall / (pp.precision + pp.recall));
	printf("AHeap\t\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%d\n", 
		total_mem_in_bytes, total_w, N, pp.precision, pp.recall, f1, thre);
	fprintf(fout, "AHeap\tAHeap%d\tAHeap%d\tAHeap%d\t%lf\t%lf\t%lf\t%d\n", 
		total_mem_in_bytes, total_w, N, NONAN(pp.precision), NONAN(pp.recall), f1, thre);


  /* for AHeap with encode test */
	AHeap* ah[3];
	ah[0] = new AHeap(m*2, KEY_LEN + 4, 3);
	ah[1] = new AHeap(m*2, KEY_LEN + 4, 4);
	ah[2] = new AHeap(m*2, KEY_LEN + 4, 0);


	ah[0]->set_keylen(5);	 
	ah[1]->set_keylen(5);	
	ah[2]->set_keylen(5);


	pp = persistentTest(ah, tuples, N, total_w, thre, 3);
	delete ah[0];	delete ah[1];	delete ah[2];

	f1 = NONAN(2 * pp.precision * pp.recall / (pp.precision + pp.recall));
	printf("AHE\t\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%d\n", 
		total_mem_in_bytes, total_w, N, pp.precision, pp.recall, f1, thre);
	fprintf(fout, "AHE\tAHE%d\tAHE%d\tAHE%d\t%lf\t%lf\t%lf\t%d\n", 
		total_mem_in_bytes, total_w, N, NONAN(pp.precision), NONAN(pp.recall), f1, thre);


  /* for FlowRadar test */
	// InsertableIBLT *iblt = new InsertableIBLT(total_mem_in_bytes);
	// pp = persistentTest(iblt, tuples, N, total_w, thre);
	// delete iblt;

	// f1 = NONAN(2 * pp.precision * pp.recall / (pp.precision + pp.recall));
	// printf("FlowRadar\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%d\n", 
	// 	total_mem_in_bytes, total_w, N, pp.precision, pp.recall, f1, thre);
	// fprintf(fout, "FlowRadar\tFlowRadar%d\tFlowRadar%d\tFlowRadar%d\t%lf\t%lf\t%lf\t%d\n", 
	// 	total_mem_in_bytes, total_w, N, NONAN(pp.precision), NONAN(pp.recall), f1, thre);
}

/*
 * ./testparam srcFile N Mb Mu step
 * srcFile: 5-tuple file
 * N: number of packet per window
 * wl: threshold lower bound
 * wu: threshold upper bound
 * step: threshold step size
 * dstFile: for storing performance
 */
int main(int argc, const char* argv[]){
	if(argc != 10){
		printf("./testpersistent srcFile N wl wu step M total_w dstFile mode\n\tsrcFile: 5-tuple file\n\tN: number of packet per window\n\twl: threshold lower bound\n\twu: threshold upper bound\n\tstep: threshold step size\n\tM: number of buckets for AHeap\n\ttotal_w: total number of time windows\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' continue test\n");
		exit(1);
	}

	int N = atoi(argv[2]);
	int wl = atoi(argv[3]);
	int wu = atoi(argv[4]);
	int step = atoi(argv[5]);
	int M = atoi(argv[6]);
	int total_w = atoi(argv[7]);


	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);

	FILE* fout;
	if(strcmp(argv[9], "w") == 0){
		fout = fopen(argv[8], "w");
		fprintf(fout, "algo\tmem\ttot_w\tp_per_w\tprec\trecall\tf1-score\tthre\n");
		printf("algo\tmem\ttot_w\tp_per_w\tprec\trecall\tf1-score\tthre\n");
	}
	else if(strcmp(argv[9], "a") == 0){
		fout = fopen(argv[8], "a");
	}
	else{
		printf("./testpersistent srcFile N wl wu step M total_w dstFile mode\n\tsrcFile: 5-tuple file\n\tN: maximum number of distinct flows\n\twl: threshold lower bound\n\twu: threshold upper bound\n\tstep: threshold step size\n\tM: number of buckets for AHeap\n\ttotal_w: total number of time windows\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' continue test\n");
		exit(1);
	}

	for(int thre = wl; thre <= wu; thre += step)
		test(M, tuples, fout, total_w, thre, N);
	fclose(fout);
}

