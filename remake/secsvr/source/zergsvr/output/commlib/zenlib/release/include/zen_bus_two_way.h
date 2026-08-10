// Restored from gunnersvr DWARF (original: src/commlib/zenlib/zen_bus_two_way.h)
// 布局与 gunnersvr 二进制一致：sizeof = 10292。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_TWO_WAY_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_TWO_WAY_H_H_

#include "src/commlib/zenlib/zen_bus_mmap_pipe.h"
#include <stddef.h>
#include <stdint.h>

// sizeof = 10292
struct ZEN_BusPipe_TwoWay : public ZEN_Bus_MMAPPipe { // line 14
protected:
    static const char BUS_PIPE_NAME[2][16]; // line 29
    static ZEN_BusPipe_TwoWay *two_way_instance_; // line 35
public:
    ZEN_BusPipe_TwoWay(); // line 40
    ~ZEN_BusPipe_TwoWay(); // line 42
    int initialize(const char *bus_mmap_name, size_t size_recv_pipe, size_t size_send_pipe, size_t max_frame_len, bool if_restore, bool if_check_pthread); // zergsvr 6 参版
    int pop_front_recvpipe(ZEN_LIB::dequechunk_node *node); // line 58
    int pop_front_sendpipe(ZEN_LIB::dequechunk_node *node); // line 60
    int push_back_sendpipe(const ZEN_LIB::dequechunk_node *node); // line 62
    int push_back_recvpipe(const ZEN_LIB::dequechunk_node *node); // line 64
    int get_frontsize_recvpipe(size_t &frontsize); // line 68
    int get_frontsize_sendpipe(size_t &frontsize); // line 70
    static void instance(ZEN_BusPipe_TwoWay *pinstatnce); // line 77
    static ZEN_BusPipe_TwoWay * instance(); // line 79
    static void clean_instance(); // line 81
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_TWO_WAY_H_H_
