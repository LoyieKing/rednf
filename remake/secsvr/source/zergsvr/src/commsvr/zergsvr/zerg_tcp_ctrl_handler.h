// Reconstructed from zergsvr DWARF + disassembly.
// TCP_Svc_Handler：TCP 服务连接处理器，sizeof = 124。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_TCP_CTRL_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_TCP_CTRL_HANDLER_H_H_

#include <sstream>
#include <stddef.h>
#include <time.h>

#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/zenlib/zen_boost_lord_rings.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"
#include "src/commlib/zenlib/zen_event_reactor_base.h"
#include "src/commlib/zenlib/zen_socket_stream.h"
#include "src/commlib/zenlib/zen_socket_addr_in.h"
#include "src/commlib/zenlib/zen_timer_handler_base.h"
#include "src/commsvr/zergsvr/zerg_auto_connect.h"
#include "src/commsvr/zergsvr/zerg_comm_manager.h"
#include "src/commsvr/zergsvr/zerg_service_info_set.h"

struct ZBuffer_Storage;
struct Comm_Stat_Monitor;
struct Comm_App_Frame;
struct ZByteBuffer;

// sizeof = 124
struct TCP_Svc_Handler : public ZEN_Event_Handler, public ZEN_Timer_Handler { // line 44
    enum HANDLER_MODE {
        HANDLER_MODE_INVALID = 0,
        HANDLER_MODE_CONNECT = 1,
        HANDLER_MODE_ACCEPTED = 2,
    };
    enum PEER_STATUS {
        PEER_STATUS_NOACTIVE = 0,
        PEER_STATUS_JUST_ACCEPT = 1,
        PEER_STATUS_JUST_CONNECT = 2,
        PEER_STATUS_ACTIVE = 3,
    };
protected:
    static const int TCPCTRL_TIME_ID[]; // line 71
    static const size_t MAX_OF_ACCEPT_PEER_SEND_DEQUE; // line 75
    static const size_t MAX_OF_CONNECT_PEER_SEND_DEQUE; // line 77
    static const size_t MAX_OF_SPEC_NO_ENCRYPT_CMD; // line 80
    static const unsigned int DEFAULT_TIME_OUT_SEC; // line 83
    static const unsigned int STAT_TIMER_INTERVAL_SEC; // line 87
    static const size_t MAX_SESSION_KEY_LEN; // line 90
    static Zerg_Comm_Manager *zerg_comm_mgr_; // line 53
    static ZBuffer_Storage *zbuffer_storage_; // line 51
    static Comm_Stat_Monitor *server_status_; // line 55
    static size_t max_accept_svr_; // line 70
    static size_t max_connect_svr_; // line 72
    static size_t accpet_threshold_warn_; // line 75
    static size_t threshold_warn_number_; // line 77
    static bool if_proxy_; // line 58
    static unsigned int max_frame_len_; // line 42
    static unsigned int connect_timeout_; // line 29
    static unsigned int receive_timeout_; // line 31
    static Zerg_Auto_Connector zerg_auto_connect_; // line 80
    static Service_Info_Set svr_peer_info_set_; // line 38
    static size_t num_accept_peer_; // line 64
    static size_t num_connect_peer_; // line 66
    static bool if_check_frame_; // line 82
    static ZEN_LIB::lordrings<TCP_Svc_Handler *> pool_of_acpthdl_; // line 85
    static ZEN_LIB::lordrings<TCP_Svc_Handler *> pool_of_cnthdl_; // line 87
    static size_t snd_buf_size_; // line 90
    static size_t connect_send_deque_size_; // line 93
    static unsigned int handler_id_builder_; // line 95
    static unsigned int game_id_; // line 61

    TCP_Svc_Handler::HANDLER_MODE handler_mode_; // +20
    SERVICES_ID my_svc_info_; // +24
    SERVICES_ID peer_svr_info_; // +30
    ZByteBuffer *rcv_buffer_; // +36
    ZEN_LIB::lordrings<ZByteBuffer *> snd_buffer_deque_; // +40
    size_t recieve_counter_; // +56
    size_t send_counter_; // +60
    size_t recieve_bytes_; // +64
    size_t send_bytes_; // +68
    ZEN_Socket_Stream socket_peer_; // +72
    ZEN_Sockaddr_In peer_address_; // +76
    TCP_Svc_Handler::PEER_STATUS peer_status_; // +104
    int timeout_time_id_; // +108
    unsigned int receive_times_; // +112
    bool sessionkey_verify_; // +116
    bool if_force_close_; // +117
    time_t start_live_time_; // +120
public:
    TCP_Svc_Handler(const TCP_Svc_Handler &arg0);
protected:
    TCP_Svc_Handler(TCP_Svc_Handler::HANDLER_MODE handler_mode); // line 108
    virtual ~TCP_Svc_Handler(); // line 401
public:
    void init_tcpsvr_handler(const SERVICES_ID &my_svcinfo,
                             const ZEN_Socket_Stream &sockstream,
                             const ZEN_Sockaddr_In &socketaddr,
                             bool sessionkey_verify); // line 160
    void init_tcpsvr_handler(const SERVICES_ID &my_svcinfo,
                             const SERVICES_ID &peer_svrinfo,
                             const ZEN_Socket_Stream &sockstream,
                             const ZEN_Sockaddr_In &socketaddr); // line 306
    virtual ZEN_SOCKET get_handle() const; // line 552
    virtual int handle_input(); // line 587
    virtual int handle_output(); // line 634
    virtual int handle_timeout(const ZEN_Time_Value &now_time,
                               const void *arg); // line 676
    virtual int handle_close(); // line 763
    short unsigned int get_peer_port(); // line 2053
    const char *get_peer_address(); // line 2058
    void dump_status_info(std::ostringstream &ostr_stream); // line 2086
    int send_simple_zerg_cmd(unsigned int cmd,
                             const SERVICES_ID &recv_services_info,
                             unsigned int option); // line 1722
    int send_zergheatbeat_reg(); // line 1755
    TCP_Svc_Handler::PEER_STATUS get_peer_status(); // line 1045
    unsigned int get_handle_id(); // line 558
    const ZEN_Sockaddr_In &get_peer_sockaddr() const; // line 2137
protected:
    int read_data_from_peer(size_t &szrevc); // line 1102
    int check_recv_full_frame(bool &bfull,
                              unsigned int &whole_frame_len); // line 1179
    int write_data_to_peer(size_t &szsend, bool &bfull); // line 1364
    int write_all_data_to_peer(); // line 1252
    int preprocess_recvframe(Comm_App_Frame *proc_frame); // line 890
    int process_connect_register(); // line 1061
    int push_frame_to_comm_mgr(); // line 1931
    int put_frame_to_sendlist(ZByteBuffer *tmpbuf); // line 1774
    void unite_frame_sendlist(); // line 1878
public:
    static int init_all_static_data(); // line 487
    static int get_tcpctrl_conf(const Zerg_MMAP_BusPipe::ZERG_CONFIG *config); // line 418
    static int uninit_all_staticdata(); // line 1556
    static void get_max_peer_num(size_t &maxaccept, size_t &maxconnect); // line 2045
    static int close_services_peer(const SERVICES_ID &svr_info); // line 2105
    static int find_services_peer(const SERVICES_ID &svr_info,
                                  TCP_Svc_Handler *&svchanle); // line 2122
    static void auto_connect_allserver(); // line 1549
    static TCP_Svc_Handler *AllocSvcHandlerFromPool(
        TCP_Svc_Handler::HANDLER_MODE handler_mode); // line 1513
    static void dump_status_staticinfo(std::ostringstream &ostr_stream); // line 2063
    static void dump_svcpeer_info(std::ostringstream &ostr_stream,
                                  size_t startno,
                                  size_t numquery); // line 2098
protected:
    int process_send_error(ZByteBuffer *tmpbuf, bool frame_encode); // line 1441
public:
    static int process_send_data(ZByteBuffer *tmpbuf); // line 1582
};

extern int TCPCTRL_TIME_ID[]; // line 71
extern const size_t MAX_OF_ACCEPT_PEER_SEND_DEQUE; // line 75
extern const size_t MAX_OF_CONNECT_PEER_SEND_DEQUE; // line 77
extern const unsigned int DEFAULT_TIME_OUT_SEC; // line 83
extern const unsigned int STAT_TIMER_INTERVAL_SEC; // line 87

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_TCP_CTRL_HANDLER_H_H_
