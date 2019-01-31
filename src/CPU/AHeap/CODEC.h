#ifndef EC_H
#define EC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <isa-l.h>	// use <isa-l.h> instead when linking against installed

#define MMAX 255
#define KMAX 255

typedef unsigned char u8;

static int gf_gen_decode_matrix_simple(u8 * encode_matrix,
                                       u8 * decode_matrix,
                                       u8 * invert_matrix,
                                       u8 * temp_matrix,
                                       u8 * decode_index,
                                       int * frag_err_list, int nerrs, int k, int m);


/*--------------usage------------
id: the switch stores the id-th block
seg: store the id-th block
-k Number of source fragments
-p Number of parity fragments
-l Length(Byte) of fragments
--------------usage------------*/
void encode(int id, char * key, char * seg,int __k, int __p, int __len)
{
    int k = __k, p = __p, len = __len;
    int i, j, m, c, e, ret;
    int nerrs = 0;

    // Fragment buffer pointers
    u8 *frag_ptrs[MMAX];
    u8 *recover_srcs[KMAX];
    u8 *recover_outp[KMAX];

    // Coefficient matrices
    u8 *encode_matrix, *decode_matrix;
    u8 *invert_matrix, *temp_matrix;
    u8 *g_tbls;
    u8 decode_index[MMAX];

    m = k + p;

    // Allocate coding matrices
    encode_matrix = (u8 *)malloc(m * k);
    decode_matrix = (u8 *)malloc(m * k);
    invert_matrix = (u8 *)malloc(m * k);
    temp_matrix = (u8 *)malloc(m * k);
    g_tbls = (u8 *)malloc(k * p * 32);

    // Allocate the src & parity buffers
    for (i = 0; i < m; i++) {
        if (NULL == (frag_ptrs[i] = (u8 *)malloc(len))) {
            printf("alloc error: Fail\n");
            return ;
        }
    }

    // Allocate buffers for recovered data
    for (i = 0; i < p; i++) {
        if (NULL == (recover_outp[i] = (u8 *)malloc(len))) {
            printf("alloc error: Fail\n");
            return ;
        }
    }

    // Fill sources with key
    for (i = 0; i < k; i++)
        for (j = 0; j < len; j++)
            frag_ptrs[i][j] = key[i*len+j];

    // Pick an encode matrix. A Cauchy matrix is a good choice as even
    // large k are always invertable keeping the recovery rule simple.
    gf_gen_cauchy1_matrix(encode_matrix, m, k);

    // Initialize g_tbls from encode matrix
    ec_init_tables(k, p, &encode_matrix[k * k], g_tbls);

    // Generate EC parity blocks from sources
    ec_encode_data(len, k, p, g_tbls, frag_ptrs, &frag_ptrs[k]);

    memcpy(seg,frag_ptrs[id],len);

    free(encode_matrix);
    free(decode_matrix);
    free(invert_matrix);
    free(temp_matrix);
    free(g_tbls);
    for(i = 0; i < m; ++i)
        free(frag_ptrs[i]);
    for(i = 0; i < p; ++i)
        free(recover_outp[i]);
    return;
}

void decode(char * key, int* frag_err_list, int nerrs, u8 **recover_srcs, int __k, int __p, int __len)
{

    int i, j, m, c, e, ret, count;
    int k = __k, p = __p, len = __len;	// Default params


    m = k + p;
    // Fragment buffer pointers
    u8 *frag_ptrs[MMAX];
    u8 *recover_outp[KMAX];


    // Coefficient matrices
    u8 *encode_matrix, *decode_matrix;
    u8 *invert_matrix, *temp_matrix;
    u8 *g_tbls;
    u8 decode_index[MMAX];


    // Allocate coding matrices
    encode_matrix = (u8 *)malloc(m * k);
    decode_matrix = (u8 *)malloc(m * k);
    invert_matrix = (u8 *)malloc(m * k);
    temp_matrix = (u8 *)malloc(m * k);
    g_tbls = (u8 *)malloc(k * p * 32);

    // Allocate the src & parity buffers
    for (i = 0; i < m; i++) {
        if (NULL == (frag_ptrs[i] = (u8 *)malloc(len))) {
            printf("alloc error: Fail\n");
            return ;
        }
    }

    // Allocate buffers for recovered data
    for (i = 0; i < p; i++) {
        if (NULL == (recover_outp[i] = (u8 *)malloc(len))) {
            printf("alloc error: Fail\n");
            return ;
        }
    }

    // Pick an encode matrix. A Cauchy matrix is a good choice as even
    // large k are always invertable keeping the recovery rule simple.
    gf_gen_cauchy1_matrix(encode_matrix, m, k);

    // Initialize g_tbls from encode matrix
    ec_init_tables(k, p, &encode_matrix[k * k], g_tbls);


    // Find a decode matrix to regenerate all erasures from remaining frags
    ret = gf_gen_decode_matrix_simple(encode_matrix, decode_matrix,
                                      invert_matrix, temp_matrix, decode_index,
                                      frag_err_list, nerrs, k, m);

    // Recover data
    ec_init_tables(k, nerrs, decode_matrix, g_tbls);
    ec_encode_data(len, k, nerrs, g_tbls, recover_srcs, recover_outp);

    // Check that recovered buffers are the same as original
    int srcCount  =  0;
    int outpCount =  0;
    int fragCount =  0;
    for(int j = 0;j<k;j++)
    {
        if(fragCount<nerrs && j==frag_err_list[fragCount])
        {
            memcpy(key+j*len, recover_outp[outpCount], len);
            outpCount++;
            fragCount++;
        }
        else
        {
            memcpy(key+j*len, recover_srcs[srcCount], len);
            srcCount++;
        }
    }

    free(encode_matrix);
    free(decode_matrix);
    free(invert_matrix);
    free(temp_matrix);
    free(g_tbls);
    for(i = 0; i < m; ++i)
        free(frag_ptrs[i]);
    for(i = 0; i < p; ++i)
        free(recover_outp[i]);
    return ;
}

/* Generate decode matrix from encode matrix and erasure list */
static int gf_gen_decode_matrix_simple(u8 * encode_matrix,
                                       u8 * decode_matrix,
                                       u8 * invert_matrix,
                                       u8 * temp_matrix,
                                       u8 * decode_index, int * frag_err_list, int nerrs, int k,
                                       int m)
{
    int i, j, p, r;
    int nsrcerrs = 0;
    u8 s, *b = temp_matrix;
    u8 frag_in_err[MMAX];

    memset(frag_in_err, 0, sizeof(frag_in_err));

    // Order the fragments in erasure for easier sorting
    for (i = 0; i < nerrs; i++) {
        if (frag_err_list[i] < k)
            nsrcerrs++;
        frag_in_err[frag_err_list[i]] = 1;
    }

    // Construct b (matrix that encoded remaining frags) by removing erased rows
    for (i = 0, r = 0; i < k; i++, r++) {
        while (frag_in_err[r])
            r++;
        for (j = 0; j < k; j++)
            b[k * i + j] = encode_matrix[k * r + j];
        decode_index[i] = r;
    }

    // Invert matrix to get recovery matrix
    if (gf_invert_matrix(b, invert_matrix, k) < 0)
        return -1;

    // Get decode matrix with only wanted recovery rows
    for (i = 0; i < nerrs; i++) {
        if (frag_err_list[i] < k)	// A src err
            for (j = 0; j < k; j++)
                decode_matrix[k * i + j] =
                        invert_matrix[k * frag_err_list[i] + j];
    }

    // For non-src (parity) erasures need to multiply encode matrix * invert
    for (p = 0; p < nerrs; p++) {
        if (frag_err_list[p] >= k) {	// A parity err
            for (i = 0; i < k; i++) {
                s = 0;
                for (j = 0; j < k; j++)
                    s ^= gf_mul(invert_matrix[j * k + i],
                                encode_matrix[k * frag_err_list[p] + j]);
                decode_matrix[k * p + i] = s;
            }
        }
    }
    return 0;
}

#endif
