// Reconstructed from libtss_sdk.so.1.0.394, CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/commlib/framework/comm_buddy_alloc.cpp
// Original address range 0x000d2db0-0x000d57a0 (the 51 non-STL symbols of the
// CU; the template instantiations of <ext/hash_map> / <vector> are emitted by
// the headers and are not repeated here).
//
// Reconstructed instruction-by-instruction from the ORIG disassembly plus the
// CU's DWARF line table.  Behavioural quirks kept on purpose (all verified in
// the ORIG code):
//   * BuddyAlloc::get_desc_obj_from_pool() dereferences the new pool head
//     unconditionally (`desc_obj_pool_->pre_ = NULL`): handing out the very
//     last descriptor of desc_obj_pool_ writes through a NULL pointer.
//   * BuddyAlloc::get_block_index_by_order() prints the messages swapped with
//     respect to the checks ("block_num == 0" for size_per_block == 0 and
//     "size_per_block == 0" for block_num == 0) and returns 0 on success.
//   * BuddyAlloc::free()/MultiBuddyAlloc::free() erase the pointer map entry
//     keyed by the value of desc->ptr_ after it was set to NULL.
//   * Counter overflow is handled by comparing with BUDDY_UNSIGNED_INT_MAX.
#include "src/commlib/comm_buddy_alloc.h"

#include <new>
#include <string.h>

#include <iostream>

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

// zen_trace_log_msg.h: ZEN_Trace_LogMsg is a struct with static members.
struct ZEN_Trace_LogMsg {
    static void debug_errorex(const char *fmt, ...);
    static void debug_debugEx(const char *fmt, ...);
};

// ---------------------------------------------------------------------------
// BuddyBitSet
// ---------------------------------------------------------------------------

BuddyBitSet::BuddyBitSet(unsigned int bit_num) {    // line 22
    bit_ptr_ = NULL;
    bit_ptr_byte_len_ = (bit_num >> 3) + 1;
}

BuddyBitSet::~BuddyBitSet() {                       // line 28
    if (bit_ptr_ != NULL) {
        operator delete[](bit_ptr_);
        bit_ptr_ = NULL;
    }
}

void BuddyBitSet::reverse(unsigned int pos) {        // line 37
    if ((pos >> 3) + 1 > bit_ptr_byte_len_) {
        ZEN_Trace_LogMsg::debug_errorex("[buddy]invalid pos argument");
        return;
    }
    if (0 == ((bit_ptr_[pos >> 3] >> ((~pos) & 0x7)) & 0x1)) {
        bit_ptr_[pos >> 3] |= (unsigned char)(0x1 << ((~pos) & 0x7));
    } else {
        bit_ptr_[pos >> 3] &= (unsigned char)~(0x1 << ((~pos) & 0x7));
    }
}

bool BuddyBitSet::is_need_merge(unsigned int index) {   // line 61
    return 0 == ((bit_ptr_[index >> 3] >> ((~index) & 0x7)) & 0x1);
}

int BuddyBitSet::init() {                           // line 69
    bit_ptr_ = new (std::nothrow) unsigned char[bit_ptr_byte_len_];
    if (bit_ptr_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyBitSet::init alloc failed");
        return -1;
    }
    memset(bit_ptr_, 0, bit_ptr_byte_len_);
    return 0;
}

void BuddyBitSet::reset(unsigned int index) {       // line 85
    bit_ptr_[index >> 3] &= (unsigned char)~(0x1 << ((~index) & 0x7));
}

// ---------------------------------------------------------------------------
// BuddyAlloc
// ---------------------------------------------------------------------------

BuddyAlloc::BuddyAlloc()                            // line 95
        : buffer_(NULL),
          buffer_len_(0),
          is_buffer_self_alloc_(false),
          lack_use_system_(false),
          base_size_(0),
          need_order_(0),
          size_used_(0),
          size_allocated_(0),
          alloc_times_(0),
          alloc_split_series_(0),
          free_times_(0),
          free_merge_series_(0),
          max_size_can_alloc_(0),
          max_size_can_alloc_order_(0),
          use_system_alloc_times_(0),
          use_system_free_times_(0),
          desc_obj_pool_(NULL),
          desc_obj_pool_buffer_(NULL),
          additional_desc_obj_pool_(NULL),
          ptr_desc_map_() {                         // line 114
}

BuddyAlloc::~BuddyAlloc() {                         // line 119
}

int BuddyAlloc::init(unsigned int *size, unsigned int base_size,    // line 123
                     bool lack_use_system) {
    if (size == NULL || *size <= BUDDY_MIN_BASE_SIZE) {
        ZEN_Trace_LogMsg::debug_errorex("[buddy]BuddyAlloc::init invalid args");
        fini();
        return -1;
    }
    if (base_size < BUDDY_MIN_BASE_SIZE) {
        base_size = BUDDY_MIN_BASE_SIZE;
    }
    base_size_ = base_size;
    lack_use_system_ = lack_use_system;
    if (0 != count_total_size(*size, base_size_, &need_order_, &buffer_len_)) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init count_total_size failed");
        fini();
        return -2;
    }
    *size = buffer_len_;
    max_size_can_alloc_ = buffer_len_;
    max_size_can_alloc_order_ = need_order_;
    init_find_array();
    if (0 != init_buffer()) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init init_buffer failed");
        fini();
        return -3;
    }
    if (0 != init_desc_pool()) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init init_desc_pool failed");
        fini();
        return -4;
    }
    if (0 != init_bitset()) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init init_bitset failed");
        fini();
        return -5;
    }
    if (0 != init_free_list_array()) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init init_free_list_array failed");
        fini();
        return -6;
    }
    return 0;
}

int BuddyAlloc::init_free_list_array() {            // line 198
    for (int i = 0; i < BUDDY_FREE_CLASSIFY_MAX_NUM; ++i) {
        free_classify_list_array_[i] = NULL;
    }
    BuddyBufDesc *desc = get_desc_obj_from_pool();
    if (desc == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_free_list_array "
                "get_desc_obj_from_pool failed");
        return -1;
    }
    desc->ptr_ = buffer_;
    desc->ptr_len_ = buffer_len_;
    desc->order_ = need_order_;
    desc->block_index_ = 0;
    ptr_desc_map_[buffer_] = desc;
    free_classify_list_array_[need_order_] = desc;
    return 0;
}

int BuddyAlloc::init_bitset() {                     // line 229
    for (int i = 0; i < need_order_; ++i) {
        unsigned int power = 0;
        if (0 != get_power_by_order(i, &power) || 0 == power) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]BuddyAlloc::init_bitset "
                    "get_power_by_order failed");
            return -1;
        }
        BuddyBitSet *bitset = new (std::nothrow)
                BuddyBitSet(buffer_len_ / (base_size_ * power * 2));
        if (bitset == NULL) {
            classify_bitset_[i] = NULL;
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]BuddyAlloc::init_bitset "
                    "new(std::nothrow) BuddyBitSet failed");
            return -1;
        }
        classify_bitset_[i] = bitset;
        if (0 != classify_bitset_[i]->init()) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]BuddyAlloc::init_bitset "
                    "classify_bitset_ init failed");
            return -1;
        }
    }
    return 0;
}

int BuddyAlloc::fini_bitset() {                     // line 270
    for (int i = 0; i < BUDDY_FREE_CLASSIFY_MAX_NUM; ++i) {
        if (classify_bitset_[i] != NULL) {
            delete classify_bitset_[i];
            classify_bitset_[i] = NULL;
        }
    }
    return 0;
}

int BuddyAlloc::init_buffer() {                     // line 286
    buffer_ = new (std::nothrow) unsigned char[buffer_len_];
    if (buffer_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_buffer "
                "new(std::nothrow) unsigned char failed");
        return -1;
    }
    is_buffer_self_alloc_ = true;
    return 0;
}

int BuddyAlloc::fini() {                            // line 301
    fini_bitset();
    fini_desc_pool();
    if (is_buffer_self_alloc_ && buffer_ != NULL) {
        operator delete[](buffer_);
        buffer_ = NULL;
    }
    return 0;
}

int BuddyAlloc::init_desc_pool() {                  // line 316
    desc_obj_pool_buffer_ = new (std::nothrow) unsigned char
            [BUDDY_BUF_DESC_POOL_MAX_NUM * sizeof(BuddyBufDesc)];
    if (desc_obj_pool_buffer_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_desc_pool "
                "new(std::nothrow) unsigned char failed");
        return -1;
    }
    for (unsigned int offset = 0;
         offset < (unsigned int)(BUDDY_BUF_DESC_POOL_MAX_NUM
                                 * sizeof(BuddyBufDesc));
         offset += sizeof(BuddyBufDesc)) {
        BuddyBufDesc *desc =
                (BuddyBufDesc *)(desc_obj_pool_buffer_ + offset);
        if (desc == NULL) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]BuddyAlloc::init_desc_pool "
                    "new(std::nothrow) BuddyBufDesc failed");
            return -1;
        }
        desc->reset();
        if (desc_obj_pool_ != NULL) {
            desc->next_ = desc_obj_pool_;
        }
        desc_obj_pool_ = desc;
    }
    return 0;
}

int BuddyAlloc::fini_desc_pool() {                  // line 351
    while (desc_obj_pool_ != NULL) {
        desc_obj_pool_ = desc_obj_pool_->next_;
        if (desc_obj_pool_ != NULL) {
            desc_obj_pool_->pre_ = NULL;
        }
    }
    if (desc_obj_pool_buffer_ != NULL) {
        operator delete[](desc_obj_pool_buffer_);
        desc_obj_pool_buffer_ = NULL;
    }
    while (additional_desc_obj_pool_ != NULL) {
        BuddyBufDesc *desc = additional_desc_obj_pool_;
        additional_desc_obj_pool_ = desc->next_;
        operator delete(desc);
    }
    return 0;
}

int BuddyAlloc::init_buddy_on_buffer(unsigned int size,     // line 390
                                     unsigned int base_size,
                                     unsigned char *buffer,
                                     unsigned int buffer_len,
                                     unsigned int *out_buffer_len,
                                     bool lack_use_system) {
    if (size <= BUDDY_MIN_BASE_SIZE || buffer == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_buddy_on_buffer invalid args");
        fini();
        return -1;
    }
    if (base_size < BUDDY_MIN_BASE_SIZE) {
        base_size = BUDDY_MIN_BASE_SIZE;
    }
    base_size_ = base_size;
    lack_use_system_ = lack_use_system;
    if (0 != count_total_size(size, base_size_, &need_order_, &buffer_len_)) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_buddy_on_buffer "
                "count_total_size failed");
        fini();
        return -2;
    }
    if (buffer_len_ > buffer_len) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_buddy_on_buffer not enough ptr_len");
        fini();
        return -3;
    }
    buffer_ = buffer;
    is_buffer_self_alloc_ = false;
    if (out_buffer_len != NULL) {
        *out_buffer_len = buffer_len_;
    }
    max_size_can_alloc_ = buffer_len_;
    max_size_can_alloc_order_ = need_order_;
    init_find_array();
    if (0 != init_desc_pool()) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_buddy_on_buffer init_desc_pool failed");
        fini();
        return -4;
    }
    if (0 != init_bitset()) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_buddy_on_buffer init_bitset failed");
        fini();
        return -1;
    }
    if (0 != init_free_list_array()) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::init_free_list_array failed");
        fini();
        return -5;
    }
    return 0;
}

int BuddyAlloc::init_find_array() {                 // line 473
    for (int i = 0; i < BUDDY_FREE_CLASSIFY_MAX_NUM; ++i) {
        unsigned int power = 0;
        get_power_by_order(i, &power);
        find_order_and_power_array[i] = base_size_ * power;
    }
    return 0;
}

int BuddyAlloc::count_total_size(unsigned int size,     // line 487
                                 unsigned int base_size,
                                 int *order,
                                 unsigned int *total_size) {
    if (size == 0 || order == NULL || total_size == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::count_total_size invalid args");
        return -1;
    }
    unsigned int power = base_size;
    int ord = 0;
    while (size > power) {
        power += power;
        ++ord;
    }
    if (ord > BUDDY_TAOTAL_SIZE_MAX_INDEX) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::count_total_size size is too large");
        return -2;
    }
    *order = ord;
    *total_size = power;
    return 0;
}

int BuddyAlloc::get_next_order_and_power(unsigned int size,     // line 518
                                         int *order,
                                         unsigned int *power) {
    if (size == 0 || order == NULL || power == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::get_next_order_and_power invalid args");
        return -1;
    }
    if (size < base_size_) {
        *order = 0;
        *power = base_size_;
        return 0;
    }
    if (size > buffer_len_) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::get_next_order_and_power size is too large");
        return -2;
    }
    // ORIG lower_bound over find_order_and_power_array[0, need_order_): the
    // last step keeps the index of an exact hit (array[low] == size).
    int low = 0;
    int high = need_order_;
    int found = 0;
    if (high > 0) {
        low = 0;
        while (true) {
            int mid = (low + high) >> 1;
            size_t value = find_order_and_power_array[mid];
            if (size >= value) {
                if (size <= value) {       // size == find_order_and_power_array[mid]
                    found = mid;
                    break;
                }
                low = mid;
            } else {
                high = mid;
            }
            if (high == low + 1) {
                if (size == find_order_and_power_array[low]) {
                    found = low;
                } else {
                    found = high;
                }
                break;
            }
            if (low >= high) {
                found = mid;
                break;
            }
        }
    } else {
        found = low;
    }
    *order = found;
    *power = (unsigned int)find_order_and_power_array[found];
    return 0;
}

int BuddyAlloc::get_power_by_order(int order, unsigned int *power) {    // line 582
    if (order < 0 || power == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::get_power_by_order invalid args");
        return -1;
    }
    if (0 == order) {
        *power = 1;
    } else {
        *power = (unsigned int)(0x1 << order);
    }
    return 0;
}

BuddyBufDesc *BuddyAlloc::get_desc_obj_from_pool() {    // line 604
    if (desc_obj_pool_ != NULL) {
        BuddyBufDesc *desc = desc_obj_pool_;
        desc_obj_pool_ = desc_obj_pool_->next_;
        desc_obj_pool_->pre_ = NULL;
        desc->reset();
        return desc;
    }
    BuddyBufDesc *desc = NULL;
    if (additional_desc_obj_pool_ != NULL) {
        desc = additional_desc_obj_pool_;
        additional_desc_obj_pool_ = desc->next_;
    } else {
        desc = new (std::nothrow) BuddyBufDesc();
        if (desc == NULL) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]BuddyAlloc::get_desc_obj_from_pool "
                    "new(std::nothrow) BuddyBufDesc failed");
            return NULL;
        }
    }
    desc->reset();
    desc->self_desc_additional_ = true;
    return desc;
}

int BuddyAlloc::put_desc_obj_to_pool(BuddyBufDesc *desc) {      // line 648
    if (desc == NULL) {
        return -1;
    }
    if (desc->self_desc_additional_) {
        if (additional_desc_obj_pool_ != NULL) {
            desc->next_ = additional_desc_obj_pool_;
            additional_desc_obj_pool_ = desc;
        } else {
            additional_desc_obj_pool_ = desc;
        }
    } else {
        desc->next_ = desc_obj_pool_;
        desc_obj_pool_ = desc;
    }
    return 0;
}

void BuddyAlloc::print_info() {                     // line 680
    if (0 == size_allocated_ || 0 == buffer_len_ || 0 == alloc_times_
        || 0 == free_times_) {
        return;
    }
    ZEN_Trace_LogMsg::debug_debugEx(
            "\n--------- buddy info ---------\n"
            "usage rate: %u%%(%u/%u)\n"
            "free rate: %u%%(%u/%u)\n"
            "max_size_can_alloc: %u\n"
            "average split times per alloc: %f(%u/%u)\n"
            "average merge times per free: %f(%u/%u)\n"
            "use system alloc rate: %u%%(%u/%u)\n"
            "ptr_desc_map_.size() = %u\n"
            "--- info end ---\n",
            (unsigned int)(size_used_ * 100 / size_allocated_),
            (unsigned int)size_used_,
            (unsigned int)size_allocated_,
            (unsigned int)((buffer_len_ - size_allocated_) * 100 / buffer_len_),
            (unsigned int)(buffer_len_ - size_allocated_),
            (unsigned int)buffer_len_,
            (unsigned int)max_size_can_alloc_,
            (double)alloc_split_series_
                    / (double)(alloc_times_ - use_system_alloc_times_),
            (unsigned int)alloc_split_series_,
            (unsigned int)alloc_times_,
            (double)free_merge_series_
                    / (double)(free_times_ - use_system_free_times_),
            (unsigned int)free_merge_series_,
            (unsigned int)free_times_,
            (unsigned int)(use_system_alloc_times_ * 100 / alloc_times_),
            (unsigned int)use_system_alloc_times_,
            (unsigned int)alloc_times_,
            (unsigned int)ptr_desc_map_.size());
}

void *BuddyAlloc::alloc(unsigned int size) {        // line 711
    if (size == 0 || size > buffer_len_) {
        ZEN_Trace_LogMsg::debug_errorex("[buddy]BuddyAlloc::alloc invalid args");
        return NULL;
    }
    int order = 0;
    unsigned int power = 0;
    if (0 != get_next_order_and_power(size, &order, &power)) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::alloc get_next_order_and_power failed");
        return NULL;
    }
    int i = order;
    if (i < BUDDY_FREE_CLASSIFY_MAX_NUM) {
        while (free_classify_list_array_[i] == NULL) {
            ++i;
            if (i >= BUDDY_FREE_CLASSIFY_MAX_NUM) {
                break;
            }
        }
    }
    if (i == BUDDY_FREE_CLASSIFY_MAX_NUM) {
        if (!lack_use_system_) {
            return NULL;
        }
        unsigned char *ptr = new (std::nothrow) unsigned char[size];
        if (ptr == NULL) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]BuddyAlloc::alloc alloc from system failed");
            return NULL;
        }
        BuddyBufDesc *desc = get_desc_obj_from_pool();
        desc->ptr_ = ptr;
        desc->ptr_len_ = size;
        desc->self_ptr_additional_ = true;
        desc->used_len_ = size;
        ptr_desc_map_[ptr] = desc;
        if (alloc_times_ == BUDDY_UNSIGNED_INT_MAX
            || use_system_alloc_times_ == BUDDY_UNSIGNED_INT_MAX) {
            alloc_times_ = 0;
            alloc_split_series_ = 0;
            use_system_alloc_times_ = 0;
        } else {
            ++alloc_times_;
            ++use_system_alloc_times_;
        }
        return ptr;
    }
    if (0 != split(i, order)) {
        ZEN_Trace_LogMsg::debug_errorex("[buddy]BuddyAlloc::alloc split failed");
        return NULL;
    }
    size_used_ += size;
    size_allocated_ += power;
    if (alloc_times_ == BUDDY_UNSIGNED_INT_MAX
        || alloc_split_series_ + (unsigned int)(i - order)
                   == BUDDY_UNSIGNED_INT_MAX) {
        alloc_times_ = 0;
        alloc_split_series_ = 0;
        use_system_alloc_times_ = 0;
    } else {
        ++alloc_times_;
        alloc_split_series_ += (unsigned int)(i - order);
    }
    BuddyBufDesc *desc = free_classify_list_array_[order];
    desc->used_len_ = size;
    modify_bitset(desc);
    del_desc_from_free_list_array(desc);
    if (max_size_can_alloc_order_ == order) {
        if (free_classify_list_array_[order] == NULL) {
            for (int j = order - 1; j >= 0; --j) {
                if (free_classify_list_array_[j] != NULL) {
                    max_size_can_alloc_ = free_classify_list_array_[j]->ptr_len_;
                    max_size_can_alloc_order_ = j;
                    return desc->ptr_;
                }
            }
            max_size_can_alloc_ = 0;
            max_size_can_alloc_order_ = -1;
        }
    }
    return desc->ptr_;
}

int BuddyAlloc::split(int order, int to_order) {    // line 850
    if (order < to_order) {
        ZEN_Trace_LogMsg::debug_errorex("[buddy]BuddyAlloc::split invalid args");
        return -1;
    }
    if (order != to_order) {
        int cur = order;
        while (cur > to_order) {
            BuddyBufDesc *desc = free_classify_list_array_[cur];
            modify_bitset(desc);
            del_desc_from_free_list_array(desc);
            ptr_desc_map_.erase(desc->ptr_);

            --cur;
            size_t half = desc->ptr_len_ >> 1;
            BuddyBufDesc *left = get_desc_obj_from_pool();
            left->ptr_ = desc->ptr_;
            left->ptr_len_ = half;
            left->order_ = cur;
            get_block_index_by_order(left);
            insert_desc_to_free_list_array(left);
            ptr_desc_map_[left->ptr_] = left;

            BuddyBufDesc *right = get_desc_obj_from_pool();
            right->ptr_ = desc->ptr_ + half;
            right->ptr_len_ = half;
            right->order_ = cur;
            get_block_index_by_order(right);
            insert_desc_to_free_list_array(right);
            ptr_desc_map_[right->ptr_] = right;

            put_desc_obj_to_pool(desc);
        }
        if (order == max_size_can_alloc_order_
            && free_classify_list_array_[order] == NULL) {
            max_size_can_alloc_ >>= 1;
            --max_size_can_alloc_order_;
        }
    }
    return 0;
}

int BuddyAlloc::insert_desc_to_free_list_array(BuddyBufDesc *desc) {    // 922
    if (desc == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::insert_desc_to_free_list_array invalid args");
        return -1;
    }
    if (free_classify_list_array_[desc->order_] != NULL) {
        desc->pre_ = NULL;
        desc->next_ = free_classify_list_array_[desc->order_];
        free_classify_list_array_[desc->order_]->pre_ = desc;
        free_classify_list_array_[desc->order_] = desc;
    } else {
        free_classify_list_array_[desc->order_] = desc;
        desc->pre_ = NULL;
        desc->next_ = NULL;
    }
    return 0;
}

int BuddyAlloc::del_desc_from_free_list_array(BuddyBufDesc *desc) {     // 948
    if (free_classify_list_array_[desc->order_] == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::del_desc_from_free_list_array invalid args");
        return -1;
    }
    if (desc->pre_ != NULL) {
        desc->pre_->next_ = desc->next_;
    } else {
        free_classify_list_array_[desc->order_] = desc->next_;
    }
    if (desc->next_ != NULL) {
        desc->next_->pre_ = desc->pre_;
    }
    return 0;
}

int BuddyAlloc::modify_bitset(BuddyBufDesc *desc) {     // line 975
    if (desc->order_ != need_order_) {
        classify_bitset_[desc->order_]->reverse(
                (unsigned int)(desc->block_index_ >> 1));
    }
    return 0;
}

int BuddyAlloc::get_block_index_by_order(BuddyBufDesc *desc) {  // line 987
    unsigned int size_per_block = 0x1 << (need_order_ - desc->order_);
    if (size_per_block == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::get_block_index_by_order block_num == 0");
        return -1;
    }
    unsigned int block_num = buffer_len_ / size_per_block;
    if (block_num == 0) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::get_block_index_by_order "
                "size_per_block == 0");
        return -1;
    }
    desc->block_index_ = (desc->ptr_ - buffer_) / (int)block_num;
    if (desc->block_index_ < 0
        || desc->block_index_ >= (ssize_t)(buffer_len_ / base_size_)) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::get_block_index_by_order block_index error");
        return -1;
    }
    return 0;
}

BuddyBufDesc *BuddyAlloc::get_ptr_desc(void *ptr) {     // line 1015
    if (ptr == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::get_ptr_desc invalid args");
        return NULL;
    }
    BUDDY_ALLOC_POINTER_DESC_HASHMAP::iterator it = ptr_desc_map_.find(ptr);
    if (it != ptr_desc_map_.end()) {
        return it->second;
    }
    ZEN_Trace_LogMsg::debug_errorex(
            "[buddy]BuddyAlloc::get_ptr_desc can not find info");
    return NULL;
}

void BuddyAlloc::free(void *ptr) {                  // line 1034
    if (ptr == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[buddy]BuddyAlloc::free invalid args");
        return;
    }
    BuddyBufDesc *desc = get_ptr_desc(ptr);
    if (desc == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]BuddyAlloc::free can not find info");
        return;
    }
    if (desc->self_ptr_additional_) {
        if (desc->ptr_ != NULL) {
            operator delete[](desc->ptr_);
        }
        desc->ptr_ = NULL;
        ptr_desc_map_.erase(desc->ptr_);
        put_desc_obj_to_pool(desc);
        if (free_times_ == BUDDY_UNSIGNED_INT_MAX
            || use_system_free_times_ == BUDDY_UNSIGNED_INT_MAX) {
            free_times_ = 0;
            free_merge_series_ = 0;
            use_system_free_times_ = 0;
        } else {
            ++use_system_free_times_;
            ++free_times_;
        }
        return;
    }
    modify_bitset(desc);
    size_used_ -= desc->used_len_;
    size_allocated_ -= desc->ptr_len_;
    insert_desc_to_free_list_array(desc);
    int merge_series = 0;
    merge(desc, &merge_series);
    if (free_times_ == BUDDY_UNSIGNED_INT_MAX
        || free_merge_series_ + (unsigned int)merge_series
                   == BUDDY_UNSIGNED_INT_MAX) {
        free_times_ = 0;
        free_merge_series_ = 0;
        use_system_free_times_ = 0;
    } else {
        ++free_times_;
        free_merge_series_ += (unsigned int)merge_series;
    }
}

BuddyBufDesc *BuddyAlloc::find_buddy(BuddyBufDesc *desc) {      // line 1112
    if (desc->block_index_ < 0
        || desc->block_index_ > (ssize_t)(buffer_len_ / base_size_) - 1) {
        ZEN_Trace_LogMsg::debug_errorex("[buddy]BuddyAlloc::find_buddy invalid args");
        return NULL;
    }
    if (desc->block_index_ & 0x1) {
        return get_ptr_desc(desc->ptr_ - desc->ptr_len_);
    }
    return get_ptr_desc(desc->ptr_ + desc->ptr_len_);
}

int BuddyAlloc::merge(BuddyBufDesc *desc, int *merge_series) {  // line 1131
    int merge_count = 0;
    int block_index = desc->block_index_ >> 1;
    while (desc->order_ != need_order_) {
        if (!classify_bitset_[desc->order_]->is_need_merge(
                    (unsigned int)block_index)) {
            break;
        }
        BuddyBufDesc *buddy = find_buddy(desc);
        if (buddy == NULL) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]BuddyAlloc::merge can not found buddy");
            return -1;
        }
        del_desc_from_free_list_array(desc);
        del_desc_from_free_list_array(buddy);
        ptr_desc_map_.erase(desc->ptr_);
        ptr_desc_map_.erase(buddy->ptr_);
        classify_bitset_[desc->order_]->reset((unsigned int)block_index);
        BuddyBufDesc *new_desc = get_desc_obj_from_pool();
        if (desc->block_index_ & 0x1) {
            new_desc->ptr_ = buddy->ptr_;
        } else {
            new_desc->ptr_ = desc->ptr_;
        }
        ++merge_count;
        new_desc->ptr_len_ = desc->ptr_len_ * 2;
        new_desc->order_ = desc->order_ + 1;
        get_block_index_by_order(new_desc);
        insert_desc_to_free_list_array(new_desc);
        ptr_desc_map_[new_desc->ptr_] = new_desc;
        modify_bitset(new_desc);
        put_desc_obj_to_pool(desc);
        put_desc_obj_to_pool(buddy);
        desc = new_desc;
        block_index = desc->block_index_ >> 1;
    }
    if (max_size_can_alloc_order_ < desc->order_) {
        max_size_can_alloc_ = desc->ptr_len_;
        max_size_can_alloc_order_ = desc->order_;
    }
    if (merge_series != NULL) {
        *merge_series = merge_count;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// MultiBuddyAlloc
// ---------------------------------------------------------------------------

MultiBuddyAlloc::MultiBuddyAlloc()                  // line 1207
        : buffer_(NULL),
          buffer_len_(0),
          lack_use_system_(false),
          num_of_steps_(0),
          buddy_max_order_(0),
          ptr_buddy_map_(),
          ptr_system_map_(),
          alloc_times_(0),                          // line 1216
          use_system_alloc_times_(0),
          free_times_(0),
          use_system_free_times_(0) {
}

MultiBuddyAlloc::~MultiBuddyAlloc() {               // line 1222
    fini();
}

int MultiBuddyAlloc::count_total_size(unsigned int size,    // line 1227
                                      int num_of_steps,
                                      unsigned int *total_size) {
    if ((unsigned int)(num_of_steps << 5) > size) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]MultiBuddyAlloc::count_total_size invalid args");
        return -1;
    }
    unsigned int step_power = MULTIBUDDY_BASE_SIZE_START / num_of_steps;
    int order = 0;
    unsigned int total = 0;
    if (size != 0) {
        do {
            total = 0;
            for (int i = 0; i < num_of_steps; ++i) {
                total += (unsigned int)
                        ((MULTIBUDDY_BASE_SIZE_START + i * step_power)
                         << order);
            }
            ++order;
        } while (total < size);
        --order;
        if (order > MULTIBUDDY_TAOTAL_SIZE_MAX_INDEX) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]MultiBuddyAlloc::count_total_size "
                    "order > MULTIBUDDY_TAOTAL_SIZE_MAX_INDEX");
            return -1;
        }
    } else {
        order = -1;
    }
    buddy_max_order_ = order;
    if (total_size != NULL) {
        *total_size = total;
    }
    ZEN_Trace_LogMsg::debug_debugEx(
            "[buddy]MultiBuddyAlloc::count_total_size need_size: %u", total);
    return 0;
}

int MultiBuddyAlloc::init(unsigned int *size, int num_of_steps,     // line 1271
                          bool lack_use_system) {
    if (size == NULL || num_of_steps <= 0 || (num_of_steps & 0x1) != 0
        || num_of_steps > MULTIBUDDY_BUDDY_POOL_MAX_NUM) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]MultiBuddyAlloc::init invalid args");
        return -1;
    }
    if (*size <= (unsigned int)(num_of_steps * MULTIBUDDY_MAX_BASE_SIZE)) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]MultiBuddyAlloc::init "
                "not enough space for multi-buddy system");
        return -2;
    }
    num_of_steps_ = num_of_steps;
    lack_use_system_ = lack_use_system;
    for (int i = 0; i < MULTIBUDDY_BUDDY_POOL_MAX_NUM; ++i) {
        buddy_pool_array_[i] = NULL;
    }
    unsigned int total_size = 0;
    if (0 != count_total_size(*size, num_of_steps_, &total_size)) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]MultiBuddyAlloc::init count_total_size failed");
        fini();
        return -3;
    }
    *size = total_size;
    buffer_len_ = (unsigned int)(num_of_steps_ * MULTIBUDDY_BUDDY_BUFFER_INTERVAL)
                  + total_size;
    buffer_ = new (std::nothrow) unsigned char[buffer_len_];
    if (buffer_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]MultiBuddyAlloc::init "
                "new unsigned char not enough memory");
        fini();
        return -4;
    }
    unsigned int offset = 0;
    for (int i = 0; i < num_of_steps_; ++i) {
        BuddyAlloc *buddy = new (std::nothrow) BuddyAlloc();
        if (buddy == NULL) {
            buddy_pool_array_[i] = NULL;
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]MultiBuddyAlloc::init "
                    "new BuddyAlloc() not enough memory");
            fini();
            return -5;
        }
        buddy_pool_array_[i] = buddy;
        unsigned int step_size =
                (unsigned int)((MULTIBUDDY_BASE_SIZE_START / num_of_steps_) * i
                               + MULTIBUDDY_BASE_SIZE_START);
        unsigned int buddy_len =
                (unsigned int)(step_size << buddy_max_order_);
        if (0 != buddy->init_buddy_on_buffer(buddy_len, step_size,
                                            buffer_ + offset, buddy_len,
                                            NULL, false)) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]MultiBuddyAlloc::init "
                    "init_buddy_on_buffer failed");
            fini();
            return -1;
        }
        offset += buddy_len + MULTIBUDDY_BUDDY_BUFFER_INTERVAL;
        if (offset > buffer_len_) {
            ZEN_Trace_LogMsg::debug_errorex(
                    "[buddy]MultiBuddyAlloc::init count error");
            return -1;
        }
    }
    return 0;
}

void *MultiBuddyAlloc::alloc(unsigned int size) {   // line 1364
    if ((unsigned int)(MULTIBUDDY_BASE_SIZE_START << buddy_max_order_) < size) {
        return NULL;
    }
    int order = 0;
    unsigned int power = 0;
    if (0 != buddy_pool_array_[0]->get_next_order_and_power(size, &order,
                                                            &power)) {
        ZEN_Trace_LogMsg::debug_errorex(
                "[buddy]MultiBuddyAlloc::alloc get_next_order_and_power error");
        return NULL;
    }
    int index = 0;
    if (num_of_steps_ > 1) {
        if (0 == order) {
            if (size <= (unsigned int)((MULTIBUDDY_BASE_SIZE_START
                                        / num_of_steps_)
                                       * (num_of_steps_ - 1) / 2
                                       + MULTIBUDDY_BASE_SIZE_START)) {
                index = (int)(size % (unsigned int)num_of_steps_)
                        % num_of_steps_;
            }
        } else {
            unsigned int remain = size - (power >> 1);
            unsigned int chunk = (unsigned int)
                    ((MULTIBUDDY_BASE_SIZE_START / num_of_steps_)
                     << (order - 1));
            int quotient = (int)(remain / chunk);
            if (remain % chunk != 0) {
                ++quotient;
            }
            index = quotient % num_of_steps_;
        }
    }
    int start = index;
    void *ptr = NULL;
    do {
        ptr = buddy_pool_array_[index]->alloc(size);
        if (ptr != NULL) {
            if (alloc_times_ == MULTIBUDDY_UNSIGNED_INT_MAX
                || use_system_alloc_times_ == MULTIBUDDY_UNSIGNED_INT_MAX) {
                alloc_times_ = 0;
                use_system_alloc_times_ = 0;
            } else {
                ++alloc_times_;
            }
            ptr_buddy_map_[ptr] = buddy_pool_array_[index];
            return ptr;
        }
        index = (index + 1) % num_of_steps_;
    } while (index != start);
    if (!lack_use_system_) {
        return NULL;
    }
    void *sys_ptr = new (std::nothrow) unsigned char[size];
    if (sys_ptr == NULL) {
        return NULL;
    }
    if (alloc_times_ == MULTIBUDDY_UNSIGNED_INT_MAX
        || use_system_alloc_times_ == MULTIBUDDY_UNSIGNED_INT_MAX) {
        alloc_times_ = 0;
        use_system_alloc_times_ = 0;
    } else {
        ++use_system_alloc_times_;
        ++alloc_times_;
    }
    ptr_system_map_[sys_ptr] = size;
    return sys_ptr;
}

void MultiBuddyAlloc::free(void *ptr) {             // line 1483
    if (ptr == NULL) {
        return;
    }
    BUDDY_ALLOC_POINTER_POOL_HASHMAP::iterator it = ptr_buddy_map_.find(ptr);
    if (it != ptr_buddy_map_.end()) {
        if (free_times_ == MULTIBUDDY_UNSIGNED_INT_MAX
            || use_system_free_times_ == MULTIBUDDY_UNSIGNED_INT_MAX) {
            free_times_ = 0;
            use_system_free_times_ = 0;
        } else {
            ++free_times_;
        }
        ptr_buddy_map_.erase(ptr);
        it->second->free(ptr);
        return;
    }
    BUDDY_ALLOC_POINTER_SIZE_HASHMAP::iterator sys_it =
            ptr_system_map_.find(ptr);
    if (sys_it != ptr_system_map_.end()) {
        operator delete[](ptr);
        ptr_system_map_.erase(ptr);
        if (free_times_ == MULTIBUDDY_UNSIGNED_INT_MAX
            || use_system_free_times_ == MULTIBUDDY_UNSIGNED_INT_MAX) {
            free_times_ = 0;
            use_system_free_times_ = 0;
        } else {
            ++use_system_free_times_;
            ++free_times_;
        }
        return;
    }
    ZEN_Trace_LogMsg::debug_errorex(
            "[buddy]MultiBuddyAlloc::free have no associated ptr");
}

int MultiBuddyAlloc::fini() {                       // line 1541
    for (int i = 0; i < MULTIBUDDY_BUDDY_POOL_MAX_NUM; ++i) {
        if (buddy_pool_array_[i] != NULL) {
            buddy_pool_array_[i]->fini();
            delete buddy_pool_array_[i];
            buddy_pool_array_[i] = NULL;
        }
    }
    if (buffer_ != NULL) {
        operator delete[](buffer_);
        buffer_ = NULL;
    }
    return 0;
}

void MultiBuddyAlloc::print_info() {                // line 1563
    unsigned int size_used_sum = 0;
    unsigned int size_allocated_sum = 0;
    unsigned int alloc_split_sum = 0;
    unsigned int free_merge_sum = 0;
    unsigned int buffer_len_sum = 0;
    size_t max_size_can_alloc = buddy_pool_array_[0]->max_size_can_alloc_;
    for (int i = 0; i < num_of_steps_; ++i) {
        size_used_sum += (unsigned int)buddy_pool_array_[i]->size_used_;
        size_allocated_sum +=
                (unsigned int)buddy_pool_array_[i]->size_allocated_;
        alloc_split_sum += buddy_pool_array_[i]->alloc_split_series_;
        free_merge_sum += buddy_pool_array_[i]->free_merge_series_;
        buffer_len_sum += (unsigned int)buddy_pool_array_[i]->buffer_len_;
        if (max_size_can_alloc < buddy_pool_array_[i]->max_size_can_alloc_) {
            max_size_can_alloc = buddy_pool_array_[i]->max_size_can_alloc_;
        }
    }
    if (0 == size_allocated_sum || 0 == buffer_len_) {
        return;
    }
    if (0 == alloc_times_ || 0 == free_times_) {
        return;
    }
    if (alloc_times_ == use_system_alloc_times_
        || free_times_ == use_system_free_times_) {
        return;
    }
    ZEN_Trace_LogMsg::debug_debugEx(
            "\n--------- multibuddy info ---------\n"
            "usage rate: %u%%(%u/%u)\n"
            "free rate: %u%%(%u/%u)\n"
            "max_size_can_alloc: %u\n"
            "average split times per alloc: %f(%u/%u)\n"
            "average merge times per free: %f(%u/%u)\n"
            "use system alloc rate: %u%%(%u/%u)\n"
            "ptr_buddy_map_.size() = %u\n"
            "ptr_system_map_.size() = %u\n"
            "--- multibuddy info end ---\n",
            (unsigned int)((unsigned long long)size_used_sum * 100
                           / size_allocated_sum),
            size_used_sum,
            size_allocated_sum,
            (unsigned int)((unsigned long long)
                                   (buffer_len_sum - size_allocated_sum)
                                   * 100 / buffer_len_sum),
            buffer_len_sum - size_allocated_sum,
            buffer_len_sum,
            (unsigned int)max_size_can_alloc,
            (double)alloc_split_sum
                    / (double)(alloc_times_ - use_system_alloc_times_),
            alloc_split_sum,
            (unsigned int)alloc_times_,
            (double)free_merge_sum
                    / (double)(free_times_ - use_system_free_times_),
            free_merge_sum,
            (unsigned int)free_times_,
            (unsigned int)((unsigned long long)use_system_alloc_times_ * 100
                           / alloc_times_),
            use_system_alloc_times_,
            (unsigned int)alloc_times_,
            (unsigned int)ptr_buddy_map_.size(),
            (unsigned int)ptr_system_map_.size());
}
