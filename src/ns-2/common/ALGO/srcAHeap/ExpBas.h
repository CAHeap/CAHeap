#ifndef EXPBAS_H
#define EXPBAS_H

#include <unordered_map>
#include <set>
#include <ctime>
#include <vector>
#include <string>
#include <stdio.h>
#include <malloc.h>
#include <algorithm>
#include <iterator>
#include "FlowCache.h"

#define MAX_INSERT_PACKAGE  110000

using namespace std;



class Experiment{
public:
    unordered_map<string, int> ground_truth;

    vector<string> insert_data;
public:
    Experiment(){
        insert_data.clear();
    }
    int load_data(const char *filename) {
        FILE *pf = fopen(filename, "rb");
        if (!pf) {
            cerr << filename << " not found." << endl;
            exit(-1);
        }
        ground_truth.clear();
        char ip[13];
        int ret = 0;
        while (fread(ip, 1, 13, pf)) {
            string str_ip(ip, 13);
            insert_data.push_back(str_ip);
            ground_truth[str_ip]++;
            ret++;
            if (ret == MAX_INSERT_PACKAGE)
                break;
        }
        fclose(pf);


        printf("load %d items, %lu distinct items\n", ret, ground_truth.size());

        return ret;
    }

};

#endif