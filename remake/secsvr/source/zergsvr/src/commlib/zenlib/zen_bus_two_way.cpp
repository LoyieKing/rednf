// Restored from zergsvr binary (DWARF + objdump), 语义与 zergsvr 一致。
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commlib/zenlib/zen_bus_two_way.cpp
// 相对 gunnersvr 的差异：initialize 少一个 unsigned char 参数（mem_type）；
// 收/发管道包装方法在 zergsvr 头文件中仅声明、未定义（二进制中无符号）。

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
                                   bool if_restore, bool if_check_pthread) {
    size_t size_of_pipe[2] = { 0, 0 };

    size_of_pipe[0] = size_recv_pipe;
    size_of_pipe[1] = size_send_pipe;
    return ZEN_Bus_MMAPPipe::initialize(bus_mmap_name, 2, size_of_pipe,
                                        max_frame_len, if_restore, if_check_pthread);
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
