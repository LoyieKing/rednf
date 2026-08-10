// Restored from zergsvr binary DWARF + disassembly (2026-08-10).
// MML_Console_Handler: sizeof 136, layout:
//   ZEN_Event_Handler (0x00..0x0b), if_console_(0x0c), if_internal_(0x0d),
//   console_data_buf_(0x10), dgram_addr_(0x14), dgram_socket_peer_(0x30),
//   remote_addr_(0x34), console_command_(0x50).
// process_mml_command is pure virtual (vtable slot +0x2c -> __cxa_pure_virtual).
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_CONSOLE_HANDLER_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_CONSOLE_HANDLER_H_H_

#include <stddef.h>

#include "output/commlib/zenlib/release/include/zen_event_handle_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_datagram.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "src/commlib/framework/comm_mml_command.h"

// sizeof = 136
struct MML_Console_Handler : public ZEN_Event_Handler { // line 13
public:
    static const size_t MAX_DATA_BUFFER_LEN = 0x10010;
protected:
    bool if_console_;                  // 0x0c
    bool if_internal_;                 // 0x0d
    char *console_data_buf_;           // 0x10
    ZEN_Sockaddr_In dgram_addr_;       // 0x14
    Zen_Socket_DataGram dgram_socket_peer_; // 0x30
    ZEN_Sockaddr_In remote_addr_;      // 0x34
    MML_Console_Command console_command_; // 0x50
    static MML_Console_Handler *instance_; // line 15
public:
    MML_Console_Handler(const MML_Console_Handler &arg0);
    MML_Console_Handler(ZEN_Reactor *reactor, ZEN_Sockaddr_In &dgram_addr); // line 15
    MML_Console_Handler(ZEN_Reactor *reactor);                              // line 26
protected:
    virtual ~MML_Console_Handler();                                        // line 35
    virtual ZEN_SOCKET get_handle() const;                                 // line 45
    virtual int handle_input();                                            // line 160
    virtual int handle_close();                                            // line 146
    virtual int read_config(const conf_zerg::zerg_config &config);         // line 52
public:
    virtual int initialize(const conf_zerg::zerg_config &config);          // line 110
    virtual int process_mml_command(char *buffer, size_t &data_len) = 0;   // line 77
    static void instance(MML_Console_Handler *instance);                   // line 244
    static MML_Console_Handler *instance();                                // line 249
    static void clean_instance();                                          // line 254
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_CONSOLE_HANDLER_H_H_
