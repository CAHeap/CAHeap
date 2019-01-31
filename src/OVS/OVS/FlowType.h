#ifndef _FLOWTYPE_H
#define _FLOWTYPE_H

#include <utility>
#include <iostream>
#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


struct MyItem{
    char *my_key;
    uint32_t my_value_count;
    uint32_t my_value_time; 
    MyItem(){
        my_key = NULL;
        my_value_count = 0;
        my_value_time = 0;
    }
    void initial_item(int key_len , const char *in_key, uint32_t in_value_count, uint32_t in_value_time) {
        my_key = new char[key_len];
        memcpy(my_key, in_key, key_len);
        my_value_count = in_value_count;
        my_value_time = in_value_time;
    }
    ~MyItem(){
        // if(my_key != NULL)
        //     delete my_key;
    }
};

bool comp_item(MyItem a, MyItem b)
{
    if(a.my_value_count > b.my_value_count){
        return true;
    }else{
        if(a.my_value_count == b.my_value_count){
            if(a.my_value_time >= b.my_value_time){
                return true;
            }else{
                return false;
            }
        }else{
            return false;
        }
    }
}

bool comp_item_window(MyItem a, MyItem b, int window_thre)
{
    if (b.my_value_count == window_thre)
    {
        return true;
    }
    if (a.my_value_count > b.my_value_count)
    {
        return true;
    }
    else
    {
        if (a.my_value_count == b.my_value_count)
        {
            if (a.my_value_time > b.my_value_time)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}


bool comp_item_by_time(MyItem a, MyItem b)
{
    if(a.my_value_time > b.my_value_time)
        return true;
    else
        return false;
}

bool same_key(MyItem a, MyItem b, uint32_t key_len){
    char *p1 = a.my_key;
    char *p2 = b.my_key;
    for(int i = 0; i < key_len; i++){
        if(p1[i] != p2[i]){
            return false;
        }
    }
    return true;
}


#endif