#ifndef UTIL_H
#define UTIL_H

#include <unordered_map>
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include "stdio.h"
#include <math.h>
#include "FlowType.h"
#include "ec.h"


using namespace std;


bool comp(pair<string , int> a, pair<string , int> b){
    return a.second > b.second;
}

void find_topk(unordered_map<string , int> &my_map, set<string > &my_set, int topK){
    vector<pair<string , int>> elems(my_map.begin(), my_map.end());
    sort(elems.begin(), elems.end(), comp);
    for(int i = 0; i < topK; i++){
        my_set.insert(elems[i].first);
    }
}

void find_heavy(unordered_map<string, int> &my_map, set<string > &my_set, int th){
    vector<pair<string , int>> elems(my_map.begin(), my_map.end());
    sort(elems.begin(), elems.end(), comp);
    for(int i = 0; i < elems.size(); i++){
        if(elems[i].second > th){
            my_set.insert(elems[i].first);
        }else{
            break;
        }

    }
}

void accuracy_result(set<string> true_s, set<string> find_s, FILE *pout, char* header){
    int true_num = 0;
    int false_num = 0;
    for(auto itr: find_s){
        if(true_s.find(itr) != true_s.end()){
            true_num ++;
        }
        else{
            false_num ++;
        }
    }

    cout << endl << "*****************Accuracy*****************" << endl;
    cout << "Accuracy: " << ((double) true_num)/true_s.size() << endl;
    cout << "****************************************" << endl;

    fprintf(pout, "%s,%f\n", header, ((double) true_num)/true_s.size());


}


double compute_accuracy(unordered_map<string, int> &true_map, unordered_map<string, int> &query_map, double err){
    double is_num = 0;
    double not_num = 0;
    for(auto itr:true_map){
        if(query_map.find(itr.first) != query_map.end()){
            if(fabs(query_map[itr.first]- itr.second) <= err){
                is_num ++;
            }else{
                not_num ++;
            }
        }else{
            not_num ++;
        }
    }
    return is_num/(is_num+not_num);
}

void MergeSingleCache(unordered_map<string, int> &un_map, FlowCache &my_cache){
    for(int i = 0; i < my_cache.bucket_num; i++){
        uint32_t now_count = my_cache.MyCache[i].my_value_count;
        if(now_count != 0){
            string key(my_cache.MyCache[i].my_key, my_cache.key_len);
            if(un_map.find(key) == un_map.end()){
                un_map[key] = now_count;
            }else{
                un_map[key] += now_count;
            }

        }
    }
}

void MergeSingleCache(unordered_map<string, vector<MyItem>> &un_map, FlowCache &my_cache ){
    for(int i = 0; i < my_cache.bucket_num; i++){
        uint32_t now_count = my_cache.MyCache[i].my_value_count;
        if(now_count != 0){
            string key(my_cache.MyCache[i].my_key, my_cache.key_len);
            if(un_map.find(key) == un_map.end()){
                vector<MyItem> tmpVector;
                tmpVector.push_back(my_cache.MyCache[i]);
                un_map[key] = tmpVector;
            }else{
                un_map[key].push_back(my_cache.MyCache[i]);
            }

        }
    }
}

bool lesssortbyTimestamp(MyItem item_a, MyItem item_b)
{
    return (item_a.my_value_time > item_b.my_value_time);
}


void FlowCacheInsert(FlowCache& MyCache, int s_id, char* key, int key_len, int k, int p, uint32_t time_s, int full_len){
    char *full_key;
    full_key = new char[full_len];
    memcpy(full_key, key, key_len);
    int key_index = 0;
    for(int i = key_len; i < full_len; i++){
        full_key[i] = key[key_index];
        key_index = (key_index + 1)%key_len;
    }
    char *seg;
    seg = (char *)malloc(full_len/k);
    int seg_len = full_len/k;
    encode(s_id, full_key, seg, k, p, full_len/k);
    MyItem flow_item;
    flow_item.initial_item(seg_len, seg, 1, time_s);
    MyCache.insert(flow_item, key, key_len);
    delete full_key;
}

void FlowCacheInsert_time_aging(FlowCache& MyCache, int s_id, char* key, int key_len, int k, int p, uint32_t time_s, int full_len, int time_thre, int counter_thre){
    char *full_key;
    full_key = new char[full_len];
    memcpy(full_key, key, key_len);
    int key_index = 0;
    for(int i = key_len; i < full_len; i++){
        full_key[i] = key[key_index];
        key_index = (key_index + 1)%key_len;
    }
    char *seg;
    seg = (char *)malloc(full_len/k);
    int seg_len = full_len/k;
    encode(s_id, full_key, seg, k, p, full_len/k);
    MyItem flow_item;
    flow_item.initial_item(seg_len, seg, 1, time_s);
    MyCache.insert(flow_item, key, key_len, time_thre, counter_thre);
    delete full_key;
}

void FlowCacheInsert_for_flowlet(FlowCache& MyCache, int s_id, char* key, int key_len, int k, int p, uint32_t time_s, int full_len){
    char *full_key;
    full_key = new char[full_len];
    memcpy(full_key, key, key_len);
    int key_index = 0;
    for(int i = key_len; i < full_len; i++){
        full_key[i] = key[key_index];
        key_index = (key_index + 1)%key_len;
    }
    char *seg;
    seg = (char *)malloc(full_len/k);
    int seg_len = full_len/k;
    encode(s_id, full_key, seg, k, p, full_len/k);
    MyItem flow_item;
    flow_item.initial_item(seg_len, seg, 1, time_s);
    MyCache.insert_aging_by_time(flow_item, key, key_len);
    delete full_key;
}

void FlowCacheInsert_for_recent(FlowCache& MyCache, int s_id, char* key, int key_len, int k, int p, uint32_t time_s, int full_len){
    char *full_key;
    full_key = new char[full_len];
    memcpy(full_key, key, key_len);
    int key_index = 0;
    for(int i = key_len; i < full_len; i++){
        full_key[i] = key[key_index];
        key_index = (key_index + 1)%key_len;
    }
    char *seg;
    seg = (char *)malloc(full_len/k);
    int seg_len = full_len/k;
    encode(s_id, full_key, seg, k, p, full_len/k);
    MyItem flow_item;
    flow_item.initial_item(seg_len, seg, 1, time_s);
    MyCache.insert_for_recent(flow_item, key, key_len, 1);
    delete full_key;
}

void FlowCacheInsert_for_burst(FlowCache& MyCache, int s_id, char* key, int key_len, int k, int p, uint32_t time_s, int full_len){
    char *full_key;
    full_key = new char[full_len];
    memcpy(full_key, key, key_len);
    int key_index = 0;
    for(int i = key_len; i < full_len; i++){
        full_key[i] = key[key_index];
        key_index = (key_index + 1)%key_len;
    }
    char *seg;
    seg = (char *)malloc(full_len/k);
    int seg_len = full_len/k;
    encode(s_id, full_key, seg, k, p, full_len/k);
    MyItem flow_item;
    flow_item.initial_item(seg_len, seg, 1, time_s);
    MyCache.insert_for_burst(flow_item, key, key_len);
    delete full_key;
}

void combine2( int n, int m, vector<int> b, vector<vector<int>> &ret, int i_c) {
    for (int i = n; i >= m; i--) {
        b[m - 1] = i - 1;
        if (m > 1) {
            combine2(i - 1, m - 1, b, ret, i_c);
        } else {
            bool i_c_in = false;
            for (int i = b.size() - 1; i >= 0; i--) {
                if(b[i] == i_c){
                    i_c_in = true;
                }
            }
            if(i_c_in == false){
                ret.push_back(b);
            }
        }
    }
}

int my_pow(int a, int b){
    if(b == 0){
        return 1;
    }
    if(b == 1){
        return a;
    }else{
        return a * my_pow(a, b-1);
    }
}

bool comp_cache(const FlowCache &a, const FlowCache &b){
    return a.Cacheid < b.Cacheid;
}



void _DecodeCaches(vector<FlowCache> &un_maps, vector<FlowCache> &caches, vector<int> &paras){
    int k = paras[0];           //e.g. 3
    int p = paras[1];           //e.g. 2
    int actual_len = paras[2];  //e.g. 13
    int full_len = paras[3];    //e.g. 15
    int level_num = paras[4];   //e.g. 4


    int *level_start = new int[level_num+1];
    memcpy(level_start, caches[0].level_start, level_num * sizeof(int));
    level_start[level_num] = caches[0].bucket_num;

    vector<vector<int>> choose_caches; choose_caches.clear();
    sort(caches.begin(), caches.end(), comp_cache);

    un_maps.clear();
    for(int i = 0; i < caches.size(); i++){
        FlowCache tmp(caches[i].min_bucket_num, caches[i].item_size, caches[i].Cacheid);
        tmp.set_keylen(actual_len);
        un_maps.push_back(tmp);
    }

    for(int i_l = 0; i_l < level_num; i_l ++){
        for(int i_c = 0; i_c < caches.size(); i_c ++){
            vector<int> tmp_v; tmp_v.clear();
            for(int vi = 0; vi < k - 1; vi++){
                tmp_v.push_back(0);
            }

            choose_caches.clear();
            combine2(caches.size(), k-1, tmp_v, choose_caches,i_c);
            for(int i = level_start[i_l]; i < level_start[i_l + 1]; i ++){  //choose first item

                vector<int> places; places.clear();
                int place_t = i - level_start[i_l];
                for(int level_i = i_l; level_i < level_num; level_i++){
                    places.push_back(place_t + level_start[level_i]);
                    place_t = place_t / 2;
                }

                vector<int> choose_place;
                for(int choose_place_i = 0; choose_place_i < k-1; choose_place_i++){
                    choose_place.push_back(0);
                }



                for(int a = 0; a < choose_caches.size(); a++){
                    vector<int> actual_caches; actual_caches.clear();
                    actual_caches.push_back(caches[i_c].Cacheid);
                    for(int index_ac = 0; index_ac < choose_caches[a].size(); index_ac++){
                        actual_caches.push_back(caches[choose_caches[a][index_ac]].Cacheid);
                    }
                    int* frag_err_list = new int[p];
                    int frag_index=0;
                    for(int f = 0; f < (k+p); f++){
                        if((find(actual_caches.begin(), actual_caches.end(), f)==actual_caches.end()))
                        {
                            frag_err_list[frag_index] = f;
                            frag_index ++;
                        }
                    }
                    int b = my_pow(places.size(), k-1);
                    for(int c = 0; c < b; c++){
                        for(int d = 0; d < k-1; d++){
                            choose_place[d] = (c%my_pow(places.size(), d+1))/my_pow(places.size(), d);

                        }

                        bool if_valid = true;
                        for(int d = 0; d < k-1;d++){
                            if(choose_caches[a][d] < i_c){
                                if(choose_place[d] == 0){
                                    if_valid = false;
                                }
                            }
                        }
                        if(if_valid){
                            char **recover_srcs = new char* [k];
                            memset(recover_srcs, 0, sizeof(char*) * k);

                            bool if_already_ic = false;
                            int srcs_index = 0;

                            bool if_empty = false;


                            for(int g = 0; g < k-1; g++) {
                                if ((!if_already_ic) && (choose_caches[a][g] > i_c)) {

                                    if(caches[i_c].MyCache[i].my_value_count != 0){
                                        recover_srcs[srcs_index] = new char[full_len/k];
                                        memcpy(recover_srcs[srcs_index], caches[i_c].MyCache[i].my_key, full_len/k);
                                    }else{
                                        if_empty = true;
                                    }
                                    srcs_index++;

                                    if_already_ic = true;

                                    if(caches[choose_caches[a][g]].MyCache[places[choose_place[g]]].my_value_count != 0){
                                        recover_srcs[srcs_index] = new char[full_len/k];
                                        memcpy(recover_srcs[srcs_index], caches[choose_caches[a][g]].MyCache[places[choose_place[g]]].my_key, full_len/k);
                                    }else{
                                        if_empty = true;
                                    }
                                    srcs_index++;
                                } else {
                                    if(caches[choose_caches[a][g]].MyCache[places[choose_place[g]]].my_value_count != 0){
                                        recover_srcs[srcs_index] = new char[full_len/k];
                                        memcpy(recover_srcs[srcs_index], caches[choose_caches[a][g]].MyCache[places[choose_place[g]]].my_key, full_len/k);
                                    }else{
                                        if_empty = true;
                                    }
                                    srcs_index++;
                                }

                            }

                            if(if_already_ic == false){
                                if(caches[i_c].MyCache[i].my_value_count != 0){
                                    recover_srcs[srcs_index] = new char[full_len/k];
                                    memcpy(recover_srcs[srcs_index], caches[i_c].MyCache[i].my_key, full_len/k);
                                }else{
                                    if_empty = true;
                                }

                            }

                            if(if_empty) {continue;}

                            char* recover_key = new char[full_len];

                            decode(recover_key, frag_err_list, p, (u8 **)recover_srcs, k, p, full_len/k);
                            char* real_key = new char[actual_len];
                            memcpy(real_key, recover_key, actual_len);

                            if(caches[i_c].check(i, real_key, actual_len)){
                                string key(real_key, actual_len);

                                MyItem Item_t;
                                uint32_t in_count;
                                uint32_t in_time_s;

                                int cache_index = i_c;

                                in_count = caches[cache_index].MyCache[i].my_value_count;
                                in_time_s = caches[cache_index].MyCache[i].my_value_time;
                                Item_t.initial_item(actual_len, key.c_str(), in_count, in_time_s);

                                un_maps[cache_index].MyCache[i].initial_item(actual_len, real_key, in_count, in_time_s);


                                for(int g = 0; g < k - 1; g++){
                                    cache_index = choose_caches[a][g];

                                    in_count = caches[cache_index].MyCache[places[choose_place[g]]].my_value_count;
                                    in_time_s = caches[cache_index].MyCache[places[choose_place[g]]].my_value_time;
                                    Item_t.initial_item(actual_len, key.c_str(), in_count, in_time_s);

                                    un_maps[cache_index].MyCache[places[choose_place[g]]].initial_item(actual_len, real_key, in_count, in_time_s);
                                }

                            }
                            for(int cccc = 0; cccc < k; ++cccc)
                                if(recover_srcs[cccc] != NULL)
                                    delete recover_srcs[cccc];
                            delete recover_srcs;

                            delete recover_key;
                            delete real_key;
                        }
                    }
                    delete frag_err_list;
                }

            }
        }
    }

    delete level_start;
}

void DecodeCaches(vector<pair<unordered_map<string, vector<MyItem>>, int>> &un_maps, vector<FlowCache> &caches, vector<int> &paras){
    un_maps.clear();
    vector<FlowCache> ret_caches; ret_caches.clear();
    _DecodeCaches(ret_caches, caches, paras);
    for(int i = 0; i < ret_caches.size(); i++){
        unordered_map<string, vector<MyItem>> tmp; tmp.clear();
        MergeSingleCache(tmp, ret_caches[i]);
        un_maps.push_back(make_pair(tmp, ret_caches[i].Cacheid));
    }

}

#endif
