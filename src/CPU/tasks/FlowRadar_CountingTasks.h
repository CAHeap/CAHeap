#ifndef _FLOW_RADAR_COUNTING_TASK_H_
#define _FLOW_RADAR_COUNTING_TASK_H_


#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include "../common/metric.h"
#include "../common/util.h"
#include "../FlowRadar/insertable_iblt.h"

using std::vector;

/* fit in tuples, return packet num */
int input(InsertableIBLT* iblt, vector<string>& tuples, int MAX_FLOW_NUM, unordered_map<string, int>& flowCount){
	int packet_cnt = 0;
	for(int i = 0; i < (int)tuples.size(); ++i){
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);

		iblt->insert(tmpKey);

		flowCount[string(tmpKey, 13)] += 1;

		packet_cnt++;

		if(flowCount.size() >= MAX_FLOW_NUM)
			break;
	}
	return packet_cnt;
}



FlowsizePerform accTest(InsertableIBLT* iblt, vector<string>& tuples, int MAX_FLOW_NUM){
	unordered_map<string, int> flowCount;
	int packet_cnt = input(iblt, tuples, MAX_FLOW_NUM, flowCount);


	unordered_map<string, int> sum;
	iblt->dump(sum);


	int load_cnt = (int)flowCount.size();
	double acc_cnt = 0, aae = 0, are = 0;
	for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it){
		char tmpStr[KEY_LEN];
		memcpy(tmpStr, it->first.c_str(), KEY_LEN);
		string s = string(tmpStr, KEY_LEN);

		int res = 0;
		if(sum.find(s) == sum.end()){
			load_cnt--;
			continue;
		}
		else{
			res = sum[s];
		}


		if(res == it->second)	acc_cnt += 1;
		else{
			aae += abs((int)res - (int)it->second);
			are += abs((int)res - (int)it->second) * 1.0 / it->second;
		}
	}
	// printf("load_cnt=%d\n", load_cnt);
	FlowsizePerform fsp;
	fsp.accuracy = acc_cnt / (int)flowCount.size();
	fsp.aae = aae / (int)flowCount.size();
	fsp.are = are / (int)flowCount.size();
	fsp.load_factor = load_cnt * 1.0 / (int)flowCount.size();
	// fsp.load_factor = load_cnt * 1.0 / packet_cnt;
	return fsp;
}






HeavychangePerform hcTest(InsertableIBLT* iblt1, InsertableIBLT* iblt2, vector<string>& tuples_1, vector<string>& tuples_2, int MAX_FLOW_NUM, int thre){
	unordered_map<string, int> flowCount_1, flowCount_2;
	unordered_map<string, int> map1, map2;

	for(int i = 0; i < (int)tuples_1.size(); ++i){
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples_1[i].c_str(), KEY_LEN);

		iblt1->insert(tmpKey);

		flowCount_1[string(tmpKey, 13)] += 1;
		if(flowCount_1.size() >= MAX_FLOW_NUM)
			break;
	}
	iblt1->dump(map1);

	for(int i = 0; i < (int)tuples_2.size(); ++i){
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples_2[i].c_str(), KEY_LEN);

		iblt2->insert(tmpKey);

		flowCount_2[string(tmpKey, 13)] += 1;
		if(flowCount_2.size() >= MAX_FLOW_NUM)
			break;
	}
	iblt2->dump(map2);

  /* find heavy change using AHeap */
	unordered_map<string, int> hc1;
	for(unordered_map<string, int>::iterator it = map1.begin(); it != map1.end(); ++it){
		if(it->second < thre)	
			continue;
		if(map2.find(it->first) == map2.end())
			hc1[it->first] = it->second;
		else if(abs(it->second - map2[it->first]) >= thre)
			hc1[it->first] = abs(it->second - map2[it->first]);
	}
	for(unordered_map<string, int>::iterator it = map2.begin(); it != map2.end(); ++it){
		if(it->second < thre)	
			continue;
		if(map1.find(it->first) == map1.end())
			hc1[it->first] = it->second;
		else if(abs(it->second - map1[it->first]) >= thre)
			hc1[it->first] = abs(it->second - map1[it->first]);
	}

	// for(unordered_map<string, int>::iterator it = hc1.begin(); it != hc1.end(); ++it)
	// 	printf("%s, %d\n", it->first.c_str(), it->second);
	
  /* find real heavy change */
	unordered_map<string, int> hc2;
	for(unordered_map<string, int>::iterator it = flowCount_1.begin(); it != flowCount_1.end(); ++it){
		if(it->second < thre)	
			continue;
		if(flowCount_2.find(it->first) == flowCount_2.end())
			hc2[it->first] = it->second;
		else if(abs(it->second - flowCount_2[it->first]) >= thre)
			hc2[it->first] = abs(it->second - flowCount_2[it->first]);
	}
	for(unordered_map<string, int>::iterator it = flowCount_2.begin(); it != flowCount_2.end(); ++it){
		if(it->second < thre)	
			continue;
		if(flowCount_1.find(it->first) == flowCount_1.end())
			hc2[it->first] = it->second;
		else if(abs(it->second - flowCount_1[it->first]) >= thre)
			hc2[it->first] = abs(it->second - flowCount_1[it->first]);
	}


  /* ******* */
	HeavychangePerform hcp;

	int acc_cnt = 0;
	for(unordered_map<string, int>::iterator it = hc1.begin(); it != hc1.end(); ++it)
		if(hc2.find(it->first) != hc2.end())
			acc_cnt++;
	hcp.get = (int)hc1.size();	
	hcp.real = (int)hc2.size();
	hcp.precision = acc_cnt * 1.0 / (int)hc1.size();
	hcp.recall = acc_cnt * 1.0 / (int)hc2.size();

	// printf("real-hc: %lu, precision: %lf, recall: %lf\n", hc2.size(), hcp.precision, hcp.recall);
	return hcp;
}

FSDPerform fsdTest(InsertableIBLT* iblt, vector<string>& tuples, int MAX_FLOW_NUM){
	unordered_map<string, int> flowCount;
	int packet_cnt = input(iblt, tuples, MAX_FLOW_NUM, flowCount);


	unordered_map<string, int> sum;
	iblt->dump(sum);
	unordered_map<int, int> fsd, fsd_std;
	Transform2Fsd(sum, fsd);
	Transform2Fsd(flowCount, fsd_std);
	double upper = 0, lower = 0;
	for(unordered_map<int, int>::iterator it = fsd_std.begin(); it != fsd_std.end(); ++it){
		int cnt_1 = it->second;
		int cnt_2 = 0;
		if(fsd.find(it->first) != fsd.end())
			cnt_2 = fsd[it->first];
		upper += abs(cnt_1 - cnt_2);
		lower += (cnt_1 + cnt_2);
	}
	FSDPerform fsdp;
	fsdp.wmre = upper / (lower / 2);
	return fsdp;
}



HeavyHitterPerform hhTest(InsertableIBLT *iblt, vector<string>& tuples, int threshold, int MAX_FLOW_NUM)
{
	unordered_map<string, int> flowCount;
	int packet_cnt = input(iblt, tuples, MAX_FLOW_NUM, flowCount);
	// printf("flowcount size: %d\n", (int)flowCount.size());

	unordered_map<string, int> sum;
	iblt->dump(sum);


	unordered_map<string, int> hh, hh_std;
	for(unordered_map<string, int>::iterator it = sum.begin(); it != sum.end(); ++it)
		if(it->second >= threshold)
			hh[it->first] = it->second;
	for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it)
		if(it->second >= threshold)
			hh_std[it->first] = it->second;

	int tp = 0;
	for(unordered_map<string, int>::iterator it = hh_std.begin(); it != hh_std.end(); ++it)
		if(hh.find(it->first) != hh.end())
			tp++;

	
	HeavyHitterPerform hhp;
	hhp.precision = NONAN(tp * 1.0 / (int)hh.size());
	hhp.recall = tp * 1.0 / (int)hh_std.size();
	// printf("%d, %d ,%d\n", tp, (int)hh.size(), (int)hh_std.size());
	return hhp;
}



EntropyPerform entropyTest(InsertableIBLT* iblt, vector<string>& tuples, int MAX_FLOW_NUM){
	unordered_map<string, int> flowCount;
	int packet_cnt = input(iblt, tuples, MAX_FLOW_NUM, flowCount);


	unordered_map<string, int> sum;
	iblt->dump(sum);
	unordered_map<int, int> fsd, fsd_std;
	Transform2Fsd(sum, fsd);
	Transform2Fsd(flowCount, fsd_std);



	EntropyPerform ep;
	memset(&ep, 0, sizeof(EntropyPerform));
	int M = (int)sum.size();
	for(unordered_map<int, int>::iterator it = fsd.begin(); it != fsd.end(); ++it){
		int f = it->first;
		double por = it->second * 1.0 / M;
		ep.est -= f * por * log(por);
	}

	M = (int)flowCount.size();
	for(unordered_map<int, int>::iterator it = fsd_std.begin(); it != fsd_std.end(); ++it){
		int f = it->first;
		double por = it->second * 1.0 / M;
		ep.real -= f * por * log(por);
	}
	
	ep.re = abs(ep.real - ep.est) / ep.real;
	return ep;
}

EntropyPerform entropyTest2(InsertableIBLT* iblt, vector<string>& tuples, int MAX_FLOW_NUM){
	unordered_map<string, int> flowCount;
	int packet_cnt = input(iblt, tuples, MAX_FLOW_NUM, flowCount);


	unordered_map<string, int> sum;
	iblt->dump(sum);
	unordered_map<int, int> fsd, fsd_std;
	Transform2Fsd(sum, fsd);
	Transform2Fsd(flowCount, fsd_std);



	EntropyPerform ep;
	memset(&ep, 0, sizeof(EntropyPerform));
	int M = (int)sum.size();
	for(unordered_map<int, int>::iterator it = fsd.begin(); it != fsd.end(); ++it){
		int f = it->first * 1.3;
		double por = it->second * 1.0 / M;
		ep.est -= f * por * log(por);
	}

	M = (int)flowCount.size();
	for(unordered_map<int, int>::iterator it = fsd_std.begin(); it != fsd_std.end(); ++it){
		int f = it->first;
		double por = it->second * 1.0 / M;
		ep.real -= f * por * log(por);
	}

	ep.re = abs(ep.real - ep.est) / ep.real;
	return ep;
}


#endif