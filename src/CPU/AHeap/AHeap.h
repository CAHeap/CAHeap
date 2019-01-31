#ifndef _AHEAP_H
#define _AHEAP_H

#include "../common/BOBHash32.h"
#include "Flow.h"
#include "ExpBas.h"
#include "Burst.h"
#include <utility>
#include <iostream>
#include <cstring>

#define KEY_LEN 13

int aging = 0;
int window_thre = 0;

class AHeap
{
private:
    BOBHash32 *hash[4];
public:
    int min_bucket_num;
    int Cacheid;
    int total_memory_in_bytes;
    int item_size;
    int bucket_num;
    int level_num = 4;
    uint32_t key_len = 13;
    int *level_start = new int[level_num];
    double ratio = 2;
    MyItem *MyCache;

    AHeap(int min_num, int _item, int in_id)
    {
        int _total = 0;
        int _next = min_num;

        min_bucket_num = min_num;
        Cacheid = in_id;

        for(int i = 0; i < level_num; i++)
        {
            hash[i] = new BOBHash32(i + 750);
        }
        
        for(int i = 0; i < level_num; i++)
        {
            _total = _total + _next;
            _next = ((double) _next) * ratio;
        }

        int _left = min_num;
        item_size = _item;
        bucket_num = _total;
        total_memory_in_bytes = _total * item_size;
        level_start[0] = 0;
        _next = min_num;
        
        for(int i = 1; i < level_num; i++)
        {
            level_start[level_num - i] = _total - _left;
            _left = _left + _next * ratio;
            _next = _next * ratio;
        }

        MyCache = new MyItem[bucket_num];
    }

    ~AHeap(){}

    // Set key (flow ID) length 
    void set_keylen(uint32_t _len)
    {
        key_len = _len;
    }

    // Insert an item into one level of AHeap
    void insert_to_place(MyItem in_item, int in_level, int in_index)
    {
        MyCache[level_start[in_level] + in_index].initial_item(key_len, in_item.my_key, in_item.my_value_count, in_item.my_value_time);
    }

    // Exchange information carried by an incoming item and a resident item
    void exchange_item(MyItem &a, MyItem &b){
        MyItem tmp;
        tmp.initial_item(key_len, a.my_key, a.my_value_count, a.my_value_time);
        a.initial_item(key_len, b.my_key, b.my_value_count, b.my_value_time);
        b.initial_item(key_len, tmp.my_key, tmp.my_value_count, tmp.my_value_time);
    }

    // Insert an item into d-left
    bool d_left_level(MyItem & flow_item, int in_level, int index)
    {

        MyItem* tmpCounter = &MyCache[level_start[in_level] + index];

        if(tmpCounter->my_value_count == 0) // If the location is empty, insert the incoming item.
        {
            insert_to_place(flow_item, in_level, index);
            return true;
        }
        else
        {
            if(same_key(flow_item, *tmpCounter, key_len)) // Check whether the incoming item has the same key with the resident item 
            {
                incrementItem(flow_item, tmpCounter);
                return true;
            }
            else
            {
                if (aging != 0) // If difference between timestamps of the two items is large, exchange the two items.  
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


    // Forcibly kick a resident item and insert an incoming item
    bool must_kick_level(MyItem & flow_item, int in_level, int index)
    {
        MyItem* tmpCounter = &MyCache[level_start[in_level] + index];

        if(tmpCounter->my_value_count == 0)
        {
            insert_to_place(flow_item, in_level, index);
            return true;
        }
        else
        {
            if(same_key(flow_item, *tmpCounter, key_len))
            {
                incrementItem(flow_item, tmpCounter);
                return true;
            }
            else
            {
                exchange_item(*tmpCounter, flow_item);
                return false;
            }

        }
    }

    // Insert an item into AHeap level by level
    void insert(MyItem & flow_item, const char* hash_key, const int hash_key_len)
    {      
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
                        }
                        else
                        {
                            if (window_thre != 0) // If difference between timestamps of the two items is large, exchange the two items.  
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
    

// Increment a counter of an item by 1 according to its timestamp or frequency
void incrementItem(MyItem &newItem, MyItem *oldItem)
{
    if (window_thre != 0) // The counter of an item is used for recording how mant time windows itself appears
    {
        if (oldItem->my_value_count < window_thre)
        {
            if ((newItem.my_value_time - oldItem->my_value_time) == 1) // If an item appears in continuous time windows, increment its counter
            {
                oldItem->my_value_count++;
            }
            else
            {
                if (newItem.my_value_time != oldItem->my_value_time) // If an item does not appear in continuous time windows, initialize its counter
                {
                    oldItem->my_value_count = 1;
                }
            }
        }
        oldItem->my_value_time = newItem.my_value_time;
    } 
    else // Counter of an item is used for recording its frequency
    {
        if(newItem.my_value_time > oldItem->my_value_time)
        {
            oldItem->my_value_time = newItem.my_value_time;
        }
        oldItem->my_value_count ++;
    }
}

// Increment a counter of an item by f according to its timestamp or frequency
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

// Insert an item into AHeap level by level and kick resident items whose timestamps are old or frequencies are small
void insert(MyItem & flow_item, const char* hash_key, const int hash_key_len, int time_thre, int counter_thre)
{
    int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        if(!d_left_level(flow_item, 0, index, time_thre, counter_thre))
        {
            index = index / 2;
            if(!d_left_level(flow_item, 1, index, time_thre, counter_thre))
            {
                index = index / 2;
                if(!must_kick_level(flow_item, 2, index))
                {
                    index = index / 2;
                    MyItem* tmpCounter = &MyCache[level_start[3] + index];
                    if(tmpCounter->my_value_count == 0)
                    {
                        insert_to_place(flow_item, 3, index);
                    }else
                    {
                        if(same_key(flow_item, *tmpCounter, key_len)){
                            tmpCounter->my_value_count += flow_item.my_value_count;
                            if(flow_item.my_value_time > tmpCounter->my_value_time)
                            {
                                tmpCounter->my_value_time = flow_item.my_value_time;
                            }
                        }
                        else if(comp_item(flow_item, *tmpCounter))
                            exchange_item(flow_item, *tmpCounter);
                    }
                }
            }
        }
}

// Insert an item into AHeap in the process of performing recent flow finding
void insert_for_recent(MyItem & flow_item, const char* hash_key, const int hash_key_len, int time_thre)
{
    int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        if(!d_left_level(flow_item, 0, index, time_thre, 0))
        {
            index = index / 2;
            if(!d_left_level(flow_item, 1, index, time_thre, 0))
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
                        if(same_key(flow_item, *tmpCounter, key_len)){
                            tmpCounter->my_value_count += flow_item.my_value_count;
                            if(flow_item.my_value_time > tmpCounter->my_value_time)
                            {
                                tmpCounter->my_value_time = flow_item.my_value_time;
                            }
                        }
                        else if(flow_item.my_value_time - tmpCounter->my_value_time >= time_thre)
                            exchange_item(flow_item, *tmpCounter);
                    }
                }
            }
        }
}

// Insert an item into AHeap with ageing mechanism
void insert_aging_by_time(MyItem & flow_item, const char* hash_key, const int hash_key_len)
{
    int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        if(!d_left_level_by_time(flow_item, 0, index))
        {
            index = index / 2;
            if(!d_left_level_by_time(flow_item, 1, index))
            {
                index = index / 2;
                if(!must_kick_level(flow_item, 2, index))
                {
                    index = index / 2;
                    MyItem* tmpCounter = &MyCache[level_start[3] + index];
                    if(tmpCounter->my_value_count == 0)
                    {
                        insert_to_place(flow_item, 3, index);
                    }else{
                        if(same_key(flow_item, *tmpCounter, key_len))
                        {
                            tmpCounter->my_value_count ++;
                            if(flow_item.my_value_time > tmpCounter->my_value_time)
                            {
                                tmpCounter->my_value_time = flow_item.my_value_time;
                            }
                        }
                        else
                        {
                            if(comp_item_by_time(flow_item, *tmpCounter))
                            {
                                exchange_item(flow_item, *tmpCounter);
                            }
                        }

                    }
                }
            }
        }
}

// Check whether an item is resident
bool check(int i, const char* hash_key, const int hash_key_len)
{
    int index = (hash[0]->run(hash_key, hash_key_len)) % (level_start[1] - level_start[0]);
        for(int j = 0; j < level_num; j++)
        {
            if( i == (index + level_start[j]))
            {
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
    // Insert an item into d-left and kick resident items whose frequencies are small or timestamps are old
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

    // Insert an item into d-left 
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



#endif
