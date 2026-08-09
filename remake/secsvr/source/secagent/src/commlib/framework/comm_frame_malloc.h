// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_frame_malloc.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_FRAME_MALLOC_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_FRAME_MALLOC_H_H_

#include "output/commlib/zenlib/release/include/zen_lock_guard.h"
#include <bits/stl_vector.h>
#include <stddef.h>

// sizeof = 60
struct AppFrame_Mallocor_Mgr<ZEN_Null_Mutex> { // line 27
protected:
static const size_t NUM_OF_FRAMELIST;
static const size_t NUM_OF_ONCE_INIT_FRAME;
static const size_t NUM_OF_ALLOW_LIST_IDLE_FRAME;
size_t size_appframe_[];
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, const lordrings<Comm_App_Frame*> &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, const lordrings<Comm_App_Frame*> &arg1); // line 298
    __normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > begin(); // line 332
    __normal_iterator<const ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > begin() const; // line 341
    __normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > end(); // line 350
    __normal_iterator<const ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*, std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_LIB::lordrings<Comm_App_Frame*>*, std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*, std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_LIB::lordrings<Comm_App_Frame*>*, std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, lordrings<Comm_App_Frame*> arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    lordrings<Comm_App_Frame*> & operator[](unsigned int arg0); // line 479
    const lordrings<Comm_App_Frame*> & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    lordrings<Comm_App_Frame*> & at(unsigned int arg0); // line 519
    const lordrings<Comm_App_Frame*> & at(unsigned int arg0) const; // line 537
    lordrings<Comm_App_Frame*> & front(); // line 548
    const lordrings<Comm_App_Frame*> & front() const; // line 556
    lordrings<Comm_App_Frame*> & back(); // line 564
    const lordrings<Comm_App_Frame*> & back() const; // line 572
    lordrings<Comm_App_Frame*> * data(); // line 583
    const lordrings<Comm_App_Frame*> * data() const; // line 587
    void push_back(const lordrings<Comm_App_Frame*> &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > insert(__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > arg0, const lordrings<Comm_App_Frame*> &arg1); // line 93
    void insert(__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > arg0, unsigned int arg1, const lordrings<Comm_App_Frame*> &arg2); // line 657
    __normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > erase(__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > arg0); // line 110
    __normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > erase(__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > __first, __normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > __last); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, const lordrings<Comm_App_Frame*> &arg1); // line 171
    void _M_fill_insert(__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > __position, unsigned int __n, const lordrings<Comm_App_Frame*> &__x); // line 311
    void _M_insert_aux(__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > arg0, const lordrings<Comm_App_Frame*> &arg1); // line 249
} frame_pool_;
ZEN_Null_Mutex zen_lock_;
static AppFrame_Mallocor_Mgr<ZEN_Null_Mutex> *instance_;
public:
void AppFrame_Mallocor_Mgr(); // line 198
void ~AppFrame_Mallocor_Mgr(); // line 216
void initialize(unsigned int init_num, unsigned int max_frame_len); // line 121
Comm_App_Frame * alloc_appframe(unsigned int frame_len); // line 273
void free_appframe(Comm_App_Frame *proc_frame); // line 329
void clone_appframe(const Comm_App_Frame *model_freame, Comm_App_Frame *&cloned_frame); // line 308
size_t GetMaxFrameLength(); // line 149
void AdjustPoolListCapacity(); // line 350
static AppFrame_Mallocor_Mgr<ZEN_Null_Mutex> * instance(); // line 400
static void clean_instance(); // line 412
protected:
void ExtendFrameListCapacity(unsigned int list_no, unsigned int extend_num); // line 385
size_t GetRoundUp(unsigned int arg0); // line 167
};
extern size_t i; // line 171
extern size_t old_capacity; // line 387
extern ZEN_Lock_Guard<ZEN_Null_Mutex> tmp_guard; // line 275
extern const size_t NUM_OF_FRAMELIST; // line 30
extern const size_t NUM_OF_ONCE_INIT_FRAME; // line 33
extern const size_t NUM_OF_ALLOW_LIST_IDLE_FRAME; // line 36

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_FRAME_MALLOC_H_H_
