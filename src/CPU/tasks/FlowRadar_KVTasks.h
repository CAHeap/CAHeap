#ifndef _FLOW_RADAR_KV_TASK_H_
#define _FLOW_RADAR_KV_TASK_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include "../common/metric.h"
#include "../common/util.h"
#include "../FlowRadar/insertable_iblt.h"
#include "../AHeap/Util.h"
#include "FlowRadar_CountingTasks.h"


HeavyHitterPerform burstTest(InsertableIBLT* iblt, vector<string>& tuples, int threshold, int MAX_FLOW_NUM, int window_size, vector<string> &real)
{
	unordered_map<string, int> sum;
	unordered_map<string, int> flowCount;
	unordered_map<string, pair<int, int>> bf;
	pair<int, int> tmpTime(0, 0); // last window, counter
	HeavyHitterPerform hhp;
	int tp = 0;
	int cur_win = 0, packet_cnt = 0;

	
	//Collect flow informtion: query while insert items
	for (int i = 0; i < (int)tuples.size(); ++i)
	{
		char insrt_key[KEY_LEN];
		packet_cnt++;
		memcpy(insrt_key, tuples[i].c_str(), KEY_LEN);
		iblt->insert(insrt_key);
		flowCount[string(insrt_key, 13)] += 1;
		string query_key;

		if (packet_cnt >= window_size)
		{
			sum.clear();	
			iblt->dump(sum);
			for (unordered_map<string, int>::iterator it = sum.begin(); it != sum.end(); it++)
			{
				//query_key = string(it->first, KEY_LEN);
				query_key = it->first;
				if (bf.find(query_key) != bf.end())
				{
					tmpTime = bf[query_key];
					if (tmpTime.second < window_thre)
					{
						if (cur_win - tmpTime.first == 1)
						{
							tmpTime.second++;
						}
						else
						{
							if (cur_win != tmpTime.first)
							{
								tmpTime.second = 1;
							}
						}
					}
					tmpTime.first = cur_win;
					bf[query_key] = tmpTime;
				}
				else
				{
					tmpTime.first = cur_win;
					tmpTime.second = 1;
					bf[query_key] = tmpTime;
				}

				
			}
			iblt->clear();
			packet_cnt = 0;
			cur_win++;
		}
		if ((int)flowCount.size() > MAX_FLOW_NUM)
		{
			break;
		}
	}

	//Find burst in the FlowRadar
	vector<string> burstflow;
	for (unordered_map<string, pair<int, int>>::iterator it = bf.begin(); it != bf.end(); it++)
	{
		tmpTime = it->second;
		if (tmpTime.second >= threshold)
		{
			burstflow.push_back(it->first);
		}
	}
	

	//Check
	for (vector<string>::iterator it = burstflow.begin(); it != burstflow.end(); it++)
	{
		for (int i = 0; i < (int)real.size(); ++i)
		{
			if (!memcmp(real[i].c_str(), (*it).c_str(), KEY_LEN))
			{
				tp++;
			}
		}
	}

	hhp.recall = (double) tp/(int)real.size();
	hhp.precision = (double) tp/(int)bf.size();
	return hhp;
}


/* DDoS */
DDoSPerform ddosTest(InsertableIBLT* iblt, vector<string>& tuples, int MAX_FLOW_NUM, int thre){
	unordered_map<string, int> flowCount;
	int packet_cnt = input(iblt, tuples, MAX_FLOW_NUM, flowCount);

	unordered_map<string, int> sum;
	iblt->dump(sum);

	unordered_map<string, int> ddos, ddos_std;
	CountIP(sum, ddos, thre, 6, 0);
	CountIP(flowCount, ddos_std, thre, 6, 0);

	int tp = 0;
	for(unordered_map<string, int>::iterator it = ddos_std.begin(); it != ddos_std.end(); ++it)
		if(ddos.find(it->first) != ddos.end()){
			tp++;
			// printf("我的天总算找到一个！\n");
		}


	DDoSPerform dp;
	dp.real = (int)ddos_std.size();
	dp.get = (int)ddos.size();
	dp.precision = tp * 1.0 / (int)ddos.size();
	dp.recall = tp * 1.0 / (int)ddos_std.size();
	// printf("real:%d, get:%d, precision:%lf, recall:%lf\n", dp.real, dp.get, dp.precision, dp.recall);
	return dp;
}


/* spreader */
DDoSPerform spreaderTest(InsertableIBLT* iblt, vector<string>& tuples, int MAX_FLOW_NUM, int thre){
	unordered_map<string, int> flowCount;
	int packet_cnt = input(iblt, tuples, MAX_FLOW_NUM, flowCount);

	unordered_map<string, int> sum;
	iblt->dump(sum);

	unordered_map<string, int> ddos, ddos_std;
	CountIP(sum, ddos, thre, 0, 6);
	CountIP(flowCount, ddos_std, thre, 0, 6);

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
	// printf("real:%d, get:%d, precision:%lf, recall:%lf\n", dp.real, dp.get, dp.precision, dp.recall);
	return dp;
}



HeavyHitterPerform rfTest(InsertableIBLT* iblt, vector<string>& tuples, int threshold, int MAX_FLOW_NUM)
{
	unordered_map<string, int> flowCount;
	int packet_cnt = 0;

	for(int i = 0; i < (int)tuples.size(); ++i){
		char tmpKey[KEY_LEN];
		memcpy(tmpKey, tuples[i].c_str(), KEY_LEN);

		iblt->insert(tmpKey);

		flowCount[string(tmpKey, 13)] = i;

		packet_cnt++;

		if(flowCount.size() >= MAX_FLOW_NUM)
			break;
	}

	unordered_map<string, int> sum;
	iblt->dump(sum);

	vector<pair<int, string> > rf;
	for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it)
		rf.push_back(pair<int, string>(it->second, it->first));
	sort(rf.begin(), rf.end());

	HeavyHitterPerform hhp;
	int tp = 0;
	for(int i = (int)rf.size() - 1; i >= (int)rf.size() - threshold && i >= 0; --i)
		if(sum.find(rf[i].second) != sum.end())
			tp++;

	hhp.recall = tp * 1.0 / (threshold < (int)rf.size() ? threshold : (int)rf.size());
	hhp.precision = tp * 1.0 / (int)sum.size();
	return hhp;
}



PersistPerform persistentTest(InsertableIBLT* iblt, vector<string>& tuples, int PACKET_PER_W, int total_w, int thre){
  /* map1 for real persistent count, map2 for fc */
	unordered_map<string, int> map1, map2;

	int packet_cnt = 0;
	for(int cur_w = 0; cur_w < total_w; ++cur_w)
	{
		// iblt->clear();
		unordered_map<string, int> flowCount, sum;
		for(int i = 0; i < PACKET_PER_W; ++i)
		{
			char tmpKey[KEY_LEN];
			memcpy(tmpKey, tuples[packet_cnt + i].c_str(), KEY_LEN);
			iblt->insert(tmpKey);
			flowCount[string(tmpKey, 13)] += 1;
		}
		packet_cnt += PACKET_PER_W;

		iblt->dump(sum);

		for(unordered_map<string, int>::iterator it = flowCount.begin(); it != flowCount.end(); ++it)
			map1[it->first]++;
		for(unordered_map<string, int>::iterator it = sum.begin(); it != sum.end(); ++it)
			map2[it->first]++;
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



/* PACKET_PER_W: size of a time window */
FlowletPerform flowletTest(InsertableIBLT* iblt, vector<string>& tuples, int PACKET_PER_W, int total_w, int thre = 1)
{
	typedef std::pair<int, int> FLOWLET;

	unordered_map<string, vector<FLOWLET> > flowlet1, flowlet2;
	unordered_map<string, FLOWLET> f1, f2;

	for(int cur_w = 0; cur_w < total_w; ++cur_w)
	{
		unordered_map<string, int> flowCount, sum;
		for(int i = 0; i < PACKET_PER_W; ++i)
		{
			char tmpKey[KEY_LEN];
			memcpy(tmpKey, tuples[PACKET_PER_W * cur_w + i].c_str(), KEY_LEN);
			iblt->insert(tmpKey);
			flowCount[string(tmpKey, KEY_LEN)] = 1;
		}
	
	  /* update standard flow let */
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

	  /* update AHeap flowlet */
		iblt->dump(sum);
		for(unordered_map<string, int>::iterator it = sum.begin(); it != sum.end(); ++it){
			if(f2.find(it->first) == f2.end())
				f2[it->first] = FLOWLET(cur_w, cur_w);
			else if(cur_w - f2[it->first].second <= thre)		// appear in last time window
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

  /* add flowlet in f1 and f2 to flowlet1 and flowlet2 */
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

  /* compare */
	int cnt = 0, real = 0, get = 0;
	for(unordered_map<string, vector<FLOWLET> >::iterator it = flowlet1.begin(); it != flowlet1.end(); ++it)
	{
		real += (int)it->second.size();
		if(flowlet2.find(it->first) == flowlet2.end())	continue;
		vector<FLOWLET>& fcFlowlet = flowlet2[it->first];

		for(int i = 0; i < (int)it->second.size(); ++i){
			for(int j = 0; j < (int)fcFlowlet.size(); ++j)
				if(it->second[i] == fcFlowlet[j])
					cnt++;
		}
	}
	for(unordered_map<string, vector<FLOWLET> >::iterator it = flowlet2.begin(); it != flowlet2.end(); ++it)
		get += (int)it->second.size();

	FlowletPerform fp;
	fp.precision = NONAN(cnt * 1.0 / get);
	fp.recall = NONAN(cnt * 1.0 / real);
	fp.f1 = NONAN(2 * fp.precision * fp.recall / (fp.precision + fp.recall));
	// printf("real=%d, get=%d, cnt=%d\n", real, get, cnt);
	return fp;
}

#endif