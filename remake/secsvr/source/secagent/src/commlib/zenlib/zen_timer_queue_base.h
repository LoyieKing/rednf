// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_timer_queue_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_BASE_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_timer_queue_base.h"
#include <bits/stl_vector.h>
#include <stddef.h>
#include <stdint.h>

// sizeof = 60
struct ZEN_Timer_Queue : public ZEN_NON_Copyable { // line 28
    enum TRIGGER_MODE {
        TRIGGER_MODE_SYSTEM_CLOCK = 1,
        TRIGGER_MODE_CPU_TICK = 2,
    };
    // sizeof = 28
    struct ZEN_TIMER_NODE {
    public:
    int time_id_;
    ZEN_Time_Value interval_time_;
    const void *action_;
    ZEN_Timer_Handler *timer_handle_;
    uint64_t next_trigger_point_;
    ZEN_TIMER_NODE(); // line 81
    ~ZEN_TIMER_NODE(); // line 90
    void clear(); // line 94
    ZEN_Timer_Queue::ZEN_TIMER_NODE & operator=(const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg0);
    };
public:
int (***_vptr.ZEN_Timer_Queue)(...);
static const int INVALID_TIMER_ID;
static const unsigned int DEFAULT_TIMER_PRECISION_MSEC;
static const uint64_t MSEC_PER_NSEC;
static const size_t ONCE_EXTEND_NODE_NUMBER;
protected:
size_t num_timer_node_;
unsigned int timer_precision_mesc_;
size_t num_use_node_;
ZEN_Timer_Queue::TRIGGER_MODE trigger_mode_;
bool dynamic_expand_node_;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg1); // line 298
    __normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > begin(); // line 332
    __normal_iterator<const ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > begin() const; // line 341
    __normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > end(); // line 350
    __normal_iterator<const ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*, std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_Timer_Queue::ZEN_TIMER_NODE*, std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*, std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_Timer_Queue::ZEN_TIMER_NODE*, std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, ZEN_Timer_Queue::ZEN_TIMER_NODE arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    ZEN_Timer_Queue::ZEN_TIMER_NODE & operator[](unsigned int arg0); // line 479
    const ZEN_Timer_Queue::ZEN_TIMER_NODE & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    ZEN_Timer_Queue::ZEN_TIMER_NODE & at(unsigned int arg0); // line 519
    const ZEN_Timer_Queue::ZEN_TIMER_NODE & at(unsigned int arg0) const; // line 537
    ZEN_Timer_Queue::ZEN_TIMER_NODE & front(); // line 548
    const ZEN_Timer_Queue::ZEN_TIMER_NODE & front() const; // line 556
    ZEN_Timer_Queue::ZEN_TIMER_NODE & back(); // line 564
    const ZEN_Timer_Queue::ZEN_TIMER_NODE & back() const; // line 572
    ZEN_Timer_Queue::ZEN_TIMER_NODE * data(); // line 583
    const ZEN_Timer_Queue::ZEN_TIMER_NODE * data() const; // line 587
    void push_back(const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > insert(__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > arg0, const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg1); // line 93
    void insert(__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > arg0, unsigned int arg1, const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg2); // line 657
    __normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > erase(__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > arg0); // line 110
    __normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > erase(__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > __first, __normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > __last); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg1); // line 171
    void _M_fill_insert(__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > __position, unsigned int __n, const ZEN_Timer_Queue::ZEN_TIMER_NODE &__x); // line 311
    void _M_insert_aux(__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > arg0, const ZEN_Timer_Queue::ZEN_TIMER_NODE &arg1); // line 249
} time_node_ary_;
int free_node_id_head_;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, const int &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, const int &arg1); // line 298
    __normal_iterator<int*,std::vector<int, std::allocator<int> > > begin(); // line 332
    __normal_iterator<const int*,std::vector<int, std::allocator<int> > > begin() const; // line 341
    __normal_iterator<int*,std::vector<int, std::allocator<int> > > end(); // line 350
    __normal_iterator<const int*,std::vector<int, std::allocator<int> > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<const int*, std::vector<int, std::allocator<int> > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<const int*, std::vector<int, std::allocator<int> > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, int arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    int & operator[](unsigned int arg0); // line 479
    const int & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    int & at(unsigned int arg0); // line 519
    const int & at(unsigned int arg0) const; // line 537
    int & front(); // line 548
    const int & front() const; // line 556
    int & back(); // line 564
    const int & back() const; // line 572
    int * data(); // line 583
    const int * data() const; // line 587
    void push_back(const int &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<int*,std::vector<int, std::allocator<int> > > insert(__normal_iterator<int*,std::vector<int, std::allocator<int> > > arg0, const int &arg1); // line 93
    void insert(__normal_iterator<int*,std::vector<int, std::allocator<int> > > arg0, unsigned int arg1, const int &arg2); // line 657
    __normal_iterator<int*,std::vector<int, std::allocator<int> > > erase(__normal_iterator<int*,std::vector<int, std::allocator<int> > > arg0); // line 110
    __normal_iterator<int*,std::vector<int, std::allocator<int> > > erase(__normal_iterator<int*,std::vector<int, std::allocator<int> > > __first, __normal_iterator<int*,std::vector<int, std::allocator<int> > > __last); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, const int &arg1); // line 171
    void _M_fill_insert(__normal_iterator<int*,std::vector<int, std::allocator<int> > > __position, unsigned int __n, const int &__x); // line 311
    void _M_insert_aux(__normal_iterator<int*,std::vector<int, std::allocator<int> > > arg0, const int &arg1); // line 249
} free_node_id_list_;
uint64_t prev_trigger_msec_;
static ZEN_Timer_Queue *instance_;
public:
ZEN_Timer_Queue(const ZEN_Timer_Queue &arg0);
protected:
ZEN_Timer_Queue(size_t arg0, unsigned int arg1, ZEN_Timer_Queue::TRIGGER_MODE arg2, bool arg3); // line 18
ZEN_Timer_Queue(); // line 34
public:
virtual ~ZEN_Timer_Queue(); // line 44
virtual int schedule_timer(ZEN_Timer_Handler *arg0, const void *arg1, const ZEN_Time_Value &arg2, const ZEN_Time_Value &arg3); // line 166
virtual int cancel_timer(int timer_id); // line 161
protected:
virtual int get_frist_nodeid(int &arg0); // line 175
virtual size_t dispatch_timer(const ZEN_Time_Value &arg0, uint64_t arg1); // line 180
public:
virtual size_t expire(); // line 394
virtual int cancel_timer(const ZEN_Timer_Handler *timer_hdl); // line 167
virtual int extend_node(size_t num_timer_node, size_t &old_num_node); // line 114
virtual int close(); // line 96
protected:
virtual int reschedule_timer(int arg0, uint64_t arg1); // line 205
int initialize(size_t num_timer_node, unsigned int timer_precision_mesc, ZEN_Timer_Queue::TRIGGER_MODE trigger_mode, bool dynamic_expand_node); // line 53
int alloc_timernode(ZEN_Timer_Handler *timer_hdl, const void *action, const ZEN_Time_Value &delay_time, const ZEN_Time_Value &interval_time, int &time_node_id, ZEN_Timer_Queue::ZEN_TIMER_NODE *&alloc_time_node); // line 205
void calc_next_trigger(int time_node_id, uint64_t now_trigger_msec, bool &continue_trigger); // line 294
int free_timernode(int time_node_id); // line 319
int get_first_timeout(ZEN_Time_Value *first_timeout); // line 349
public:
static ZEN_Timer_Queue * instance(); // line 424
static void instance(ZEN_Timer_Queue *pinstatnce); // line 430
static void clean_instance(); // line 438
};
extern const int INVALID_TIMER_ID; // line 32
extern const unsigned int DEFAULT_TIMER_PRECISION_MSEC; // line 47

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_TIMER_QUEUE_BASE_H_H_
