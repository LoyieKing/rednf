// Reconstructed from zergsvr binary (zerg_buf_storage.cpp DWARF + disassembly).

#include "src/commsvr/zergsvr/zerg_buf_storage.h"
#include "output/commlib/framework/release/include/comm_stat_monitor.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include <new>
#include <stddef.h>
#include <string.h>

ZBuffer_Storage * ZBuffer_Storage::instance_ = NULL;

// line 28
ZByteBuffer::ZByteBuffer() {
    size_of_use_ = 0;
    size_of_buffer_ = 0;
}

// line 35
ZByteBuffer::~ZByteBuffer() {
}

// line 40
void ZByteBuffer::fill_write_data(size_t szdata, const char *data) {
    memcpy(buffer_data_, data, szdata);
    size_of_buffer_ += szdata;
}

// line 47
void ZByteBuffer::get_read_data(size_t &szdata, char *data) {
    memcpy(data, buffer_data_, szdata);
}

// line 53
void ZByteBuffer::clear_buffer() {
    size_of_buffer_ = 0;
    size_of_use_ = 0;
}

// line 62
ZBuffer_Storage::ZBuffer_Storage() {
}

// line 68
ZBuffer_Storage::~ZBuffer_Storage() {
    uninit_buffer_list();
    if (buffer_deque_.value_ptr_ != NULL) {
        delete[] buffer_deque_.value_ptr_;
        buffer_deque_.value_ptr_ = NULL;
    }
}

// line 74
void ZBuffer_Storage::init_buflist_by_hdlnum(size_t num_of_hdl) {
    size_t szlist = num_of_hdl / 10;
    if (szlist < MIN_HDL_LIST_NUMBER) {
        szlist = MIN_HDL_LIST_NUMBER;
    }
    if (szlist > MAX_HDL_LIST_NUMBER) {
        szlist = MAX_HDL_LIST_NUMBER;
    }
    init_buffer_list(szlist);
}

// line 96
void ZBuffer_Storage::init_buffer_list(size_t szlist) {
    extend_bufferlist(szlist);
}

// line 102
void ZBuffer_Storage::uninit_buffer_list() {
    size_t sz_of_buffer = buffer_deque_.size();
    for (size_t i = 0; i < sz_of_buffer; ++i) {
        size_t index =
            (buffer_deque_.cycdeque_start_ + i) % buffer_deque_.cycdeque_capacity_;
        if (buffer_deque_.value_ptr_[index] != NULL) {
            delete buffer_deque_.value_ptr_[index];
        }
        buffer_deque_.value_ptr_[index] = NULL;
    }
    buffer_deque_.cycdeque_start_ = 0;
    buffer_deque_.cycdeque_size_ = 0;
    size_of_bufferalloc_ = 0;
}

// line 130
ZByteBuffer * ZBuffer_Storage::allocate_buffer() {
    if (buffer_deque_.empty()) {
        extend_bufferlist(EXTEND_NUM_OF_LIST);
    }
    ZByteBuffer *tmppr = NULL;
    buffer_deque_.pop_front(tmppr);
    return tmppr;
}

// line 145
void ZBuffer_Storage::free_byte_buffer(ZByteBuffer *ptrbuf) {
    ptrbuf->clear_buffer();
    buffer_deque_.push_back(ptrbuf, false);
}

// line 165
void ZBuffer_Storage::extend_bufferlist(size_t szlist) {
    ZEN_Trace_LogMsg::debug_infoex(
        "[zergsvr] extend_bufferlist size:[%u] total:[%d] need memory [%u] ,total use memory [%u].",
        szlist, size_of_bufferalloc_, szlist * ZByteBuffer::CAPACITY_OF_BUFFER,
        size_of_bufferalloc_ * ZByteBuffer::CAPACITY_OF_BUFFER);

    size_t new_total = size_of_bufferalloc_ + szlist;
    if (new_total >= buffer_deque_.size()) {
        buffer_deque_.resize(new_total);
    }
    for (size_t i = 0; i < szlist; ++i) {
        ZByteBuffer *tmppr = new ZByteBuffer();
        tmppr->clear_buffer();
        buffer_deque_.push_back(tmppr, false);
    }
    size_of_bufferalloc_ += szlist;
    Comm_Stat_Monitor::instance()->set_by_statid(0x232d, 0, 0, size_of_bufferalloc_);
}

// line 192
ZBuffer_Storage * ZBuffer_Storage::instance() {
    if (instance_ == NULL) {
        instance_ = new ZBuffer_Storage();
    }
    return instance_;
}

// line 205
void ZBuffer_Storage::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}
