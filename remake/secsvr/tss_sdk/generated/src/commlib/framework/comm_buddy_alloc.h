// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_buddy_alloc.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BUDDY_ALLOC_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BUDDY_ALLOC_H_H_

#include <stddef.h>
#include <unistd.h>

// sizeof = 8
struct BuddyBitSet { // line 24
private:
unsigned char *bit_ptr_;
size_t bit_ptr_byte_len_;
public:
BuddyBitSet(size_t arg0); // line 27
~BuddyBitSet(); // line 28
int init(); // line 31
void reverse(size_t arg0); // line 34
bool is_need_merge(size_t arg0); // line 37
void reset(size_t arg0); // line 40
};
// sizeof = 32
struct BuddyBufDesc { // line 48
public:
unsigned char *ptr_;
size_t ptr_len_;
size_t used_len_;
BuddyBufDesc *pre_;
BuddyBufDesc *next_;
int order_;
ssize_t block_index_;
bool self_desc_additional_;
bool self_ptr_additional_;
BuddyBufDesc(); // line 49
~BuddyBufDesc(); // line 54
void reset(); // line 58
};
// sizeof = 1
struct BuddyPointerHashFun { // line 91
public:
size_t operator()(void *arg0) const; // line 92
};
typedef hash_map<void*,BuddyBufDesc*,BuddyPointerHashFun,std::equal_to<void*>,std::allocator<BuddyBufDesc*> > BUDDY_ALLOC_POINTER_DESC_HASHMAP;
// sizeof = 476
struct BuddyAlloc { // line 101
private:
static const size_t BUDDY_MIN_BASE_SIZE;
static const int BUDDY_BUF_DESC_POOL_MAX_NUM;
static const int BUDDY_FREE_CLASSIFY_MAX_NUM;
static const int BUDDY_TAOTAL_SIZE_MAX_INDEX;
static const unsigned int BUDDY_UNSIGNED_INT_MAX;
unsigned char *buffer_;
size_t buffer_len_;
bool is_buffer_self_alloc_;
bool lack_use_system_;
size_t base_size_;
int need_order_;
size_t size_used_;
size_t size_allocated_;
unsigned int alloc_times_;
unsigned int alloc_split_series_;
unsigned int free_times_;
unsigned int free_merge_series_;
size_t max_size_can_alloc_;
int max_size_can_alloc_order_;
unsigned int use_system_alloc_times_;
unsigned int use_system_free_times_;
BuddyBufDesc *desc_obj_pool_;
unsigned char *desc_obj_pool_buffer_;
BuddyBufDesc *additional_desc_obj_pool_;
BUDDY_ALLOC_POINTER_DESC_HASHMAP ptr_desc_map_;
BuddyBufDesc *free_classify_list_array_[];
BuddyBitSet *classify_bitset_[];
size_t find_order_and_power_array[];
public:
BuddyAlloc(); // line 104
~BuddyAlloc(); // line 105
int init(size_t *arg0, size_t arg1, bool arg2); // line 114
void * alloc(size_t arg0); // line 117
void free(void *arg0); // line 120
int fini(); // line 123
void print_info(); // line 125
private:
int init_buddy_on_buffer(size_t arg0, size_t arg1, unsigned char *arg2, size_t arg3, size_t *arg4, bool arg5); // line 194
int init_find_array(); // line 197
static int count_total_size(size_t arg0, size_t arg1, int *arg2, size_t *arg3); // line 200
int get_power_by_order(int arg0, size_t *arg1); // line 203
int get_next_order_and_power(size_t arg0, int *arg1, size_t *arg2); // line 206
int init_buffer(); // line 209
int init_free_list_array(); // line 212
int insert_desc_to_free_list_array(BuddyBufDesc *arg0); // line 215
int del_desc_from_free_list_array(BuddyBufDesc *arg0); // line 218
int get_block_index_by_order(BuddyBufDesc *arg0); // line 221
int split(int arg0, int arg1); // line 224
BuddyBufDesc * find_buddy(BuddyBufDesc *arg0); // line 227
int merge(BuddyBufDesc *arg0, int *arg1); // line 230
int init_bitset(); // line 234
int fini_bitset(); // line 236
int modify_bitset(BuddyBufDesc *arg0); // line 238
BuddyBufDesc * get_ptr_desc(void *arg0); // line 242
int init_desc_pool(); // line 246
int fini_desc_pool(); // line 248
BuddyBufDesc * get_desc_obj_from_pool(); // line 250
int put_desc_obj_to_pool(BuddyBufDesc *arg0); // line 252
};
extern const size_t BUDDY_MIN_BASE_SIZE; // line 131
extern const int BUDDY_FREE_CLASSIFY_MAX_NUM; // line 133
extern const int MULTIBUDDY_DEFAULT_STEPS; // line 291
extern const int MULTIBUDDY_BUDDY_POOL_MAX_NUM; // line 292
extern const int MULTIBUDDY_BASE_SIZE_START; // line 293

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BUDDY_ALLOC_H_H_
