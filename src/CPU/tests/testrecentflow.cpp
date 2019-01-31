#include <stdio.h>
#include <stdlib.h>
#include "../AHeap/AHeap.h"
#include "../common/IO.h"
#include "../common/metric.h"
#include "../FlowRadar/insertable_iblt.h"
#include "../tasks/FlowRadar_KVTasks.h"
#include "../tasks/AHeap_KVTasks.h"


void test(vector<string>& tuples, FILE* fout, int threshold, int flow_num, int bucket_num)
{
	int d1 = 2, d2 = 2;
	int total_mem_in_bytes = bucket_num * (KEY_LEN + 4);
	int m = bucket_num / (pow(2, d1 + d2) - 1);
	// printf("d1=%d, d2=%d, m=%d\n", d1, d2, m);
	HeavyHitterPerform hhp;
	
	AHeap* aheap = new AHeap(m, KEY_LEN + 4, -1);
	aheap->set_keylen(13);
	hhp = rfTest(aheap, tuples, threshold, flow_num);
	delete aheap;
	double f1 = 2 * hhp.recall * hhp.precision / (hhp.recall + hhp.precision);
	printf("AHEAP:\t\tRecall: %lf, Precision: %lf\n", hhp.recall, hhp.precision);
	fprintf(fout, "AHeap\t%d\tAHeap%d\t%f\t%f\t%lf\t%d\n", total_mem_in_bytes, flow_num, hhp.precision, hhp.recall, f1, threshold);
	
	// InsertableIBLT *iblt = new InsertableIBLT(total_mem_in_bytes);
	// hhp = rfTest(iblt, tuples, threshold, flow_num);
	// delete iblt;
	// f1 = 2 * hhp.recall * hhp.precision / (hhp.recall + hhp.precision);
	// printf("FlowRadar:\tRecall: %lf, Precision: %lf\n", hhp.recall, hhp.precision);
	// fprintf(fout, "FlowRadar\t%d\tFlowRadar\t%d\t%f\t%f\t%f\n", total_mem_in_bytes, flow_num, hhp.precision, hhp.recall, f1);


  /* for AHeap test */
	AHeap* ah[3];
	ah[0] = new AHeap(m*2, KEY_LEN + 4, 3);
	ah[1] = new AHeap(m*2, KEY_LEN + 4, 4);
	ah[2] = new AHeap(m*2, KEY_LEN + 4, 0);


	ah[0]->set_keylen(5);	 
	ah[1]->set_keylen(5);	
	ah[2]->set_keylen(5);


	hhp = rfTest(ah, tuples, threshold, flow_num, 3);
	delete ah[0];	delete ah[1];	delete ah[2];
	f1 = 2 * hhp.recall * hhp.precision / (hhp.recall + hhp.precision);
	printf("AHE:\t\tRecall: %lf, Precision: %lf\n\n", hhp.recall, hhp.precision);
	fprintf(fout, "AHE\t%d\tAHE%d\t%f\t%f\t%lf\t%d\n", total_mem_in_bytes, flow_num, hhp.precision, hhp.recall, f1, threshold);
}

// ./findrecentflows srcFile bucket_num flow_num threshold mode
int main(int argc, char const *argv[])
{
	if(argc != 10){
		printf("\t./findrecentflow srcFile flow_num bucket_num thresholdlow thresholdup step dstFile mode aging\n\t\"srcFile\": file containing 5-tuples\n\t\"flow_num\":MAX_DISTINCT_FLOW_NUM\n\t\"bucket_num\":MAX_BUCKE_NUM\n\t\"thresholdlow\":lower bound of threshold\n\t\"thresholdup\":upper bound of threshold\n\t\"step\":step of threshold\n\t\"dstFile\": path of output file\n\t\"mode\":output mode\n\t\"aging\": threshold of aging");
		printf("%d\n", argc);
		exit(1);
	}

	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);
   	int flow_num = atoi(argv[2]);
   	int bucket_num = atoi(argv[3]);
   	double thresholdlow = atof(argv[4]);
   	double thresholdup = atof(argv[5]);
   	double step = atof(argv[6]);
   	aging = atoi(argv[9]);

   	// if (thresholdlow >= thresholdup || step > thresholdup - thresholdlow)
   	// {
   	// 	printf("Incorrect threshold setting.\n");

   	// 	exit(1);
   	// }

	FILE*  fout;

	if (strcmp(argv[8], "w") == 0)
	{
		fout = fopen(argv[7],"w");
		fprintf(fout, "algo\tmem\tflownum\tprecision\trecall\tf1\tthreshold\n");
	} else if (strcmp(argv[8], "a") == 0)
	{
		fout = fopen(argv[7],"a");
	} else
	{
		printf("Choose correct mode: w/a.\n");
	}

	for(double thre = thresholdlow; thre <= thresholdup; thre += step)
	{
		test(tuples, fout, thre, flow_num, bucket_num);
	}

	fclose(fout);
	return 0;
}