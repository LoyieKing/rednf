// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_transaction_manager.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TRANSACTION_MANAGER_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TRANSACTION_MANAGER_H_H_

#include "src/commlib/framework/comm_service_info.h"
#include <ext/hash_map>
#include <ext/hash_set>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

typedef lordrings<Transaction_Base*> POOL_OF_REGISTERTRANS;
// sizeof = 68
struct CREATE_TRANS_RECORD { // line 68
public:
unsigned int trans_command_;
POOL_OF_REGISTERTRANS crttrs_cmd_pool_;
bool if_auto_trans_lock_;
unsigned int trans_lock_cmd_;
uint64_t create_trans_num_;
uint64_t destroy_right_num_;
uint64_t destroy_timeout_num_;
uint64_t destroy_exception_num_;
uint64_t trans_consume_time_;
CREATE_TRANS_RECORD(unsigned int arg0); // line 71
~CREATE_TRANS_RECORD(); // line 72
};
// sizeof = 8
struct TRANS_LOCK_RECORD { // line 112
public:
unsigned int lock_qq_uin_;
unsigned int lock_trans_cmd_;
TRANS_LOCK_RECORD(unsigned int arg0, unsigned int arg1); // line 121
TRANS_LOCK_RECORD(); // line 122
~TRANS_LOCK_RECORD(); // line 123
};
// sizeof = 1
struct HASH_OF_TRANS_LOCK { // line 128
public:
size_t operator()(const TRANS_LOCK_RECORD &arg0) const; // line 130
};
// sizeof = 1
struct EQUAL_OF_TRANS_LOCK { // line 138
public:
bool operator()(const TRANS_LOCK_RECORD &arg0, const TRANS_LOCK_RECORD &arg1) const; // line 140
};
// sizeof = 120
struct Transaction_Manager { // line 151
public:
int (***_vptr.Transaction_Manager)(...);
protected:
static const size_t MAX_ONCE_PROCESS_FRAME;
static const size_t POOL_EXTEND_TRANSACTION_NUM;
static const size_t RECYCLE_POOL_THRESHOLD_VALUE;
static const size_t INIT_FRAME_MALLOC_NUMBER;
static const size_t INNER_QUEUE_WATER_MARK;
// anonymous struct, sizeof = 20
struct {
    private:
    // anonymous struct, sizeof = 20
    struct {
        private:
        // anonymous struct, sizeof = 1
        struct {
            public:
            void allocator(); // line 97
            void allocator(const /*anon struct*/ int &arg0); // line 99
            void ~allocator(); // line 105
            void allocator<std::pair<const unsigned int, Transaction_Base*> >(const allocator<std::pair<const unsigned int, Transaction_Base*> > &arg0); // line 103
        } _M_node_allocator;
        // anonymous struct, sizeof = 1
        struct {
            public:
            size_t operator()(unsigned int arg0) const; // line 150
        } _M_hash;
        // anonymous struct, sizeof = 1
        struct {
            public:
            bool operator()(const unsigned int &arg0, const unsigned int &arg1) const; // line 199
        } _M_equals;
        // anonymous struct, sizeof = 1
        struct {
            public:
            const unsigned int & operator()(/*anon struct*/ int &arg0) const; // line 546
            const unsigned int & operator()(const /*anon struct*/ int &arg0) const; // line 550
        } _M_get_key;
        // anonymous struct, sizeof = 12
        struct {
            public:
            void vector(const /*anon struct*/ int &arg0); // line 202
            void vector(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &arg1, const /*anon struct*/ int &arg2); // line 215
            void vector(const /*anon struct*/ int &arg0); // line 232
            void ~vector(); // line 271
            /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
            void assign(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &arg1); // line 298
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > begin(); // line 332
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >* const*,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > begin() const; // line 341
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > end(); // line 350
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >* const*,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > end() const; // line 359
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > > rbegin(); // line 368
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >* const*, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > > rbegin() const; // line 377
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > > rend(); // line 386
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >* const*, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > > rend() const; // line 395
            size_t size() const; // line 401
            size_t max_size() const; // line 406
            void resize(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *arg1); // line 421
            size_t capacity() const; // line 434
            bool empty() const; // line 443
            void reserve(unsigned int __n); // line 69
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *& operator[](unsigned int arg0); // line 479
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const & operator[](unsigned int arg0) const; // line 494
            protected:
            void _M_range_check(unsigned int arg0) const; // line 500
            public:
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *& at(unsigned int arg0); // line 519
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const & at(unsigned int arg0) const; // line 537
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *& front(); // line 548
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const & front() const; // line 556
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *& back(); // line 564
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const & back() const; // line 572
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > ** data(); // line 583
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const * data() const; // line 587
            void push_back(_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &arg0); // line 602
            void pop_back(); // line 623
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &arg1); // line 93
            void insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > arg0, unsigned int arg1, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &arg2); // line 657
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > arg0); // line 110
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > arg0, __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > arg1); // line 122
            void swap(/*anon struct*/ int &arg0); // line 733
            void clear(); // line 748
            protected:
            void _M_fill_assign(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &arg1); // line 171
            void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > __position, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &__x); // line 311
            void _M_insert_aux(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &arg1); // line 249
            _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > ** _M_allocate_and_copy<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**>(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > **arg1, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > **arg2); // line 765
        } _M_buckets;
        size_t _M_num_elements;
        public:
        /*anon struct*/ int hash_funct() const; // line 272
        /*anon struct*/ int key_eq() const; // line 276
        allocator<std::pair<const unsigned int, Transaction_Base*> > get_allocator() const; // line 285
        private:
        _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > * _M_get_node(); // line 296
        void _M_put_node(_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *arg0); // line 300
        public:
        void hashtable(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3, const allocator<std::pair<const unsigned int, Transaction_Base*> > &arg4); // line 328
        void hashtable(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const allocator<std::pair<const unsigned int, Transaction_Base*> > &arg3); // line 335
        void hashtable(const /*anon struct*/ int &arg0); // line 340
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 347
        void ~hashtable(); // line 360
        size_t size() const; // line 364
        size_t max_size() const; // line 368
        bool empty() const; // line 372
        void swap(/*anon struct*/ int &arg0); // line 376
        _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > begin(); // line 386
        _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > end(); // line 395
        _Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > begin() const; // line 399
        _Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > end() const; // line 408
        size_t bucket_count() const; // line 419
        size_t max_bucket_count() const; // line 423
        size_t elems_in_bucket(unsigned int arg0) const; // line 427
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> >,bool> insert_unique(const /*anon struct*/ int &arg0); // line 436
        _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > insert_equal(const /*anon struct*/ int &arg0); // line 443
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> >,bool> insert_unique_noresize(const /*anon struct*/ int &arg0); // line 749
        _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > insert_equal_noresize(const /*anon struct*/ int &arg0); // line 768
        /*anon struct*/ int & find_or_insert(const /*anon struct*/ int &__obj); // line 793
        _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > find(const unsigned int &arg0); // line 509
        _Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > find(const unsigned int &arg0) const; // line 521
        size_t count(const unsigned int &arg0) const; // line 533
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> >,__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> > > equal_range(const unsigned int &arg0); // line 815
        pair<__gnu_cxx::_Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> >,__gnu_cxx::_Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> > > equal_range(const unsigned int &arg0) const; // line 841
        size_t erase(const unsigned int &__key); // line 869
        void erase(const _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > &arg0); // line 908
        void erase(_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > arg0, _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > arg1); // line 947
        void erase(const _Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > &arg0); // line 983
        void erase(_Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > arg0, _Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > arg1); // line 972
        void resize(unsigned int __num_elements_hint); // line 990
        void clear(); // line 1078
        private:
        size_t _M_next_size(unsigned int arg0) const; // line 574
        void _M_initialize_buckets(unsigned int arg0); // line 578
        size_t _M_bkt_num_key(const unsigned int &arg0) const; // line 587
        size_t _M_bkt_num(const /*anon struct*/ int &arg0) const; // line 591
        size_t _M_bkt_num_key(const unsigned int &arg0, unsigned int arg1) const; // line 595
        size_t _M_bkt_num(const /*anon struct*/ int &arg0, unsigned int arg1) const; // line 599
        _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > * _M_new_node(const /*anon struct*/ int &arg0); // line 603
        void _M_delete_node(_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *arg0); // line 620
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *arg1, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *arg2); // line 1037
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *arg1); // line 1062
        void _M_copy_from(const /*anon struct*/ int &arg0); // line 1097
    } _M_ht;
    public:
    /*anon struct*/ int hash_funct() const; // line 122
    /*anon struct*/ int key_eq() const; // line 126
    allocator<std::pair<const unsigned int, Transaction_Base*> > get_allocator() const; // line 130
    void hash_map(); // line 134
    void hash_map(unsigned int arg0); // line 138
    void hash_map(unsigned int arg0, const /*anon struct*/ int &arg1); // line 141
    void hash_map(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const allocator<std::pair<const unsigned int, Transaction_Base*> > &arg3); // line 145
    size_t size() const; // line 173
    size_t max_size() const; // line 177
    bool empty() const; // line 181
    void swap(/*anon struct*/ int &arg0); // line 185
    _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > begin(); // line 194
    _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > end(); // line 198
    _Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > begin() const; // line 202
    _Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > end() const; // line 206
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> >,bool> insert(const /*anon struct*/ int &arg0); // line 211
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> >,bool> insert_noresize(const /*anon struct*/ int &arg0); // line 220
    _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > find(const unsigned int &arg0); // line 224
    _Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > find(const unsigned int &arg0) const; // line 228
    Transaction_Base *& operator[](const unsigned int &arg0); // line 232
    size_t count(const unsigned int &arg0) const; // line 236
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> >,__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> > > equal_range(const unsigned int &arg0); // line 240
    pair<__gnu_cxx::_Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> >,__gnu_cxx::_Hashtable_const_iterator<std::pair<const unsigned int, Transaction_Base*>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >, std::equal_to<unsigned int>, std::allocator<Transaction_Base*> > > equal_range(const unsigned int &arg0) const; // line 244
    size_t erase(const unsigned int &arg0); // line 248
    void erase(_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > arg0); // line 252
    void erase(_Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > arg0, _Hashtable_iterator<std::pair<const unsigned int, Transaction_Base*>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, Transaction_Base*> >,std::equal_to<unsigned int>,std::allocator<Transaction_Base*> > arg1); // line 256
    void clear(); // line 260
    void resize(unsigned int arg0); // line 264
    size_t bucket_count() const; // line 268
    size_t max_bucket_count() const; // line 272
    size_t elems_in_bucket(unsigned int arg0) const; // line 276
    void ~hash_map();
} transc_map_;
// anonymous struct, sizeof = 20
struct {
    private:
    // anonymous struct, sizeof = 20
    struct {
        private:
        // anonymous struct, sizeof = 1
        struct {
            public:
            void allocator(); // line 97
            void allocator(const /*anon struct*/ int &arg0); // line 99
            void ~allocator(); // line 105
            void allocator<std::pair<const unsigned int, CREATE_TRANS_RECORD> >(const allocator<std::pair<const unsigned int, CREATE_TRANS_RECORD> > &arg0); // line 103
        } _M_node_allocator;
        // anonymous struct, sizeof = 1
        struct {
            public:
            size_t operator()(unsigned int arg0) const; // line 150
        } _M_hash;
        // anonymous struct, sizeof = 1
        struct {
            public:
            bool operator()(const unsigned int &arg0, const unsigned int &arg1) const; // line 199
        } _M_equals;
        // anonymous struct, sizeof = 1
        struct {
            public:
            const unsigned int & operator()(/*anon struct*/ int &arg0) const; // line 546
            const unsigned int & operator()(const /*anon struct*/ int &arg0) const; // line 550
        } _M_get_key;
        // anonymous struct, sizeof = 12
        struct {
            public:
            void vector(const /*anon struct*/ int &arg0); // line 202
            void vector(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &arg1, const /*anon struct*/ int &arg2); // line 215
            void vector(const /*anon struct*/ int &arg0); // line 232
            void ~vector(); // line 271
            /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
            void assign(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &arg1); // line 298
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > begin(); // line 332
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >* const*,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > begin() const; // line 341
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > end(); // line 350
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >* const*,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > end() const; // line 359
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > > rbegin(); // line 368
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >* const*, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > > rbegin() const; // line 377
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > > rend(); // line 386
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >* const*, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > > rend() const; // line 395
            size_t size() const; // line 401
            size_t max_size() const; // line 406
            void resize(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *arg1); // line 421
            size_t capacity() const; // line 434
            bool empty() const; // line 443
            void reserve(unsigned int __n); // line 69
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *& operator[](unsigned int arg0); // line 479
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const & operator[](unsigned int arg0) const; // line 494
            protected:
            void _M_range_check(unsigned int arg0) const; // line 500
            public:
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *& at(unsigned int arg0); // line 519
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const & at(unsigned int arg0) const; // line 537
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *& front(); // line 548
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const & front() const; // line 556
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *& back(); // line 564
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const & back() const; // line 572
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > ** data(); // line 583
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const * data() const; // line 587
            void push_back(_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &arg0); // line 602
            void pop_back(); // line 623
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &arg1); // line 93
            void insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > arg0, unsigned int arg1, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &arg2); // line 657
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > arg0); // line 110
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > arg0, __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > arg1); // line 122
            void swap(/*anon struct*/ int &arg0); // line 733
            void clear(); // line 748
            protected:
            void _M_fill_assign(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &arg1); // line 171
            void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > __position, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &__x); // line 311
            void _M_insert_aux(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &arg1); // line 249
            _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > ** _M_allocate_and_copy<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**>(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > **arg1, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > **arg2); // line 765
        } _M_buckets;
        size_t _M_num_elements;
        public:
        /*anon struct*/ int hash_funct() const; // line 272
        /*anon struct*/ int key_eq() const; // line 276
        allocator<std::pair<const unsigned int, CREATE_TRANS_RECORD> > get_allocator() const; // line 285
        private:
        _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > * _M_get_node(); // line 296
        void _M_put_node(_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *arg0); // line 300
        public:
        void hashtable(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3, const allocator<std::pair<const unsigned int, CREATE_TRANS_RECORD> > &arg4); // line 328
        void hashtable(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const allocator<std::pair<const unsigned int, CREATE_TRANS_RECORD> > &arg3); // line 335
        void hashtable(const /*anon struct*/ int &arg0); // line 340
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 347
        void ~hashtable(); // line 360
        size_t size() const; // line 364
        size_t max_size() const; // line 368
        bool empty() const; // line 372
        void swap(/*anon struct*/ int &arg0); // line 376
        _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > begin(); // line 386
        _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > end(); // line 395
        _Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > begin() const; // line 399
        _Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > end() const; // line 408
        size_t bucket_count() const; // line 419
        size_t max_bucket_count() const; // line 423
        size_t elems_in_bucket(unsigned int arg0) const; // line 427
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> >,bool> insert_unique(const /*anon struct*/ int &arg0); // line 436
        _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > insert_equal(const /*anon struct*/ int &arg0); // line 443
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> >,bool> insert_unique_noresize(const /*anon struct*/ int &arg0); // line 749
        _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > insert_equal_noresize(const /*anon struct*/ int &arg0); // line 768
        /*anon struct*/ int & find_or_insert(const /*anon struct*/ int &__obj); // line 793
        _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > find(const unsigned int &arg0); // line 509
        _Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > find(const unsigned int &arg0) const; // line 521
        size_t count(const unsigned int &arg0) const; // line 533
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> >,__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> > > equal_range(const unsigned int &arg0); // line 815
        pair<__gnu_cxx::_Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> >,__gnu_cxx::_Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> > > equal_range(const unsigned int &arg0) const; // line 841
        size_t erase(const unsigned int &arg0); // line 869
        void erase(const _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > &arg0); // line 908
        void erase(_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > arg0, _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > arg1); // line 947
        void erase(const _Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > &arg0); // line 983
        void erase(_Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > arg0, _Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > arg1); // line 972
        void resize(unsigned int __num_elements_hint); // line 990
        void clear(); // line 1078
        private:
        size_t _M_next_size(unsigned int arg0) const; // line 574
        void _M_initialize_buckets(unsigned int arg0); // line 578
        size_t _M_bkt_num_key(const unsigned int &arg0) const; // line 587
        size_t _M_bkt_num(const /*anon struct*/ int &arg0) const; // line 591
        size_t _M_bkt_num_key(const unsigned int &arg0, unsigned int arg1) const; // line 595
        size_t _M_bkt_num(const /*anon struct*/ int &arg0, unsigned int arg1) const; // line 599
        _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > * _M_new_node(const /*anon struct*/ int &arg0); // line 603
        void _M_delete_node(_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *arg0); // line 620
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *arg1, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *arg2); // line 1037
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *arg1); // line 1062
        void _M_copy_from(const /*anon struct*/ int &arg0); // line 1097
    } _M_ht;
    public:
    /*anon struct*/ int hash_funct() const; // line 122
    /*anon struct*/ int key_eq() const; // line 126
    allocator<std::pair<const unsigned int, CREATE_TRANS_RECORD> > get_allocator() const; // line 130
    void hash_map(); // line 134
    void hash_map(unsigned int arg0); // line 138
    void hash_map(unsigned int arg0, const /*anon struct*/ int &arg1); // line 141
    void hash_map(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const allocator<std::pair<const unsigned int, CREATE_TRANS_RECORD> > &arg3); // line 145
    size_t size() const; // line 173
    size_t max_size() const; // line 177
    bool empty() const; // line 181
    void swap(/*anon struct*/ int &arg0); // line 185
    _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > begin(); // line 194
    _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > end(); // line 198
    _Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > begin() const; // line 202
    _Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > end() const; // line 206
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> >,bool> insert(const /*anon struct*/ int &arg0); // line 211
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> >,bool> insert_noresize(const /*anon struct*/ int &arg0); // line 220
    _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > find(const unsigned int &arg0); // line 224
    _Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > find(const unsigned int &arg0) const; // line 228
    CREATE_TRANS_RECORD & operator[](const unsigned int &arg0); // line 232
    size_t count(const unsigned int &arg0) const; // line 236
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> >,__gnu_cxx::_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> > > equal_range(const unsigned int &arg0); // line 240
    pair<__gnu_cxx::_Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> >,__gnu_cxx::_Hashtable_const_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >, std::equal_to<unsigned int>, std::allocator<CREATE_TRANS_RECORD> > > equal_range(const unsigned int &arg0) const; // line 244
    size_t erase(const unsigned int &arg0); // line 248
    void erase(_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > arg0); // line 252
    void erase(_Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > arg0, _Hashtable_iterator<std::pair<const unsigned int, CREATE_TRANS_RECORD>,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Select1st<std::pair<const unsigned int, CREATE_TRANS_RECORD> >,std::equal_to<unsigned int>,std::allocator<CREATE_TRANS_RECORD> > arg1); // line 256
    void clear(); // line 260
    void resize(unsigned int arg0); // line 264
    size_t bucket_count() const; // line 268
    size_t max_bucket_count() const; // line 272
    size_t elems_in_bucket(unsigned int arg0) const; // line 276
    void ~hash_map();
} regtrans_pool_map_;
// anonymous struct, sizeof = 20
struct {
    private:
    // anonymous struct, sizeof = 20
    struct {
        private:
        // anonymous struct, sizeof = 1
        struct {
            public:
            void allocator(); // line 97
            void allocator(const /*anon struct*/ int &arg0); // line 99
            void ~allocator(); // line 105
            void allocator<TRANS_LOCK_RECORD>(const /*anon struct*/ int &arg0); // line 103
        } _M_node_allocator;
        HASH_OF_TRANS_LOCK _M_hash;
        EQUAL_OF_TRANS_LOCK _M_equals;
        // anonymous struct, sizeof = 1
        struct {
            public:
            TRANS_LOCK_RECORD & operator()(TRANS_LOCK_RECORD &arg0) const; // line 533
            const TRANS_LOCK_RECORD & operator()(const TRANS_LOCK_RECORD &arg0) const; // line 537
        } _M_get_key;
        // anonymous struct, sizeof = 12
        struct {
            public:
            void vector(const /*anon struct*/ int &arg0); // line 202
            void vector(unsigned int arg0, _Hashtable_node<TRANS_LOCK_RECORD> *const &arg1, const /*anon struct*/ int &arg2); // line 215
            void vector(const /*anon struct*/ int &arg0); // line 232
            void ~vector(); // line 271
            /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
            void assign(unsigned int arg0, _Hashtable_node<TRANS_LOCK_RECORD> *const &arg1); // line 298
            __normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > begin(); // line 332
            __normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>* const*,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > begin() const; // line 341
            __normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > end(); // line 350
            __normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>* const*,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > end() const; // line 359
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**, std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > > rbegin(); // line 368
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>* const*, std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > > rbegin() const; // line 377
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**, std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > > rend(); // line 386
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>* const*, std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > > rend() const; // line 395
            size_t size() const; // line 401
            size_t max_size() const; // line 406
            void resize(unsigned int arg0, _Hashtable_node<TRANS_LOCK_RECORD> *arg1); // line 421
            size_t capacity() const; // line 434
            bool empty() const; // line 443
            void reserve(unsigned int __n); // line 69
            _Hashtable_node<TRANS_LOCK_RECORD> *& operator[](unsigned int arg0); // line 479
            _Hashtable_node<TRANS_LOCK_RECORD> *const & operator[](unsigned int arg0) const; // line 494
            protected:
            void _M_range_check(unsigned int arg0) const; // line 500
            public:
            _Hashtable_node<TRANS_LOCK_RECORD> *& at(unsigned int arg0); // line 519
            _Hashtable_node<TRANS_LOCK_RECORD> *const & at(unsigned int arg0) const; // line 537
            _Hashtable_node<TRANS_LOCK_RECORD> *& front(); // line 548
            _Hashtable_node<TRANS_LOCK_RECORD> *const & front() const; // line 556
            _Hashtable_node<TRANS_LOCK_RECORD> *& back(); // line 564
            _Hashtable_node<TRANS_LOCK_RECORD> *const & back() const; // line 572
            _Hashtable_node<TRANS_LOCK_RECORD> ** data(); // line 583
            _Hashtable_node<TRANS_LOCK_RECORD> *const * data() const; // line 587
            void push_back(_Hashtable_node<TRANS_LOCK_RECORD> *const &arg0); // line 602
            void pop_back(); // line 623
            __normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > insert(__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > arg0, _Hashtable_node<TRANS_LOCK_RECORD> *const &arg1); // line 93
            void insert(__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > arg0, unsigned int arg1, _Hashtable_node<TRANS_LOCK_RECORD> *const &arg2); // line 657
            __normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > arg0); // line 110
            __normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > arg0, __normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > arg1); // line 122
            void swap(/*anon struct*/ int &arg0); // line 733
            void clear(); // line 748
            protected:
            void _M_fill_assign(unsigned int arg0, _Hashtable_node<TRANS_LOCK_RECORD> *const &arg1); // line 171
            void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > __position, unsigned int __n, _Hashtable_node<TRANS_LOCK_RECORD> *const &__x); // line 311
            void _M_insert_aux(__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > arg0, _Hashtable_node<TRANS_LOCK_RECORD> *const &arg1); // line 249
            _Hashtable_node<TRANS_LOCK_RECORD> ** _M_allocate_and_copy<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**>(unsigned int arg0, _Hashtable_node<TRANS_LOCK_RECORD> **arg1, _Hashtable_node<TRANS_LOCK_RECORD> **arg2); // line 765
        } _M_buckets;
        size_t _M_num_elements;
        public:
        HASH_OF_TRANS_LOCK hash_funct() const; // line 272
        EQUAL_OF_TRANS_LOCK key_eq() const; // line 276
        /*anon struct*/ int get_allocator() const; // line 285
        private:
        _Hashtable_node<TRANS_LOCK_RECORD> * _M_get_node(); // line 296
        void _M_put_node(_Hashtable_node<TRANS_LOCK_RECORD> *arg0); // line 300
        public:
        void hashtable(unsigned int arg0, const HASH_OF_TRANS_LOCK &arg1, const EQUAL_OF_TRANS_LOCK &arg2, const /*anon struct*/ int &arg3, const /*anon struct*/ int &arg4); // line 328
        void hashtable(unsigned int arg0, const HASH_OF_TRANS_LOCK &arg1, const EQUAL_OF_TRANS_LOCK &arg2, const /*anon struct*/ int &arg3); // line 335
        void hashtable(const /*anon struct*/ int &arg0); // line 340
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 347
        void ~hashtable(); // line 360
        size_t size() const; // line 364
        size_t max_size() const; // line 368
        bool empty() const; // line 372
        void swap(/*anon struct*/ int &arg0); // line 376
        _Hashtable_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > begin(); // line 386
        _Hashtable_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > end(); // line 395
        _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > begin() const; // line 399
        _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > end() const; // line 408
        size_t bucket_count() const; // line 419
        size_t max_bucket_count() const; // line 423
        size_t elems_in_bucket(unsigned int arg0) const; // line 427
        pair<__gnu_cxx::_Hashtable_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> >,bool> insert_unique(const TRANS_LOCK_RECORD &arg0); // line 436
        _Hashtable_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > insert_equal(const TRANS_LOCK_RECORD &arg0); // line 443
        pair<__gnu_cxx::_Hashtable_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> >,bool> insert_unique_noresize(const TRANS_LOCK_RECORD &__obj); // line 749
        _Hashtable_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > insert_equal_noresize(const TRANS_LOCK_RECORD &arg0); // line 768
        TRANS_LOCK_RECORD & find_or_insert(const TRANS_LOCK_RECORD &arg0); // line 793
        _Hashtable_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > find(const TRANS_LOCK_RECORD &arg0); // line 509
        _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > find(const TRANS_LOCK_RECORD &arg0) const; // line 521
        size_t count(const TRANS_LOCK_RECORD &arg0) const; // line 533
        pair<__gnu_cxx::_Hashtable_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> >,__gnu_cxx::_Hashtable_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> > > equal_range(const TRANS_LOCK_RECORD &arg0); // line 815
        pair<__gnu_cxx::_Hashtable_const_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> >,__gnu_cxx::_Hashtable_const_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> > > equal_range(const TRANS_LOCK_RECORD &arg0) const; // line 841
        size_t erase(const TRANS_LOCK_RECORD &__key); // line 869
        void erase(const _Hashtable_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > &arg0); // line 908
        void erase(_Hashtable_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > arg0, _Hashtable_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > arg1); // line 947
        void erase(const _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > &arg0); // line 983
        void erase(_Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > arg0, _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > arg1); // line 972
        void resize(unsigned int __num_elements_hint); // line 990
        void clear(); // line 1078
        private:
        size_t _M_next_size(unsigned int arg0) const; // line 574
        void _M_initialize_buckets(unsigned int arg0); // line 578
        size_t _M_bkt_num_key(const TRANS_LOCK_RECORD &arg0) const; // line 587
        size_t _M_bkt_num(const TRANS_LOCK_RECORD &arg0) const; // line 591
        size_t _M_bkt_num_key(const TRANS_LOCK_RECORD &arg0, unsigned int arg1) const; // line 595
        size_t _M_bkt_num(const TRANS_LOCK_RECORD &arg0, unsigned int arg1) const; // line 599
        _Hashtable_node<TRANS_LOCK_RECORD> * _M_new_node(const TRANS_LOCK_RECORD &arg0); // line 603
        void _M_delete_node(_Hashtable_node<TRANS_LOCK_RECORD> *arg0); // line 620
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<TRANS_LOCK_RECORD> *arg1, _Hashtable_node<TRANS_LOCK_RECORD> *arg2); // line 1037
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<TRANS_LOCK_RECORD> *arg1); // line 1062
        void _M_copy_from(const /*anon struct*/ int &arg0); // line 1097
    } _M_ht;
    public:
    HASH_OF_TRANS_LOCK hash_funct() const; // line 123
    EQUAL_OF_TRANS_LOCK key_eq() const; // line 127
    /*anon struct*/ int get_allocator() const; // line 131
    void hash_set(); // line 135
    void hash_set(unsigned int arg0); // line 139
    void hash_set(unsigned int arg0, const HASH_OF_TRANS_LOCK &arg1); // line 142
    void hash_set(unsigned int arg0, const HASH_OF_TRANS_LOCK &arg1, const EQUAL_OF_TRANS_LOCK &arg2, const /*anon struct*/ int &arg3); // line 146
    size_t size() const; // line 174
    size_t max_size() const; // line 178
    bool empty() const; // line 182
    void swap(/*anon struct*/ int &arg0); // line 186
    _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > begin() const; // line 195
    _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > end() const; // line 199
    pair<__gnu_cxx::_Hashtable_const_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> >,bool> insert(const TRANS_LOCK_RECORD &arg0); // line 204
    pair<__gnu_cxx::_Hashtable_const_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> >,bool> insert_noresize(const TRANS_LOCK_RECORD &arg0); // line 216
    _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > find(const TRANS_LOCK_RECORD &arg0) const; // line 224
    size_t count(const TRANS_LOCK_RECORD &arg0) const; // line 228
    pair<__gnu_cxx::_Hashtable_const_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> >,__gnu_cxx::_Hashtable_const_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> > > equal_range(const TRANS_LOCK_RECORD &arg0) const; // line 232
    size_t erase(const TRANS_LOCK_RECORD &arg0); // line 236
    void erase(_Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > arg0); // line 240
    void erase(_Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > arg0, _Hashtable_const_iterator<TRANS_LOCK_RECORD,TRANS_LOCK_RECORD,HASH_OF_TRANS_LOCK,std::_Identity<TRANS_LOCK_RECORD>,EQUAL_OF_TRANS_LOCK,std::allocator<TRANS_LOCK_RECORD> > arg1); // line 244
    void clear(); // line 248
    void resize(unsigned int arg0); // line 253
    size_t bucket_count() const; // line 257
    size_t max_bucket_count() const; // line 261
    size_t elems_in_bucket(unsigned int arg0) const; // line 265
    void ~hash_set();
} trans_lock_pool_;
size_t max_trans_;
unsigned int trans_id_builder_;
SERVICES_ID self_services_id_;
ZEN_Timer_Queue *timer_queue_;
Zerg_MMAP_BusPipe *zerg_mmap_pipe_;
const ZEN_Time_Value *statistics_clock_;
Comm_App_Frame *trans_send_buffer_;
Comm_App_Frame *trans_recv_buffer_;
AppFrame_Mallocor_Mgr<ZEN_Null_Mutex> *inner_frame_malloc_;
ZEN_Message_Queue_Deque<ZEN_NULL_SYNCH,Comm_App_Frame*> *inner_message_queue_;
uint64_t gen_trans_counter_;
unsigned int cycle_gentrans_counter_;
static Transaction_Manager *instance_;
public:
Transaction_Manager(const Transaction_Manager &arg0);
Transaction_Manager(); // line 51
virtual ~Transaction_Manager(); // line 79
protected:
int get_clone_from_pool(unsigned int frame_cmd, unsigned int qq_uin, Transaction_Base *&crt_trans); // line 435
int return_clone_to_pool(unsigned int frame_cmd, Transaction_Base *&rt_txbase); // line 521
int process_appframe(Comm_App_Frame *ppetappframe, bool &bcrttx); // line 672
public:
void recycle_clone_from_pool(); // line 567
int process_pipe_frame(size_t &proc_frame, size_t &create_trans); // line 370
int process_queue_frame(size_t &proc_frame, size_t &create_trans); // line 971
int lock_qquin_trnas_cmd(unsigned int qq_uin, unsigned int trnas_lock_id, unsigned int frame_cmd); // line 632
void unlock_qquin_trans_cmd(unsigned int qq_uin, unsigned int lock_trnas_id); // line 651
int register_trans_cmd(unsigned int cmd, Transaction_Base *ptxbase, bool if_auto_lock_trans, unsigned int lock_trans_cmd); // line 211
int get_handler_by_transid(unsigned int transid, unsigned int trans_cmd, Transaction_Base *&ptxbase); // line 783
int create_self(Transaction_Base *arg0); // line 295
void initialize(size_t szregtrans, size_t sztransmap, SERVICES_ID selfsvr, ZEN_Timer_Queue *timer_queue, Zerg_MMAP_BusPipe *zerg_mmap_pipe, unsigned int max_frame_len, bool init_inner_queue, bool init_lock_pool); // line 158
const SERVICES_ID * self_svc_info(); // line 507
void get_manager_load_foctor(unsigned int &load_max, unsigned int &load_cur); // line 1019
void get_manager_load_foctor2(unsigned int &load_max, unsigned int &load_cur); // line 1033
int unregiester_trans_id(unsigned int transid, unsigned int trans_cmd, int run_state, time_t trans_start); // line 308
void enable_trans_statistics(const ZEN_Time_Value *stat_clock); // line 933
void dump_statistics_info() const; // line 865
void dump_all_trans_info() const; // line 816
void dump_trans_pool_info() const; // line 840
void dump_all_debug_info() const; // line 893
int mgr_sendbuf_to_service(unsigned int cmd, unsigned int qquin, unsigned int trans_id, unsigned int backfill_trans_id, const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc, const SERVICES_ID &sndsvc, const unsigned char *buf, size_t buf_len, unsigned int app_id, unsigned int option); // line 1106
int push_back_sendpipe(Comm_App_Frame *proc_frame); // line 412
protected:
int mgr_sendmsghead_to_service(unsigned int cmd, unsigned int qquin, const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc, unsigned int backfill_trans_id, unsigned int app_id, unsigned int option); // line 907
int mgr_postframe_to_msgqueue(Comm_App_Frame *post_frame); // line 940
int regiester_trans_id(unsigned int transid, unsigned int trans_cmd, Transaction_Base *ptxbase); // line 265
public:
static void instance(Transaction_Manager *pinstatnce); // line 1076
static Transaction_Manager * instance(); // line 1061
static void clean_instance(); // line 1084
};
extern const size_t MAX_ONCE_PROCESS_FRAME; // line 158
extern const size_t POOL_EXTEND_TRANSACTION_NUM; // line 161
extern const size_t RECYCLE_POOL_THRESHOLD_VALUE; // line 164
extern const size_t INIT_FRAME_MALLOC_NUMBER; // line 166
extern const size_t INNER_QUEUE_WATER_MARK; // line 170


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TRANSACTION_MANAGER_H_H_
