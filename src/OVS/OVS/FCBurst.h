#ifndef _FC_BURST_H_
#define _FC_BURST_H_

#include <stdint.h>
#include "FlowType.h"


#define GET_BURSTY_FIELD(x) (x & 0xF0000000)
#define GET_COUNTER_VALUE(x) (x & 0x0FFFFFFF)


#define SET_BURSTY_TO_ZERO(x) (x & 0x0FFFFFFF)
#define SET_BURSTY_TO_ONE(x) ((x & 0x0FFFFFFF) | 0x10000000)
#define SET_BURSTY_TO_TWO(x) ((x & 0x0FFFFFFF) | 0x20000000)
#define SET_BURSTY_TO_THREE(x) ((x & 0x0FFFFFFF) | 0x30000000)
#define SET_BURSTY_TO_FOUR(x) ((x & 0x0FFFFFFF) | 0x40000000)
#define SET_BURSTY_TO_FIVE(x) ((x & 0x0FFFFFFF) | 0x50000000)
#define SET_BURSTY_TO_SIX(x) ((x & 0x0FFFFFFF) | 0x60000000)
#define SET_BURSTY_TO_SEVEN(x) ((x & 0x0FFFFFFF) | 0x70000000)
#define SET_BURSTY_TO_EIGHT(x) ((x & 0x0FFFFFFF) | 0x80000000)


#define _ZERO (0x00000000)
#define _ONE (0x10000000)
#define _TWO (0x20000000)
#define _THREE (0x30000000)
#define _FOUR (0x40000000)
#define _FIVE (0x50000000)
#define _SIX (0x60000000)
#define _SEVEN (0x70000000)
#define _EIGHT (0x80000000)

void increment_bursty(uint32_t& x)
{
	uint32_t bursty_field = GET_BURSTY_FIELD(x);
	switch(bursty_field){
		case _ZERO:	x = SET_BURSTY_TO_ONE(x); break;
		case _ONE:	x = SET_BURSTY_TO_TWO(x); break;
		case _TWO:	x = SET_BURSTY_TO_THREE(x); break;
		case _THREE:	x = SET_BURSTY_TO_FOUR(x); break;
		case _FOUR:	x = SET_BURSTY_TO_FIVE(x); break;
		case _FIVE:	x = SET_BURSTY_TO_SIX(x); break;
		case _SIX:	x = SET_BURSTY_TO_SEVEN(x); break;
		case _SEVEN: x = SET_BURSTY_TO_EIGHT(x); break;
		default: break;
	}
}

/* item1 is the counter with bursty field 
 * item2 is a new inserted item */
void exchange_item_with_bursty(MyItem& item1, MyItem& item2, int key_len)
{
	char tmp_key[20];
	memcpy(tmp_key, item2.my_key, key_len);
	uint32_t tmp_time = item2.my_value_time;
	uint32_t tmp_count = item2.my_value_count;

	memcpy(item2.my_key, item1.my_key, key_len);
	item2.my_value_time = item1.my_value_time;
	item2.my_value_count = SET_BURSTY_TO_ZERO(item1.my_value_count);

	memcpy(item1.my_key, tmp_key, key_len);
	item1.my_value_time = tmp_time;
	item1.my_value_count = SET_BURSTY_TO_ONE(tmp_count);
}


#endif