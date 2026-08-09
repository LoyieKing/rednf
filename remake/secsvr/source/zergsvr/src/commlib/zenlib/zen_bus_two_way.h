// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_bus_two_way.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_TWO_WAY_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_TWO_WAY_H_H_

#include "output/commlib/zenlib/release/include/zen_bus_mmap_pipe.h"
#include <stddef.h>

// sizeof = 10268
struct ZEN_BusPipe_TwoWay : public ZEN_Bus_MMAPPipe { // line 14
protected:
static const char BUS_PIPE_NAME[][];
static ZEN_BusPipe_TwoWay *two_way_instance_;
public:
ZEN_BusPipe_TwoWay(); // line 40
~ZEN_BusPipe_TwoWay(); // line 42
int initialize(const char *arg0, size_t arg1, size_t arg2, size_t arg3, bool arg4, bool arg5); // line 52
int pop_front_recvpipe(/*anon struct*/ int *arg0); // line 57
int pop_front_sendpipe(/*anon struct*/ int *arg0); // line 59
int push_back_sendpipe(const /*anon struct*/ int *arg0); // line 61
int push_back_recvpipe(const /*anon struct*/ int *arg0); // line 63
int get_frontsize_recvpipe(size_t &arg0); // line 67
int get_frontsize_sendpipe(size_t &arg0); // line 69
static void instance(ZEN_BusPipe_TwoWay *arg0); // line 76
static ZEN_BusPipe_TwoWay * instance(); // line 78
static void clean_instance(); // line 80
};
extern const char BUS_PIPE_NAME[][]; // line 29

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_TWO_WAY_H_H_
