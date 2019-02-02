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

#define KEYLENMAX 20

class MyItem{
public:
    friend class boost::serialization::access;
    char my_key[KEYLENMAX];
    uint32_t my_value_count;
    uint32_t my_value_time;
    MyItem(){
        //my_key = nullptr;
        my_value_count = 0;
        my_value_time = 0;
    }
    void initial_item(int key_len ,char *in_key, uint32_t in_value_count, uint32_t in_value_time) {
        //my_key = new char[key_len];
        memcpy(my_key, in_key, key_len * sizeof(char));
        my_value_count = in_value_count;
        my_value_time = in_value_time;
    }
	template<class Archive>
    void save(Archive & ar, const unsigned int version)const
    {
			int i;
            for(i = 0; i < KEYLENMAX; ++i)
            ar &my_key[i];
            ar &my_value_count;
            ar &my_value_time;
	}
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
			int i;
            for(i = 0; i < KEYLENMAX; ++i)
            ar &my_key[i];
            ar &my_value_count;
            ar &my_value_time;
	}
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};
BOOST_CLASS_EXPORT_GUID(MyItem,"MyItem")

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