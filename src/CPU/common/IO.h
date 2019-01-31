#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
using std::vector;
using std::map;

// #define SUMMARY

// Read tuples from file
template<int len>
void Read5Tuples(const char* srcFile, vector<string>& tuples){

	int cnt = 0;
	string item;
	char buf[len];

	FILE* fin = fopen(srcFile, "rb");

#ifdef SUMMARY
	unordered_map<string, int> sum1;
#endif

	while(fread(buf, 1, len, fin) == len){
		cnt++;
		tuples.push_back(string(buf, len));

#ifdef SUMMARY
		string s(buf, len);
		sum1[s]++;
#endif
	}
	fclose(fin);
	
#ifdef SUMMARY
	printf("%lu distinct flows\n", sum1.size());
#endif
}

template<int len>
void ReadItems(const char* srcFile, vector<pair <string, uint32_t>>& tuples){
	FILE* fin = fopen(srcFile, "rb");
	int cnt = 0;
	string item;
	char buf[len];
	uint32_t* timestamp;
	pair <string, uint32_t> tmp;

#ifdef SUMMARY
	unordered_map<string, int> sum1;
#endif

	while(fread(buf, 1, len, fin) == len){
		if (fread(timestamp, 1, 4, fin) == 4)
		{
			tmp.first = string(buf, len);
			tmp.second = *timestamp;
			tuples.push_back(tmp);
			cnt++;
		}
		

#ifdef SUMMARY
		string s(buf, len);
		sum1[s]++;
		if((int)sum1.size() > 16000)
			break;
#endif
	}
	fclose(fin);

#ifdef SUMMARY
	printf("%lu distinct flows\n", sum1.size());
	map<int, vector<string> > sum2;
	for(unordered_map<string, int>::iterator it = sum1.begin(); it != sum1.end(); ++it){
		if(sum2.find(it->second) == sum2.end()){
			vector<string> tmpVec;
			tmpVec.push_back(it->first);
			sum2[it->second] = tmpVec;
		}
		else
			sum2[it->second].push_back(it->first);
	}
	for(map<int, vector<string> >::iterator it = sum2.begin(); it != sum2.end(); ++it){
		printf("%d, %lu\n", it->first, it->second.size());
	}
#endif
}






#endif