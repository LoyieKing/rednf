// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_socket_channel_bus.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SOCKET_CHANNEL_BUS_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SOCKET_CHANNEL_BUS_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"

// sizeof = 12
struct SocketChannelBus : public ZEN_NON_Copyable { // line 24
private:
ZEN_Thread_MT_Bus_Pipe *bus_pipe_;
static const int RECV_BUF_SIZE;
static const int SEND_BUF_SIZE;
static const int MAX_FRAME_LEN;
unsigned char *send_bus_pop_buf_;
unsigned char *recv_bus_pop_buf_;
public:
SocketChannelBus(); // line 26
~SocketChannelBus(); // line 27
int init(); // line 29
int fini(); // line 30
int push_send_bus(const void *arg0); // line 32
int pop_send_bus(void *&arg0); // line 33
int push_recv_bus(void *arg0); // line 35
int pop_recv_bus(void *arg0); // line 36
int peek_send_bus(size_t &arg0); // line 38
};
extern const int RECV_BUF_SIZE; // line 46
extern const int SEND_BUF_SIZE; // line 47
extern const int MAX_FRAME_LEN; // line 48

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SOCKET_CHANNEL_BUS_H_H_
