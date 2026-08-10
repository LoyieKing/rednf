// Restored from gunnersvr binary (DWARF + objdump), 语义与 gunnersvr 一致。
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_bus_two_way.cpp

#include <stddef.h>
#include <stdint.h>

#include "src/commlib/zenlib/zen_bus_two_way.h"

// ---------------------------------------------------------------------------
// 静态成员
// ---------------------------------------------------------------------------
const char ZEN_BusPipe_TwoWay::BUS_PIPE_NAME[2][16] = {
    "RECV_PIPE",
    "SEND_PIPE",
};

ZEN_BusPipe_TwoWay *ZEN_BusPipe_TwoWay::two_way_instance_ = NULL;

// ---------------------------------------------------------------------------
// 构造 / 析构
// ---------------------------------------------------------------------------
ZEN_BusPipe_TwoWay::ZEN_BusPipe_TwoWay() {
}

ZEN_BusPipe_TwoWay::~ZEN_BusPipe_TwoWay() {
}

// ---------------------------------------------------------------------------
// initialize：两条管道（recv/send）
// ---------------------------------------------------------------------------
int ZEN_BusPipe_TwoWay::initialize(const char *bus_mmap_name, size_t size_recv_pipe,
                                   size_t size_send_pipe, size_t max_frame_len,
                                   bool if_restore, bool if_check_pthread, uint8_t mem_type) {
    size_t size_of_pipe[2] = { 0, 0 };

    size_of_pipe[0] = size_recv_pipe;
    size_of_pipe[1] = size_send_pipe;
    return ZEN_Bus_MMAPPipe::initialize(bus_mmap_name, 2, size_of_pipe,
                                        max_frame_len, if_restore, if_check_pthread, mem_type);
}

// ---------------------------------------------------------------------------
// 单例
// ---------------------------------------------------------------------------
ZEN_BusPipe_TwoWay * ZEN_BusPipe_TwoWay::instance() {
    if (two_way_instance_ == NULL) {
        two_way_instance_ = new ZEN_BusPipe_TwoWay();
    }
    return two_way_instance_;
}

void ZEN_BusPipe_TwoWay::instance(ZEN_BusPipe_TwoWay *pinstatnce) {
    clean_instance();
    two_way_instance_ = pinstatnce;
}

void ZEN_BusPipe_TwoWay::clean_instance() {
    if (two_way_instance_ != NULL) {
        delete two_way_instance_;
    }
    two_way_instance_ = NULL;
}

// ---------------------------------------------------------------------------
// 收/发管道操作（原头文件内联实现；gunnersvr 中无 out-of-line 符号）
// RECV_PIPE = 管道 0，SEND_PIPE = 管道 1
// ---------------------------------------------------------------------------
int ZEN_BusPipe_TwoWay::pop_front_recvpipe(ZEN_LIB::dequechunk_node *node) {
    return pop_front_bus(0, node);
}

int ZEN_BusPipe_TwoWay::pop_front_sendpipe(ZEN_LIB::dequechunk_node *node) {
    return pop_front_bus(1, node);
}

int ZEN_BusPipe_TwoWay::push_back_sendpipe(const ZEN_LIB::dequechunk_node *node) {
    return push_back_bus(1, node);
}

int ZEN_BusPipe_TwoWay::push_back_recvpipe(const ZEN_LIB::dequechunk_node *node) {
    return push_back_bus(0, node);
}

int ZEN_BusPipe_TwoWay::get_frontsize_recvpipe(size_t &frontsize) {
    return get_front_nodesize(0, frontsize);
}

int ZEN_BusPipe_TwoWay::get_frontsize_sendpipe(size_t &frontsize) {
    return get_front_nodesize(1, frontsize);
}
