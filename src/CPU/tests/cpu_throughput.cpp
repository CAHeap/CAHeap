/*
 * flow size test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../srcAHeap/AHeap.h"
#include "../srcAHeap/FlowCache.h"
#include "../common/IO.h"
#include "../common/metric.h"
#include "../srcFlowRadar/insertable_iblt.h"
#include "../FlowRadarTest/FR_CTTasks.h"
#include "../AHeapTest/AH_CTTasks.h"
#define MAX_FLOW_NUM 16000

void test(int M, vector<string>& tuples, FILE* fout, int N = MAX_FLOW_NUM, int test_cycles = 10)
{
	int total_mem_in_bytes = M * (KEY_LEN + 4);
	int d1 = 2, d2 = 2, m = M / (pow(2, d1 + d2) - 1);
	int packet_cnt = (int)tuples.size();

	FlowCache* fc = NULL;


	MyItem* myitems = new MyItem[packet_cnt];
	for(int i = 0; i < packet_cnt; ++i)
	{
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);
		myitems[i].initial_item(KEY_LEN, tmpKey, 1, i);
	}

	timespec time1, time2;
	long long resns;

	clock_gettime(CLOCK_MONOTONIC, &time1);
    for(int t = 0; t < test_cycles; ++t)
    {
    	// printf("cycle-%d\n", t);
      fc = new FlowCache(m, KEY_LEN + 4, -1);
      for(int i = 0; i < packet_cnt; ++i)
        fc->insert(myitems[i], myitems[i].my_key, KEY_LEN);
      delete fc;
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);

    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double th = (double)1000.0 * test_cycles * packet_cnt / resns;

    /* free memory */
	delete[] myitems;

    printf("throughput is %lf mbps\n", th);
}




int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("please enter source data filepath\n");
		exit(1);
	}

	vector<string> tuples;
	Read5Tuples<KEY_LEN>(argv[1], tuples);

	FILE* fout = fopen("../table/speed/cpu_th.txt", "w");
	test(16000, tuples, fout);
	fclose(fout);
} 