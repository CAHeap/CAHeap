#ifndef _COMMON_UTIL_H_
#define _COMMON_UTIL_H_


#include <vector>
#include <unordered_map>
#include "../srcAHeap/FlowType.h"


void Merge(unordered_map<string, vector<MyItem> >& sum, unordered_map<string, int>& m){
	for(unordered_map<string, vector<MyItem> >::iterator it = sum.begin(); it != sum.end(); ++it)
		for(int i = 0; i < it->second.size(); ++i)
			m[it->first] += it->second[i].my_value_count;
}




#endif