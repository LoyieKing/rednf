// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_shm_hash_expire.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_EXPIRE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_EXPIRE_H_H_

#include "output/commlib/zenlib/release/include/zen_shm_predefine.h"
#include "src/formmog/secagent/secagent_user_mgr.h"
#include <bits/stl_function.h>
#include <stddef.h>

extern size_t idx; // line 863
extern size_t i; // line 300
extern const size_t LIST_ADD_NODE_NUMBER; // line 245
extern size_t expire_num; // line 1002
extern const size_t num_primes_list; // line 295
extern size_t num_primes; // line 299
extern /*anon struct*/ int hash_fun; // line 523
extern ExtractSecAgentUserKey get_key; // line 515
extern size_t first; // line 864
extern /*anon struct*/ int equal_key; // line 867
extern size_t first_prv; // line 877
extern size_t first_nxt; // line 878
extern size_t prev; // line 726
extern size_t freenext; // line 485
extern size_t pos_next; // line 487
extern size_t pos_prev; // line 488
extern /*anon struct*/ int &tmp; // line 153
extern size_t oldseq; // line 103
extern size_t bucket; // line 110
extern size_t sz_alloc; // line 323
extern /*anon struct*/ int *hashhead; // line 349
extern shm_hashtable_expire<SecAgentUserInfo,long long unsigned int,ZEN_LIB::smem_hash<long long unsigned int>,ExtractSecAgentUserKey,std::equal_to<long long unsigned int>,TimeOutProc> *instance; // line 368
extern char *tmp_base; // line 372
extern /*anon struct*/ int *pindex; // line 415
extern size_t list_idx; // line 1001
extern size_t del_iter; // line 1009
extern TimeOutProc wash_fun; // line 1010
extern /*anon struct*/ int iter_tmp; // line 1014
extern size_t itseq; // line 769
extern size_t newnode; // line 597

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_EXPIRE_H_H_
