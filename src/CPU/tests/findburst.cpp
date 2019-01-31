/*Find burst*/
#include <stdio.h>
#include <stdlib.h>
#include "../AHeap/AHeap.h"
#include "../common/IO.h"
#include "../common/metric.h"
#include "../FlowRadar/insertable_iblt.h"
#include "../tasks/FlowRadar_KVTasks.h"
#include "../tasks/AHeap_KVTasks.h"


void test(vector<string>& tuples, FILE* fout, int threshold, int flow_num, int bucket_num, int window_size){
	//set size of AHeap and FlowRadar
	int d1 = 2, d2 = 2;
	int total_mem_in_bytes = bucket_num * (KEY_LEN + 4);
	int m = bucket_num / (pow(2, d1 + d2) - 1);
	//printf("d1=%d, d2=%d, m=%d\n", d1, d2, m);
	HeavyHitterPerform hhp;
	int packet_num;
	vector<string> real;
	window_thre = threshold;

	AHeap* aheap = new AHeap(m, KEY_LEN + 4, -1);
	aheap->set_keylen(13);
	hhp = burstTest(aheap, tuples, threshold, flow_num, window_size, real, &packet_num);
	delete aheap;
	double f1 = 2 * (hhp.precision * hhp.recall) / (hhp.precision + hhp.recall);
	printf("AHeap: recall: %lf, precision: %lf\n\n", hhp.recall, hhp.precision);
	fprintf(fout, "AHeap\tAHeap%d\tAHeap%d\t%d\t%lf\t%lf\t%lf\t%d\n", 
		total_mem_in_bytes, flow_num, packet_num, hhp.precision, hhp.recall, f1, threshold);
	
	InsertableIBLT *iblt = new InsertableIBLT(total_mem_in_bytes);
	hhp = burstTest(iblt, tuples, threshold, flow_num, window_size, real);
	delete iblt;
	f1 = NONAN(2 * (hhp.precision * hhp.recall) / (hhp.precision + hhp.recall));
	printf("FlowRadar: recall: %lf, precision: %lf\n\n", hhp.recall, hhp.precision);
	fprintf(fout, "FlowRadar\tFlowRadar%d\tFlowRadar%d\t%d\t%lf\t%lf\t%lf\t%d\n", 
		total_mem_in_bytes, flow_num, packet_num, hhp.precision, hhp.recall, f1, threshold);
}


int main(int argc, const char* argv[]){
	
	if(argc != 10){
		printf("\t./testburst srcFile flow_num bucket_num thresholdlow thresholdup step dstFile mode window_size \n\t\"srcFile\": file containing 5-tuples\n\t\"flow_num\":MAX_DISTINCT_FLOW_NUM\n\t\"bucket_num\":MAX_BUCKE_NUM\n\t\"thresholdlow\":lower bound of threshold\n\t\"thresholdup\":upper bound of threshold\n\t\"step\":step of threshold\n\t\"dstFile\":output file\n\t\"mode\":output mode\n\t\"window_size\":window_size\n");
		exit(1);
	}
	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);
   	int flow_num = atoi(argv[2]);
   	int bucket_num = atoi(argv[3]);
   	int thresholdlow = atoi(argv[4]);
   	int thresholdup = atoi(argv[5]);
    int step = atoi(argv[6]);
    int window_size = atoi(argv[9]);

   	if (thresholdlow >= thresholdup || step > thresholdup - thresholdlow)
   	{
   		printf("Incorrect threshold setting.\n");

   		exit(1);
   	}

	FILE*  fout;

	if (strcmp(argv[8], "w") == 0)
	{
		fout = fopen(argv[7],"w");
		fprintf(fout, "algo\tmem\tflownum\tpacketnum\tprecision\trecall\tf1\tthre\n");
	} else if (strcmp(argv[8], "a") == 0)
	{
		fout = fopen(argv[7],"a");
	} else
	{
		printf("Choose correct mode: w/a.\n");
	}
	for(int thre = thresholdlow; thre <= thresholdup; thre += step)
	{
		test(tuples, fout, thre, flow_num, bucket_num, window_size);
	}

	fclose(fout);
	
	return 0;

}