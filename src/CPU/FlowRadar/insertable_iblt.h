#ifndef _FLOW_RADAR_13_H_
#define _FLOW_RADAR_13_H_

#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector> 
#include <unordered_map>
#include <iterator>
#include <string.h>
#include "../common/BOBHash32.h"

using namespace std;

class InsertableIBLT
{
private:
    int key_len;
    class Node {
    public:
        // uint32_t flow_sum;
        char *flow_sum;
        int flow_num;
        int packet_num;
        int lllen;
        Node(){flow_sum = NULL; flow_num = 0; packet_num = 0;}
        void init(int len){flow_sum = new char[len]; memset(flow_sum, 0, len); lllen = len;}
        void clear(){flow_num = 0; packet_num = 0; memset(flow_sum, 0, lllen);}
        ~Node(){if(flow_sum != NULL) delete flow_sum;}
    };

    int total_in_bytes;
    int bf_k, iblt_k;       // number of hash used in bf and iblt
	int bf_size_in_bytes;
    int iblt_size_in_bytes;

    int w_bf;       // number of bits in bf
    int w_iblt;     // number of bucket in iblt
    BOBHash32 * bobhash[7];
    uint64_t * bf;
    Node * iblt;

public:
    int num_flow = 0;

    InsertableIBLT(uint64_t total_in_bytes, int key_len = 13, uint32_t bf_k = 4, uint32_t iblt_k = 3):
		total_in_bytes(total_in_bytes), key_len(key_len), bf_k(bf_k), iblt_k(iblt_k)
	{
		bf_size_in_bytes = total_in_bytes * 0.1;
		iblt_size_in_bytes = total_in_bytes * 0.9;
		w_bf = bf_size_in_bytes * 8;
		w_iblt = iblt_size_in_bytes / sizeof(Node);

        bf = new uint64_t[w_bf >> 6];
        memset(bf, 0, sizeof(uint64_t) * (w_bf >> 6));
        iblt = new Node[w_iblt];
        for(int i = 0; i < w_iblt; ++i)  iblt[i].init(key_len);
        // memset(iblt, 0, sizeof(Node) * w_iblt);

        for (unsigned i = 0; i < bf_k + iblt_k; i++) {
            bobhash[i] = new BOBHash32(i + 500);
        }
        // printf("FlowRadar build done:\n");
        // printf("number of bits in bloom filter: %d\n", w_bf);
        // printf("number of nodes in iblt: %d\n", w_iblt);
        // printf("total memory in bytes: %lu\n", (w_bf >> 3) + w_iblt * sizeof(Node));
    }

    void clear(){
        memset(bf, 0, sizeof(uint64_t) * (w_bf >> 6));
        for(int i = 0; i < w_iblt; ++i)  iblt[i].clear();
    }

    void insert(char* key, int f = 1) {
        int flag = 1;
        int index[10];

    /* check bloom filter */
        for (unsigned i = 0; i < bf_k; i++) {
            index[i] = (bobhash[i]->run(key, key_len)) % w_bf;
            if (((bf[index[i] >> 6] >> (index[i] & 0x3F)) & 1) == 0) {
                flag = 0;
            }
            bf[index[i] >> 6] |= ((uint64_t) 1 << (index[i] & 0x3F));
        }
        
    /* flag = 0 means key is not in bloom filter */
        for (unsigned i = 0; i < iblt_k; i++) {
            index[i] = (bobhash[i + bf_k]->run(key, key_len)) % w_iblt;
            if (flag == 0) {
                num_flow++;
                // iblt[index[i]].flow_sum ^= key;
                for(int k = 0; k < key_len; ++k){
                    iblt[index[i]].flow_sum[k] ^= key[k];
                }
                iblt[index[i]].flow_num++;
            }
            iblt[index[i]].packet_num += f;
        }
    }

    void build(char** data, int n) {
        for (int i = 0; i < n; ++i) {
            insert(data[i]);
        }
    }

    void dump(unordered_map<string, int> &result) {
        // uint32_t key;
        char key[100];
        int count;
        int index[10];

        while (1) {
            bool flag = true;
            for (int i = 0; i < w_iblt; i++) {
                if (iblt[i].flow_num == 1) {
                    // key = iblt[i].flow_sum;
                    memcpy(key, iblt[i].flow_sum, key_len);
                    count = iblt[i].packet_num;
                    result[string(key, key_len)] = count;

                    for (unsigned j = 0; j < iblt_k; j++) {
                        index[j] = (bobhash[j + bf_k]->run(key, key_len)) % w_iblt;

                        iblt[index[j]].flow_num--;
                        // iblt[index[j]].flow_sum ^= key;
                        for(int k = 0; k < key_len; ++k)
                            iblt[index[j]].flow_sum[k] ^= key[k];
                        iblt[index[j]].packet_num -= count;
                    }
                    flag = false;
                }
            }
            if (flag)
                break;
        }
    }

    int approximate_query(char* key) {
        for (unsigned i = 0; i < bf_k; i++) {
            int idx = (bobhash[i]->run(key, key_len)) % w_bf;
            if (((bf[idx >> 6] >> (idx & 0x3F)) & 1) == 0) {
                return 0;
            }
        }
        return -1;
    }
    
    ~InsertableIBLT()
    {
    	//cout << "delete" << endl;
		delete bf;
    	//cout << "delete bf" << endl;
    	delete[] iblt;
    	//cout << "delete iblt" << endl;
    	for (unsigned i = 0; i < 7; i++) {
            delete bobhash[i] ;
        }
        //cout << "delete bobhash" << endl;
        return;
	}
};

#endif//_FLOW_RADAR_13_H_