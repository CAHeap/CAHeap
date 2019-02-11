#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <algorithm>
#include <unordered_map>
typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned char uint8;

using namespace std;

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

#define MIN(a,b)  ((a)>(b)?(b):(a))

#define NEW(a,b)  ((a)<(b)?(b):(a))


inline
cudaError_t checkCuda(cudaError_t result)
{
#if defined(DEBUG) || defined(_DEBUG)
    if (result != cudaSuccess) {
    fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
    assert(result == cudaSuccess);
    }
#endif
    return result;
}

// BOB hash
__host__ __device__
uint32 hash0(uint32 key) {
        //register ub4 a,b,c,len;
    uint32 a,b,c;
    uint32 len = 4;
    char* str = (char*)&key;
    //  uint32_t initval = 0;
    /* Set up the internal state */
    //len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = 8311;//prime32[1000];         /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12)
    {
        a += (str[0] +((uint32)str[1]<<8) +((uint32)str[2]<<16) +((uint32)str[3]<<24));
        b += (str[4] +((uint32)str[5]<<8) +((uint32)str[6]<<16) +((uint32)str[7]<<24));
        c += (str[8] +((uint32)str[9]<<8) +((uint32)str[10]<<16)+((uint32)str[11]<<24));
        mix(a,b,c);
        str += 12; len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += len;
    switch(len)              /* all the case statements fall through */
    {
        case 11: c+=((uint32)str[10]<<24);
        // fall through
        case 10: c+=((uint32)str[9]<<16);
        // fall through
        case 9 : c+=((uint32)str[8]<<8);
        /* the first byte of c is reserved for the length */
        // fall through
        case 8 : b+=((uint32)str[7]<<24);
        // fall through
        case 7 : b+=((uint32)str[6]<<16);
        // fall through
        case 6 : b+=((uint32)str[5]<<8);
        // fall through
        case 5 : b+=str[4];
        // fall through
        case 4 : a+=((uint32)str[3]<<24);
        // fall through
        case 3 : a+=((uint32)str[2]<<16);
        // fall through
        case 2 : a+=((uint32)str[1]<<8);
        // fall through
        case 1 : a+=str[0];
        /* case 0: nothing left to add */
    }
    mix(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}


const int LEN = 36000;
const int BLOCK_SIZE = 1;
const int FIRST_LAYER_BUCKET = 16384;
const int LAST_LAYER_BUCKET = 512;
const int L1 = 2;
const int L2 = 2;


 
__device__ uint32 kernel_bucket_key[LEN];
__device__ uint32 kernel_bucket_timestamp[LEN];
__device__ uint32 kernel_bucket_freq[LEN];
__device__ uint32 mutex[LAST_LAYER_BUCKET];     //2^9 protect the same last layer
__device__ uint32 stamp;
uint32 bucket_key[LEN];
uint32 bucket_timestamp[LEN];
uint32 bucket_freq[LEN];


// read in data
uint32 * read_keys(char *filename,uint32* len){
    FILE* fp=fopen(filename,"rb");
    fseek(fp,0L,SEEK_END);
    uint32 size = ftell(fp);
    rewind(fp);
    if(size % 13){
        printf("wrong file size: %d\n",size);
        fclose(fp);
        return NULL;
    }
    *len = size/13;
    printf("size:  %d\n",*len);
    uint32 * keys;
    checkCuda(cudaMallocHost((void **)&keys,sizeof(uint32 )* (*len)));
    for(int i=0;i< *len;++i){
        char tmp[13];
        fread(tmp,13,sizeof(char),fp);
        *(keys+i)=*(uint32*)tmp;
    }
    fclose(fp);
    return keys;
}

// init memory
__global__ void kernel_init() {
    for (int i = 0; i < LEN; ++i)
        kernel_bucket_timestamp[i] = 0;

    for (int i = 0; i < LEN; ++i)
        kernel_bucket_freq[i] = 0;

    for (int i = 0; i < LAST_LAYER_BUCKET; ++i)
        mutex[i] = 0;
}


//   insert into AHeap
__device__ inline void insert_bucket(uint32  key,uint32 freq,uint32 timestamp){
    uint32 hash_val= hash0(key);
    uint32 hash_pos= hash_val % FIRST_LAYER_BUCKET;
    uint32* mutex_pos = (mutex + ((hash_val % FIRST_LAYER_BUCKET)>>(L1 +L2 -1)));
    uint32 layer_start_pos = 0;
    uint32 layer_bucket_num = FIRST_LAYER_BUCKET;
    bool next=true;
    int cnt=40; 
    while(next&&cnt--){
        int v=atomicCAS(mutex_pos,0,1);
    if(v==0){
    // d-left part insert
    for(int i=0;i< L1;++i){
        if(kernel_bucket_freq[layer_start_pos + hash_pos]== 0){
            kernel_bucket_timestamp[layer_start_pos+hash_pos] = timestamp;
            kernel_bucket_key[layer_start_pos+hash_pos] = key;
            kernel_bucket_freq[layer_start_pos+hash_pos] = freq;
            atomicExch(mutex+ ((hash_val% FIRST_LAYER_BUCKET)>>(L1+L2-1)),0);
            return;
        }
        if(kernel_bucket_key[layer_start_pos+hash_pos]==key){
            uint32 kernel_in_timestamp = kernel_bucket_timestamp[layer_start_pos + hash_pos];
            kernel_bucket_timestamp[layer_start_pos+hash_pos]= NEW(kernel_in_timestamp,timestamp);
            kernel_bucket_freq[layer_start_pos+hash_pos]+=freq;
            atomicExch(mutex+ ((hash_val% FIRST_LAYER_BUCKET)>>(L1+L2-1)),0);
            return;
        }
        else{
            uint32 kernel_in_timestamp = kernel_bucket_timestamp[layer_start_pos + hash_pos]; 
            uint32 kernel_in_freq = kernel_bucket_freq[layer_start_pos+hash_pos];
            if(timestamp==NEW(kernel_in_timestamp,timestamp)&&kernel_in_freq==1){
                kernel_bucket_timestamp[layer_start_pos+hash_pos] = timestamp;
                kernel_bucket_key[layer_start_pos+hash_pos]=key;
                kernel_bucket_freq[layer_start_pos+hash_pos] = freq;
                atomicExch(mutex+ ((hash_val% FIRST_LAYER_BUCKET)>>(L1+L2-1)),0);
                return;
            }
            if(timestamp==NEW(kernel_in_timestamp,timestamp)&& kernel_in_timestamp + 10000 < timestamp){
                kernel_bucket_timestamp[layer_start_pos+hash_pos] = timestamp;
                kernel_bucket_key[layer_start_pos+hash_pos]=key;
                kernel_bucket_freq[layer_start_pos+hash_pos] = freq;
                atomicExch(mutex+ ((hash_val% FIRST_LAYER_BUCKET)>>(L1+L2-1)),0);
                return;
            } 
            layer_start_pos += layer_bucket_num;
            layer_bucket_num /= 2;
            hash_pos /= 2;
        }
    }
    //  winner-tree part insert 
    for(int i=0;i< L2-1;++i){
        if(kernel_bucket_freq[layer_start_pos + hash_pos]== 0){
            kernel_bucket_timestamp[layer_start_pos+hash_pos] = timestamp;
            kernel_bucket_key[layer_start_pos+hash_pos]=key;
            kernel_bucket_freq[layer_start_pos+hash_pos] = freq;
            atomicExch(mutex+ ((hash_val% FIRST_LAYER_BUCKET)>>(L1+L2-1)),0);
            return;
        }
        if(kernel_bucket_key[layer_start_pos+hash_pos]==key){
            uint32 kernel_in_timestamp = kernel_bucket_timestamp[layer_start_pos + hash_pos];
            kernel_bucket_timestamp[layer_start_pos+hash_pos]= NEW(kernel_in_timestamp,timestamp);
            kernel_bucket_freq[layer_start_pos+hash_pos]+=freq;
            atomicExch(mutex+ ((hash_val% FIRST_LAYER_BUCKET)>>(L1+L2-1)),0);
            return;
        }
        else{
           // uint32 kernel_insert_weight = freq;
            uint32 kernel_in_timestamp= kernel_bucket_timestamp[layer_start_pos+hash_pos];
            uint32 kernel_in_freq=kernel_bucket_freq[layer_start_pos+hash_pos];
            uint32 kernel_in_key;
            kernel_in_key=kernel_bucket_key[layer_start_pos+hash_pos];
          //  uint32 kernel_in_weight = kernel_in_timestamp+kernel_in_freq;
            if( freq > kernel_in_freq){
                kernel_bucket_key[layer_start_pos+hash_pos]=key;
                kernel_bucket_timestamp[layer_start_pos+hash_pos]=timestamp;
                kernel_bucket_freq[layer_start_pos+hash_pos]=freq;
                key=kernel_in_key;
                freq = kernel_in_freq;
                timestamp =kernel_in_timestamp;
            }
            layer_start_pos += layer_bucket_num;
            layer_bucket_num /= 2;
            hash_pos /= 2; 
        }
    }
        next = false;
        atomicExch(mutex+ ((hash_val% FIRST_LAYER_BUCKET)>>(L1+L2-1)),0);
        
        
        return;
    }
    }
    return;

}


// kernel function, allocate packet to insert
__global__ void kernel_insert(uint32 * kernel_keys,uint32 start_timestamps, uint32 len){
    const uint32 id=threadIdx.x+blockIdx.x *blockDim.x;
    if (id >= len ) return;
    uint32  keys_at_bucket=kernel_keys[id];
    uint32 freq = 1;
    uint32 timestamp =  start_timestamps +id;
    insert_bucket(keys_at_bucket,freq,timestamp);
}

double expr_result[10];
int expr_pos=0;


// batch processing and multi-streaming
void insert(uint32 *keys,uint32 len){
    uint32 * kernel_keys;
    checkCuda(cudaMalloc((void **)&kernel_keys,sizeof(uint32)*len));
        
     
    cudaEvent_t startEvent,stopEvent;
    checkCuda( cudaEventCreate(&startEvent));
    checkCuda( cudaEventCreate(&stopEvent) );
    
    int batch_num = (len + BATCH_SIZE -1) / BATCH_SIZE;
    cudaStream_t stream[batch_num];
    for(int i=0;i<batch_num;++i)
        checkCuda(cudaStreamCreate(&stream[i]));
    


    checkCuda( cudaEventRecord(startEvent,0));

    for(uint32 i=0, left=len; i< len;i+=BATCH_SIZE,left-=BATCH_SIZE ){
        uint32 size= MIN(left,BATCH_SIZE);
        checkCuda(cudaMemcpyAsync(kernel_keys+i,keys+i,sizeof(uint32)*size,cudaMemcpyHostToDevice,stream[i/BATCH_SIZE]));
    }
    for(uint32 i=0, left=len; i< len;i+=BATCH_SIZE,left-=BATCH_SIZE ){
        uint32 size= MIN(left,BATCH_SIZE);
        kernel_insert<<<(size+BLOCK_SIZE-1)/BLOCK_SIZE,BLOCK_SIZE,0,stream[i/BATCH_SIZE]>>>(kernel_keys+i,i,size);    
    }
    for(uint32 i=0;i<batch_num;++i)
        checkCuda(cudaStreamSynchronize(stream[i]));
    checkCuda(cudaEventRecord(stopEvent,0));
    checkCuda(cudaEventSynchronize(stopEvent));
    
    float ms;
    checkCuda(cudaEventElapsedTime(&ms,startEvent,stopEvent));

    expr_result[expr_pos++] = len/ms/1000;
    cudaFree(kernel_keys);
    for(uint32 i=0;i<batch_num;++i)
    checkCuda(cudaStreamDestroy(stream[i]));
    

}

uint32* true_result(uint32 * keys,uint32 * len){
    unordered_map<uint32,uint32> s;
    for(int i=0;i<*len;++i) ++s[keys[i]];
    *len=s.size();
    uint32* res=(uint32*)malloc(sizeof(uint32)*(*len));
    int pos=0;
    for(auto p:s){
        keys[pos]=p.first;
        res[pos]=p.second;
        ++pos;
    }
    return res;
}

// Result query
uint32* query(uint32* keys,uint32 len){
    uint32* res=(uint32*)malloc(sizeof(uint32)*len);
    checkCuda(cudaMemcpyFromSymbol(bucket_key,kernel_bucket_key,LEN*sizeof(uint32)));
    checkCuda(cudaMemcpyFromSymbol(bucket_freq,kernel_bucket_freq,LEN*sizeof(uint32)));
    for(int i=0;i<len;++i){
        res[i]=0;
        uint32 hash_val=hash0(keys[i]);
        uint32 layer_start_pos = 0;
        uint32 layer_bucket_num = FIRST_LAYER_BUCKET;
        uint32 hash_pos=hash_val% FIRST_LAYER_BUCKET;
        for(int j=0;j<L1+L2;++j){
            if(bucket_key[layer_start_pos+hash_pos]==keys[i])
                res[i]+=bucket_freq[layer_start_pos+hash_pos];
            hash_pos /= 2;
            layer_start_pos +=layer_bucket_num;
            layer_bucket_num /= 2;
        }
    }
    return res;
}

void compare(uint32 * real,uint32* my,uint32 len){
    int accuracy = 0;
    double are =0;
    for(int i=0;i<len;++i){
        accuracy += (real[i]==my[i]);
        are += ((real[i]>my[i])?(real[i]-my[i]):(my[i]-real[i]))/(double)real[i];
    }
//    printf("%d %lf\n",len,accuracy/(double)len);  
//Notice about decreasing dataset package number or increasing buckets in AHeap
}

void experiment_filename(char * filename){
    uint32 len;
    uint32* keys=read_keys(filename,&len);
    kernel_init<<<1,1>>>();
	insert(keys,len);
    uint32 * true_res=true_result(keys,&len);
    uint32 * my_res=query(keys,len);
    compare(true_res,my_res,len);
    free(my_res);
    free(true_res);
    cudaFreeHost(keys);

}

int main(){
    char filename[100];
    printf("%d ",BATCH_SIZE);
/**/    for(int i=0;i<10;++i){
        sprintf(filename,"../../data/5s/%d.dat",i);
        experiment_filename(filename);
    }/**/
    sort(expr_result,expr_result+10);
    printf("%lf %lf %lf Mpps\n",
        (expr_result[0]+expr_result[1])/2,
        (expr_result[4]+expr_result[5])/2,
        (expr_result[8]+expr_result[9])/2
    );
    return 0;
    
}
