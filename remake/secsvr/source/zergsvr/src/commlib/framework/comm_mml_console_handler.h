// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_mml_console_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_CONSOLE_HANDLER_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_CONSOLE_HANDLER_H_H_

#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_socket_datagram.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "src/commlib/framework/comm_mml_command.h"

// sizeof = 136
struct MML_Console_Handler : public ZEN_Event_Handler { // line 13
public:
static const size_t MAX_DATA_BUFFER_LEN;
protected:
bool if_console_;
bool if_internal_;
char *console_data_buf_;
ZEN_Sockaddr_In dgram_addr_;
Zen_Socket_DataGram dgram_socket_peer_;
ZEN_Sockaddr_In remote_addr_;
MML_Console_Command console_command_;
static MML_Console_Handler *instance_;
public:
MML_Console_Handler(const MML_Console_Handler &arg0);
MML_Console_Handler(ZEN_Reactor *arg0, ZEN_Sockaddr_In &arg1); // line 15
MML_Console_Handler(ZEN_Reactor *arg0); // line 26
protected:
virtual ~MML_Console_Handler(); // line 35
virtual ZEN_SOCKET get_handle() const; // line 45
virtual int handle_input(); // line 160
virtual int handle_close(); // line 146
virtual int read_config(const Zerg_MMAP_BusPipe::ZERG_CONFIG &config); // line 52
public:
virtual int initialize(const Zerg_MMAP_BusPipe::ZERG_CONFIG &config); // line 110
virtual int process_mml_command(char *arg0, size_t &arg1); // line 77
static void instance(MML_Console_Handler *instance); // line 244
static MML_Console_Handler * instance(); // line 249
static void clean_instance(); // line 254
};
extern const size_t MAX_DATA_BUFFER_LEN; // line 15

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_CONSOLE_HANDLER_H_H_
