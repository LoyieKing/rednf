// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_thread_wait_mgr.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_WAIT_MGR_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_WAIT_MGR_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include <bits/stl_list.h>

// sizeof = 8
struct Zen_Thread_Wait_Manager : public ZEN_NON_Copyable { // line 16
    // sizeof = 8
    struct MANAGE_WAIT_INFO {
    public:
    ZEN_THREAD_ID wait_thr_id_;
    int wait_group_id_;
    MANAGE_WAIT_INFO(ZEN_THREAD_ID arg0, int arg1); // line 24
    ~MANAGE_WAIT_INFO(); // line 29
    };
protected:
static Zen_Thread_Wait_Manager *instance_;
// anonymous struct, sizeof = 8
struct {
    protected:
    _List_node<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> * _M_create_node(const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg0); // line 446
    public:
    void list(const /*anon struct*/ int &arg0); // line 468
    void list(unsigned int arg0, const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg1, const /*anon struct*/ int &arg2); // line 480
    void list(const /*anon struct*/ int &arg0); // line 491
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 120
    void assign(unsigned int arg0, const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg1); // line 544
    /*anon struct*/ int get_allocator() const; // line 570
    _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> begin(); // line 579
    _List_const_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> begin() const; // line 588
    _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> end(); // line 597
    _List_const_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> end() const; // line 606
    reverse_iterator<std::_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> > rbegin(); // line 615
    reverse_iterator<std::_List_const_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> > rbegin() const; // line 624
    reverse_iterator<std::_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> > rend(); // line 633
    reverse_iterator<std::_List_const_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> > rend() const; // line 642
    bool empty() const; // line 651
    size_t size() const; // line 656
    size_t max_size() const; // line 661
    void resize(unsigned int arg0, Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO arg1); // line 105
    Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO & front(); // line 683
    const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO & front() const; // line 691
    Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO & back(); // line 699
    const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO & back() const; // line 711
    void push_front(const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg0); // line 730
    void pop_front(); // line 746
    void push_back(const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg0); // line 760
    void pop_back(); // line 775
    _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> insert(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg1); // line 85
    void insert(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, unsigned int arg1, const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg2); // line 806
    _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> erase(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> __position); // line 95
    _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> erase(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg1); // line 871
    void swap(/*anon struct*/ int &arg0); // line 888
    void clear(); // line 898
    void splice(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, /*anon struct*/ int &arg1); // line 915
    void splice(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, /*anon struct*/ int &arg1, _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg2); // line 931
    void splice(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, /*anon struct*/ int &arg1, _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg2, _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg3); // line 953
    void remove(const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg0); // line 174
    void unique(); // line 191
    void merge(/*anon struct*/ int &arg0); // line 211
    void reverse(); // line 1051
    void sort(); // line 238
    protected:
    void _M_fill_assign(unsigned int arg0, const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg1); // line 142
    void _M_fill_insert(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, unsigned int arg1, const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg2); // line 1123
    void _M_transfer(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg1, _List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg2); // line 1132
    void _M_insert(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0, const Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO &arg1); // line 1137
    void _M_erase(_List_iterator<Zen_Thread_Wait_Manager::MANAGE_WAIT_INFO> arg0); // line 1145
    public:
    void ~list();
} wait_thread_list_;
public:
Zen_Thread_Wait_Manager(); // line 54
~Zen_Thread_Wait_Manager(); // line 55
void record_wait_thread(ZEN_THREAD_ID wait_thr_id, int wait_group_id); // line 58
void record_wait_thread(const Zen_Thread_Task *wait_thr_task); // line 60
void wait_all(); // line 63
void wait_group(int group_id); // line 66
static Zen_Thread_Wait_Manager * instance(); // line 71
static void clean_instance(); // line 73
};
namespace Zen_Thread_Wait_Manager {
    // sizeof = 8
    struct MANAGE_WAIT_INFO { // line 21
    public:
    ZEN_THREAD_ID wait_thr_id_;
    int wait_group_id_;
    MANAGE_WAIT_INFO(ZEN_THREAD_ID arg0, int arg1); // line 24
    ~MANAGE_WAIT_INFO(); // line 29
    };
} // namespace Zen_Thread_Wait_Manager

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_WAIT_MGR_H_H_
