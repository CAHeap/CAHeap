#ifndef _A_HEAP_H_
#define _A_HEAP_H_

#include <stdint.h>
#include <string.h>
#include <math.h>
#include "BOBHash32.h"


struct Tuple{
	uint32_t sIP;
	uint16_t sPort;
	uint32_t dIP;
	uint16_t dPort;
	uint8_t type;
};

#define KEY_LEN 13
struct Item{
	char key[KEY_LEN];
	uint32_t timestamp;
	uint32_t counter;	// Item is empty if counter = 0
	// uint64_t value;
};

/*
 * d1: # layers for d-left
 * d2: # layers for winner tree
 * m: # counters in the last layer
 * M: # counters in the 1st layer
 */
// template <uint32_t d, uint32_t m>
class AHeap
{
	uint32_t d, d1, d2, m, M;
	uint32_t itemNum;
	Item** items;
	BOBHash32 *hash;

	bool aging_counter, aging_time;			// if aging
	bool probpush;		// if probabilistic pushing

	uint32_t aging_counter_thre, aging_time_thre;

public:
	AHeap(uint32_t d1, uint32_t d2, uint32_t m, bool probpush = false,
		bool aging_counter = false, bool aging_time = false, 
		uint32_t aging_counter_thre = 1, uint32_t aging_time_thre = 1):
	d1(d1), d2(d2), d(d1+d2), m(m), probpush(probpush), 
	aging_counter(aging_counter), aging_time(aging_time), 
	aging_counter_thre(aging_counter_thre), aging_time_thre(aging_time_thre)
	{
		M = m * (int)pow(2, d-1);
		itemNum = m * ((int)pow(2, d) - 1);

		int k = M;
		items = new Item*[d];
		for(int i = 0; i < d; ++i){
			items[i] = new Item[k];
			memset(items[i], 0, k * sizeof(Item));
			k /= 2;
		}

		hash = new BOBHash32(100);
		printf("AHeap has %d buckets\n", itemNum);
	}
	~AHeap(){
		for(int i = 0; i < d; ++i)
			delete items[i];
		delete items;
		delete hash;
	}


	/* 
	 * insert for flow size 
	 * p.key: flow id
	 * p.counter: packet size or packet number
	 * p.timestamp: timestamp
	 * p.value: 
	 */
	void insert_flowsize(Item p){
		uint32_t hash_val = hash->run(p.key, KEY_LEN);
		uint32_t idx = hash_val % M;
		uint32_t cur_w = M;

		for(int i = 0; i < d; ++i){
			if(items[i][idx].counter == 0){
				memcpy(&items[i][idx], &p, sizeof(Item));
				return;
			}
			else if(memcmp(items[i][idx].key, p.key, KEY_LEN) == 0){
				items[i][idx].counter += p.counter;
				items[i][idx].timestamp = p.timestamp;
				return;
			}
			else if(i < d1){
				if(aging(&items[i][idx], &p)){
					memcpy(&items[i][idx], &p, sizeof(Item));
					return;
				}
			}
			else if(d1 <= i && probpush){
				if(i < d-1){
					/* probabilistic pushing */
					uint32_t randnum = ((p.timestamp + 7547) * p.timestamp) % (items[i][idx].counter);
					if(randnum < p.counter){
						swap_item(items[i][idx], p);
					}
				}
				else if(i == d-1 && items[i][idx].counter <= p.counter){
					memcpy(&items[i][idx], &p, sizeof(Item));
				}
			}
			else if(d1 <= i && !probpush){
				if(items[i][idx].counter <= p.counter){
					swap_item(items[i][idx], p);
				}
			}
			cur_w = cur_w >> 1;
			idx = idx >> 1;
		}
	}


	uint32_t query_flowsize(char* key){
		uint32_t hash_val = hash->run(key, KEY_LEN);
		uint32_t idx = hash_val % M;
		
		for(int i = 0; i < d; ++i){
			if(memcmp(items[i][idx].key, key, KEY_LEN) == 0)
				return items[i][idx].counter;
			idx = idx >> 1;
		}
		return 0xFFFFFFFF;
	}


	/* for debug */
	void print(){
		int cur_w = M;
		for(int i = 0; i < d; ++i){
			for(int j = 0; j < cur_w; ++j)
				printf("(%s, %d, %d)\t", items[i][j].key, items[i][j].timestamp, items[i][j].counter);
			printf("\n*********************************\n");
			cur_w /= 2;
		}
	}
private:

	/* 
	 * judge if to aging
	 * item1 is stored in AHeap
	 * item2 is a new-arriving packet
	 */
	bool aging(Item* item1, Item* item2){
		if(aging_counter && !aging_time && item1->counter <= aging_counter_thre)
			return true;
		else if(!aging_counter && aging_time && item2->timestamp - item1->timestamp > aging_time_thre)
			return true;
		else if(aging_counter && aging_time && item2->timestamp - item1->timestamp > aging_time_thre 
					&& item1->counter <= aging_counter_thre)
			return true;
		return false;
	}

	void swap_item(Item &item1, Item &item2){
		Item tmp;
		memcpy(&tmp, &item1, KEY_LEN);
		memcpy(&item1, &item2, KEY_LEN);
		memcpy(&item2, &tmp, KEY_LEN);
	}
};






#endif