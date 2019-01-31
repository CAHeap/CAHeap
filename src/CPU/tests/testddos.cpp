/*
 * ddos test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AHeap/AHeap.h"
#include "../common/IO.h"
#include "../common/metric.h"
#include "../FlowRadar/insertable_iblt.h"
#include "../tasks/FlowRadar_KVTasks.h"
#include "../tasks/AHeap_KVTasks.h"

#define MAX_FLOW_NUM 16000
	

/* 
 * parameters:
 *	  M: size of 1st layer
 *    tuples: input file containing packets
 *    dstFile: output file to store the performance
 */
void test(int M, vector<string>& tuples, FILE* fout, int thre, int N = MAX_FLOW_NUM){
	int total_mem_in_bytes = M * (KEY_LEN + 4);
	DDoSPerform dp;
	int packet_num;

  /* for AHeap test */
	int d1 = 2, d2 = 2, m = M / (pow(2, d1 + d2) - 1);
	AHeap* aheap = new AHeap(m, KEY_LEN + 4, -1);
	dp = ddosTest(aheap, tuples, N, thre, &packet_num);
	delete aheap;
	double f1 = 2 * (dp.precision * dp.recall) / (dp.precision + dp.recall);
	printf("AHeap\t\t%d\t%d\t%d\t%lf\t%lf\t%lf\n",
		total_mem_in_bytes, N, packet_num, NONAN(dp.precision), NONAN(dp.recall), NONAN(f1));
	fprintf(fout, "AHeap\tAHeap%d\t%d\t%d\t%lf\t%lf\t%lf\n",
		total_mem_in_bytes, N, packet_num, NONAN(dp.precision), NONAN(dp.recall), NONAN(f1));
	
  /* for FlowRadar test */
	InsertableIBLT *iblt = new InsertableIBLT(total_mem_in_bytes);
	dp = ddosTest(iblt, tuples, N, thre);
	delete iblt;
	f1 = 2 * (dp.precision * dp.recall) / (dp.precision + dp.recall);
	printf("FlowRadar\t%d\t%d\t%d\t%lf\t%lf\t%lf\n",
		total_mem_in_bytes, N, packet_num, NONAN(dp.precision), NONAN(dp.recall), NONAN(f1));
	fprintf(fout, "FlowRadar\tFlowRadar%d\t%d\t%d\t%lf\t%lf\t%lf\n",
		total_mem_in_bytes, N, packet_num, NONAN(dp.precision), NONAN(dp.recall), NONAN(f1));

  /* for AHeap with encode test */
	AHeap* ah[3];
	ah[0] = new AHeap(m*2, KEY_LEN + 4, 3);
	ah[1] = new AHeap(m*2, KEY_LEN + 4, 4);
	ah[2] = new AHeap(m*2, KEY_LEN + 4, 0);


	ah[0]->set_keylen(5);	 
	ah[1]->set_keylen(5);	
	ah[2]->set_keylen(5);


	dp = ddosTest(ah, tuples, N, thre, 3);
	delete ah[0];	delete ah[1];	delete ah[2];
	f1 = 2 * (dp.precision * dp.recall) / (dp.precision + dp.recall);
	printf("AHE\t\t%d\t%d\t%d\t%lf\t%lf\t%lf\n",
		total_mem_in_bytes, N, packet_num, NONAN(dp.precision), NONAN(dp.recall), NONAN(f1));
	fprintf(fout, "AHE\tAHE%d\t%d\t%d\t%lf\t%lf\t%lf\n",
		total_mem_in_bytes, N, packet_num, NONAN(dp.precision), NONAN(dp.recall), NONAN(f1));
}

/*
 * ./testheavychange srcFile1 srcFile2 N M threb threu step dstFile w
 * srcFile: 5-tuple file
 * N: maximum number of distinct flows
 * M: bucket number
 * threb: threshold lower bound
 * threu: threshold upper bound
 * step: step size for varing threshold
 * dstFile: for storing performance
 */
int main(int argc, const char* argv[]){
	if(argc != 9){
		printf("./testddos srcFile N M threb threu step dstFile mode\n\tsrcFile: 5-tuple file\n\tN: maximum number of distinct flows\n\tM: bucket number\n\tthreb: threshold lower bound\n\tthreu: threshold upper bound\n\tstep: step size for varing threshold\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' continue test\n");
		exit(1);
	}

	int N = atoi(argv[2]);
	int M = atoi(argv[3]);
	int threb = atoi(argv[4]);
	int threu = atoi(argv[5]);
	int step = atoi(argv[6]);


	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);

	FILE* fout;
	if(strcmp(argv[8], "w") == 0){
		fout = fopen(argv[7], "w");
		fprintf(fout, "algo\tmem\tflownum\tpacketnum\tprecision\trecall\tf1\n");
	}
	else if(strcmp(argv[8], "a") == 0){
		fout = fopen(argv[7], "a");
	}
	else{
		printf("./testddos srcFile N M threb threu step dstFile mode\n\tsrcFile: 5-tuple file\n\tN: maximum number of distinct flows\n\tM: bucket number\n\tthreb: threshold lower bound\n\tthreu: threshold upper bound\n\tstep: step size for varing threshold\n\tdstFile: for storing performance\n\tmode: 'w' for new test, 'a' continue test\n");
		exit(1);
	}

	for(int thre = threb; thre <= threu; thre += step)
		test(M, tuples, fout, thre, N);
	fclose(fout);
	return 0;
	// return flowletTest(NULL, tuples, 1, 1);
}

