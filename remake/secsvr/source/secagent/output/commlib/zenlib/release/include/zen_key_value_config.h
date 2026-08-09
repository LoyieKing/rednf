// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_key_value_config.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_KEY_VALUE_CONFIG_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_KEY_VALUE_CONFIG_H_H_

#include <bits/stl_map.h>

// sizeof = 28
struct KeyValueConfig { // line 16
    enum IntValueType {
        EVALUE_INT10 = 0,
        EVALUE_INT16 = 1,
    };
public:
int (***_vptr.KeyValueConfig)(...);
private:
// anonymous struct, sizeof = 24
struct {
    private:
    // anonymous struct, sizeof = 24
    struct {
        protected:
        _Rb_tree_impl<std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,false> _M_impl;
        public:
        /*anon struct*/ int get_allocator() const; // line 352
        protected:
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_get_node(); // line 357
        void _M_put_node(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *arg0); // line 361
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_create_node(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg0); // line 365
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_clone_node(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *arg0); // line 379
        void destroy_node(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *arg0); // line 389
        /*anon struct*/ int *& _M_root(); // line 441
        const /*anon struct*/ int * _M_root() const; // line 445
        /*anon struct*/ int *& _M_leftmost(); // line 449
        const /*anon struct*/ int * _M_leftmost() const; // line 453
        /*anon struct*/ int *& _M_rightmost(); // line 457
        const /*anon struct*/ int * _M_rightmost() const; // line 461
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_begin(); // line 465
        const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_begin() const; // line 469
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_end(); // line 476
        const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_end() const; // line 480
        static const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > & _S_value(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *arg0); // line 484
        static const /*anon struct*/ int & _S_key(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *arg0); // line 488
        static _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _S_left(/*anon struct*/ int *arg0); // line 492
        static const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _S_left(const /*anon struct*/ int *arg0); // line 496
        static _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _S_right(/*anon struct*/ int *arg0); // line 500
        static const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _S_right(const /*anon struct*/ int *arg0); // line 504
        static const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > & _S_value(const /*anon struct*/ int *arg0); // line 508
        static const /*anon struct*/ int & _S_key(const /*anon struct*/ int *arg0); // line 512
        static /*anon struct*/ int * _S_minimum(/*anon struct*/ int *arg0); // line 516
        static const /*anon struct*/ int * _S_minimum(const /*anon struct*/ int *arg0); // line 520
        static /*anon struct*/ int * _S_maximum(/*anon struct*/ int *arg0); // line 524
        static const /*anon struct*/ int * _S_maximum(const /*anon struct*/ int *arg0); // line 528
        private:
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > _M_insert(/*anon struct*/ int *__x, /*anon struct*/ int *__p, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &__v); // line 813
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > _M_insert(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg2); // line 831
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_copy(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *arg0, _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *arg1); // line 1223
        void _M_erase(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *__x); // line 1259
        public:
        void _Rb_tree(); // line 554
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 557
        void _Rb_tree(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 561
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 565
        void ~_Rb_tree(); // line 577
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 791
        /*anon struct*/ int key_comp() const; // line 585
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > begin(); // line 589
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > begin() const; // line 596
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > end(); // line 603
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > end() const; // line 607
        reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rbegin(); // line 614
        reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rbegin() const; // line 618
        reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rend(); // line 622
        reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rend() const; // line 626
        bool empty() const; // line 630
        size_t size() const; // line 634
        size_t max_size() const; // line 638
        void swap(/*anon struct*/ int &arg0); // line 867
        pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,bool> insert_unique(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &__v); // line 913
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert_equal(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg0); // line 850
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert_unique(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > __position, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &__v); // line 939
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert_unique(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg1); // line 996
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert_equal(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg1); // line 1053
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert_equal(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg1); // line 1107
        void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0); // line 1183
        void erase(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0); // line 1197
        size_t erase(const /*anon struct*/ int &arg0); // line 1211
        void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg1); // line 1275
        void erase(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg1); // line 1288
        void erase(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1); // line 1301
        void clear(); // line 690
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > find(const /*anon struct*/ int &arg0); // line 1311
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > find(const /*anon struct*/ int &__k) const; // line 1332
        size_t count(const /*anon struct*/ int &arg0) const; // line 1354
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > lower_bound(const /*anon struct*/ int &__k); // line 1365
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > lower_bound(const /*anon struct*/ int &arg0) const; // line 1383
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > upper_bound(const /*anon struct*/ int &arg0); // line 1401
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > upper_bound(const /*anon struct*/ int &arg0) const; // line 1419
        pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > equal_range(const /*anon struct*/ int &arg0); // line 1440
        pair<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > equal_range(const /*anon struct*/ int &arg0) const; // line 1450
        bool __rb_verify() const; // line 1461
    } _M_t;
    public:
    void map(); // line 156
    void map(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 164
    void map(const /*anon struct*/ int &arg0); // line 174
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 225
    /*anon struct*/ int get_allocator() const; // line 233
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > begin(); // line 243
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > begin() const; // line 252
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > end(); // line 260
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > end() const; // line 269
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rbegin(); // line 278
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rbegin() const; // line 287
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rend(); // line 296
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rend() const; // line 305
    bool empty() const; // line 313
    size_t size() const; // line 318
    size_t max_size() const; // line 323
    /*anon struct*/ int & operator[](const /*anon struct*/ int &arg0); // line 340
    /*anon struct*/ int & at(const /*anon struct*/ int &arg0); // line 362
    const /*anon struct*/ int & at(const /*anon struct*/ int &arg0) const; // line 371
    pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,bool> insert(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg0); // line 395
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg1); // line 419
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0); // line 445
    size_t erase(const /*anon struct*/ int &arg0); // line 460
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg1); // line 475
    void swap(/*anon struct*/ int &arg0); // line 490
    void clear(); // line 500
    /*anon struct*/ int key_comp() const; // line 509
    value_compare value_comp() const; // line 517
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > find(const /*anon struct*/ int &arg0); // line 533
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > find(const /*anon struct*/ int &arg0) const; // line 548
    size_t count(const /*anon struct*/ int &arg0) const; // line 560
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > lower_bound(const /*anon struct*/ int &arg0); // line 575
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > lower_bound(const /*anon struct*/ int &arg0) const; // line 590
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > upper_bound(const /*anon struct*/ int &arg0); // line 600
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > upper_bound(const /*anon struct*/ int &arg0) const; // line 610
    pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > equal_range(const /*anon struct*/ int &arg0); // line 629
    pair<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > equal_range(const /*anon struct*/ int &arg0) const; // line 648
    void ~map();
} values_;
public:
KeyValueConfig(const KeyValueConfig &arg0);
KeyValueConfig(); // line 8
virtual ~KeyValueConfig(); // line 12
int load_from_file(const char *conf_file_path); // line 38
int get_str(const char *config_name, char *value, unsigned int value_len, const char *default_value); // line 97
int get_int(const char *config_name, int *value, int default_value, KeyValueConfig::IntValueType type); // line 149
int get_short(const char *config_name, short int *value, short int default_value, KeyValueConfig::IntValueType type); // line 190
int get_bool(const char *config_name, bool *value, bool default_value); // line 208
private:
void add_item(const string &name, const string &value); // line 18
const string * get_item_value(const string &name) const; // line 24
};
namespace KeyValueConfig {
    enum IntValueType {
        EVALUE_INT10 = 0,
        EVALUE_INT16 = 1,
    };
} // namespace KeyValueConfig


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_KEY_VALUE_CONFIG_H_H_
