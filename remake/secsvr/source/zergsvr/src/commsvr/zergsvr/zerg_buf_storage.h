// Auto-generated header stub from DWARF info
// Original path: src/commsvr/zergsvr/zerg_buf_storage.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BUF_STORAGE_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BUF_STORAGE_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include <stddef.h>

// sizeof = 65560
struct ZByteBuffer { // line 13
public:
static const unsigned int CAPACITY_OF_BUFFER;
size_t size_of_use_;
size_t size_of_buffer_;
char buffer_data_[];
ZByteBuffer(); // line 33
~ZByteBuffer(); // line 35
void fill_write_data(size_t arg0, const char *arg1); // line 38
void get_read_data(size_t &arg0, char *arg1); // line 40
char * get_use_point(); // line 43
size_t get_leave_size(); // line 48
bool is_full(); // line 54
bool is_empty(); // line 65
void clear_buffer(); // line 77
};
// sizeof = 20
struct ZBuffer_Storage { // line 87
protected:
static const size_t EXTEND_NUM_OF_LIST;
static const size_t MIN_HDL_LIST_NUMBER;
static const size_t MAX_HDL_LIST_NUMBER;
size_t size_of_bufferalloc_;
// anonymous struct, sizeof = 16
struct {
    protected:
    size_t cycdeque_start_;
    size_t cycdeque_size_;
    size_t cycdeque_capacity_;
    ZByteBuffer **value_ptr_;
    public:
    void lordrings(); // line 169
    void lordrings(unsigned int arg0); // line 178
    void ~lordrings(); // line 188
    void initialize(unsigned int arg0); // line 198
    void finit(); // line 217
    void clear(); // line 232
    size_t size() const; // line 239
    size_t freesize() const; // line 244
    size_t capacity() const; // line 250
    bool full() const; // line 256
    bool empty() const; // line 268
    bool resize(unsigned int arg0); // line 280
    bool push_back(ZByteBuffer *const &arg0, bool arg1); // line 317
    bool push_front(ZByteBuffer *const &arg0, bool arg1); // line 348
    bool pop_front(ZByteBuffer *&arg0); // line 381
    bool pop_front(); // line 397
    bool pop_back(ZByteBuffer *&arg0); // line 412
    bool pop_back(); // line 427
    ZByteBuffer *& operator[](unsigned int arg0); // line 441
    ZByteBuffer *const & operator[](unsigned int arg0) const; // line 447
    _lordrings_iterator<ZByteBuffer*> begin(); // line 453
    _lordrings_iterator<ZByteBuffer*> end(); // line 459
} buffer_deque_;
static ZBuffer_Storage *instance_;
ZBuffer_Storage(const ZBuffer_Storage &arg0); // line 118
ZBuffer_Storage & operator=(const ZBuffer_Storage &arg0); // line 120
public:
ZBuffer_Storage(); // line 124
~ZBuffer_Storage(); // line 125
void init_buffer_list(size_t arg0); // line 128
void init_buflist_by_hdlnum(size_t arg0); // line 131
void uninit_buffer_list(); // line 134
ZByteBuffer * allocate_buffer(); // line 137
void free_byte_buffer(ZByteBuffer *arg0); // line 140
void extend_bufferlist(size_t arg0); // line 143
static ZBuffer_Storage * instance(); // line 149
static void clean_instance(); // line 151
};
extern const unsigned int CAPACITY_OF_BUFFER; // line 16
extern const size_t EXTEND_NUM_OF_LIST; // line 93
extern const size_t MIN_HDL_LIST_NUMBER; // line 96
extern const size_t MAX_HDL_LIST_NUMBER; // line 98

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BUF_STORAGE_H_H_
