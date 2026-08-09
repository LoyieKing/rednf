// Auto-generated header stub from DWARF info
// Original path: output/commsvr/zergsvr/release/include/zerg_comm_manager.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_COMM_MANAGER_H_H_
#define SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_COMM_MANAGER_H_H_

#include <bits/stl_vector.h>
#include <stddef.h>

// sizeof = 116
struct Zerg_Comm_Manager { // line 27
protected:
static const unsigned int MAX_ONCE_SEND_FRAME;
static const size_t MAX_NUMBER_OF_MONITOR_FRAME;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, TCP_Accept_Handler *const &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, TCP_Accept_Handler *const &arg1); // line 298
    __normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > begin(); // line 332
    __normal_iterator<TCP_Accept_Handler* const*,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > begin() const; // line 341
    __normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > end(); // line 350
    __normal_iterator<TCP_Accept_Handler* const*,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<TCP_Accept_Handler**, std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<TCP_Accept_Handler* const*, std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<TCP_Accept_Handler**, std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<TCP_Accept_Handler* const*, std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, TCP_Accept_Handler *arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    TCP_Accept_Handler *& operator[](unsigned int arg0); // line 479
    TCP_Accept_Handler *const & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    TCP_Accept_Handler *& at(unsigned int arg0); // line 519
    TCP_Accept_Handler *const & at(unsigned int arg0) const; // line 537
    TCP_Accept_Handler *& front(); // line 548
    TCP_Accept_Handler *const & front() const; // line 556
    TCP_Accept_Handler *& back(); // line 564
    TCP_Accept_Handler *const & back() const; // line 572
    TCP_Accept_Handler ** data(); // line 583
    TCP_Accept_Handler *const * data() const; // line 587
    void push_back(TCP_Accept_Handler *const &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > insert(__normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > arg0, TCP_Accept_Handler *const &arg1); // line 93
    void insert(__normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > arg0, unsigned int arg1, TCP_Accept_Handler *const &arg2); // line 657
    __normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > erase(__normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > arg0); // line 110
    __normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > erase(__normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > arg0, __normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > arg1); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, TCP_Accept_Handler *const &arg1); // line 171
    void _M_fill_insert(__normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > arg0, unsigned int arg1, TCP_Accept_Handler *const &arg2); // line 311
    void _M_insert_aux(__normal_iterator<TCP_Accept_Handler**,std::vector<TCP_Accept_Handler*, std::allocator<TCP_Accept_Handler*> > > arg0, TCP_Accept_Handler *const &arg1); // line 249
} zerg_acceptor_;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, UDP_Svc_Handler *const &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, UDP_Svc_Handler *const &arg1); // line 298
    __normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > begin(); // line 332
    __normal_iterator<UDP_Svc_Handler* const*,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > begin() const; // line 341
    __normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > end(); // line 350
    __normal_iterator<UDP_Svc_Handler* const*,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<UDP_Svc_Handler**, std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<UDP_Svc_Handler* const*, std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<UDP_Svc_Handler**, std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<UDP_Svc_Handler* const*, std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, UDP_Svc_Handler *arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    UDP_Svc_Handler *& operator[](unsigned int arg0); // line 479
    UDP_Svc_Handler *const & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    UDP_Svc_Handler *& at(unsigned int arg0); // line 519
    UDP_Svc_Handler *const & at(unsigned int arg0) const; // line 537
    UDP_Svc_Handler *& front(); // line 548
    UDP_Svc_Handler *const & front() const; // line 556
    UDP_Svc_Handler *& back(); // line 564
    UDP_Svc_Handler *const & back() const; // line 572
    UDP_Svc_Handler ** data(); // line 583
    UDP_Svc_Handler *const * data() const; // line 587
    void push_back(UDP_Svc_Handler *const &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > insert(__normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > arg0, UDP_Svc_Handler *const &arg1); // line 93
    void insert(__normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > arg0, unsigned int arg1, UDP_Svc_Handler *const &arg2); // line 657
    __normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > erase(__normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > arg0); // line 110
    __normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > erase(__normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > arg0, __normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > arg1); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, UDP_Svc_Handler *const &arg1); // line 171
    void _M_fill_insert(__normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > arg0, unsigned int arg1, UDP_Svc_Handler *const &arg2); // line 311
    void _M_insert_aux(__normal_iterator<UDP_Svc_Handler**,std::vector<UDP_Svc_Handler*, std::allocator<UDP_Svc_Handler*> > > arg0, UDP_Svc_Handler *const &arg1); // line 249
} zerg_updsvc_;
unsigned int error_try_num_;
size_t monitor_size_;
unsigned int monitor_cmd_[];
Zerg_MMAP_BusPipe *zerg_mmap_pipe_;
ZBuffer_Storage *zbuffer_storage_;
Comm_Stat_Monitor *server_status_;
static const unsigned int SEND_FRAME_ALERT_VALUE;
unsigned int count_start_time_;
unsigned int send_frame_count_;
static Zerg_Comm_Manager *instance_;
Zerg_Comm_Manager(); // line 39
~Zerg_Comm_Manager(); // line 40
public:
int get_config(const Zerg_MMAP_BusPipe::ZERG_CONFIG &arg0); // line 45
int init_socketpeer(ZERG_SERVICES_INFO &arg0); // line 48
int check_safeport(ZEN_Sockaddr_In &arg0); // line 51
int popall_sendpipe_write(size_t arg0, size_t &arg1); // line 55
void pushback_recvpipe(Comm_App_Frame *arg0); // line 58
void check_freamcount(unsigned int arg0); // line 61
static Zerg_Comm_Manager * instance(); // line 108
static void clean_instance(); // line 110
};
extern int ret; // line 143
extern const size_t MAX_NUMBER_OF_MONITOR_FRAME; // line 69
extern size_t i; // line 133
extern const unsigned int SEND_FRAME_ALERT_VALUE; // line 95

#endif // SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_COMM_MANAGER_H_H_
