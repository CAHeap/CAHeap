#ifndef _FLOWCACHE_H
#define _FLOWCACHE_H
#include "../common/BOBHash32.h"
#include "FlowType.h"
#include "ExpBas.h"
#include <utility>
#include <iostream>
#include <cstring>
#define MAXBUCKET 160000


void get_i(vector<int>& potential_i, int input_i, int min_num, int level_num, double ratio){

    int *level_start = new int[level_num];

    int _tot = 0;
    int _next = min_num;
    for(int i = 0; i < level_num; i++) {
        _tot = _tot + _next;
        _next = ((double) _next) * ratio;
    }

    level_start[0] = 0;
    _next = min_num;
    int _left = min_num;
    for(int i = 1; i < level_num; i++){
        level_start[level_num - i] = _tot - _left;
        _left = _left + _next * ratio;
        _next = _next * ratio;
    }

    int input_level = 0;
    for(int i = 0; i < level_num; i++){
        if(input_i >= level_start[i]){
            input_level = i;
        }
    }

    if(input_level == 0){
        potential_i.push_back(input_i + level_start[0]);
        potential_i.push_back(input_i/2 + level_start[1]);
        potential_i.push_back(input_i/2/2 + level_start[2]);
        potential_i.push_back(input_i/2/2/2 + level_start[3]);
    }

    if(input_level == 1){
        int tmpi = input_i - level_start[1];
        potential_i.push_back(tmpi*2 + level_start[0]);
        potential_i.push_back(tmpi*2 + 1 + level_start[0]);
        potential_i.push_back(tmpi + level_start[1]);
        potential_i.push_back(tmpi/2 + level_start[2]);
        potential_i.push_back(tmpi/2/2 + level_start[3]);
    }

    if(input_level == 2){
        int tmpi = input_i - level_start[2];
        potential_i.push_back(tmpi*2*2 + level_start[0]);
        potential_i.push_back((tmpi*2)*2 + 1 + level_start[0]);
        potential_i.push_back((tmpi*2+1)*2 + level_start[0]);
        potential_i.push_back((tmpi*2+1)*2 + 1 + level_start[0]);

        potential_i.push_back(tmpi*2 + level_start[1]);
        potential_i.push_back(tmpi*2 + 1 + level_start[1]);

        potential_i.push_back(tmpi + level_start[2]);
        potential_i.push_back(tmpi/2 + level_start[3]);
    }

    if(input_level == 3){
        int tmpi = input_i - level_start[3];
        potential_i.push_back(tmpi*2*2*2 + level_start[0]);
        potential_i.push_back(tmpi*2*2*2+1 + level_start[0]);
        potential_i.push_back((tmpi*2*2+1)*2 + level_start[0]);
        potential_i.push_back((tmpi*2*2+1)*2+1 + level_start[0]);
        potential_i.push_back((tmpi*2+1)*2*2 + level_start[0]);
        potential_i.push_back((tmpi*2+1)*2*2+1 + level_start[0]);
        potential_i.push_back(((tmpi*2+1)*2+1)*2 + level_start[0]);
        potential_i.push_back(((tmpi*2+1)*2+1)*2+1 + level_start[0]);

        potential_i.push_back(tmpi*2*2 + level_start[1]);
        potential_i.push_back((tmpi*2)*2 + 1 + level_start[1]);
        potential_i.push_back((tmpi*2+1)*2 + level_start[1]);
        potential_i.push_back((tmpi*2+1)*2 + 1 + level_start[1]);

        potential_i.push_back(tmpi*2 + level_start[2]);
        potential_i.push_back(tmpi*2 + 1 + level_start[2]);

        potential_i.push_back(tmpi + level_start[3]);
    }

}

class FlowCache{
private:
    BOBHash32 hash[4];
    friend class boost::serialization::access;
public:
    int min_bucket_num;
    int Cacheid;
    int tot_memory_in_bytes;
    int item_size;
    int bucket_num;
    int level_num = 4;
    uint32_t key_len = 13;
    //int *level_start = new int[level_num];
    int level_start[4];
    double ratio = 2;
    //MyItem *MyCache;
    MyItem MyCache[MAXBUCKET];
    FlowCache(){}

    FlowCache(int min_num, int _item, int in_id){
        min_bucket_num = min_num;
        Cacheid = in_id;
        for(int i = 0; i < level_num; i++){
            hash[i].initialize(i + 750);
        }
        int _tot = 0;
        int _next = min_num;
        for(int i = 0; i < level_num; i++) {
            _tot = _tot + _next;
            _next = ((double) _next) * ratio;
        }
        item_size = _item;
        bucket_num = _tot;
        tot_memory_in_bytes = _tot * item_size;
        level_start[0] = 0;
        _next = min_num;
        int _left = min_num;
        for(int i = 1; i < level_num; i++){
            level_start[level_num - i] = _tot - _left;
            _left = _left + _next * ratio;
            _next = _next * ratio;
        }
        //MyCache = new MyItem[bucket_num];

        // cout << endl << "********FLOW CACHE INFORMATION**********" << endl;
        // cout << "FlowCache " << Cacheid << ":" << endl;
        // cout << "Memory: " << tot_memory_in_bytes << endl;
        // cout << "number of counters: " << bucket_num << endl;
        // cout << "ratio: " << ratio << endl;
        // cout << "level: " << level_num << endl;
        // cout << "****************************************" << endl << endl;

    }

    void set_keylen(uint32_t _len){
        key_len = _len;
    }

    void setID(int id)
    {
        Cacheid = id;
    }

    void insert_to_place(MyItem in_item, int in_level, int in_index){

        MyCache[level_start[in_level] + in_index].initial_item\
        (key_len, in_item.my_key, in_item.my_value_count, in_item.my_value_time);
    }



    void exchange_item(MyItem &a, MyItem &b){
        MyItem tmp;
        tmp.initial_item(key_len, a.my_key, a.my_value_count, a.my_value_time);
        a.initial_item(key_len, b.my_key, b.my_value_count, b.my_value_time);
        b.initial_item(key_len, tmp.my_key, tmp.my_value_count, tmp.my_value_time);
    }

    bool d_left_level(MyItem & flow_item, int in_level, int index){

        MyItem* tmpCounter = &MyCache[level_start[in_level] + index];

        if(tmpCounter->my_value_count == 0){//empty
            insert_to_place(flow_item, in_level, index);
            return true;
        }else{
            if(same_key(flow_item, *tmpCounter, key_len)){//check same
                tmpCounter->my_value_count ++;
                if(flow_item.my_value_time > tmpCounter->my_value_time){
                    tmpCounter->my_value_time = flow_item.my_value_time;
                }
                return true;
            }else{
                if(tmpCounter->my_value_count == 1){//d-left

                    exchange_item(*tmpCounter, flow_item);

                    return false;
                }
                else{
                    return false;
                }
            }
        }
    }

    bool must_kick_level(MyItem & flow_item, int in_level, int index){
        MyItem* tmpCounter = &MyCache[level_start[in_level] + index];

        if(tmpCounter->my_value_count == 0){
            insert_to_place(flow_item, in_level, index);
            return true;
        }else{
            if(same_key(flow_item, *tmpCounter, key_len)){
                tmpCounter->my_value_count ++;
                if(flow_item.my_value_time > tmpCounter->my_value_time){
                    tmpCounter->my_value_time = flow_item.my_value_time;
                }
                return true;
            }else{
                exchange_item(*tmpCounter, flow_item);
                return false;
            }

        }

    }



    void insert(MyItem & flow_item, const char* hash_key, const int hash_key_len){
        int index = (hash[0].run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        if(!d_left_level(flow_item, 0, index)){
            index = index / 2;
            if(!d_left_level(flow_item, 1, index)){
                index = index / 2;
                if(!must_kick_level(flow_item, 2, index)){
                    index = index / 2;
                    MyItem* tmpCounter = &MyCache[level_start[3] + index];
                    if(tmpCounter->my_value_count == 0){
                        insert_to_place(flow_item, 3, index);
                    }else{
                        if(same_key(flow_item, *tmpCounter, key_len)){
                            //BUG!!! tmpCounter->my_value_count ++;
                            tmpCounter->my_value_count += flow_item.my_value_count;
                            if(flow_item.my_value_time > tmpCounter->my_value_time)
                            {
                                tmpCounter->my_value_time = flow_item.my_value_time;
                            }
                        }else{
                            if(comp_item(flow_item, *tmpCounter)){
                                exchange_item(flow_item, *tmpCounter);
                            }
                        }

                    }
                }
            }
        }

    }

    bool check(int i, const char* hash_key, const int hash_key_len){
        int index = (hash[0].run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        vector<int> potential_i;
        potential_i.clear();
        get_i(potential_i, index, min_bucket_num, level_num, ratio);
        for(auto itr: potential_i){
            if(itr == i){
                return true;
            }
        }
        return false;
    }

    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {

        int i;
       for(i = 0; i < 4; ++i)
           ar & hash[i];
        ar & min_bucket_num;
        ar & Cacheid;
        ar & tot_memory_in_bytes;
        ar & item_size;
        ar & bucket_num;
        ar & level_num;
        ar & key_len;
        ar & ratio;
        for(i = 0; i < 4; ++i)
        ar & level_start[i];
        for(i = 0; i < MAXBUCKET; ++i)
        ar & MyCache[i];
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version) 
    {

        int i;
       for(i = 0; i < 4; ++i)
           ar & hash[i];
        ar & min_bucket_num;
        ar & Cacheid;
        ar & tot_memory_in_bytes;
        ar & item_size;
        ar & bucket_num;
        ar & level_num;
        ar & key_len;
        ar & ratio;
        for(i = 0; i < 4; ++i)
        ar & level_start[i];
        for(i = 0; i < MAXBUCKET; ++i)
        ar & MyCache[i];
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};
BOOST_CLASS_EXPORT_GUID(FlowCache,"FlowCache")
#endif//_FLOWCACHE_H