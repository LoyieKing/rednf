// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_shm_hash_expire.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SHM_HASH_EXPIRE_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SHM_HASH_EXPIRE_H_H_

#include "output/commlib/zenlib/release/include/zen_shm_predefine.h"
#include "src/formmog/tss_sdk/plugins/uic/uic_user_input_info.h"
#include <bits/stl_function.h>
#include <stddef.h>

extern size_t idx; // line 667
extern size_t i; // line 300
extern const size_t LIST_ADD_NODE_NUMBER; // line 245
extern const size_t num_primes_list; // line 295
extern size_t num_primes; // line 299
extern /*anon struct*/ int hash_fun; // line 523
extern /*anon struct*/ int get_key; // line 515
extern size_t first; // line 668
extern /*anon struct*/ int equal_key; // line 671
extern size_t prev; // line 768
extern size_t itseq; // line 769
extern size_t freenext; // line 485
extern size_t pos_next; // line 487
extern size_t pos_prev; // line 488
extern size_t sz_alloc; // line 323
extern /*anon struct*/ int *hashhead; // line 349
extern shm_hashtable_expire<tss_sdk_impl::UicUserInputInfo,long long unsigned int,ZEN_LIB::smem_hash<long long unsigned int>,tss_sdk_impl::UicExtractUserInfoKey,std::equal_to<long long unsigned int>,tss_sdk_impl::UicTimeOutProc> *instance; // line 368
extern char *tmp_base; // line 372
extern /*anon struct*/ int *pindex; // line 415
extern size_t newnode; // line 597
extern size_t list_idx; // line 1001
extern size_t expire_num; // line 1002
extern size_t del_iter; // line 1009
extern /*anon struct*/ int wash_fun; // line 1010
extern /*anon struct*/ int iter_tmp; // line 1014
extern size_t oldseq; // line 103
extern size_t bucket; // line 110

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SHM_HASH_EXPIRE_H_H_
