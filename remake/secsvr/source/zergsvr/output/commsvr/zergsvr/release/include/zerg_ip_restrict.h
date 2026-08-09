// Auto-generated header stub from DWARF info
// Original path: output/commsvr/zergsvr/release/include/zerg_ip_restrict.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_IP_RESTRICT_H_H_
#define SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_IP_RESTRICT_H_H_

#include <ext/hash_set>

// sizeof = 40
struct Zerg_IPRestrict_Mgr { // line 10
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
            unsigned int & operator()(unsigned int &arg0) const; // line 533
            const unsigned int & operator()(const unsigned int &arg0) const; // line 537
        } _M_get_key;
        // anonymous struct, sizeof = 12
        struct {
            public:
            void vector(const /*anon struct*/ int &arg0); // line 202
            void vector(unsigned int arg0, _Hashtable_node<unsigned int> *const &arg1, const /*anon struct*/ int &arg2); // line 215
            void vector(const /*anon struct*/ int &arg0); // line 232
            void ~vector(); // line 271
            /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
            void assign(unsigned int arg0, _Hashtable_node<unsigned int> *const &arg1); // line 298
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > begin(); // line 332
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>* const*,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > begin() const; // line 341
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > end(); // line 350
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>* const*,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > end() const; // line 359
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**, std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > > rbegin(); // line 368
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>* const*, std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > > rbegin() const; // line 377
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**, std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > > rend(); // line 386
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>* const*, std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > > rend() const; // line 395
            size_t size() const; // line 401
            size_t max_size() const; // line 406
            void resize(unsigned int arg0, _Hashtable_node<unsigned int> *arg1); // line 421
            size_t capacity() const; // line 434
            bool empty() const; // line 443
            void reserve(unsigned int arg0); // line 69
            _Hashtable_node<unsigned int> *& operator[](unsigned int arg0); // line 479
            _Hashtable_node<unsigned int> *const & operator[](unsigned int arg0) const; // line 494
            protected:
            void _M_range_check(unsigned int arg0) const; // line 500
            public:
            _Hashtable_node<unsigned int> *& at(unsigned int arg0); // line 519
            _Hashtable_node<unsigned int> *const & at(unsigned int arg0) const; // line 537
            _Hashtable_node<unsigned int> *& front(); // line 548
            _Hashtable_node<unsigned int> *const & front() const; // line 556
            _Hashtable_node<unsigned int> *& back(); // line 564
            _Hashtable_node<unsigned int> *const & back() const; // line 572
            _Hashtable_node<unsigned int> ** data(); // line 583
            _Hashtable_node<unsigned int> *const * data() const; // line 587
            void push_back(_Hashtable_node<unsigned int> *const &arg0); // line 602
            void pop_back(); // line 623
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > insert(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, _Hashtable_node<unsigned int> *const &arg1); // line 93
            void insert(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, unsigned int arg1, _Hashtable_node<unsigned int> *const &arg2); // line 657
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0); // line 110
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg1); // line 122
            void swap(/*anon struct*/ int &arg0); // line 733
            void clear(); // line 748
            protected:
            void _M_fill_assign(unsigned int arg0, _Hashtable_node<unsigned int> *const &arg1); // line 171
            void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, unsigned int arg1, _Hashtable_node<unsigned int> *const &arg2); // line 311
            void _M_insert_aux(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, _Hashtable_node<unsigned int> *const &arg1); // line 249
        } _M_buckets;
        size_t _M_num_elements;
        public:
        /*anon struct*/ int hash_funct() const; // line 272
        /*anon struct*/ int key_eq() const; // line 276
        /*anon struct*/ int get_allocator() const; // line 285
        private:
        _Hashtable_node<unsigned int> * _M_get_node(); // line 296
        void _M_put_node(_Hashtable_node<unsigned int> *arg0); // line 300
        public:
        void hashtable(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3, const /*anon struct*/ int &arg4); // line 328
        void hashtable(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3); // line 335
        void hashtable(const /*anon struct*/ int &arg0); // line 340
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 347
        void ~hashtable(); // line 360
        size_t size() const; // line 364
        size_t max_size() const; // line 368
        bool empty() const; // line 372
        void swap(/*anon struct*/ int &arg0); // line 376
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > begin(); // line 386
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > end(); // line 395
        _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > begin() const; // line 399
        _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > end() const; // line 408
        size_t bucket_count() const; // line 419
        size_t max_bucket_count() const; // line 423
        size_t elems_in_bucket(unsigned int arg0) const; // line 427
        pair<__gnu_cxx::_Hashtable_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,bool> insert_unique(const unsigned int &arg0); // line 436
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > insert_equal(const unsigned int &arg0); // line 443
        pair<__gnu_cxx::_Hashtable_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,bool> insert_unique_noresize(const unsigned int &arg0); // line 749
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > insert_equal_noresize(const unsigned int &arg0); // line 768
        unsigned int & find_or_insert(const unsigned int &arg0); // line 793
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > find(const unsigned int &arg0); // line 509
        _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > find(const unsigned int &arg0) const; // line 521
        size_t count(const unsigned int &arg0) const; // line 533
        pair<__gnu_cxx::_Hashtable_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> > > equal_range(const unsigned int &arg0); // line 815
        pair<__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> > > equal_range(const unsigned int &arg0) const; // line 841
        size_t erase(const unsigned int &arg0); // line 869
        void erase(const _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > &arg0); // line 908
        void erase(_Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg0, _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg1); // line 947
        void erase(const _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > &arg0); // line 983
        void erase(_Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg0, _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg1); // line 972
        void resize(unsigned int arg0); // line 990
        void clear(); // line 1078
        private:
        size_t _M_next_size(unsigned int arg0) const; // line 574
        void _M_initialize_buckets(unsigned int arg0); // line 578
        size_t _M_bkt_num_key(const unsigned int &arg0) const; // line 587
        size_t _M_bkt_num(const unsigned int &arg0) const; // line 591
        size_t _M_bkt_num_key(const unsigned int &arg0, unsigned int arg1) const; // line 595
        size_t _M_bkt_num(const unsigned int &arg0, unsigned int arg1) const; // line 599
        _Hashtable_node<unsigned int> * _M_new_node(const unsigned int &arg0); // line 603
        void _M_delete_node(_Hashtable_node<unsigned int> *arg0); // line 620
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<unsigned int> *arg1, _Hashtable_node<unsigned int> *arg2); // line 1037
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<unsigned int> *arg1); // line 1062
        void _M_copy_from(const /*anon struct*/ int &arg0); // line 1097
    } _M_ht;
    public:
    /*anon struct*/ int hash_funct() const; // line 123
    /*anon struct*/ int key_eq() const; // line 127
    /*anon struct*/ int get_allocator() const; // line 131
    void hash_set(); // line 135
    void hash_set(unsigned int arg0); // line 139
    void hash_set(unsigned int arg0, const /*anon struct*/ int &arg1); // line 142
    void hash_set(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3); // line 146
    size_t size() const; // line 174
    size_t max_size() const; // line 178
    bool empty() const; // line 182
    void swap(/*anon struct*/ int &arg0); // line 186
    _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > begin() const; // line 195
    _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > end() const; // line 199
    pair<__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,bool> insert(const unsigned int &arg0); // line 204
    pair<__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,bool> insert_noresize(const unsigned int &arg0); // line 216
    _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > find(const unsigned int &arg0) const; // line 224
    size_t count(const unsigned int &arg0) const; // line 228
    pair<__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> > > equal_range(const unsigned int &arg0) const; // line 232
    size_t erase(const unsigned int &arg0); // line 236
    void erase(_Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg0); // line 240
    void erase(_Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg0, _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg1); // line 244
    void clear(); // line 248
    void resize(unsigned int arg0); // line 253
    size_t bucket_count() const; // line 257
    size_t max_bucket_count() const; // line 261
    size_t elems_in_bucket(unsigned int arg0) const; // line 265
} allow_ip_set_;
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
            unsigned int & operator()(unsigned int &arg0) const; // line 533
            const unsigned int & operator()(const unsigned int &arg0) const; // line 537
        } _M_get_key;
        // anonymous struct, sizeof = 12
        struct {
            public:
            void vector(const /*anon struct*/ int &arg0); // line 202
            void vector(unsigned int arg0, _Hashtable_node<unsigned int> *const &arg1, const /*anon struct*/ int &arg2); // line 215
            void vector(const /*anon struct*/ int &arg0); // line 232
            void ~vector(); // line 271
            /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
            void assign(unsigned int arg0, _Hashtable_node<unsigned int> *const &arg1); // line 298
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > begin(); // line 332
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>* const*,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > begin() const; // line 341
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > end(); // line 350
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>* const*,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > end() const; // line 359
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**, std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > > rbegin(); // line 368
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>* const*, std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > > rbegin() const; // line 377
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**, std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > > rend(); // line 386
            reverse_iterator<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>* const*, std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > > rend() const; // line 395
            size_t size() const; // line 401
            size_t max_size() const; // line 406
            void resize(unsigned int arg0, _Hashtable_node<unsigned int> *arg1); // line 421
            size_t capacity() const; // line 434
            bool empty() const; // line 443
            void reserve(unsigned int arg0); // line 69
            _Hashtable_node<unsigned int> *& operator[](unsigned int arg0); // line 479
            _Hashtable_node<unsigned int> *const & operator[](unsigned int arg0) const; // line 494
            protected:
            void _M_range_check(unsigned int arg0) const; // line 500
            public:
            _Hashtable_node<unsigned int> *& at(unsigned int arg0); // line 519
            _Hashtable_node<unsigned int> *const & at(unsigned int arg0) const; // line 537
            _Hashtable_node<unsigned int> *& front(); // line 548
            _Hashtable_node<unsigned int> *const & front() const; // line 556
            _Hashtable_node<unsigned int> *& back(); // line 564
            _Hashtable_node<unsigned int> *const & back() const; // line 572
            _Hashtable_node<unsigned int> ** data(); // line 583
            _Hashtable_node<unsigned int> *const * data() const; // line 587
            void push_back(_Hashtable_node<unsigned int> *const &arg0); // line 602
            void pop_back(); // line 623
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > insert(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, _Hashtable_node<unsigned int> *const &arg1); // line 93
            void insert(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, unsigned int arg1, _Hashtable_node<unsigned int> *const &arg2); // line 657
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0); // line 110
            __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > erase(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, __normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg1); // line 122
            void swap(/*anon struct*/ int &arg0); // line 733
            void clear(); // line 748
            protected:
            void _M_fill_assign(unsigned int arg0, _Hashtable_node<unsigned int> *const &arg1); // line 171
            void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, unsigned int arg1, _Hashtable_node<unsigned int> *const &arg2); // line 311
            void _M_insert_aux(__normal_iterator<__gnu_cxx::_Hashtable_node<unsigned int>**,std::vector<__gnu_cxx::_Hashtable_node<unsigned int>*, std::allocator<__gnu_cxx::_Hashtable_node<unsigned int>*> > > arg0, _Hashtable_node<unsigned int> *const &arg1); // line 249
        } _M_buckets;
        size_t _M_num_elements;
        public:
        /*anon struct*/ int hash_funct() const; // line 272
        /*anon struct*/ int key_eq() const; // line 276
        /*anon struct*/ int get_allocator() const; // line 285
        private:
        _Hashtable_node<unsigned int> * _M_get_node(); // line 296
        void _M_put_node(_Hashtable_node<unsigned int> *arg0); // line 300
        public:
        void hashtable(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3, const /*anon struct*/ int &arg4); // line 328
        void hashtable(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3); // line 335
        void hashtable(const /*anon struct*/ int &arg0); // line 340
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 347
        void ~hashtable(); // line 360
        size_t size() const; // line 364
        size_t max_size() const; // line 368
        bool empty() const; // line 372
        void swap(/*anon struct*/ int &arg0); // line 376
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > begin(); // line 386
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > end(); // line 395
        _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > begin() const; // line 399
        _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > end() const; // line 408
        size_t bucket_count() const; // line 419
        size_t max_bucket_count() const; // line 423
        size_t elems_in_bucket(unsigned int arg0) const; // line 427
        pair<__gnu_cxx::_Hashtable_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,bool> insert_unique(const unsigned int &arg0); // line 436
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > insert_equal(const unsigned int &arg0); // line 443
        pair<__gnu_cxx::_Hashtable_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,bool> insert_unique_noresize(const unsigned int &arg0); // line 749
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > insert_equal_noresize(const unsigned int &arg0); // line 768
        unsigned int & find_or_insert(const unsigned int &arg0); // line 793
        _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > find(const unsigned int &arg0); // line 509
        _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > find(const unsigned int &arg0) const; // line 521
        size_t count(const unsigned int &arg0) const; // line 533
        pair<__gnu_cxx::_Hashtable_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> > > equal_range(const unsigned int &arg0); // line 815
        pair<__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> > > equal_range(const unsigned int &arg0) const; // line 841
        size_t erase(const unsigned int &arg0); // line 869
        void erase(const _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > &arg0); // line 908
        void erase(_Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg0, _Hashtable_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg1); // line 947
        void erase(const _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > &arg0); // line 983
        void erase(_Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg0, _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg1); // line 972
        void resize(unsigned int arg0); // line 990
        void clear(); // line 1078
        private:
        size_t _M_next_size(unsigned int arg0) const; // line 574
        void _M_initialize_buckets(unsigned int arg0); // line 578
        size_t _M_bkt_num_key(const unsigned int &arg0) const; // line 587
        size_t _M_bkt_num(const unsigned int &arg0) const; // line 591
        size_t _M_bkt_num_key(const unsigned int &arg0, unsigned int arg1) const; // line 595
        size_t _M_bkt_num(const unsigned int &arg0, unsigned int arg1) const; // line 599
        _Hashtable_node<unsigned int> * _M_new_node(const unsigned int &arg0); // line 603
        void _M_delete_node(_Hashtable_node<unsigned int> *arg0); // line 620
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<unsigned int> *arg1, _Hashtable_node<unsigned int> *arg2); // line 1037
        void _M_erase_bucket(unsigned int arg0, _Hashtable_node<unsigned int> *arg1); // line 1062
        void _M_copy_from(const /*anon struct*/ int &arg0); // line 1097
    } _M_ht;
    public:
    /*anon struct*/ int hash_funct() const; // line 123
    /*anon struct*/ int key_eq() const; // line 127
    /*anon struct*/ int get_allocator() const; // line 131
    void hash_set(); // line 135
    void hash_set(unsigned int arg0); // line 139
    void hash_set(unsigned int arg0, const /*anon struct*/ int &arg1); // line 142
    void hash_set(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2, const /*anon struct*/ int &arg3); // line 146
    size_t size() const; // line 174
    size_t max_size() const; // line 178
    bool empty() const; // line 182
    void swap(/*anon struct*/ int &arg0); // line 186
    _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > begin() const; // line 195
    _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > end() const; // line 199
    pair<__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,bool> insert(const unsigned int &arg0); // line 204
    pair<__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,bool> insert_noresize(const unsigned int &arg0); // line 216
    _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > find(const unsigned int &arg0) const; // line 224
    size_t count(const unsigned int &arg0) const; // line 228
    pair<__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> >,__gnu_cxx::_Hashtable_const_iterator<unsigned int, unsigned int, __gnu_cxx::hash<unsigned int>, std::_Identity<unsigned int>, std::equal_to<unsigned int>, std::allocator<unsigned int> > > equal_range(const unsigned int &arg0) const; // line 232
    size_t erase(const unsigned int &arg0); // line 236
    void erase(_Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg0); // line 240
    void erase(_Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg0, _Hashtable_const_iterator<unsigned int,unsigned int,__gnu_cxx::hash<unsigned int>,std::_Identity<unsigned int>,std::equal_to<unsigned int>,std::allocator<unsigned int> > arg1); // line 244
    void clear(); // line 248
    void resize(unsigned int arg0); // line 253
    size_t bucket_count() const; // line 257
    size_t max_bucket_count() const; // line 261
    size_t elems_in_bucket(unsigned int arg0) const; // line 265
} reject_ip_set_;
static Zerg_IPRestrict_Mgr *instance_;
Zerg_IPRestrict_Mgr(); // line 16
~Zerg_IPRestrict_Mgr(); // line 17
public:
int get_iprestrict_conf(const Zerg_MMAP_BusPipe::ZERG_CONFIG &arg0); // line 21
int check_iprestrict(const ZEN_Sockaddr_In &arg0); // line 23
static Zerg_IPRestrict_Mgr * instance(); // line 39
static void clean_instance(); // line 41
};

#endif // SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_IP_RESTRICT_H_H_
