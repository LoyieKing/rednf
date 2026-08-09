// Auto-generated header stub from DWARF info
// Original path: output/commsvr/zergsvr/release/include/zerg_service_info_set.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_SERVICE_INFO_SET_H_H_
#define SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_SERVICE_INFO_SET_H_H_

#include <ext/hash_map>
#include <stddef.h>

// sizeof = 20
struct Service_Info_Set { // line 12
protected:
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
            void allocator<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >(const allocator<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > &arg0); // line 103
        } _M_node_allocator;
        HashofSvrInfo _M_hash;
        // anonymous struct, sizeof = 1
        struct {
            public:
            bool operator()(const SERVICES_ID &arg0, const SERVICES_ID &arg1) const; // line 199
        } _M_equals;
        // anonymous struct, sizeof = 1
        struct {
            public:
            const SERVICES_ID & operator()(/*anon struct*/ int &arg0) const; // line 546
            const SERVICES_ID & operator()(const /*anon struct*/ int &arg0) const; // line 550
        } _M_get_key;
        // anonymous struct, sizeof = 12
        struct {
            public:
            void vector(const /*anon struct*/ int &arg0); // line 202
            void vector(unsigned int arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const &arg1, const /*anon struct*/ int &arg2); // line 215
            void vector(const /*anon struct*/ int &arg0); // line 232
            void ~vector(); // line 271
            /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
            void assign(unsigned int arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const &arg1); // line 298
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > begin(); // line 332
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >* const*,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > begin() const; // line 341
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > end(); // line 350
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >* const*,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > end() const; // line 359
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > > rbegin(); // line 368
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >* const*, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > > rbegin() const; // line 377
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > > rend(); // line 386
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >* const*, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > > rend() const; // line 395
            size_t size() const; // line 401
            size_t max_size() const; // line 406
            void resize(unsigned int arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *arg1); // line 421
            size_t capacity() const; // line 434
            bool empty() const; // line 443
            void reserve(unsigned int __n); // line 69
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *& operator[](unsigned int arg0); // line 479
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const & operator[](unsigned int arg0) const; // line 494
            protected:
            void _M_range_check(unsigned int arg0) const; // line 500
            public:
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *& at(unsigned int arg0); // line 519
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const & at(unsigned int arg0) const; // line 537
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *& front(); // line 548
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const & front() const; // line 556
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *& back(); // line 564
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const & back() const; // line 572
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > ** data(); // line 583
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const * data() const; // line 587
            void push_back(_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const &arg0); // line 602
            void pop_back(); // line 623
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const &arg1); // line 93
            void insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > arg0, unsigned int arg1, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const &arg2); // line 657
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > arg0); // line 110
            __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > arg0, __normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > arg1); // line 122
            void swap(/*anon struct*/ int &arg0); // line 733
            void clear(); // line 748
            protected:
            void _M_fill_assign(unsigned int arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const &arg1); // line 171
            void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > __position, unsigned int __n, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const &__x); // line 311
            void _M_insert_aux(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >*> > > arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *const &arg1); // line 249
            _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > ** _M_allocate_and_copy<__gnu_cxx::_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >**>(unsigned int arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > **arg1, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > **arg2); // line 765
        } _M_buckets;
        size_t _M_num_elements;
        public:
        HashofSvrInfo hash_funct() const; // line 272
        /*anon struct*/ int key_eq() const; // line 276
        allocator<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > get_allocator() const; // line 285
        private:
        _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > * _M_get_node(); // line 296
        void _M_put_node(_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *arg0); // line 300
        public:
        void hashtable(unsigned int arg0, const HashofSvrInfo &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3, const allocator<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > &arg4); // line 328
        void hashtable(unsigned int arg0, const HashofSvrInfo &arg1, const /*anon struct*/ int &arg2, const allocator<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > &arg3); // line 335
        void hashtable(const /*anon struct*/ int &arg0); // line 340
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 347
        void ~hashtable(); // line 360
        size_t size() const; // line 364
        size_t max_size() const; // line 368
        bool empty() const; // line 372
        void swap(/*anon struct*/ int &arg0); // line 376
        _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > begin(); // line 386
        _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > end(); // line 395
        _Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > begin() const; // line 399
        _Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > end() const; // line 408
        size_t bucket_count() const; // line 419
        size_t max_bucket_count() const; // line 423
        size_t elems_in_bucket(unsigned int arg0) const; // line 427
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> >,bool> insert_unique(const /*anon struct*/ int &arg0); // line 436
        _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > insert_equal(const /*anon struct*/ int &arg0); // line 443
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> >,bool> insert_unique_noresize(const /*anon struct*/ int &arg0); // line 749
        _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > insert_equal_noresize(const /*anon struct*/ int &arg0); // line 768
        /*anon struct*/ int & find_or_insert(const /*anon struct*/ int &__obj); // line 793
        _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > find(const SERVICES_ID &arg0); // line 509
        _Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > find(const SERVICES_ID &arg0) const; // line 521
        size_t count(const SERVICES_ID &arg0) const; // line 533
        pair<__gnu_cxx::_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> >,__gnu_cxx::_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> > > equal_range(const SERVICES_ID &arg0); // line 815
        pair<__gnu_cxx::_Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> >,__gnu_cxx::_Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> > > equal_range(const SERVICES_ID &arg0) const; // line 841
        size_t erase(const SERVICES_ID &__key); // line 869
        void erase(const _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > &arg0); // line 908
        void erase(_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > arg0, _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > arg1); // line 947
        void erase(const _Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > &arg0); // line 983
        void erase(_Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > arg0, _Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > arg1); // line 972
        void resize(unsigned int __num_elements_hint); // line 990
        void clear(); // line 1078
        private:
        size_t _M_next_size(unsigned int arg0) const; // line 574
        void _M_initialize_buckets(unsigned int arg0); // line 578
        size_t _M_bkt_num_key(const SERVICES_ID &arg0) const; // line 587
        size_t _M_bkt_num(const /*anon struct*/ int &arg0) const; // line 591
        size_t _M_bkt_num_key(const SERVICES_ID &arg0, unsigned int arg1) const; // line 595
        size_t _M_bkt_num(const /*anon struct*/ int &arg0, unsigned int arg1) const; // line 599
        _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > * _M_new_node(const /*anon struct*/ int &arg0); // line 603
        void _M_delete_node(_Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *arg0); // line 620
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *arg1, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *arg2); // line 1037
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > *arg1); // line 1062
        void _M_copy_from(const /*anon struct*/ int &arg0); // line 1097
    } _M_ht;
    public:
    HashofSvrInfo hash_funct() const; // line 122
    /*anon struct*/ int key_eq() const; // line 126
    allocator<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > get_allocator() const; // line 130
    void hash_map(); // line 134
    void hash_map(unsigned int arg0); // line 138
    void hash_map(unsigned int arg0, const HashofSvrInfo &arg1); // line 141
    void hash_map(unsigned int arg0, const HashofSvrInfo &arg1, const /*anon struct*/ int &arg2, const allocator<std::pair<const SERVICES_ID, TCP_Svc_Handler*> > &arg3); // line 145
    size_t size() const; // line 173
    size_t max_size() const; // line 177
    bool empty() const; // line 181
    void swap(/*anon struct*/ int &arg0); // line 185
    _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > begin(); // line 194
    _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > end(); // line 198
    _Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > begin() const; // line 202
    _Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > end() const; // line 206
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> >,bool> insert(const /*anon struct*/ int &arg0); // line 211
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> >,bool> insert_noresize(const /*anon struct*/ int &arg0); // line 220
    _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > find(const SERVICES_ID &arg0); // line 224
    _Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > find(const SERVICES_ID &arg0) const; // line 228
    TCP_Svc_Handler *& operator[](const SERVICES_ID &arg0); // line 232
    size_t count(const SERVICES_ID &arg0) const; // line 236
    pair<__gnu_cxx::_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> >,__gnu_cxx::_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> > > equal_range(const SERVICES_ID &arg0); // line 240
    pair<__gnu_cxx::_Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> >,__gnu_cxx::_Hashtable_const_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>, SERVICES_ID, HashofSvrInfo, std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >, std::equal_to<SERVICES_ID>, std::allocator<TCP_Svc_Handler*> > > equal_range(const SERVICES_ID &arg0) const; // line 244
    size_t erase(const SERVICES_ID &arg0); // line 248
    void erase(_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > arg0); // line 252
    void erase(_Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > arg0, _Hashtable_iterator<std::pair<const SERVICES_ID, TCP_Svc_Handler*>,SERVICES_ID,HashofSvrInfo,std::_Select1st<std::pair<const SERVICES_ID, TCP_Svc_Handler*> >,std::equal_to<SERVICES_ID>,std::allocator<TCP_Svc_Handler*> > arg1); // line 256
    void clear(); // line 260
    void resize(unsigned int arg0); // line 264
    size_t bucket_count() const; // line 268
    size_t max_bucket_count() const; // line 272
    size_t elems_in_bucket(unsigned int arg0) const; // line 276
    void ~hash_map();
} svr_info_set_;
public:
Service_Info_Set(); // line 20
~Service_Info_Set(); // line 21
void init_services_peerinfo(size_t szpeer); // line 24
int find_services_peerinfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *&svrhandle); // line 28
int add_services_peerinfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *new_svchdl); // line 32
int replace_services_peerInfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *new_svchdl, TCP_Svc_Handler *&old_svchdl); // line 37
size_t del_services_peerInfo(const SERVICES_ID &svrinfo); // line 40
size_t get_services_peersize(); // line 43
void dump_svr_peerinfo(ostringstream &ostr_stream, size_t startno, size_t numquery); // line 46
void clear_and_closeall(); // line 49
};

#endif // SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_SERVICE_INFO_SET_H_H_
