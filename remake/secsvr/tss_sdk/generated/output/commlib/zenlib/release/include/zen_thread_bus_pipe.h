// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_thread_bus_pipe.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_BUS_PIPE_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_BUS_PIPE_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_lock_guard.h"
#include <stddef.h>

typedef ZEN_Thread_Bus_Pipe<ZEN_Thread_Mutex> ZEN_Thread_MT_Bus_Pipe;
// sizeof = 84
struct ZEN_Thread_Bus_Pipe<ZEN_Thread_Mutex> : public ZEN_NON_Copyable { // line 26
protected:
char *pipe_buffer_;
size_t size_pipe_[];
size_t size_room_[];
/*anon struct*/ int *bus_pipe_[];
ZEN_Thread_Mutex bus_lock_[];
static ZEN_Thread_Bus_Pipe<ZEN_Thread_Mutex> *instance_;
public:
void ZEN_Thread_Bus_Pipe(); // line 62
void ~ZEN_Thread_Bus_Pipe(); // line 74
int initialize(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 85
bool pop_front_recvpipe(/*anon struct*/ int *&arg0); // line 136
bool push_back_recvpipe(const /*anon struct*/ int *arg0); // line 143
bool pop_front_sendpipe(/*anon struct*/ int *&arg0); // line 151
bool push_back_sendpipe(const /*anon struct*/ int *arg0); // line 158
int get_frontsize_recvpipe(size_t &arg0); // line 166
int get_frontsize_sendpipe(size_t &arg0); // line 180
ZEN_Thread_Bus_Pipe<ZEN_Thread_Mutex> * instance(); // line 199
void instance(ZEN_Thread_Bus_Pipe<ZEN_Thread_Mutex> *arg0); // line 210
void clean_instance(); // line 218
};
extern ZEN_Lock_Guard<ZEN_Thread_Mutex> lock_guard; // line 153
extern const size_t FIXED_INTERVALS; // line 97
extern size_t sz_malloc; // line 98

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_BUS_PIPE_H_H_
