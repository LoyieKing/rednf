// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_socket_channel_info.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SOCKET_CHANNEL_INFO_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SOCKET_CHANNEL_INFO_H_H_

typedef vector<SocketChannelBaseHandler*,std::allocator<SocketChannelBaseHandler*> > SOCKET_CHANNEL_HANDLERS_VECTOR;
// sizeof = 48
struct SocketChannelBusinessInfo { // line 27
private:
int business_id_;
SocketChannelBus *sndrcv_bus_;
SOCKET_CHANNEL_HANDLERS_VECTOR *handlers_vector_;
ZEN_Thread_RW_Mutex handlers_vector_rwlock_;
public:
SocketChannelBusinessInfo(); // line 29
~SocketChannelBusinessInfo(); // line 30
int init(); // line 32
int fini(); // line 33
int add_handler(SocketChannelBaseHandler *arg0); // line 35
SocketChannelBaseHandler * get_first_valid_handler(); // line 38
SocketChannelBus * get_sndrcv_bus() const; // line 40
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SOCKET_CHANNEL_INFO_H_H_
