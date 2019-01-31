/*
 * heavy change test
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
 *	  M: size of 1st layer
 *    tuples: input file containing packets
 *    dstFile: output file to store the performance
 */
void test(int M, vector<string>& tuples_1, vector<string>& tuples_2, FILE* fout, int thre, int N = MAX_FLOW_NUM){
	int total_mem_in_bytes = M * (KEY_LEN + 4);
	HeavychangePerform hcp;
	int packet_num;

  /* for AHeap test */
	int d1 = 2, d2 = 2, m = M / (pow(2, d1 + d2) - 1);
	AHeap* aheap_1 = new AHeap(m, KEY_LEN + 4, -1);
	AHeap* aheap_2 = new AHeap(m, KEY_LEN + 4, -1);
	hcp = hcTest(aheap_1, aheap_2, tuples_1, tuples_2, N, thre, &packet_num);
	delete aheap_1;
	delete aheap_2; 
	double f1 = 2 * (hcp.precision * hcp.recall) / (hcp.precision + hcp.recall);
	printf("AHeap\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%d\n",
		total_mem_in_bytes, N, packet_num, hcp.precision, hcp.recall, f1, thre);
	fprintf(fout, "AHeap\tAHeap%d\tAHeap%d\t%d\t%lf\t%lf\t%lf\t%d\n",
		total_mem_in_bytes, N, packet_num, hcp.precision, hcp.recall, f1, thre);
	
  /* for FlowRadar test */
	InsertableIBLT *iblt1 = new InsertableIBLT(total_mem_in_bytes);
	InsertableIBLT *iblt2 = new InsertableIBLT(total_mem_in_bytes);
	hcp = hcTest(iblt1, iblt2, tuples_1, tuples_2, N, thre);
	delete iblt1;
	delete iblt2;
	f1 = NONAN(2 * (hcp.precision * hcp.recall) / (hcp.precision + hcp.recall));
	printf("FlowRadar\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%d\n",
		total_mem_in_bytes, N, packet_num, NONAN(hcp.precision), NONAN(hcp.recall), f1, thre);
	fprintf(fout, "FlowRadar\tFlowRadar%d\tFlowRadar%d\t%d\t%lf\t%lf\t%lf\t%d\n",
		total_mem_in_bytes, N, packet_num, NONAN(hcp.precision), NONAN(hcp.recall), f1, thre);
	
  /* encode */
  	AHeap* ah1[3];
  	AHeap* ah2[3];
	ah1[0] = new AHeap(m*2, KEY_LEN + 4, 0);	ah2[0] = new AHeap(m*2, KEY_LEN + 4, 0);
	ah1[1] = new AHeap(m*2, KEY_LEN + 4, 3);	ah2[1] = new AHeap(m*2, KEY_LEN + 4, 3);
	ah1[2] = new AHeap(m*2, KEY_LEN + 4, 4);	ah2[2] = new AHeap(m*2, KEY_LEN + 4, 4);

	ah1[0]->set_keylen(5);	ah2[0]->set_keylen(5);	
	ah1[1]->set_keylen(5);	ah2[1]->set_keylen(5);	
	ah1[2]->set_keylen(5);	ah2[2]->set_keylen(5);	


	hcp = hcTest(ah1, ah2, tuples_1, tuples_2, N, thre, 3);
	delete ah1[0];	delete ah1[1];	delete ah1[2];
	delete ah2[0];	delete ah2[1];	delete ah2[2];
	f1 = NONAN(2 * (hcp.precision * hcp.recall) / (hcp.precision + hcp.recall));
	printf("AHE\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%d\n",
		total_mem_in_bytes, N, packet_num, hcp.precision, hcp.recall, f1, thre);
	fprintf(fout, "AHE\tAHE%d\tAHE%d\t%d\t%lf\t%lf\t%lf\t%d\n",
		total_mem_in_bytes, N, packet_num, hcp.precision, hcp.recall, f1, thre);

	printf("\n");
}


/*
 * ./testheavychange srcFile1 srcFile2 N M threb threu step dstFile w
 * srcFile1: 5-tuple file in the second time-window
 * srcFile2: 5-tuple file in the second time-window
 * N: maximum number of distinct flows
 * M: bucket number
 * threb: threshold lower bound
 * threu: threshold upper bound
 * step: step size for varing threshold
 * dstFile: for storing performance
 */
int main(int argc, const char* argv[]){
	if(argc != 10){
		printf("./testheavychange srcFile1 srcFile2 N M threb threu step dstFile\n\tsrcFile1: 5-tuple file in the first time-window\n\tsrcFile2: 5-tuple file in the second time-window2\n\tN: maximum number of distinct flows\n\tM: bucket number\n\tthreb: threshold lower bound\n\tthreu: threshold upper bound\n\tstep: step size for varing threshold\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' continue test\n");
		exit(1);
	}

	int N = atoi(argv[3]);
	int M = atoi(argv[4]);
	int threb = atoi(argv[5]);
	int threu = atoi(argv[6]);
	int step = atoi(argv[7]);


	vector<string> tuples_1, tuples_2;
	Read5Tuples<KEY_LEN>(argv[1], tuples_1);
	Read5Tuples<KEY_LEN>(argv[2], tuples_2);

	FILE* fout;
	if(strcmp(argv[9], "w") == 0){
		fout = fopen(argv[8], "w");
		fprintf(fout, "algo\tmem\tflownum\tpacketnum\tprecision\trecall\tf1\tthre\n");
	}
	else if(strcmp(argv[9], "a") == 0){
		fout = fopen(argv[8], "a");
	}
	else{
		printf("./testheavychange srcFile1 srcFile2 N M threb threu step dstFile\n\tsrcFile1: 5-tuple file in the first time-window\n\tsrcFile2: 5-tuple file in the second time-window2\n\tN: maximum number of distinct flows\n\tM: bucket number\n\tthreb: threshold lower bound\n\tthreu: threshold upper bound\n\tstep: step size for varing threshold\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' continue test\n");
		exit(1);
	}

	for(int thre = threb; thre <= threu; thre += step)
		test(M, tuples_1, tuples_2, fout, thre, N);
	fclose(fout);
}

