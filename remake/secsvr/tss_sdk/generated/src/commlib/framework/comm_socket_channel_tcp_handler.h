// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_socket_channel_tcp_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SOCKET_CHANNEL_TCP_HANDLER_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SOCKET_CHANNEL_TCP_HANDLER_H_H_

#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_socket_stream.h"
#include <stddef.h>

// sizeof = 88
struct SocketChannelTCPHandler : public SocketChannelBaseHandler { // line 26
private:
static const int BUF_MAX_SIZE;
static const int PACK_BUF_MAX_SIZE;
ZEN_Socket_Stream sock_;
volatile int socket_status_;
unsigned char *send_buf_;
volatile size_t send_buf_len_;
volatile size_t next_send_pos_;
unsigned char *recv_buf_;
unsigned char *pack_buf_;
int nextRecvStart;
int nextReadStart;
public:
SocketChannelTCPHandler(const SocketChannelTCPHandler &arg0);
SocketChannelTCPHandler(); // line 21
virtual ~SocketChannelTCPHandler(); // line 34
virtual int handle_input(); // line 427
virtual int handle_output(); // line 214
virtual int handle_close(); // line 368
virtual int handle_timeout(const ZEN_Time_Value &arg0, const void *arg1); // line 421
virtual ZEN_SOCKET get_handle() const; // line 42
virtual bool is_valid(); // line 332
virtual bool is_idle(); // line 337
virtual int send(unsigned char *buf, size_t len); // line 240
virtual int init_connect(SocketChannelInitData &init_data, SocketChannelBus *sndrcv_bus); // line 47
virtual int connect_server(); // line 122
private:
int init_tcp_handler(const ZEN_Socket_Stream &sockStream); // line 175
int disconnect_fini(); // line 86
int close_socket(); // line 352
void reset(); // line 112
int process_connect_register(); // line 342
int send_buf_to_net(); // line 277
int read_data_from_tcp(); // line 381
virtual int check_if_full_frame(unsigned char *arg0, size_t arg1, bool *arg2, int *arg3); // line 89
virtual void define_comm_head_except_frame_length(Comm_App_Frame *arg0); // line 92
};
extern const int BUF_MAX_SIZE; // line 95

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SOCKET_CHANNEL_TCP_HANDLER_H_H_
