#ifndef _COMMON_UTIL_H_
#define _COMMON_UTIL_H_


#include <vector>
#include <unordered_map>
#include <set>
#include <math.h>
#include "../AHeap/Flow.h"

#define NONAN(x) (std::isnan(x) ? 0 : x)

// Merge values of counters of a flow from different levels
void Merge(unordered_map<string, vector<MyItem> >& sum, unordered_map<string, int>& m){
	for(unordered_map<string, vector<MyItem> >::iterator it = sum.begin(); it != sum.end(); ++it)
		for(int i = 0; i < it->second.size(); ++i)
			m[it->first] += it->second[i].my_value_count;
}

// Get flow size distribution after merging the values of the counters
void Transform2Fsd(unordered_map<string, int>& map, unordered_map<int, int>& fsd){
	for(unordered_map<string, int>::iterator it = map.begin(); it != map.end(); ++it)
		fsd[it->second] += 1;
}

// Get ddos/super-spreader by counting destination/source IP addresses 
void CountIP(unordered_map<string, int>& m, unordered_map<string, int>& ddos, int thre, int key_start = 0, int val_start = 6)
{
	unordered_map<string, vector<string> > tmpMap;

	for(unordered_map<string, int>::iterator it = m.begin(); it != m.end(); ++it){
		char tuple[KEY_LEN];
		memcpy(tuple, it->first.c_str(), KEY_LEN);
		
		string key(tuple + key_start, 4);
		string val(tuple + val_start, 4);

		if(tmpMap.find(key) == tmpMap.end()){
			vector<string> vec;
			vec.push_back(val);
			tmpMap[key] = vec;
		}
		else
			tmpMap[key].push_back(val);
	}

	for(unordered_map<string, vector<string> >::iterator it = tmpMap.begin(); it != tmpMap.end(); ++it){
		set<string> s;
		for(int i = 0; i < (int)it->second.size(); ++i)
			s.insert(it->second[i]);
		if((int)s.size() >= thre){
			ddos[it->first] = (int)s.size();
		}
	}
}

// Get ddos/super-spreader after merging
void CountIP(unordered_map<string, vector<MyItem> >& sum, unordered_map<string, int>& ddos, int thre, int key_start = 0, int val_start = 6)
{
	unordered_map<string, int> m;
	Merge(sum, m);

	CountIP(m, ddos, thre, key_start, val_start);
}

#endif