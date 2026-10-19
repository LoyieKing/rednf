// Auto-generated header stub from DWARF info
// Original path: src/formmog/tss_sdk/plugins/proto_proc/dp_hack_event_notify.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_HACK_EVENT_NOTIFY_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_HACK_EVENT_NOTIFY_H_H_

#include <bits/stl_iterator.h>
#include <bits/stl_vector.h>

typedef struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, IDPHacker *const &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, IDPHacker *const &arg1); // line 298
    __normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > begin(); // line 332
    __normal_iterator<tss_dpsdk::IDPHacker* const*,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > begin() const; // line 341
    __normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > end(); // line 350
    __normal_iterator<tss_dpsdk::IDPHacker* const*,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<tss_dpsdk::IDPHacker**, std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<tss_dpsdk::IDPHacker* const*, std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<tss_dpsdk::IDPHacker**, std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<tss_dpsdk::IDPHacker* const*, std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, IDPHacker *arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    IDPHacker *& operator[](unsigned int arg0); // line 479
    IDPHacker *const & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    IDPHacker *& at(unsigned int arg0); // line 519
    IDPHacker *const & at(unsigned int arg0) const; // line 537
    IDPHacker *& front(); // line 548
    IDPHacker *const & front() const; // line 556
    IDPHacker *& back(); // line 564
    IDPHacker *const & back() const; // line 572
    IDPHacker ** data(); // line 583
    IDPHacker *const * data() const; // line 587
    void push_back(IDPHacker *const &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > insert(__normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > arg0, IDPHacker *const &arg1); // line 93
    void insert(__normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > arg0, unsigned int arg1, IDPHacker *const &arg2); // line 657
    __normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > erase(__normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > arg0); // line 110
    __normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > erase(__normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > arg0, __normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > arg1); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, IDPHacker *const &arg1); // line 171
    void _M_fill_insert(__normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > arg0, unsigned int arg1, IDPHacker *const &arg2); // line 311
    void _M_insert_aux(__normal_iterator<tss_dpsdk::IDPHacker**,std::vector<tss_dpsdk::IDPHacker*, std::allocator<tss_dpsdk::IDPHacker*> > > arg0, IDPHacker *const &arg1); // line 249
} DPHACKERS;
extern DPHACKERS &const hacker; // line 60
extern /*anon struct*/ int IT; // line 61
namespace tss_dpsdk {
    enum E_EVENT {
        E_LOGIN = 0,
        E_LOGOUT = 1,
        E_PROC = 2,
        E_BAN_MODE = 3,
        E_DROP_CMD = 4,
        E_MSG2CLIENT = 5,
        E_VERIFY_MSG = 6,
        E_SAFE_MODE = 7,
        E_SPECIAL_CONTROL = 8,
        E_EVENT_COUNT = 9,
        E_LOGIN = 0,
        E_LOGOUT = 1,
        E_PROC = 2,
        E_BAN_MODE = 3,
        E_DROP_CMD = 4,
        E_MSG2CLIENT = 5,
        E_VERIFY_MSG = 6,
        E_SAFE_MODE = 7,
        E_SPECIAL_CONTROL = 8,
        E_EVENT_COUNT = 9,
    };
} // namespace tss_dpsdk

#endif // SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_HACK_EVENT_NOTIFY_H_H_
