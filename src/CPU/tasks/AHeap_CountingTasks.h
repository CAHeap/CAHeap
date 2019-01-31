#ifndef _AHEAP_COUNTING_TASK_H_
#define _AHEAP_COUNTING_TASK_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include "../common/metric.h"
#include "../AHeap/AHeap.h"
#include "../AHeap/Util.h"
#include "../common/util.h"
#include "../common/BOBHash32.h"

using std::vector;
#define PPPP 3
#define DEBUG_LEN 10000

// Fit in tuples and get the number of flows and packets
int input(AHeap* ah, vector<string>& tuples, int MAX_FLOW_NUM, unordered_map<string, int>& flowCount)
{
	int packet_cnt = 0;

	for(int i = 0; i < (int)tuples.size(); ++i){
		char tmpKey[KEY_LEN];
		MyItem flowitem;

		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);
		flowitem.initial_item(KEY_LEN, tmpKey, 1, i);
		ah->insert(flowitem, tmpKey, KEY_LEN);
		flowCount[string(tmpKey, 13)] += 1;
		packet_cnt++;

		if(flowCount.size() >= MAX_FLOW_NUM)
			break;
	}
	return packet_cnt;
}

void exchange_byte(char& a, char& b){
	char tmp = a;
	a = b;
	b = tmp;
}

// Fit in tuples and get the number of flows and packets
int input(AHeap **ah, vector<string>& tuples, int MAX_FLOW_NUM, unordered_map<string, int>& flowCount, int ah_num)
{

	int packet_cnt = 0;
	for(int i = 0; i < (int)tuples.size(); ++i){
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);
		
		for(int k = 0; k < ah_num; ++k)
			AHeapInsert(*ah[k], ah[k]->Cacheid, tmpKey, KEY_LEN, 3, PPPP, i, 15);
		flowCount[string(tmpKey, 13)] += 1;

		packet_cnt++;

		if(flowCount.size() >= MAX_FLOW_NUM)
			break;
	}
	return packet_cnt;
	printf("insertion done\n");
}



// Test accuracy in terms of flow size with code mechanism
FlowsizePerform accTest(AHeap** ah, vector<string>& tuples, int MAX_FLOW_NUM, int ah_num = 3, int *packet_count = NULL)
{
	unordered_map<string, int> flowCount;
	// Feed in tuples 
	int tmpCnt = input(ah, tuples, MAX_FLOW_NUM, flowCount, ah_num);
	if(packet_count != NULL)	*packet_count = tmpCnt;


	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(3);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec;
	for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
	// Retrieve flow information from caches by decoding
	DecodeCaches(AllRes, ahVec, parasVec); 

	unordered_map<string, vector<MyItem> >& sum = AllRes[0].first;

	int load_cnt = (int)flowCount.size();
	double acc_cnt = 0, aae = 0, are = 0;

	int err[DEBUG_LEN] = {0};
	// Get flow size from the retrieve results and calculate average absolute error and average realtive error 
	for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it){
		char tmpStr[KEY_LEN];
		memcpy(tmpStr, it->first.c_str(), KEY_LEN);
		string s = string(tmpStr, KEY_LEN);

		int res = 0;
		if(sum.find(s) == sum.end()) // Check whether a key exists in the AHeap
		{
			load_cnt--;
			continue;
		}
		else // Get flow size
		{
			vector<MyItem> vec = sum[s];
			for(int i = 0; i < vec.size(); ++i)
				res += vec[i].my_value_count;
		}


		if(res == it->second)	acc_cnt += 1;
		else{
			aae += abs((int)res - (int)it->second);
			are += abs((int)res - (int)it->second) * 1.0 / it->second;
			for(int err_enum = 1; err_enum <= DEBUG_LEN; ++err_enum)
				if(abs((int)res - (int)it->second) == err_enum)
					err[err_enum - 1]++;
		}
	}
	
	FlowsizePerform fsp;
	fsp.accuracy = acc_cnt / (int)flowCount.size();
	fsp.aae = aae / (int)flowCount.size();
	fsp.are = are / (int)flowCount.size();
	fsp.load_factor = load_cnt * 1.0 / (int)flowCount.size();

	return fsp;
}

// Test accuracy in terms of flow size
FlowsizePerform accTest(AHeap* ah, vector<string>& tuples, int MAX_FLOW_NUM, int* packet_count = NULL)
{
	
	unordered_map<string, int> flowCount;
	int tmpCnt = input(ah, tuples, MAX_FLOW_NUM, flowCount);
	if(packet_count != NULL)	*packet_count = tmpCnt;

	unordered_map<string, vector<MyItem> > sum;
	MergeSingleCache(sum, *ah);


	int load_cnt = (int)flowCount.size();
	double acc_cnt = 0, aae = 0, are = 0;
	int err[DEBUG_LEN] = {0};
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
			vector<MyItem> vec = sum[s];
			for(int i = 0; i < vec.size(); ++i)
				res += vec[i].my_value_count;
		}


		if(res == it->second)	acc_cnt += 1;
		else{
			aae += abs((int)res - (int)it->second);
			are += abs((int)res - (int)it->second) * 1.0 / it->second;
			for(int err_enum = 1; err_enum <= DEBUG_LEN; ++err_enum)
				if(abs((int)res - (int)it->second) == err_enum)
					err[err_enum - 1]++;
		}
	}
	
	FlowsizePerform fsp;
	fsp.accuracy = acc_cnt / (int)flowCount.size();
	fsp.aae = aae / (int)flowCount.size();
	fsp.are = are / (int)flowCount.size();
	fsp.load_factor = load_cnt * 1.0 / (int)flowCount.size();
	
	return fsp;
}

// Find heavy hitter with code mechanism
HeavyHitterPerform hhTest(AHeap** ah, vector<string>& tuples, int threshold, int MAX_FLOW_NUM, int ah_num = 3)
{
	unordered_map<string, int> flowCount;
	int tmpCnt = input(ah, tuples, MAX_FLOW_NUM, flowCount, ah_num);

	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(PPPP);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec;
	for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
	
	DecodeCaches(AllRes, ahVec, parasVec); 
	
	unordered_map<string, int> sum;
	Merge(AllRes[0].first, sum);
	
	// Find heavy hitter using AHeap
	unordered_map<string, int> hh, hh_std;
	for(unordered_map<string, int>::iterator it = sum.begin(); it != sum.end(); ++it)
		if(it->second >= threshold)
			hh[it->first] = it->second;
	// Find real heavy hitter
	for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it)
		if(it->second >= threshold)
			hh_std[it->first] = it->second;

	// Calculate recall and precision 
	int tp = 0;
	for(unordered_map<string, int>::iterator it = hh_std.begin(); it != hh_std.end(); ++it)
		if(hh.find(it->first) != hh.end())
		{
			if(it->second != hh[it->first])
			{
				printf("true: %d, est: %d\n", it->second, hh[it->first]);
			}
			tp++;
		}

	
	HeavyHitterPerform hhp;
	hhp.precision = tp * 1.0 / (int)hh.size();
	hhp.recall = tp * 1.0 / (int)hh_std.size();
	
	return hhp;
}

// Find heavy hitter
HeavyHitterPerform hhTest(AHeap* ah, vector<string>& tuples, int threshold, int MAX_FLOW_NUM)
{
	unordered_map<string, int> flowCount;
	int tmpCnt = input(ah, tuples, MAX_FLOW_NUM, flowCount);

	unordered_map<string, vector<MyItem> > map;
	unordered_map<string, int> sum;
	MergeSingleCache(map, *ah);
	Merge(map, sum);
	
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
	hhp.precision = tp * 1.0 / (int)hh.size();
	hhp.recall = tp * 1.0 / (int)hh_std.size();
	
	return hhp;
}

// Find heavy change with code mechanism
HeavychangePerform hcTest(AHeap** ah1, AHeap** ah2, vector<string>& tuples_1, vector<string>& tuples_2, int MAX_FLOW_NUM, int thre, int ah_num = 3){
	unordered_map<string, int> flowCount_1, flowCount_2;
	unordered_map<string, int> map1, map2;

	// Feed in tuples 
	input(ah1, tuples_1, MAX_FLOW_NUM, flowCount_1, ah_num);
	input(ah2, tuples_2, MAX_FLOW_NUM, flowCount_2, ah_num);

	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(PPPP);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec_1, ahVec_2;
	for(int i = 0; i < ah_num; ++i){
		ahVec_1.push_back(*ah1[i]);
		ahVec_2.push_back(*ah2[i]);
	}
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes_1, AllRes_2;
	
	DecodeCaches(AllRes_1, ahVec_1, parasVec); 
	DecodeCaches(AllRes_2, ahVec_2, parasVec); 
	Merge(AllRes_1[0].first, map1);
	Merge(AllRes_2[0].first, map2);


  	// Find heavy change using AHeap
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

	
 	//Find real heavy change 
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


	HeavychangePerform hcp;

	int acc_cnt = 0;
	for(unordered_map<string, int>::iterator it = hc1.begin(); it != hc1.end(); ++it)
		if(hc2.find(it->first) != hc2.end())
			acc_cnt++;
	hcp.get = (int)hc1.size();		
	hcp.real = (int)hc2.size();
	hcp.precision = acc_cnt * 1.0 / (int)hc1.size();
	hcp.recall = acc_cnt * 1.0 / (int)hc2.size();

	return hcp;
}

// Find heavy change
HeavychangePerform hcTest(AHeap* ah1, AHeap* ah2, vector<string>& tuples_1, vector<string>& tuples_2, int MAX_FLOW_NUM, int thre, int* packet_num){
	unordered_map<string, int> flowCount_1, flowCount_2;
	unordered_map<string, vector<MyItem> > sum_1, sum_2;
	unordered_map<string, int> map1, map2;

	*packet_num = 0;
	for(int i = 0; i < (int)tuples_1.size(); ++i){
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples_1[i].c_str(), KEY_LEN);
		MyItem flowitem;
		flowitem.initial_item(KEY_LEN, tmpKey, 1, i);

		ah1->insert(flowitem, tmpKey, KEY_LEN);

		flowCount_1[string(tmpKey, 13)] += 1;
		*packet_num += 1;
		if(flowCount_1.size() >= MAX_FLOW_NUM)
			break;
	}
	MergeSingleCache(sum_1, *ah1);

	for(int i = 0; i < (int)tuples_2.size(); ++i){
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples_2[i].c_str(), KEY_LEN);
		MyItem flowitem;
		flowitem.initial_item(KEY_LEN, tmpKey, 1, i);

		ah2->insert(flowitem, tmpKey, KEY_LEN);

		flowCount_2[string(tmpKey, 13)] += 1;
		if(flowCount_2.size() >= MAX_FLOW_NUM)
			break;
	}
	MergeSingleCache(sum_2, *ah2);

	Merge(sum_1, map1);	Merge(sum_2, map2);

  	// Find heavy change using AHeap
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

	// Find real heavy change
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


	HeavychangePerform hcp;

	int acc_cnt = 0;
	for(unordered_map<string, int>::iterator it = hc1.begin(); it != hc1.end(); ++it)
		if(hc2.find(it->first) != hc2.end())
			acc_cnt++;
	hcp.get = (int)hc1.size();		
	hcp.real = (int)hc2.size();
	hcp.precision = acc_cnt * 1.0 / (int)hc1.size();
	hcp.recall = acc_cnt * 1.0 / (int)hc2.size();

	return hcp;
}

// Get flow size distribution with code mechanism
FSDPerform fsdTest(AHeap** ah, vector<string>& tuples, int MAX_FLOW_NUM, int ah_num = 3)
{
	unordered_map<string, int> flowCount;
	input(ah, tuples, MAX_FLOW_NUM, flowCount, ah_num);

	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(PPPP);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec;
	for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
	DecodeCaches(AllRes, ahVec, parasVec); 
	unordered_map<string, int> map;
	Merge(AllRes[0].first, map);

	unordered_map<int, int> fsd, fsd_std;
	// Get flow size distribution using AHeap
	Transform2Fsd(map, fsd);
	// Get real flow size distribution
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

// Get flow size distribution
FSDPerform fsdTest(AHeap* ah, vector<string>& tuples, int MAX_FLOW_NUM, int* packet_count = NULL)
{
	unordered_map<string, int> flowCount;
	int packet_cnt = input(ah, tuples, MAX_FLOW_NUM, flowCount);
	if(packet_count != NULL)	*packet_count = packet_cnt;

	unordered_map<string, vector<MyItem> > sum;
	unordered_map<string, int> map;
	MergeSingleCache(sum, *ah);
	Merge(sum, map);
	unordered_map<int, int> fsd, fsd_std;
	Transform2Fsd(map, fsd);
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

// Estimate entropy with code mechanism
EntropyPerform entropyTest(AHeap** ah, vector<string>& tuples, int MAX_FLOW_NUM, int ah_num = 3){
	unordered_map<string, int> flowCount;
	input(ah, tuples, MAX_FLOW_NUM, flowCount, ah_num);

	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(PPPP);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec;
	for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;

	DecodeCaches(AllRes, ahVec, parasVec); 

	unordered_map<string, int> map;
	Merge(AllRes[0].first, map);

	unordered_map<int, int> fsd, fsd_std;
	Transform2Fsd(map, fsd);
	Transform2Fsd(flowCount, fsd_std);


	EntropyPerform ep;
	memset(&ep, 0, sizeof(EntropyPerform));
	int M = (int)map.size();
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

// Estimate entropy 
EntropyPerform entropyTest(AHeap* ah, vector<string>& tuples, int MAX_FLOW_NUM, int* packet_count = NULL){
	unordered_map<string, int> flowCount;
	int tmpCnt = input(ah, tuples, MAX_FLOW_NUM, flowCount);
	if(packet_count != NULL)	*packet_count = tmpCnt;



	unordered_map<string, vector<MyItem> > sum;
	unordered_map<string, int> MM;
	MergeSingleCache(sum, *ah);
	Merge(sum, MM);
	unordered_map<int, int> fsd, fsd_std;
	Transform2Fsd(MM, fsd);
	Transform2Fsd(flowCount, fsd_std);


	EntropyPerform ep;
	memset(&ep, 0, sizeof(EntropyPerform));
	int M = (int)MM.size();
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

#endif 
