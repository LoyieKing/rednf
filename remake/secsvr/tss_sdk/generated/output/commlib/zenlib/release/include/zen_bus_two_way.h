// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_bus_two_way.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_BUS_TWO_WAY_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_BUS_TWO_WAY_H_H_

#include "output/commlib/zenlib/release/include/zen_bus_mmap_pipe.h"
#include <stddef.h>
#include <stdint.h>

// sizeof = 10292
struct ZEN_BusPipe_TwoWay : public ZEN_Bus_MMAPPipe { // line 14
protected:
static const char BUS_PIPE_NAME[][];
static ZEN_BusPipe_TwoWay *two_way_instance_;
public:
ZEN_BusPipe_TwoWay(); // line 40
~ZEN_BusPipe_TwoWay(); // line 42
int initialize(const char *arg0, size_t arg1, size_t arg2, size_t arg3, bool arg4, bool arg5, uint8_t arg6); // line 53
int pop_front_recvpipe(/*anon struct*/ int *arg0); // line 58
int pop_front_sendpipe(/*anon struct*/ int *arg0); // line 60
int push_back_sendpipe(const /*anon struct*/ int *arg0); // line 62
int push_back_recvpipe(const /*anon struct*/ int *arg0); // line 64
int get_frontsize_recvpipe(size_t &arg0); // line 68
int get_frontsize_sendpipe(size_t &arg0); // line 70
static void instance(ZEN_BusPipe_TwoWay *arg0); // line 77
static ZEN_BusPipe_TwoWay * instance(); // line 79
static void clean_instance(); // line 81
};
extern const char BUS_PIPE_NAME[][]; // line 29

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_BUS_TWO_WAY_H_H_
