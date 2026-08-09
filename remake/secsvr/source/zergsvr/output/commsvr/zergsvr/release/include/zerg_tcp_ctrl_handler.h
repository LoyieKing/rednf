// Auto-generated header stub from DWARF info
// Original path: output/commsvr/zergsvr/release/include/zerg_tcp_ctrl_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_TCP_CTRL_HANDLER_H_H_
#define SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_TCP_CTRL_HANDLER_H_H_

#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_socket_stream.h"
#include "src/commsvr/zergsvr/zerg_auto_connect.h"
#include "src/commsvr/zergsvr/zerg_service_info_set.h"
#include <stddef.h>
#include <time.h>

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
static const int TCPCTRL_TIME_ID[];
static const size_t MAX_OF_ACCEPT_PEER_SEND_DEQUE;
static const size_t MAX_OF_CONNECT_PEER_SEND_DEQUE;
static const size_t MAX_OF_SPEC_NO_ENCRYPT_CMD;
static const unsigned int DEFAULT_TIME_OUT_SEC;
static const unsigned int STAT_TIMER_INTERVAL_SEC;
static const size_t MAX_SESSION_KEY_LEN;
static Zerg_Comm_Manager *zerg_comm_mgr_;
static ZBuffer_Storage *zbuffer_storage_;
static Comm_Stat_Monitor *server_status_;
static size_t max_accept_svr_;
static size_t max_connect_svr_;
static size_t accpet_threshold_warn_;
static size_t threshold_warn_number_;
static bool if_proxy_;
static unsigned int max_frame_len_;
static unsigned int connect_timeout_;
static unsigned int receive_timeout_;
static Zerg_Auto_Connector zerg_auto_connect_;
static Service_Info_Set svr_peer_info_set_;
static size_t num_accept_peer_;
static size_t num_connect_peer_;
static bool if_check_frame_;
static /*anon struct*/ int pool_of_acpthdl_;
static /*anon struct*/ int pool_of_cnthdl_;
static size_t snd_buf_size_;
static size_t connect_send_deque_size_;
static unsigned int handler_id_builder_;
static unsigned int game_id_;
TCP_Svc_Handler::HANDLER_MODE handler_mode_;
SERVICES_ID my_svc_info_;
SERVICES_ID peer_svr_info_;
ZByteBuffer *rcv_buffer_;
// anonymous struct, sizeof = 16
struct {
    protected:
    size_t cycdeque_start_;
    size_t cycdeque_size_;
    size_t cycdeque_capacity_;
    ZByteBuffer **value_ptr_;
    public:
    void lordrings(); // line 169
    void lordrings(unsigned int arg0); // line 178
    void ~lordrings(); // line 188
    void initialize(unsigned int arg0); // line 198
    void finit(); // line 217
    void clear(); // line 232
    size_t size() const; // line 239
    size_t freesize() const; // line 244
    size_t capacity() const; // line 250
    bool full() const; // line 256
    bool empty() const; // line 268
    bool resize(unsigned int arg0); // line 280
    bool push_back(ZByteBuffer *const &arg0, bool arg1); // line 317
    bool push_front(ZByteBuffer *const &arg0, bool arg1); // line 348
    bool pop_front(ZByteBuffer *&arg0); // line 381
    bool pop_front(); // line 397
    bool pop_back(ZByteBuffer *&arg0); // line 412
    bool pop_back(); // line 427
    ZByteBuffer *& operator[](unsigned int arg0); // line 441
    ZByteBuffer *const & operator[](unsigned int arg0) const; // line 447
    _lordrings_iterator<ZByteBuffer*> begin(); // line 453
    _lordrings_iterator<ZByteBuffer*> end(); // line 459
} snd_buffer_deque_;
size_t recieve_counter_;
size_t send_counter_;
size_t recieve_bytes_;
size_t send_bytes_;
ZEN_Socket_Stream socket_peer_;
ZEN_Sockaddr_In peer_address_;
TCP_Svc_Handler::PEER_STATUS peer_status_;
int timeout_time_id_;
unsigned int receive_times_;
bool sessionkey_verify_;
bool if_force_close_;
time_t start_live_time_;
public:
TCP_Svc_Handler(const TCP_Svc_Handler &arg0);
protected:
TCP_Svc_Handler(TCP_Svc_Handler::HANDLER_MODE arg0); // line 108
virtual ~TCP_Svc_Handler(); // line 401
public:
void init_tcpsvr_handler(const SERVICES_ID &my_svcinfo, const ZEN_Socket_Stream &sockstream, const ZEN_Sockaddr_In &socketaddr, bool sessionkey_verify); // line 160
void init_tcpsvr_handler(const SERVICES_ID &my_svcinfo, const SERVICES_ID &peer_svrinfo, const ZEN_Socket_Stream &sockstream, const ZEN_Sockaddr_In &socketaddr); // line 306
virtual ZEN_SOCKET get_handle() const; // line 552
virtual int handle_input(); // line 587
virtual int handle_output(); // line 634
virtual int handle_timeout(const ZEN_Time_Value &now_time, const void *arg); // line 676
virtual int handle_close(); // line 763
short unsigned int get_peer_port(); // line 2053
const char * get_peer_address(); // line 2058
void dump_status_info(ostringstream &ostr_stream); // line 2086
int send_simple_zerg_cmd(unsigned int cmd, const SERVICES_ID &recv_services_info, unsigned int option); // line 1722
int send_zergheatbeat_reg(); // line 1755
TCP_Svc_Handler::PEER_STATUS get_peer_status(); // line 1045
unsigned int get_handle_id(); // line 558
const ZEN_Sockaddr_In & get_peer_sockaddr() const; // line 2137
protected:
int read_data_from_peer(size_t &szrevc); // line 1102
int check_recv_full_frame(bool &bfull, unsigned int &whole_frame_len); // line 1179
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
static int popall_sendpipe_write(size_t &arg0); // line 322
static void get_max_peer_num(size_t &maxaccept, size_t &maxconnect); // line 2045
static int close_services_peer(const SERVICES_ID &svr_info); // line 2105
static int find_services_peer(const SERVICES_ID &svr_info, TCP_Svc_Handler *&svchanle); // line 2122
static void auto_connect_allserver(); // line 1549
static TCP_Svc_Handler * AllocSvcHandlerFromPool(TCP_Svc_Handler::HANDLER_MODE handler_mode); // line 1513
static void dump_status_staticinfo(ostringstream &ostr_stream); // line 2063
static void dump_svcpeer_info(ostringstream &ostr_stream, size_t startno, size_t numquery); // line 2098
protected:
int process_send_error(ZByteBuffer *tmpbuf, bool frame_encode); // line 1441
public:
static int process_send_data(ZByteBuffer *tmpbuf); // line 1582
};
extern int TCPCTRL_TIME_ID[]; // line 71
extern const size_t MAX_OF_CONNECT_PEER_SEND_DEQUE; // line 77
extern const unsigned int STAT_TIMER_INTERVAL_SEC; // line 87
namespace TCP_Svc_Handler {
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
} // namespace TCP_Svc_Handler

#endif // SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_TCP_CTRL_HANDLER_H_H_
