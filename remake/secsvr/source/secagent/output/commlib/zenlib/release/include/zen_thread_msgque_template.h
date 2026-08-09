// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_thread_msgque_template.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_MSGQUE_TEMPLATE_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_MSGQUE_TEMPLATE_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include <bits/stl_deque.h>
#include <stddef.h>

// sizeof = 48
struct ZEN_Message_Queue<ZEN_NULL_SYNCH,Comm_App_Frame*,std::deque<Comm_App_Frame*, std::allocator<Comm_App_Frame*> > > : public ZEN_NON_Copyable { // line 11
protected:
size_t queue_max_size_;
size_t queue_cur_size_;
// anonymous struct, sizeof = 40
struct {
    protected:
    static size_t _S_buffer_size(); // line 639
    public:
    void deque(const /*anon struct*/ int &arg0); // line 666
    void deque(unsigned int arg0, Comm_App_Frame *const &arg1, const /*anon struct*/ int &arg2); // line 678
    void deque(const /*anon struct*/ int &arg0); // line 689
    void ~deque(); // line 724
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 69
    void assign(unsigned int arg0, Comm_App_Frame *const &arg1); // line 749
    /*anon struct*/ int get_allocator() const; // line 774
    /*anon struct*/ int begin(); // line 783
    _Deque_iterator<Comm_App_Frame*,Comm_App_Frame* const&,Comm_App_Frame* const*> begin() const; // line 791
    /*anon struct*/ int end(); // line 800
    _Deque_iterator<Comm_App_Frame*,Comm_App_Frame* const&,Comm_App_Frame* const*> end() const; // line 809
    reverse_iterator<std::_Deque_iterator<Comm_App_Frame*, Comm_App_Frame*&, Comm_App_Frame**> > rbegin(); // line 818
    reverse_iterator<std::_Deque_iterator<Comm_App_Frame*, Comm_App_Frame* const&, Comm_App_Frame* const*> > rbegin() const; // line 827
    reverse_iterator<std::_Deque_iterator<Comm_App_Frame*, Comm_App_Frame*&, Comm_App_Frame**> > rend(); // line 836
    reverse_iterator<std::_Deque_iterator<Comm_App_Frame*, Comm_App_Frame* const&, Comm_App_Frame* const*> > rend() const; // line 844
    size_t size() const; // line 850
    size_t max_size() const; // line 855
    void resize(unsigned int arg0, Comm_App_Frame *arg1); // line 870
    bool empty() const; // line 884
    Comm_App_Frame *& operator[](unsigned int arg0); // line 900
    Comm_App_Frame *const & operator[](unsigned int arg0) const; // line 915
    protected:
    void _M_range_check(unsigned int arg0) const; // line 921
    public:
    Comm_App_Frame *& at(unsigned int arg0); // line 940
    Comm_App_Frame *const & at(unsigned int arg0) const; // line 958
    Comm_App_Frame *& front(); // line 969
    Comm_App_Frame *const & front() const; // line 977
    Comm_App_Frame *& back(); // line 985
    Comm_App_Frame *const & back() const; // line 997
    void push_front(Comm_App_Frame *const &arg0); // line 1015
    void push_back(Comm_App_Frame *const &arg0); // line 1036
    void pop_front(); // line 1057
    void pop_back(); // line 1078
    /*anon struct*/ int insert(/*anon struct*/ int arg0, Comm_App_Frame *const &arg1); // line 90
    void insert(/*anon struct*/ int arg0, unsigned int arg1, Comm_App_Frame *const &arg2); // line 1112
    /*anon struct*/ int erase(/*anon struct*/ int arg0); // line 111
    /*anon struct*/ int erase(/*anon struct*/ int arg0, /*anon struct*/ int arg1); // line 132
    void swap(/*anon struct*/ int &arg0); // line 1180
    void clear(); // line 172
    protected:
    void _M_fill_initialize(Comm_App_Frame *const &arg0); // line 263
    void _M_fill_assign(unsigned int arg0, Comm_App_Frame *const &arg1); // line 1310
    void _M_push_back_aux(Comm_App_Frame *const &__t); // line 345
    void _M_push_front_aux(Comm_App_Frame *const &arg0); // line 368
    void _M_pop_back_aux(); // line 391
    void _M_pop_front_aux(); // line 406
    void _M_fill_insert(/*anon struct*/ int arg0, unsigned int arg1, Comm_App_Frame *const &arg2); // line 220
    /*anon struct*/ int _M_insert_aux(/*anon struct*/ int arg0, Comm_App_Frame *const &arg1); // line 472
    void _M_insert_aux(/*anon struct*/ int arg0, unsigned int arg1, Comm_App_Frame *const &arg2); // line 505
    /*anon struct*/ int _M_reserve_elements_at_front(unsigned int arg0); // line 1402
    /*anon struct*/ int _M_reserve_elements_at_back(unsigned int arg0); // line 1412
    void _M_new_elements_at_front(unsigned int arg0); // line 679
    void _M_new_elements_at_back(unsigned int arg0); // line 701
    void _M_reserve_map_at_back(unsigned int arg0); // line 1440
    void _M_reserve_map_at_front(unsigned int arg0); // line 1448
    void _M_reallocate_map(unsigned int __nodes_to_add, bool __add_at_front); // line 723
} message_queue_;
public:
void ZEN_Message_Queue(unsigned int arg0); // line 30
void ~ZEN_Message_Queue(); // line 36
bool empty(); // line 41
bool full(); // line 52
int enqueue(Comm_App_Frame *const &arg0); // line 65
int enqueue(Comm_App_Frame *const &arg0, ZEN_Time_Value &arg1); // line 80
int dequeue(Comm_App_Frame *&arg0); // line 86
int dequeue(Comm_App_Frame *&arg0, ZEN_Time_Value &arg1); // line 101
void clear(); // line 107
size_t size(); // line 114
};
// sizeof = 48
struct ZEN_Message_Queue_Deque<ZEN_NULL_SYNCH,Comm_App_Frame*> : public ZEN_Message_Queue<ZEN_NULL_SYNCH,Comm_App_Frame*,std::deque<Comm_App_Frame*, std::allocator<Comm_App_Frame*> > > { // line 87
public:
void ZEN_Message_Queue_Deque(unsigned int arg0); // line 163
void ~ZEN_Message_Queue_Deque(); // line 168
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_MSGQUE_TEMPLATE_H_H_
