#ifndef _AHEAP_KV_TASK_H_
#define _AHEAP_KV_TASK_H_


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
#include "AHeap_CountingTasks.h"

using std::vector;
using std::pair;


// Find DDos with code mechanism
DDoSPerform ddosTest(AHeap** ah, vector<string>& tuples, int MAX_FLOW_NUM, int thre, int ah_num = 3)
{
	unordered_map<string, int> flowCount;
	input(ah, tuples, MAX_FLOW_NUM, flowCount, ah_num);

	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(3);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec;
	for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
	
	DecodeCaches(AllRes, ahVec, parasVec); 

	unordered_map<string, vector<MyItem> > sum = AllRes[0].first;

	unordered_map<string, int> ddos, ddos_std;
	// Find DDos using AHeap
	CountIP(sum, ddos, thre, 6, 0);
	// Find real DDos 
	CountIP(flowCount, ddos_std, thre, 6, 0);

	int tp = 0;
	for(unordered_map<string, int>::iterator it = ddos_std.begin(); it != ddos_std.end(); ++it)
		if(ddos.find(it->first) != ddos.end()){
			tp++;
		}


	DDoSPerform dp;
	dp.real = (int)ddos_std.size();
	dp.get = (int)ddos.size();
	dp.precision = tp * 1.0 / (int)ddos.size();
	dp.recall = tp * 1.0 / (int)ddos_std.size();
	
	return dp;
}

// Find DDos
DDoSPerform ddosTest(AHeap* ah, vector<string>& tuples, int MAX_FLOW_NUM, int thre, int* packet_count = NULL){
	unordered_map<string, int> flowCount;
	int tmpCnt = input(ah, tuples, MAX_FLOW_NUM, flowCount);
	if(packet_count != NULL)	*packet_count = tmpCnt;

	unordered_map<string, vector<MyItem> > sum;
	MergeSingleCache(sum, *ah);

	unordered_map<string, int> ddos, ddos_std;
	CountIP(sum, ddos, thre, 6, 0);
	CountIP(flowCount, ddos_std, thre, 6, 0);

	int tp = 0;
	for(unordered_map<string, int>::iterator it = ddos_std.begin(); it != ddos_std.end(); ++it)
		if(ddos.find(it->first) != ddos.end()){
			tp++;
		}


	DDoSPerform dp;
	dp.real = (int)ddos_std.size();
	dp.get = (int)ddos.size();
	dp.precision = tp * 1.0 / (int)ddos.size();
	dp.recall = tp * 1.0 / (int)ddos_std.size();

	return dp;
}


//Find super-spreader with code mechanism
DDoSPerform spreaderTest(AHeap** ah, vector<string>& tuples, int MAX_FLOW_NUM, int thre, int ah_num = 3)
{
	unordered_map<string, int> flowCount;
	input(ah, tuples, MAX_FLOW_NUM, flowCount, ah_num);

	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(3);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec;
	for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;

	DecodeCaches(AllRes, ahVec, parasVec); 

	unordered_map<string, vector<MyItem> > sum = AllRes[0].first;

	unordered_map<string, int> sspreader, sspreader_std;
	CountIP(sum, sspreader, thre, 0, 6);
	CountIP(flowCount, sspreader_std, thre, 0, 6);

	int tp = 0;
	for(unordered_map<string, int>::iterator it = sspreader_std.begin(); it != sspreader_std.end(); ++it)
		if(sspreader.find(it->first) != sspreader.end())
			tp++;

	DDoSPerform dp;
	dp.real = (int)sspreader_std.size();
	dp.get = (int)sspreader.size();
	dp.precision = tp * 1.0 / (int)sspreader.size();
	dp.recall = tp * 1.0 / (int)sspreader_std.size();
	return dp;
}

// Find super-spreader
DDoSPerform spreaderTest(AHeap* ah, vector<string>& tuples, int MAX_FLOW_NUM, int thre, int* packet_count = NULL){
	unordered_map<string, int> flowCount;
	int tmpCnt = input(ah, tuples, MAX_FLOW_NUM, flowCount);
	if(packet_count != NULL)	*packet_count = tmpCnt;

	unordered_map<string, vector<MyItem> > sum;
	MergeSingleCache(sum, *ah);

	unordered_map<string, int> sspreader, sspreader_std;
	CountIP(sum, sspreader, thre, 0, 6);
	CountIP(flowCount, sspreader_std, thre, 0, 6);

	int tp = 0;
	for(unordered_map<string, int>::iterator it = sspreader_std.begin(); it != sspreader_std.end(); ++it)
		if(sspreader.find(it->first) != sspreader.end())
			tp++;

	DDoSPerform dp;
	dp.real = (int)sspreader_std.size();
	dp.get = (int)sspreader.size();
	dp.precision = tp * 1.0 / (int)sspreader.size();
	dp.recall = tp * 1.0 / (int)sspreader_std.size();
	return dp;
}

// Find persistent flow with code mechanism
PersistPerform persistentTest(AHeap** ah, vector<string>& tuples, int PACKET_PER_W, int total_w, int thre, int ah_num){
  	// map1 for real persistent count, map2 for aheap
	unordered_map<string, int> map1, map2;

	int packet_cnt = 0;
	for(int cur_w = 0; cur_w < total_w; ++cur_w)
	{
		unordered_map<string, int> flowCount, sum;
		for(int i = 0; i < PACKET_PER_W; ++i)
		{
			char tmpKey[KEY_LEN];
			memcpy(tmpKey, tuples[packet_cnt + i].c_str(), KEY_LEN);
			// Insert items with aging mechanism
			for(int k = 0; k < ah_num; ++k)
				AHeapInsert_time_aging(*ah[k], ah[k]->Cacheid, tmpKey, KEY_LEN, 3, PPPP, i + packet_cnt, 15, PACKET_PER_W, 1);
			
			flowCount[string(tmpKey, 13)] += 1;
		}


		vector<int> parasVec;
		parasVec.push_back(3);	parasVec.push_back(3);	parasVec.push_back(13);	
		parasVec.push_back(15);	parasVec.push_back(4);		
		vector<AHeap> ahVec;
		for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
		vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
		
		DecodeCaches(AllRes, ahVec, parasVec); 
		unordered_map<string, vector<MyItem> >& tmpSet = AllRes[0].first;


		for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it)
			map1[it->first]++;
		for(unordered_map<string, vector<MyItem> >::iterator it = tmpSet.begin(); it != tmpSet.end(); ++it)
			for(int i = 0; i < (int)it->second.size(); ++i)
				if(it->second[i].my_value_time >= packet_cnt)
				{
					map2[it->first]++;
					break;
				}
		packet_cnt += PACKET_PER_W;
	}

	// Find persistent flow using AHeap
	unordered_map<string, int> pers1, pers2;
	for(unordered_map<string, int>::iterator it = map1.begin(); it != map1.end(); ++it)
		if(it->second >= thre)
			pers1[it->first] = it->second;
	// Find real persistent flow
	for(unordered_map<string, int>::iterator it = map2.begin(); it != map2.end(); ++it)
		if(it->second >= thre)
			pers2[it->first] = it->second;

	int tp = 0;
	for(unordered_map<string, int>::iterator it = pers1.begin(); it != pers1.end(); ++it)
		if(pers2.find(it->first) != pers2.end())
			tp++;

	PersistPerform pp;
	pp.precision = NONAN(tp * 1.0 / (int)pers2.size());
	pp.recall = NONAN(tp * 1.0 / (int)pers1.size());
	return pp;
}

// Find persistent flow
PersistPerform persistentTest(AHeap* ah, vector<string>& tuples, int PACKET_PER_W, int total_w, int thre){
  	//map1 for real persistent count, map2 for aheap
	unordered_map<string, int> map1, map2;

	int packet_cnt = 0;
	for(int cur_w = 0; cur_w < total_w; ++cur_w)
	{
		unordered_map<string, int> flowCount, sum;
		for(int i = 0; i < PACKET_PER_W; ++i)
		{
			char tmpKey[KEY_LEN];
			memcpy(tmpKey, tuples[packet_cnt + i].c_str(), KEY_LEN);
			MyItem flowitem;
			flowitem.initial_item(KEY_LEN, tmpKey, 1, packet_cnt + i);
			ah->insert(flowitem, tmpKey, KEY_LEN, PACKET_PER_W, 1);
			flowCount[string(tmpKey, 13)] += 1;
		}

		unordered_map<string, vector<MyItem> > tmpSet;
		MergeSingleCache(tmpSet, *ah);

		for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it)
			map1[it->first]++;
		for(unordered_map<string, vector<MyItem> >::iterator it = tmpSet.begin(); it != tmpSet.end(); ++it)
			for(int i = 0; i < (int)it->second.size(); ++i)
				if(it->second[i].my_value_time >= packet_cnt){
					map2[it->first]++;
					break;
				}
		packet_cnt += PACKET_PER_W;
	}

	unordered_map<string, int> pers1, pers2;
	for(unordered_map<string, int>::iterator it = map1.begin(); it != map1.end(); ++it)
		if(it->second >= thre)
			pers1[it->first] = it->second;
	for(unordered_map<string, int>::iterator it = map2.begin(); it != map2.end(); ++it)
		if(it->second >= thre)
			pers2[it->first] = it->second;

	int tp = 0;
	for(unordered_map<string, int>::iterator it = pers1.begin(); it != pers1.end(); ++it)
		if(pers2.find(it->first) != pers2.end())
			tp++;

	PersistPerform pp;
	pp.precision = NONAN(tp * 1.0 / (int)pers2.size());
	pp.recall = NONAN(tp * 1.0 / (int)pers1.size());
	return pp;
}



// Find flowlet with code mechanism
/* PACKET_PER_W: size of a time window */
FlowletPerform flowletTest(AHeap** ah, vector<string>& tuples, int PACKET_PER_W, int total_w, int ah_num = 3, int thre = 1)
{
	typedef std::pair<int, int> FLOWLET;

	unordered_map<string, vector<FLOWLET> > flowlet1, flowlet2;
	unordered_map<string, FLOWLET> f1, f2;

	for(int cur_w = 0; cur_w < total_w; ++cur_w)
	{
		unordered_map<string, int> flowCount;
		for(int i = 0; i < PACKET_PER_W; ++i)
		{
			char tmpKey[KEY_LEN];
			memcpy(tmpKey, tuples[PACKET_PER_W * cur_w + i].c_str(), KEY_LEN);

			for(int k = 0; k < ah_num; ++k)
				AHeapInsert_for_flowlet(*ah[k], ah[k]->Cacheid, tmpKey, KEY_LEN, 3, PPPP, PACKET_PER_W * cur_w + i, 15);
			flowCount[string(tmpKey, KEY_LEN)] = 1;
		}
		
	  	// Update standard flowlet
		for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it){
			if(f1.find(it->first) == f1.end())
				f1[it->first] = FLOWLET(cur_w, cur_w);
			else if(cur_w - f1[it->first].second <= thre)		// appear in last time window
				f1[it->first].second = cur_w;
			else{
				if(flowlet1.find(it->first) == flowlet1.end()){
					vector<FLOWLET> vec;
					vec.push_back(f1[it->first]);
					flowlet1[it->first] = vec;
				}
				else
					flowlet1[it->first].push_back(f1[it->first]);
				f1[it->first] = FLOWLET(cur_w, cur_w);
			}
		}

	  	// Update AHeap flowlet
		vector<int> parasVec;
		parasVec.push_back(3);	parasVec.push_back(3);	parasVec.push_back(13);	
		parasVec.push_back(15);	parasVec.push_back(4);		
		vector<AHeap> ahVec;
		for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
		vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
		
		DecodeCaches(AllRes, ahVec, parasVec); 
		unordered_map<string, vector<MyItem> >& tmpSet = AllRes[0].first;

		for(unordered_map<string, vector<MyItem> >::iterator it = tmpSet.begin(); it != tmpSet.end(); ++it){
			bool flag = true;
			for(int i = 0; i < (int)it->second.size(); ++i)
				if(it->second[i].my_value_time >= cur_w * PACKET_PER_W){
					flag = false;
					break;
				}
			if(flag)
				continue;


			if(f2.find(it->first) == f2.end())
				f2[it->first] = FLOWLET(cur_w, cur_w);
			else if(cur_w - f2[it->first].second <= thre)		// Appear in last time window
				f2[it->first].second = cur_w;
			else{
				if(flowlet2.find(it->first) == flowlet2.end()){
					vector<FLOWLET> vec;
					vec.push_back(f2[it->first]);
					flowlet2[it->first] = vec;
				}
				else
					flowlet2[it->first].push_back(f2[it->first]);
				f2[it->first] = FLOWLET(cur_w, cur_w);
			}
		}
	}

  	// Add flowlet in f1 and f2 to flowlet1 and flowlet2 
	for(unordered_map<string, FLOWLET>::iterator it = f1.begin(); it != f1.end(); ++it)
		if(flowlet1.find(it->first) == flowlet1.end()){
			vector<FLOWLET> vec;
			vec.push_back(it->second);
			flowlet1[it->first] = vec;
		}
		else
			flowlet1[it->first].push_back(it->second);
	for(unordered_map<string, FLOWLET>::iterator it = f2.begin(); it != f2.end(); ++it)
		if(flowlet2.find(it->first) == flowlet2.end()){
			vector<FLOWLET> vec;
			vec.push_back(it->second);
			flowlet2[it->first] = vec;
		}
		else
			flowlet2[it->first].push_back(it->second);


	int cnt = 0, real = 0, get = 0;
	for(unordered_map<string, vector<FLOWLET> >::iterator it = flowlet1.begin(); it != flowlet1.end(); ++it)
	{
		real += (int)it->second.size();
		if(flowlet2.find(it->first) == flowlet2.end())	continue;
		vector<FLOWLET>& ahFlowlet = flowlet2[it->first];

		for(int i = 0; i < (int)it->second.size(); ++i){
			for(int j = 0; j < (int)ahFlowlet.size(); ++j)
				if(it->second[i] == ahFlowlet[j])
					cnt++;
		}
	}
	for(unordered_map<string, vector<FLOWLET> >::iterator it = flowlet2.begin(); it != flowlet2.end(); ++it)
		get += (int)it->second.size();

	FlowletPerform fp;
	fp.precision = NONAN(cnt * 1.0 / get);
	fp.recall = NONAN(cnt * 1.0 / real);
	fp.f1 = NONAN(2 * fp.precision * fp.recall / (fp.precision + fp.recall));

	return fp;
}


// Find flowlet
/* PACKET_PER_W: size of a time window */
FlowletPerform flowletTest(AHeap* ah, vector<string>& tuples, int PACKET_PER_W, int total_w, int thre = 1)
{
	typedef std::pair<int, int> FLOWLET;

	unordered_map<string, vector<FLOWLET> > flowlet1, flowlet2;
	unordered_map<string, FLOWLET> f1, f2;

	for(int cur_w = 0; cur_w < total_w; ++cur_w)
	{
		unordered_map<string, int> flowCount;
		for(int i = 0; i < PACKET_PER_W; ++i)
		{
			char tmpKey[KEY_LEN];
			memcpy(tmpKey, tuples[PACKET_PER_W * cur_w + i].c_str(), KEY_LEN);
			MyItem flowitem;
			flowitem.initial_item(KEY_LEN, tmpKey, 1, PACKET_PER_W * cur_w + i);
			ah->insert_aging_by_time(flowitem, tmpKey, KEY_LEN);
			flowCount[string(tmpKey, KEY_LEN)] = 1;
		}
	
	  	// Update standard flowlet
		for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it){
			if(f1.find(it->first) == f1.end())
				f1[it->first] = FLOWLET(cur_w, cur_w);
			else if(cur_w - f1[it->first].second <= thre)		// Appear in last time window
				f1[it->first].second = cur_w;
			else{
				if(flowlet1.find(it->first) == flowlet1.end()){
					vector<FLOWLET> vec;
					vec.push_back(f1[it->first]);
					flowlet1[it->first] = vec;
				}
				else
					flowlet1[it->first].push_back(f1[it->first]);
				f1[it->first] = FLOWLET(cur_w, cur_w);
			}
		}

	  	// Update AHeap flowlet
		unordered_map<string, vector<MyItem> > tmpSet;
		MergeSingleCache(tmpSet, *ah);
		for(unordered_map<string, vector<MyItem> >::iterator it = tmpSet.begin(); it != tmpSet.end(); ++it){
			bool flag = true;
			for(int i = 0; i < (int)it->second.size(); ++i)
				if(it->second[i].my_value_time >= cur_w * PACKET_PER_W){
					flag = false;
					break;
				}
			if(flag)
				continue;


			if(f2.find(it->first) == f2.end())
				f2[it->first] = FLOWLET(cur_w, cur_w);
			else if(cur_w - f2[it->first].second <= thre)		// Appear in last time window
				f2[it->first].second = cur_w;
			else{
				if(flowlet2.find(it->first) == flowlet2.end()){
					vector<FLOWLET> vec;
					vec.push_back(f2[it->first]);
					flowlet2[it->first] = vec;
				}
				else
					flowlet2[it->first].push_back(f2[it->first]);
				f2[it->first] = FLOWLET(cur_w, cur_w);
			}
		}
	}

  	// Add flowlet in f1 and f2 to flowlet1 and flowlet2 
	for(unordered_map<string, FLOWLET>::iterator it = f1.begin(); it != f1.end(); ++it)
		if(flowlet1.find(it->first) == flowlet1.end()){
			vector<FLOWLET> vec;
			vec.push_back(it->second);
			flowlet1[it->first] = vec;
		}
		else
			flowlet1[it->first].push_back(it->second);
	for(unordered_map<string, FLOWLET>::iterator it = f2.begin(); it != f2.end(); ++it)
		if(flowlet2.find(it->first) == flowlet2.end()){
			vector<FLOWLET> vec;
			vec.push_back(it->second);
			flowlet2[it->first] = vec;
		}
		else
			flowlet2[it->first].push_back(it->second);

	int cnt = 0, real = 0, get = 0;
	for(unordered_map<string, vector<FLOWLET> >::iterator it = flowlet1.begin(); it != flowlet1.end(); ++it)
	{
		real += (int)it->second.size();
		if(flowlet2.find(it->first) == flowlet2.end())	continue;
		vector<FLOWLET>& ahFlowlet = flowlet2[it->first];

		for(int i = 0; i < (int)it->second.size(); ++i){
			for(int j = 0; j < (int)ahFlowlet.size(); ++j)
				if(it->second[i] == ahFlowlet[j])
					cnt++;
		}
	}
	for(unordered_map<string, vector<FLOWLET> >::iterator it = flowlet2.begin(); it != flowlet2.end(); ++it)
		get += (int)it->second.size();

	FlowletPerform fp;
	fp.precision = NONAN(cnt * 1.0 / get);
	fp.recall = NONAN(cnt * 1.0 / real);
	fp.f1 = NONAN(2 * fp.precision * fp.recall / (fp.precision + fp.recall));
	
	return fp;
}


// Find recent flow
HeavyHitterPerform rfTest(AHeap* ah, vector<string>& tuples, int threshold, int MAX_FLOW_NUM)
{
	unordered_map<string, int> flowCount;
	int packet_cnt = 0;

	for(int i = 0; i < (int)tuples.size(); ++i){
		char tmpKey[KEY_LEN];
		MyItem flowitem;

		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);
		flowitem.initial_item(KEY_LEN, tmpKey, 1, i);
		ah->insert_for_recent(flowitem, tmpKey, KEY_LEN, 1);
		flowCount[string(tmpKey, 13)]  = i;
		packet_cnt++;

		if(flowCount.size() >= MAX_FLOW_NUM)
			break;
	}

	unordered_map<string, vector<MyItem> > sum;
	MergeSingleCache(sum, *ah);

	vector<pair<int, string> > rf_1, rf_2;

	// Get the latest timestamp of each flow
	for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it)
		rf_1.push_back(pair<int, string>(it->second, it->first));
	for(unordered_map<string, vector<MyItem> >::iterator it = sum.begin(); it != sum.end(); ++it)
	{
		int max_value_time = 0;
		for(int i = 0; i < (int)it->second.size(); ++i)
			max_value_time = max_value_time >= it->second[i].my_value_time ? max_value_time : it->second[i].my_value_time;
		rf_2.push_back(pair<int, string>(max_value_time, it->first));		 
	}
	//Sort flows according to their the latest timestamps
	sort(rf_1.begin(), rf_1.end());
	sort(rf_2.begin(), rf_2.end());
	
	int tp = 0;
	for(int i = (int)rf_1.size() - 1; i >= (int)rf_1.size() - threshold && i >= 0; --i)
		for(int j = (int)rf_2.size() - 1; j >= (int)rf_2.size() - threshold && j >= 0; --j)
			if(rf_1[i].second == rf_2[j].second){
				tp++;
				break;
			}

	HeavyHitterPerform hhp;

	hhp.recall = tp * 1.0 / (threshold < (int)rf_1.size() ? threshold : (int)rf_1.size());
	hhp.precision = tp * 1.0 / (threshold < (int)rf_2.size() ? threshold : (int)rf_2.size());
	return hhp;
}

// Find recent flow with code mechanism 
HeavyHitterPerform rfTest(AHeap** ah, vector<string>& tuples, int threshold, int MAX_FLOW_NUM, int ah_num = 3)
{
	unordered_map<string, int> flowCount;
	int packet_cnt = 0;

	for(int i = 0; i < (int)tuples.size(); ++i){
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);
		for(int k = 0; k < ah_num; ++k)
			AHeapInsert_for_recent(*ah[k], ah[k]->Cacheid, tmpKey, KEY_LEN, 3, PPPP, i, 15);
		flowCount[string(tmpKey, 13)]  = i;
		packet_cnt++;

		if(flowCount.size() >= MAX_FLOW_NUM)
			break;
	}

	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(3);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec;
	for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
	
	DecodeCaches(AllRes, ahVec, parasVec); 
	unordered_map<string, vector<MyItem> >& sum = AllRes[0].first;
	

	vector<pair<int, string> > rf_1, rf_2;

	for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it)
		rf_1.push_back(pair<int, string>(it->second, it->first));
	for(unordered_map<string, vector<MyItem> >::iterator it = sum.begin(); it != sum.end(); ++it)
	{
		int max_value_time = 0;
		for(int i = 0; i < (int)it->second.size(); ++i)
			max_value_time = max_value_time >= it->second[i].my_value_time ? max_value_time : it->second[i].my_value_time;
		rf_2.push_back(pair<int, string>(max_value_time, it->first));		 
	}
	sort(rf_1.begin(), rf_1.end());
	sort(rf_2.begin(), rf_2.end());
	
	int tp = 0;
	for(int i = (int)rf_1.size() - 1; i >= (int)rf_1.size() - threshold && i >= 0; --i)
		for(int j = (int)rf_2.size() - 1; j >= (int)rf_2.size() - threshold && j >= 0; --j)
			if(rf_1[i].second == rf_2[j].second){
				tp++;
				break;
			}

	HeavyHitterPerform hhp;
	
	hhp.recall = tp * 1.0 / (threshold < (int)rf_1.size() ? threshold : (int)rf_1.size());
	hhp.precision = tp * 1.0 / (threshold < (int)rf_2.size() ? threshold : (int)rf_2.size());
	return hhp;
}

// FInd burst 
BurstPerform burstTest(AHeap* ah, vector<string>& tuples, int MAX_FLOW_NUM, int* packet_count = NULL)
{
	unordered_map<string, int> burst_1, burst_2;	// 1 for ah, 2 for standard

	/* first integer: last timestamp
	   counter for bursty field */
	typedef std::pair<int, int> BURST;	
	unordered_map<string, BURST> flowCount;
	int packet_cnt = 0;

	for(int i = 0; i < (int)tuples.size(); ++i){
		
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);
		string s = string(tmpKey, KEY_LEN);

		MyItem flowitem;
		flowitem.initial_item(KEY_LEN, tmpKey, 1, i);
		ah->insert_for_burst(flowitem, tmpKey, KEY_LEN);

		if(flowCount.find(s) == flowCount.end()){
			flowCount[s] = BURST(i, 1);
		}
		else{
			flowCount[s].first = i;
			flowCount[s].second++;
			if(flowCount[s].second == 8){
				burst_2[s]++;
				flowCount[s].second = 0;
			}
		}

		packet_cnt++;
		if(flowCount.size() >= MAX_FLOW_NUM)
			break;
	}
	if(packet_count != NULL)	*packet_count = packet_cnt;


	BurstPerform bp;
	int tp = 0, real = 0, get = 0;

	unordered_map<string, vector<MyItem> > sum;
	MergeSingleCache(sum, *ah);

	// Find burst using the AHeap
	for(unordered_map<string, vector<MyItem> >::iterator it = sum.begin(); it != sum.end(); ++it)
	{
		for(int i = 0; i < (int)it->second.size(); ++i)
			if(GET_BURSTY_FIELD(it->second[i].my_value_count) == _EIGHT){
				burst_1[it->first] = it->second[i].my_value_time;
				break;
			}
	}
	real = (int)burst_2.size();
	get = (int)burst_1.size();

	for(unordered_map<string, int>::iterator it = burst_2.begin(); it != burst_2.end(); ++it)
		if(burst_1.find(it->first) != burst_1.end())
			tp++;


	bp.precision = NONAN(tp * 1.0 / get);
	bp.recall = NONAN(tp * 1.0 / real);
	bp.f1 = NONAN(2 * bp.precision * bp.recall / (bp.precision + bp.recall));
	printf("real:%d, get:%d, tp:%d\n", real, get, tp);
	return bp;
}

// Find burst with code mechanism
BurstPerform burstTest(AHeap** ah, vector<string>& tuples, int MAX_FLOW_NUM, int ah_num)
{
	unordered_map<string, int> burst_1, burst_2;	// 1 for ah, 2 for standard

	/* first integer: last timestamp
	   counter for bursty field */
	typedef std::pair<int, int> BURST;	
	unordered_map<string, BURST> flowCount;	
	int packet_cnt = 0;

	for(int i = 0; i < (int)tuples.size(); ++i){
		
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);
		string s = string(tmpKey, KEY_LEN);

		MyItem flowitem;
		flowitem.initial_item(KEY_LEN, tmpKey, 1, i);
		
		for(int k = 0; k < ah_num; ++k)
			AHeapInsert_for_burst(*ah[k], ah[k]->Cacheid, tmpKey, KEY_LEN, 3, PPPP, i, 15);

		if(flowCount.find(s) == flowCount.end()){
			flowCount[s] = BURST(i, 1);
		}
		else{
			flowCount[s].first = i;
			flowCount[s].second++;
			if(flowCount[s].second == 8){
				burst_2[s]++;
				flowCount[s].second = 0;
			}
		}

		packet_cnt++;
		if(flowCount.size() >= MAX_FLOW_NUM)
			break;
	}


	BurstPerform bp;
	int tp = 0, real = 0, get = 0;

	vector<int> parasVec;
	parasVec.push_back(3);	parasVec.push_back(3);	parasVec.push_back(13);	
	parasVec.push_back(15);	parasVec.push_back(4);		
	vector<AHeap> ahVec;
	for(int i = 0; i < ah_num; ++i)		ahVec.push_back(*ah[i]);
	vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;

	DecodeCaches(AllRes, ahVec, parasVec); 
	unordered_map<string, vector<MyItem> >& sum = AllRes[0].first;

	for(unordered_map<string, vector<MyItem> >::iterator it = sum.begin(); it != sum.end(); ++it)
	{
		for(int i = 0; i < (int)it->second.size(); ++i)
			if(GET_BURSTY_FIELD(it->second[i].my_value_count) == _EIGHT){
				burst_1[it->first] = it->second[i].my_value_time;
				break;
			}
	}
	real = (int)burst_2.size();
	get = (int)burst_1.size();

	for(unordered_map<string, int>::iterator it = burst_2.begin(); it != burst_2.end(); ++it)
		if(burst_1.find(it->first) != burst_1.end())
			tp++;


	bp.precision = NONAN(tp * 1.0 / get);
	bp.recall = NONAN(tp * 1.0 / real);
	bp.f1 = NONAN(2 * bp.precision * bp.recall / (bp.precision + bp.recall));
	printf("real:%d, get:%d, tp:%d\n", real, get, tp);
	return bp;
}


#endif
