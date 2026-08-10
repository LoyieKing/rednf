// Reconstructed from zergsvr DWARF + disassembly.
// Original path: src/commsvr/zergsvr/zerg_comm_manager.h
// sizeof(Zerg_Comm_Manager) = 116：zerg_acceptor_@0 zerg_updsvc_@12 error_try_num_@24
//   monitor_size_@28 monitor_cmd_[16]@32 zerg_mmap_pipe_@96 zbuffer_storage_@100
//   server_status_@104 count_start_time_@108 send_frame_count_@112
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_COMM_MANAGER_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_COMM_MANAGER_H_H_

#include <stddef.h>
#include <vector>

#include "src/commlib/framework/comm_zerg_mmappipe.h"

struct TCP_Accept_Handler;
struct UDP_Svc_Handler;
struct ZBuffer_Storage;
struct Comm_Stat_Monitor;
struct Comm_App_Frame;
struct ZEN_Sockaddr_In;
struct ZERG_SERVICES_INFO;

// sizeof = 116
struct Zerg_Comm_Manager { // line 27
public:
    static const unsigned int MAX_ONCE_SEND_FRAME = 0x3000; // line 66（12288，run_instance 对拍确认）
    static const size_t MAX_NUMBER_OF_MONITOR_FRAME = 0x10; // line 69（16，get_config 上限确认）
    std::vector<TCP_Accept_Handler *> zerg_acceptor_; // 0x00
    std::vector<UDP_Svc_Handler *> zerg_updsvc_;      // 0x0c
    unsigned int error_try_num_;                      // 0x18
    size_t monitor_size_;                             // 0x1c
    unsigned int monitor_cmd_[MAX_NUMBER_OF_MONITOR_FRAME]; // 0x20
    Zerg_MMAP_BusPipe *zerg_mmap_pipe_;               // 0x60
    ZBuffer_Storage *zbuffer_storage_;                // 0x64
    Comm_Stat_Monitor *server_status_;                // 0x68
    static const unsigned int SEND_FRAME_ALERT_VALUE = 0x9c40; // line 95（40000）
    unsigned int count_start_time_;                   // 0x6c
    unsigned int send_frame_count_;                   // 0x70
    static Zerg_Comm_Manager *instance_;              // line 103
    Zerg_Comm_Manager(); // line 39
    ~Zerg_Comm_Manager(); // line 40
public:
    int get_config(const Zerg_MMAP_BusPipe::ZERG_CONFIG &config); // line 45
    int init_socketpeer(ZERG_SERVICES_INFO &init_svcid); // line 48
    int check_safeport(ZEN_Sockaddr_In &inetadd); // line 51
    int popall_sendpipe_write(size_t want_send_frame, size_t &num_send_frame); // line 55
    void pushback_recvpipe(Comm_App_Frame *proc_frame); // line 58
    void check_freamcount(unsigned int now); // line 61
    static Zerg_Comm_Manager * instance(); // line 108
    static void clean_instance(); // line 110
};

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_COMM_MANAGER_H_H_
