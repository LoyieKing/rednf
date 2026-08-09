// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_server_status.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_STATUS_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_STATUS_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_share_mem_posix.h"
#include <bits/stl_vector.h>
#include <ext/hash_map>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

enum ZEN_STATUS_STATICS_TYPE {
    STATICS_INVALID_TYPE = 0,
    STATICS_PER_FIVE_MINTUES = 1,
    STATICS_PER_HOUR = 2,
    STATICS_PER_DAYS = 3,
    STATICS_ABSOLUTE_VALUE = 11,
};
// sizeof = 12
struct ZEN_STATUS_ITEM_ID { // line 64
public:
unsigned int statics_id_;
unsigned int app_id_;
unsigned int classify_id_;
ZEN_STATUS_ITEM_ID(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 78
ZEN_STATUS_ITEM_ID(); // line 79
~ZEN_STATUS_ITEM_ID(); // line 80
bool operator==(const ZEN_STATUS_ITEM_ID &arg0) const; // line 82
};
// sizeof = 24
struct ZEN_STATUS_ITEM { // line 107
public:
ZEN_STATUS_ITEM_ID item_id_;
ZEN_STATUS_STATICS_TYPE statics_type_;
uint64_t counter_;
ZEN_STATUS_ITEM(); // line 111
ZEN_STATUS_ITEM(unsigned int arg0, ZEN_STATUS_STATICS_TYPE arg1); // line 113
~ZEN_STATUS_ITEM(); // line 114
};
// sizeof = 92
struct ZEN_STATUS_ITEM_WITHNAME { // line 132
public:
static const size_t MAX_COUNTER_NAME_LEN;
ZEN_STATUS_ITEM statics_item_;
char item_name_[];
ZEN_STATUS_ITEM_WITHNAME(unsigned int arg0, ZEN_STATUS_STATICS_TYPE arg1, const char *arg2); // line 138
ZEN_STATUS_ITEM_WITHNAME(); // line 139
~ZEN_STATUS_ITEM_WITHNAME(); // line 140
};
// sizeof = 76
struct ZEN_Server_Status : public ZEN_NON_Copyable { // line 186
public:
int (***_vptr.ZEN_Server_Status)(...);
protected:
static const size_t MAX_MONITOR_STAT_ITEM;
static const time_t FIVE_MINTUE_SECONDS;
static const time_t ONE_HOURS_SECONDS;
static const time_t ONE_DAY_SECONDS;
static const uint64_t INCREASE_VALUE_ONCE;
ZEN_Lock_Base *stat_lock_;
time_t clear_time_;
ZEN_ShareMem_Posix stat_file_;
ZEN_STATUS_HEAD *stat_file_head_;
shm_vector<ZEN_STATUS_ITEM> *status_stat_sandy_;
shm_vector<ZEN_STATUS_ITEM> *status_copy_mandy_;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1); // line 298
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > begin(); // line 332
    __normal_iterator<const ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > begin() const; // line 341
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > end(); // line 350
    __normal_iterator<const ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, ZEN_STATUS_ITEM_WITHNAME arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int __n); // line 69
    ZEN_STATUS_ITEM_WITHNAME & operator[](unsigned int arg0); // line 479
    const ZEN_STATUS_ITEM_WITHNAME & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    ZEN_STATUS_ITEM_WITHNAME & at(unsigned int arg0); // line 519
    const ZEN_STATUS_ITEM_WITHNAME & at(unsigned int arg0) const; // line 537
    ZEN_STATUS_ITEM_WITHNAME & front(); // line 548
    const ZEN_STATUS_ITEM_WITHNAME & front() const; // line 556
    ZEN_STATUS_ITEM_WITHNAME & back(); // line 564
    const ZEN_STATUS_ITEM_WITHNAME & back() const; // line 572
    ZEN_STATUS_ITEM_WITHNAME * data(); // line 583
    const ZEN_STATUS_ITEM_WITHNAME * data() const; // line 587
    void push_back(const ZEN_STATUS_ITEM_WITHNAME &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > insert(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1); // line 93
    void insert(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0, unsigned int arg1, const ZEN_STATUS_ITEM_WITHNAME &arg2); // line 657
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > erase(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0); // line 110
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > erase(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > __first, __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > __last); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1); // line 171
    void _M_fill_insert(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > __position, unsigned int __n, const ZEN_STATUS_ITEM_WITHNAME &__x); // line 311
    void _M_insert_aux(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > __position, const ZEN_STATUS_ITEM_WITHNAME &__x); // line 249
    ZEN_STATUS_ITEM_WITHNAME * _M_allocate_and_copy<ZEN_STATUS_ITEM_WITHNAME*>(unsigned int arg0, ZEN_STATUS_ITEM_WITHNAME *arg1, ZEN_STATUS_ITEM_WITHNAME *arg2); // line 765
} conf_stat_ary_;
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
            void allocator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >(const allocator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > &arg0); // line 103
        } _M_node_allocator;
        HASH_ZEN_STATUS_ITEM_ID _M_hash;
        // anonymous struct, sizeof = 1
        struct {
            public:
            bool operator()(const ZEN_STATUS_ITEM_ID &arg0, const ZEN_STATUS_ITEM_ID &arg1) const; // line 199
        } _M_equals;
        // anonymous struct, sizeof = 1
        struct {
            public:
            const ZEN_STATUS_ITEM_ID & operator()(/*anon struct*/ int &arg0) const; // line 546
            const ZEN_STATUS_ITEM_ID & operator()(const /*anon struct*/ int &arg0) const; // line 550
        } _M_get_key;
        // anonymous struct, sizeof = 12
        struct {
            public:
            void vector(const /*anon struct*/ int &arg0); // line 202
            void vector(unsigned int arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &arg1, const /*anon struct*/ int &arg2); // line 215
            void vector(const /*anon struct*/ int &arg0); // line 232
            void ~vector(); // line 271
            /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
            void assign(unsigned int arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &arg1); // line 298
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > begin(); // line 332
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >* const*,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > begin() const; // line 341
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > end(); // line 350
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >* const*,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > end() const; // line 359
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > > rbegin(); // line 368
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >* const*, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > > rbegin() const; // line 377
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > > rend(); // line 386
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >* const*, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > > rend() const; // line 395
            size_t size() const; // line 401
            size_t max_size() const; // line 406
            void resize(unsigned int arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *arg1); // line 421
            size_t capacity() const; // line 434
            bool empty() const; // line 443
            void reserve(unsigned int __n); // line 69
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *& operator[](unsigned int arg0); // line 479
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const & operator[](unsigned int arg0) const; // line 494
            protected:
            void _M_range_check(unsigned int arg0) const; // line 500
            public:
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *& at(unsigned int arg0); // line 519
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const & at(unsigned int arg0) const; // line 537
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *& front(); // line 548
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const & front() const; // line 556
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *& back(); // line 564
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const & back() const; // line 572
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > ** data(); // line 583
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const * data() const; // line 587
            void push_back(_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &arg0); // line 602
            void pop_back(); // line 623
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &arg1); // line 93
            void insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > arg0, unsigned int arg1, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &arg2); // line 657
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > arg0); // line 110
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > arg0, __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > arg1); // line 122
            void swap(/*anon struct*/ int &arg0); // line 733
            void clear(); // line 748
            protected:
            void _M_fill_assign(unsigned int arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &arg1); // line 171
            void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > __position, unsigned int __n, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &__x); // line 311
            void _M_insert_aux(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &arg1); // line 249
            _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > ** _M_allocate_and_copy<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**>(unsigned int arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > **arg1, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > **arg2); // line 765
        } _M_buckets;
        size_t _M_num_elements;
        public:
        HASH_ZEN_STATUS_ITEM_ID hash_funct() const; // line 272
        /*anon struct*/ int key_eq() const; // line 276
        allocator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > get_allocator() const; // line 285
        private:
        _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > * _M_get_node(); // line 296
        void _M_put_node(_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *arg0); // line 300
        public:
        void hashtable(unsigned int arg0, const HASH_ZEN_STATUS_ITEM_ID &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3, const allocator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > &arg4); // line 328
        void hashtable(unsigned int arg0, const HASH_ZEN_STATUS_ITEM_ID &arg1, const /*anon struct*/ int &arg2, const allocator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > &arg3); // line 335
        void hashtable(const /*anon struct*/ int &arg0); // line 340
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 347
        void ~hashtable(); // line 360
        size_t size() const; // line 364
        size_t max_size() const; // line 368
        bool empty() const; // line 372
        void swap(/*anon struct*/ int &arg0); // line 376
        _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > begin(); // line 386
        _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > end(); // line 395
        _Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > begin() const; // line 399
        _Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > end() const; // line 408
        size_t bucket_count() const; // line 419
        size_t max_bucket_count() const; // line 423
        size_t elems_in_bucket(unsigned int arg0) const; // line 427
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> >,bool> insert_unique(const /*anon struct*/ int &arg0); // line 436
        _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > insert_equal(const /*anon struct*/ int &arg0); // line 443
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> >,bool> insert_unique_noresize(const /*anon struct*/ int &arg0); // line 749
        _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > insert_equal_noresize(const /*anon struct*/ int &arg0); // line 768
        /*anon struct*/ int & find_or_insert(const /*anon struct*/ int &__obj); // line 793
        _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > find(const ZEN_STATUS_ITEM_ID &arg0); // line 509
        _Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > find(const ZEN_STATUS_ITEM_ID &arg0) const; // line 521
        size_t count(const ZEN_STATUS_ITEM_ID &arg0) const; // line 533
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> > > equal_range(const ZEN_STATUS_ITEM_ID &arg0); // line 815
        pair<__gnu_cxx::_Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> > > equal_range(const ZEN_STATUS_ITEM_ID &arg0) const; // line 841
        size_t erase(const ZEN_STATUS_ITEM_ID &arg0); // line 869
        void erase(const _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > &arg0); // line 908
        void erase(_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > arg0, _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > arg1); // line 947
        void erase(const _Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > &arg0); // line 983
        void erase(_Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > arg0, _Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > arg1); // line 972
        void resize(unsigned int __num_elements_hint); // line 990
        void clear(); // line 1078
        private:
        size_t _M_next_size(unsigned int arg0) const; // line 574
        void _M_initialize_buckets(unsigned int arg0); // line 578
        size_t _M_bkt_num_key(const ZEN_STATUS_ITEM_ID &arg0) const; // line 587
        size_t _M_bkt_num(const /*anon struct*/ int &arg0) const; // line 591
        size_t _M_bkt_num_key(const ZEN_STATUS_ITEM_ID &arg0, unsigned int arg1) const; // line 595
        size_t _M_bkt_num(const /*anon struct*/ int &arg0, unsigned int arg1) const; // line 599
        _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > * _M_new_node(const /*anon struct*/ int &arg0); // line 603
        void _M_delete_node(_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *arg0); // line 620
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *arg1, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *arg2); // line 1037
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *arg1); // line 1062
        void _M_copy_from(const /*anon struct*/ int &arg0); // line 1097
    } _M_ht;
    public:
    HASH_ZEN_STATUS_ITEM_ID hash_funct() const; // line 122
    /*anon struct*/ int key_eq() const; // line 126
    allocator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > get_allocator() const; // line 130
    void hash_map(); // line 134
    void hash_map(unsigned int arg0); // line 138
    void hash_map(unsigned int arg0, const HASH_ZEN_STATUS_ITEM_ID &arg1); // line 141
    void hash_map(unsigned int arg0, const HASH_ZEN_STATUS_ITEM_ID &arg1, const /*anon struct*/ int &arg2, const allocator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > &arg3); // line 145
    size_t size() const; // line 173
    size_t max_size() const; // line 177
    bool empty() const; // line 181
    void swap(/*anon struct*/ int &arg0); // line 185
    _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > begin(); // line 194
    _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > end(); // line 198
    _Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > begin() const; // line 202
    _Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > end() const; // line 206
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> >,bool> insert(const /*anon struct*/ int &arg0); // line 211
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> >,bool> insert_noresize(const /*anon struct*/ int &arg0); // line 220
    _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > find(const ZEN_STATUS_ITEM_ID &arg0); // line 224
    _Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > find(const ZEN_STATUS_ITEM_ID &arg0) const; // line 228
    unsigned int & operator[](const ZEN_STATUS_ITEM_ID &arg0); // line 232
    size_t count(const ZEN_STATUS_ITEM_ID &arg0) const; // line 236
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> > > equal_range(const ZEN_STATUS_ITEM_ID &arg0); // line 240
    pair<__gnu_cxx::_Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_const_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>, ZEN_STATUS_ITEM_ID, HASH_ZEN_STATUS_ITEM_ID, std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >, std::equal_to<ZEN_STATUS_ITEM_ID>, std::allocator<unsigned int> > > equal_range(const ZEN_STATUS_ITEM_ID &arg0) const; // line 244
    size_t erase(const ZEN_STATUS_ITEM_ID &arg0); // line 248
    void erase(_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > arg0); // line 252
    void erase(_Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > arg0, _Hashtable_iterator<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int>,ZEN_STATUS_ITEM_ID,HASH_ZEN_STATUS_ITEM_ID,std::_Select1st<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >,std::equal_to<ZEN_STATUS_ITEM_ID>,std::allocator<unsigned int> > arg1); // line 256
    void clear(); // line 260
    void resize(unsigned int arg0); // line 264
    size_t bucket_count() const; // line 268
    size_t max_bucket_count() const; // line 272
    size_t elems_in_bucket(unsigned int arg0) const; // line 276
    void ~hash_map();
} statid_to_index_;
bool multi_thread_guard_;
bool initialized_;
static ZEN_Server_Status *instance_;
public:
ZEN_Server_Status(const ZEN_Server_Status &arg0);
ZEN_Server_Status(); // line 95
virtual ~ZEN_Server_Status(); // line 107
protected:
int initialize(const char *stat_filename, bool restore_mmap, bool multi_thread); // line 136
int find_insert_idx(unsigned int statics_id, unsigned int app_id, unsigned int classify_id, size_t *sandy_idx); // line 242
public:
int initialize(const char *stat_filename); // line 297
int initialize(const char *stat_filename, size_t num_stat_item, const ZEN_STATUS_ITEM_WITHNAME *item_ary, bool multi_thread); // line 321
void add_status_item(size_t num_add_stat_item, const ZEN_STATUS_ITEM_WITHNAME *item_ary); // line 198
void modify_multi_thread_guard(bool multi_thread); // line 211
int increase_by_statid(unsigned int statics_id, unsigned int app_id, unsigned int classify_id, int64_t incre_value); // line 354
int increase_by_statid(unsigned int arg0, unsigned int arg1, int64_t arg2); // line 417
int increase_by_statid(unsigned int arg0, int64_t arg1); // line 424
int increase_once(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 395
int increase_once(unsigned int arg0, unsigned int arg1); // line 402
int increase_once(unsigned int arg0); // line 408
int set_by_statid(unsigned int statics_id, unsigned int app_id, unsigned int classify_id, uint64_t set_value); // line 385
int set_by_statid(unsigned int arg0, unsigned int arg1, uint64_t arg2); // line 433
int set_by_statid(unsigned int arg0, uint64_t arg1); // line 440
uint64_t get_counter(unsigned int statics_id, unsigned int app_id, unsigned int classify_id); // line 418
size_t num_of_counter(); // line 436
uint32_t get_copy_time(); // line 443
void check_overtime(time_t now_time); // line 451
void copy_stat_counter(); // line 543
void dump_all(/*anon struct*/ int &array_status, bool dump_copy); // line 508
void dump_status_info(ostringstream &strstream, bool dump_copy); // line 558
void dump_status_info(bool dump_copy); // line 608
void get_stat_head(ZEN_STATUS_HEAD *stat_head); // line 662
void report_monitor_time(uint32_t report_time); // line 668
static ZEN_Server_Status * instance(); // line 677
static void instance(ZEN_Server_Status *pinstatnce); // line 688
static void clean_instance(); // line 696
};
extern const size_t MAX_COUNTER_NAME_LEN; // line 144
extern const uint64_t INCREASE_VALUE_ONCE; // line 347

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SERVER_STATUS_H_H_
