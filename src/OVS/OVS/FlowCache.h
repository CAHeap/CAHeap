#ifndef _FLOWCACHE_H
#define _FLOWCACHE_H

#include "BOBHash32.h"
#include "FlowType.h"
#include "ExpBas.h"
#include "FCBurst.h"
#include <utility>
#include <iostream>
#include <cstring>


int aging = 0;
int window_thre = 0;


class FlowCache{
private:
    BOBHash32 *hash[4];
public:
    int min_bucket_num;
    int Cacheid; 
    int tot_memory_in_bytes;
    int item_size;
    int bucket_num;
    int level_num = 4;
    uint32_t key_len = 13;
    int *level_start = new int[level_num];
    double ratio = 2;
    MyItem *MyCache;

    FlowCache(int min_num, int _item, int in_id){
        min_bucket_num = min_num;
        Cacheid = in_id;
        for(int i = 0; i < level_num; i++){
            hash[i] = new BOBHash32(i + 750);
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
        MyCache = new MyItem[bucket_num];

        // cout << endl << "********FLOW CACHE INFORMATION**********" << endl;
        // cout << "FlowCache " << Cacheid << ":" << endl;
        // cout << "Memory: " << tot_memory_in_bytes << endl;
        // cout << "number of counters: " << bucket_num << endl;
        // cout << "ratio: " << ratio << endl;
        // cout << "level: " << level_num << endl;
        // cout << "****************************************" << endl << endl;

    }

    ~FlowCache(){
        // delete level_start;
        // for(int i = 0; i < level_num; i++){
        //     delete hash[i];
        // }
        // delete[] MyCache;
    }

    void set_keylen(uint32_t _len){
        key_len = _len;
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

        if(tmpCounter->my_value_count == 0)
        {//empty
            insert_to_place(flow_item, in_level, index);
            return true;
        }
        else
        {
            if(same_key(flow_item, *tmpCounter, key_len))
            {//check same
                conflict_cnt++;
                incrementItem(flow_item, tmpCounter);
                return true;
            }
            else
            {
                if (aging != 0)
                {
                    if (flow_item.my_value_time - tmpCounter->my_value_time > aging ) 
                    {
                        exchange_item(*tmpCounter, flow_item);
                        return false;
                    }
                }
                else
                {
                    if (tmpCounter->my_value_count == 1)
                    {
                        exchange_item(*tmpCounter, flow_item);
                        return false;
                    }
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
                conflict_cnt++;
                incrementItem(flow_item, tmpCounter);
                return true;
            }else{
                exchange_item(*tmpCounter, flow_item);
                return false;
            }

        }

    }


    int conflict_cnt = 0;
    void insert(MyItem & flow_item, const char* hash_key, const int hash_key_len){
       
        int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        if(!d_left_level(flow_item, 0, index))
        {
            index = index / 2;
            if(!d_left_level(flow_item, 1, index))
            {
                index = index / 2;
                if(!must_kick_level(flow_item, 2, index))
                {
                    index = index / 2;
                    MyItem* tmpCounter = &MyCache[level_start[3] + index];
                    if(tmpCounter->my_value_count == 0)
                    {
                        insert_to_place(flow_item, 3, index);
                    }
                    else
                    {
                        if(same_key(flow_item, *tmpCounter, key_len))
                        {
                            incrementItem(flow_item, tmpCounter, flow_item.my_value_count);
                            conflict_cnt++;
                        }
                        else
                        {
                            if (window_thre != 0)
                            {
                                if (comp_item_window(flow_item, *tmpCounter, window_thre))
                                {
                                    exchange_item(flow_item, *tmpCounter);
                                }
                            }
                            else
                            {
                                if (comp_item(flow_item, *tmpCounter))
                                {
                                    exchange_item(flow_item, *tmpCounter);
                                }
                            }
                        }    
                    }
                }
            }
        }
    }
    

//
void incrementItem(MyItem &newItem, MyItem *oldItem)
{
    if (window_thre != 0)
    {
        if (oldItem->my_value_count < window_thre)
        {
            if ((newItem.my_value_time - oldItem->my_value_time) == 1)
            {
                oldItem->my_value_count++;
            }
            else
            {
                if (newItem.my_value_time != oldItem->my_value_time)
                {
                    oldItem->my_value_count = 1;
                }
            }
        }
        oldItem->my_value_time = newItem.my_value_time;
    }
    else
    {
        if(newItem.my_value_time > oldItem->my_value_time)
        {
            oldItem->my_value_time = newItem.my_value_time;
        }
        oldItem->my_value_count ++;
    }
}

void incrementItem(MyItem &newItem, MyItem *oldItem, int f)
{
    if (window_thre != 0)
    {
        if((newItem.my_value_time - oldItem->my_value_time) == 1 )
        {
            if (newItem.my_value_count < window_thre)
            {
                oldItem->my_value_count += f;
            }
            else
            {
                oldItem->my_value_count = 0;
            }
            oldItem->my_value_time = newItem.my_value_time;
        }
    }
    else
    {
        if(newItem.my_value_time > oldItem->my_value_time)
        {
            oldItem->my_value_time = newItem.my_value_time;
        }
        oldItem->my_value_count += f;
    }

}
/* please don't delete */
    void insert(MyItem & flow_item, const char* hash_key, const int hash_key_len, int time_thre, int counter_thre){
        int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);

        if(!d_left_level(flow_item, 0, index, time_thre, counter_thre)){
            index = index / 2;
            if(!d_left_level(flow_item, 1, index, time_thre, counter_thre)){
                index = index / 2;
                if(!must_kick_level(flow_item, 2, index)){
                    index = index / 2;
                    MyItem* tmpCounter = &MyCache[level_start[3] + index];
                    if(tmpCounter->my_value_count == 0){
                        insert_to_place(flow_item, 3, index);
                    }else{
                        if(same_key(flow_item, *tmpCounter, key_len)){
                            tmpCounter->my_value_count += flow_item.my_value_count;
                            if(flow_item.my_value_time > tmpCounter->my_value_time){
                                tmpCounter->my_value_time = flow_item.my_value_time;
                            }
                        }else if(comp_item(flow_item, *tmpCounter))
                            exchange_item(flow_item, *tmpCounter);
                    }
                }
            }
        }
    }

    void insert_for_recent(MyItem & flow_item, const char* hash_key, const int hash_key_len, int time_thre){
        int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);

        if(!d_left_level(flow_item, 0, index, time_thre, 0)){
            index = index / 2;
            if(!d_left_level(flow_item, 1, index, time_thre, 0)){
                index = index / 2;
                if(!must_kick_level(flow_item, 2, index)){
                    index = index / 2;
                    MyItem* tmpCounter = &MyCache[level_start[3] + index];
                    if(tmpCounter->my_value_count == 0){
                        insert_to_place(flow_item, 3, index);
                    }else{
                        if(same_key(flow_item, *tmpCounter, key_len)){
                            tmpCounter->my_value_count += flow_item.my_value_count;
                            if(flow_item.my_value_time > tmpCounter->my_value_time){
                                tmpCounter->my_value_time = flow_item.my_value_time;
                            }
                        }else if(flow_item.my_value_time - tmpCounter->my_value_time >= time_thre)
                            exchange_item(flow_item, *tmpCounter);
                    }
                }
            }
        }
    }


  /* please don't delete */
    void insert_aging_by_time(MyItem & flow_item, const char* hash_key, const int hash_key_len){
        int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);

        if(!d_left_level_by_time(flow_item, 0, index)){
            index = index / 2;
            if(!d_left_level_by_time(flow_item, 1, index)){
                index = index / 2;
                if(!must_kick_level(flow_item, 2, index)){
                    index = index / 2;
                    MyItem* tmpCounter = &MyCache[level_start[3] + index];
                    if(tmpCounter->my_value_count == 0){
                        insert_to_place(flow_item, 3, index);
                    }else{
                        if(same_key(flow_item, *tmpCounter, key_len)){
                            tmpCounter->my_value_count ++;
                            if(flow_item.my_value_time > tmpCounter->my_value_time){
                                tmpCounter->my_value_time = flow_item.my_value_time;
                            }
                        }else{
                            if(comp_item_by_time(flow_item, *tmpCounter)){
                                exchange_item(flow_item, *tmpCounter);
                            }
                        }

                    }
                }
            }
        }
    }

    bool check(int i, const char* hash_key, const int hash_key_len){

        int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        for(int j = 0; j < level_num; j++){
            if( i == (index + level_start[j])){
                return true;
            }
            index = index / 2;
        }
        return false;
    }



/* insert for burst */
    /*
     * return 0: not a burst
     * return 1: is a burst
     */
    int insert_for_burst(MyItem & flow_item, const char* hash_key, const int hash_key_len)
    {
        int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        MyItem* tmpCounter = NULL;

        /* level - 1 */
        // printf("into level - 1\n");
        tmpCounter = &MyCache[level_start[0] + index];
        if(tmpCounter->my_value_count == 0){
            tmpCounter->initial_item(key_len, flow_item.my_key,
                SET_BURSTY_TO_ONE(flow_item.my_value_count), flow_item.my_value_time);
            return 0;
        }
        if(same_key(flow_item, *tmpCounter, key_len)){
            increment_same_key(tmpCounter, flow_item);
            return 0;
        }
        if(GET_COUNTER_VALUE(tmpCounter->my_value_count) == 1){
            exchange_item_with_bursty(*tmpCounter, flow_item, key_len);
        }
        
        /* level - 2 */
        // printf("into level - 2\n");
        index = index / 2;
        tmpCounter = &MyCache[level_start[1] + index];
        if(tmpCounter->my_value_count == 0){
            tmpCounter->initial_item(key_len, flow_item.my_key,
                SET_BURSTY_TO_ONE(flow_item.my_value_count), flow_item.my_value_time);
            return 0;
        }
        if(same_key(flow_item, *tmpCounter, key_len)){
            return increment_same_key(tmpCounter, flow_item);
        }
        if(GET_COUNTER_VALUE(tmpCounter->my_value_count) == 1){
            exchange_item_with_bursty(*tmpCounter, flow_item, key_len);
        }

        /* level - 3 */
        // printf("into level - 3\n");
        index = index / 2;
        tmpCounter = &MyCache[level_start[2] + index];
        if(tmpCounter->my_value_count == 0){
            tmpCounter->initial_item(key_len, flow_item.my_key,
                SET_BURSTY_TO_ONE(flow_item.my_value_count), flow_item.my_value_time);
            return 0;
        }
        if(same_key(flow_item, *tmpCounter, key_len)){
            return increment_same_key(tmpCounter, flow_item);
        }
        exchange_item_with_bursty(*tmpCounter, flow_item, key_len);

        /* level - 4 */
        // printf("into level - 4\n");
        index = index / 2;
        tmpCounter = &MyCache[level_start[3] + index];
        if(tmpCounter->my_value_count == 0){
            tmpCounter->initial_item(key_len, flow_item.my_key,
                SET_BURSTY_TO_ONE(flow_item.my_value_count), flow_item.my_value_time);
            return 0;
        }
        if(same_key(flow_item, *tmpCounter, key_len)){
            return increment_same_key(tmpCounter, flow_item);
        }
        if(GET_COUNTER_VALUE(tmpCounter->my_value_count) < flow_item.my_value_count)
            exchange_item_with_bursty(*tmpCounter, flow_item, key_len);
        return 0;
    }

    /* return 1 if find a burst, else return 0 */
    int increment_same_key(MyItem* counter, MyItem& flow_item){
        counter->my_value_time = flow_item.my_value_time > counter->my_value_time ? flow_item.my_value_time : counter->my_value_time;
        counter->my_value_count += flow_item.my_value_count;
        if(GET_BURSTY_FIELD(counter->my_value_count) == _EIGHT){
            // counter->my_value_count = SET_BURSTY_TO_ZERO(counter->my_value_count);
            return 1;
        }
        increment_bursty(counter->my_value_count);
        return 0;
    }

private:
    /* please don't delete */
    bool d_left_level(MyItem & flow_item, int in_level, int index, int time_thre, int counter_thre){

        MyItem* tmpCounter = &MyCache[level_start[in_level] + index];

        if(tmpCounter->my_value_count == 0){//empty
            insert_to_place(flow_item, in_level, index);
            return true;
        }else{
            if(same_key(flow_item, *tmpCounter, key_len)){//check same
                tmpCounter->my_value_count += flow_item.my_value_count;
                if(flow_item.my_value_time > tmpCounter->my_value_time){
                    tmpCounter->my_value_time = flow_item.my_value_time;
                }
                return true;
            }else{
                if(tmpCounter->my_value_count <= counter_thre || 
                    flow_item.my_value_time - tmpCounter->my_value_time >= time_thre){//d-left

                    exchange_item(*tmpCounter, flow_item);

                    return false;
                }
                else{
                    return false;
                }
            }
        }
    }

    /* please don't delete */
    bool d_left_level_by_time(MyItem & flow_item, int in_level, int index){

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
                if (flow_item.my_value_time > tmpCounter->my_value_time){
                    exchange_item(*tmpCounter, flow_item);
                    return false;
                }
                else{
                    return false;
                }
            }
        }
    }

};



#endif//_FLOWCACHE_H
