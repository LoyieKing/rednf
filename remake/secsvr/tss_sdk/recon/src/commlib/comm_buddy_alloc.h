// Reconstructed from libtss_sdk.so.1.0.394, CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/commlib/framework/comm_buddy_alloc.cpp
//
// The original declarations lived in
//   src/commlib/framework/comm_buddy_alloc.h
// Line numbers quoted in the comments refer to that header.  Layouts are taken
// from the CU's DWARF (sizeof: BuddyBitSet 8, BuddyBufDesc 32, BuddyAlloc 476,
// MultiBuddyAlloc 108) and were re-checked against every member access in the
// ORIG disassembly.
//
// Quirks kept on purpose:
//   * BuddyBufDesc::reset() does not touch self_desc_additional_; only
//     self_ptr_additional_ is cleared.
//   * BuddyAlloc::free/multi free invalidate the ptr_desc_map entry with the key
//     value NULL, not with the released pointer.
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BUDDY_ALLOC_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BUDDY_ALLOC_H_H_

#include <stddef.h>
#include <sys/types.h>

#include <functional>
#include <ext/hash_map>

// ---------------------------------------------------------------------------
// BuddyBitSet (header line 24): bit array addressed MSB first, 8 bits/byte.
// ---------------------------------------------------------------------------
class BuddyBitSet {
public:
    BuddyBitSet(unsigned int bit_num);      // 27
    ~BuddyBitSet();                         // 28

    int init();                             // 31
    void reverse(unsigned int pos);         // 34
    bool is_need_merge(unsigned int index); // 37
    void reset(unsigned int index);         // 40

private:
    unsigned char *bit_ptr_;                // 43
    size_t bit_ptr_byte_len_;               // 44
};

// ---------------------------------------------------------------------------
// BuddyBufDesc (header line 48): one block descriptor, 32 bytes.
// ---------------------------------------------------------------------------
struct BuddyBufDesc {
    BuddyBufDesc() {}                       // 49
    ~BuddyBufDesc() {}                      // 54

    void reset() {                          // 58
        ptr_ = NULL;                        // 60
        ptr_len_ = 0;                       // 61

        used_len_ = 0;                      // 63

        pre_ = NULL;                        // 65
        next_ = NULL;                       // 66

        order_ = -1;                        // 68
        block_index_ = -1;                  // 69

        self_ptr_additional_ = false;       // 72
    }

    unsigned char *ptr_;                    // 75
    size_t ptr_len_;                        // 76
    size_t used_len_;                       // 78
    BuddyBufDesc *pre_;                     // 80
    BuddyBufDesc *next_;                    // 81
    int order_;                             // 83
    ssize_t block_index_;                   // 84
    bool self_desc_additional_;             // 86
    bool self_ptr_additional_;              // 87
};

// ---------------------------------------------------------------------------
// BuddyPointerHashFun (header line 91): the ORIG hashes a pointer to itself.
// ---------------------------------------------------------------------------
struct BuddyPointerHashFun {
    size_t operator()(void *ptr) const {    // 92
        return (size_t)ptr;
    }
};

typedef __gnu_cxx::hash_map<void*, BuddyBufDesc*, BuddyPointerHashFun,
                             std::equal_to<void*>,
                             std::allocator<BuddyBufDesc*> >
        BUDDY_ALLOC_POINTER_DESC_HASHMAP;

typedef __gnu_cxx::hash_map<void*, class BuddyAlloc*, BuddyPointerHashFun,
                             std::equal_to<void*>,
                             std::allocator<class BuddyAlloc*> >
        BUDDY_ALLOC_POINTER_POOL_HASHMAP;

typedef __gnu_cxx::hash_map<void*, unsigned int, BuddyPointerHashFun,
                             std::equal_to<void*>,
                             std::allocator<unsigned int> >
        BUDDY_ALLOC_POINTER_SIZE_HASHMAP;

// ---------------------------------------------------------------------------
// BuddyAlloc (header line 101): single buddy system over one buffer.
// ---------------------------------------------------------------------------
class MultiBuddyAlloc;

class BuddyAlloc {
public:
    BuddyAlloc();                           // 104
    ~BuddyAlloc();                          // 105

    int init(unsigned int *size, unsigned int base_size,            // 114
             bool lack_use_system);
    void *alloc(unsigned int size);                                 // 117
    void free(void *ptr);                                           // 120
    int fini();                                                     // 123
    void print_info();                                              // 125

private:
    static const size_t BUDDY_MIN_BASE_SIZE = 16;           // 131
    static const int BUDDY_BUF_DESC_POOL_MAX_NUM = 100000;
    static const int BUDDY_FREE_CLASSIFY_MAX_NUM = 32;      // 133
    static const int BUDDY_TAOTAL_SIZE_MAX_INDEX = 32;
    static const unsigned int BUDDY_UNSIGNED_INT_MAX = 0xffffffffu;

    int init_buddy_on_buffer(unsigned int size, unsigned int base_size,   // 194
                             unsigned char *buffer, unsigned int buffer_len,
                             unsigned int *out_buffer_len,
                             bool lack_use_system);
    int init_find_array();                                  // 197
    static int count_total_size(unsigned int size, unsigned int base_size, // 200
                                int *order, unsigned int *total_size);
    int get_power_by_order(int order, unsigned int *power);         // 203
    int get_next_order_and_power(unsigned int size, int *order,     // 206
                                 unsigned int *power);
    int init_buffer();                                      // 209
    int init_free_list_array();                             // 212
    int insert_desc_to_free_list_array(BuddyBufDesc *desc); // 215
    int del_desc_from_free_list_array(BuddyBufDesc *desc);  // 218
    int get_block_index_by_order(BuddyBufDesc *desc);       // 221
    int split(int order, int to_order);                     // 224
    BuddyBufDesc *find_buddy(BuddyBufDesc *desc);           // 227
    int merge(BuddyBufDesc *desc, int *merge_series);       // 230
    int init_bitset();                                      // 234
    int fini_bitset();                                      // 236
    int modify_bitset(BuddyBufDesc *desc);                  // 238
    BuddyBufDesc *get_ptr_desc(void *ptr);                  // 242
    int init_desc_pool();                                   // 246
    int fini_desc_pool();                                   // 248
    BuddyBufDesc *get_desc_obj_from_pool();                 // 250
    int put_desc_obj_to_pool(BuddyBufDesc *desc);           // 252

    // +0x00: every data member below is private (DWARF acc=3) and is reached
    //        by MultiBuddyAlloc through the friend declaration further down.
    unsigned char *buffer_;                 // 139
    size_t buffer_len_;                     // 140
    bool is_buffer_self_alloc_;             // 141
    bool lack_use_system_;                  // 142

    size_t base_size_;                      // 145
    int need_order_;                        // 146

    size_t size_used_;                      // 149
    size_t size_allocated_;                 // 150

    unsigned int alloc_times_;              // 153
    unsigned int alloc_split_series_;       // 154
    unsigned int free_times_;               // 155
    unsigned int free_merge_series_;        // 156

    size_t max_size_can_alloc_;             // 159
    int max_size_can_alloc_order_;          // 160

    unsigned int use_system_alloc_times_;   // 163
    unsigned int use_system_free_times_;    // 164

    BuddyBufDesc *desc_obj_pool_;           // 167
    unsigned char *desc_obj_pool_buffer_;   // 170
    BuddyBufDesc *additional_desc_obj_pool_; // 173

    BUDDY_ALLOC_POINTER_DESC_HASHMAP ptr_desc_map_;         // 176

    BuddyBufDesc *free_classify_list_array_[BUDDY_FREE_CLASSIFY_MAX_NUM]; // 179
    BuddyBitSet *classify_bitset_[BUDDY_FREE_CLASSIFY_MAX_NUM];           // 182
    size_t find_order_and_power_array[BUDDY_FREE_CLASSIFY_MAX_NUM];       // 185

    friend class MultiBuddyAlloc;
};

// ---------------------------------------------------------------------------
// MultiBuddyAlloc (header line 260): several buddy systems of growing base
// size, 108 bytes.
// ---------------------------------------------------------------------------
class MultiBuddyAlloc {
public:
    MultiBuddyAlloc();                                              // 262
    ~MultiBuddyAlloc();                                             // 263

    int init(unsigned int *size, int num_of_steps,                  // 274
             bool lack_use_system);
    void *alloc(unsigned int size);                                 // 278
    void free(void *ptr);                                           // 281
    // The ORIG emits no exception region at the call sites of
    // MultiBuddyAlloc::fini() (checked: ~MultiBuddyAlloc is 0x66 bytes there
    // and holds no cleanup landing pad).  Marking the callee nothrow
    // reproduces that code generation exactly; the body of fini() itself is
    // unchanged.
    int fini() __attribute__((__nothrow__));                        // 284
    void print_info();                                              // 287

private:
    static const int MULTIBUDDY_DEFAULT_STEPS = 4;                  // 291
    static const int MULTIBUDDY_BUDDY_POOL_MAX_NUM = 8;             // 292
    static const int MULTIBUDDY_BASE_SIZE_START = 16;               // 293
    static const int MULTIBUDDY_MAX_BASE_SIZE = 32;                 // 294
    static const int MULTIBUDDY_BASE_SIZE_MAX_DISTANCE = 16;        // 295
    static const unsigned int MULTIBUDDY_UNSIGNED_INT_MAX = 0xffffffffu; // 296
    static const int MULTIBUDDY_BUDDY_BUFFER_INTERVAL = 4;          // 297
    static const int MULTIBUDDY_TAOTAL_SIZE_MAX_INDEX = 30;         // 298

    int count_total_size(unsigned int size, int num_of_steps,       // 329
                         unsigned int *total_size);

    unsigned char *buffer_;                                         // 302
    size_t buffer_len_;                                             // 303

    bool lack_use_system_;                                          // 305

    BuddyAlloc *buddy_pool_array_[MULTIBUDDY_BUDDY_POOL_MAX_NUM];    // 308

    int num_of_steps_;                                              // 311

    int buddy_max_order_;                                           // 314

    BUDDY_ALLOC_POINTER_POOL_HASHMAP ptr_buddy_map_;                // 317
    BUDDY_ALLOC_POINTER_SIZE_HASHMAP ptr_system_map_;               // 320

    unsigned int alloc_times_;                                      // 323
    unsigned int use_system_alloc_times_;                           // 324
    unsigned int free_times_;                                       // 325
    unsigned int use_system_free_times_;                            // 326
};

#endif  // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_BUDDY_ALLOC_H_H_
