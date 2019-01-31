/* Find Heavy Hitter */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AHeap/AHeap.h"
#include "../common/IO.h"
#include "../common/metric.h"
#include "../FlowRadar/insertable_iblt.h"
#include "../tasks/FlowRadar_CountingTasks.h"
#include "../tasks/AHeap_CountingTasks.h"

//
void test(vector<string>& tuples, FILE* fout, int threshold, int flow_num, int bucket_num){
	int d1 = 2, d2 = 2;
	int total_mem_in_bytes = bucket_num * (KEY_LEN + 4);
	int m = bucket_num / (pow(2, d1 + d2) - 1);
	//printf("d1=%d, d2=%d, m=%d\n", d1, d2, m);
	HeavyHitterPerform hhp;
	
	AHeap* aheap = new AHeap(m, KEY_LEN + 4, -1);
	aheap->set_keylen(13);
	hhp = hhTest(aheap, tuples, threshold, flow_num);
	delete aheap;
	double f1 = 2 * (hhp.precision * hhp.recall) / (hhp.precision + hhp.recall);
	printf("AHeap: recall: %lf, precision: %lf, f1-score: %lf\n", hhp.recall, hhp.precision, f1);
	fprintf(fout, "AHeap\tAHeap%d\tAHeap%d\t%lf\t%lf\t%lf\t%d\n", 
		total_mem_in_bytes, flow_num, hhp.precision, hhp.recall, f1, threshold);

	InsertableIBLT *iblt = new InsertableIBLT(total_mem_in_bytes);
	hhp = hhTest(iblt, tuples, threshold, flow_num);
	delete iblt;
	f1 = NONAN(2 * (hhp.precision * hhp.recall) / (hhp.precision + hhp.recall));
	printf("FlowRadar: recall: %lf, precision: %lf, f1-score: %lf\n", hhp.recall, hhp.precision, f1);
	fprintf(fout, "FlowRadar\tFlowRadar%d\tFlowRadar%d\t%lf\t%lf\t%lf\t%d\n", 
		total_mem_in_bytes, flow_num, hhp.precision, hhp.recall, f1, threshold);

	/* for AHeap with encode test */
	AHeap* ah[5];
	ah[0] = new AHeap(m*2, KEY_LEN + 4, 0);
	ah[1] = new AHeap(m*2, KEY_LEN + 4, 3);
	ah[2] = new AHeap(m*2, KEY_LEN + 4, 4);


	ah[0]->set_keylen(5);	
	ah[1]->set_keylen(5);	
	ah[2]->set_keylen(5);


	hhp = hhTest(ah, tuples, threshold, flow_num, 3);
	delete ah[0];	delete ah[1];	delete ah[2];
	f1 = NONAN(2 * (hhp.precision * hhp.recall) / (hhp.precision + hhp.recall));
	fprintf(fout, "AHE\tAHE%d\tAHE%d\t%lf\t%lf\t%lf\t%d\n",
		total_mem_in_bytes, flow_num, hhp.precision, hhp.recall, f1, threshold);
	printf("AHE: recall: %lf, precision: %lf, f1-score: %lf\n",
		hhp.recall, hhp.precision, f1);

	printf("\n");
}
 
/*
 * ./testparam srcFile M dstFile
 * M is the size of first layer
 */
int main(int argc, const char* argv[]){
	
	if(argc != 9){
		printf("\t./testheavyhitter srcFile flow_num bucket_num thresholdlow thresholdup step dstFile mode\n\t\"srcFile\": file containing 5-tuples\n\t\"flow_num\":MAX_DISTINCT_FLOW_NUM\n\t\"bucket_num\":MAX_BUCKE_NUM\n\t\"thresholdlow\":lower bound of threshold\n\t\"thresholdup\":upper bound of threshold\n\t\"step\":step of threshold\n\t\"mode\":output mode\n");
		// printf("%d\n", argc);
		exit(1);
	}

	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);
   	int flow_num = atoi(argv[2]);
   	int bucket_num = atoi(argv[3]);
   	int thresholdlow = atoi(argv[4]);
   	int thresholdup = atoi(argv[5]);
    int step = atoi(argv[6]);

   	// if (thresholdlow >= thresholdup || step > thresholdup - thresholdlow)
   	// {
   	// 	printf("Incorrect threshold setting.\n");

   	// 	exit(1);
   	// }

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
		test(tuples, fout, thre, flow_num, bucket_num);
	}

	fclose(fout);
	return 0;

}