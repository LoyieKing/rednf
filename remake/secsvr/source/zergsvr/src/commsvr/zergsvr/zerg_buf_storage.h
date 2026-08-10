// Reconstructed from zergsvr DWARF + disassembly.
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BUF_STORAGE_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BUF_STORAGE_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include <stddef.h>

// sizeof = 65560 (0x10018): 8 header + CAPACITY_OF_BUFFER data
struct ZByteBuffer { // line 13
public:
    static const unsigned int CAPACITY_OF_BUFFER = 0x10010; // line 16
    size_t size_of_use_;       // +0
    size_t size_of_buffer_;    // +4
    char buffer_data_[CAPACITY_OF_BUFFER]; // +8
    ZByteBuffer(); // line 33
    ~ZByteBuffer(); // line 35
    void fill_write_data(size_t len, const char *data); // line 38
    void get_read_data(size_t &len, char *data); // line 40
    char * get_use_point() { return buffer_data_ + size_of_use_; } // line 43（内联）
    size_t get_leave_size() { return CAPACITY_OF_BUFFER - size_of_use_; } // line 48（内联）
    bool is_full() { return size_of_use_ >= CAPACITY_OF_BUFFER; } // line 54（内联）
    bool is_empty() { return size_of_use_ == 0; } // line 65（内联）
    void clear_buffer(); // line 77
};

// sizeof = 20
struct ZBuffer_Storage { // line 87
protected:
    static const size_t EXTEND_NUM_OF_LIST = 0x400;     // line 93
    static const size_t MIN_HDL_LIST_NUMBER = 0x80;     // line 96
    static const size_t MAX_HDL_LIST_NUMBER = 0x3200;   // line 98
    size_t size_of_bufferalloc_; // +0
    ZEN_LIB::lordrings<ZByteBuffer *> buffer_deque_;    // +4
    static ZBuffer_Storage *instance_; // line 110
    ZBuffer_Storage(const ZBuffer_Storage &arg0); // line 118
    ZBuffer_Storage & operator=(const ZBuffer_Storage &arg0); // line 120
public:
    ZBuffer_Storage(); // line 124
    ~ZBuffer_Storage(); // line 125
    void init_buffer_list(size_t num); // line 128
    void init_buflist_by_hdlnum(size_t numhdl); // line 131
    void uninit_buffer_list(); // line 134
    ZByteBuffer * allocate_buffer(); // line 137
    void free_byte_buffer(ZByteBuffer *buf); // line 140
    void extend_bufferlist(size_t extend_num); // line 143
    static ZBuffer_Storage * instance(); // line 149
    static void clean_instance(); // line 151
};

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BUF_STORAGE_H_H_
