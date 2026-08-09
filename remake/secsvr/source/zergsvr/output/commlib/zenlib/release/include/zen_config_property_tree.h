// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_config_property_tree.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_CONFIG_PROPERTY_TREE_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_CONFIG_PROPERTY_TREE_H_H_

#include <bits/stl_multimap.h>
#include <stddef.h>

typedef ZenConfPropertyTree PROPERTY_TREE_NODE;
// sizeof = 48
struct ZenConfPropertyTree { // line 38
protected:
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
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > * _M_copy(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *__x, _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > *__p); // line 1223
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
        pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,bool> insert_unique(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg0); // line 913
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert_equal(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &__v); // line 850
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert_unique(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg1); // line 939
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
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > lower_bound(const /*anon struct*/ int &arg0); // line 1365
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > lower_bound(const /*anon struct*/ int &arg0) const; // line 1383
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > upper_bound(const /*anon struct*/ int &arg0); // line 1401
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > upper_bound(const /*anon struct*/ int &arg0) const; // line 1419
        pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > equal_range(const /*anon struct*/ int &arg0); // line 1440
        pair<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > equal_range(const /*anon struct*/ int &arg0) const; // line 1450
        bool __rb_verify() const; // line 1461
    } _M_t;
    public:
    void multimap(); // line 169
    void multimap(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 178
    void multimap(const /*anon struct*/ int &arg0); // line 188
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 240
    /*anon struct*/ int get_allocator() const; // line 248
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > begin(); // line 258
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > begin() const; // line 267
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > end(); // line 276
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > end() const; // line 285
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rbegin(); // line 294
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rbegin() const; // line 303
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rend(); // line 312
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > rend() const; // line 321
    bool empty() const; // line 327
    size_t size() const; // line 332
    size_t max_size() const; // line 337
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg0); // line 354
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg1); // line 378
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0); // line 405
    size_t erase(const /*anon struct*/ int &arg0); // line 420
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg1); // line 435
    void swap(/*anon struct*/ int &arg0); // line 450
    void clear(); // line 460
    /*anon struct*/ int key_comp() const; // line 469
    value_compare value_comp() const; // line 477
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > find(const /*anon struct*/ int &arg0); // line 493
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > find(const /*anon struct*/ int &arg0) const; // line 508
    size_t count(const /*anon struct*/ int &arg0) const; // line 517
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > lower_bound(const /*anon struct*/ int &arg0); // line 532
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > lower_bound(const /*anon struct*/ int &arg0) const; // line 547
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > upper_bound(const /*anon struct*/ int &arg0); // line 557
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > upper_bound(const /*anon struct*/ int &arg0) const; // line 567
    pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > equal_range(const /*anon struct*/ int &arg0); // line 584
    pair<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > equal_range(const /*anon struct*/ int &arg0) const; // line 601
    void ~multimap();
} leaf_node_map_;
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
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _M_get_node(); // line 357
        void _M_put_node(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > *arg0); // line 361
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _M_create_node(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg0); // line 365
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _M_clone_node(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > *arg0); // line 379
        void destroy_node(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > *arg0); // line 389
        /*anon struct*/ int *& _M_root(); // line 441
        const /*anon struct*/ int * _M_root() const; // line 445
        /*anon struct*/ int *& _M_leftmost(); // line 449
        const /*anon struct*/ int * _M_leftmost() const; // line 453
        /*anon struct*/ int *& _M_rightmost(); // line 457
        const /*anon struct*/ int * _M_rightmost() const; // line 461
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _M_begin(); // line 465
        const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _M_begin() const; // line 469
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _M_end(); // line 476
        const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _M_end() const; // line 480
        static const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> & _S_value(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > *arg0); // line 484
        static const /*anon struct*/ int & _S_key(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > *arg0); // line 488
        static _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _S_left(/*anon struct*/ int *arg0); // line 492
        static const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _S_left(const /*anon struct*/ int *arg0); // line 496
        static _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _S_right(/*anon struct*/ int *arg0); // line 500
        static const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _S_right(const /*anon struct*/ int *arg0); // line 504
        static const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> & _S_value(const /*anon struct*/ int *arg0); // line 508
        static const /*anon struct*/ int & _S_key(const /*anon struct*/ int *arg0); // line 512
        static /*anon struct*/ int * _S_minimum(/*anon struct*/ int *arg0); // line 516
        static const /*anon struct*/ int * _S_minimum(const /*anon struct*/ int *arg0); // line 520
        static /*anon struct*/ int * _S_maximum(/*anon struct*/ int *arg0); // line 524
        static const /*anon struct*/ int * _S_maximum(const /*anon struct*/ int *arg0); // line 528
        private:
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > _M_insert(/*anon struct*/ int *__x, /*anon struct*/ int *__p, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &__v); // line 813
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > _M_insert(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg2); // line 831
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > * _M_copy(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > *__x, _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > *__p); // line 1223
        void _M_erase(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > *__x); // line 1259
        public:
        void _Rb_tree(); // line 554
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 557
        void _Rb_tree(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 561
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 565
        void ~_Rb_tree(); // line 577
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 791
        /*anon struct*/ int key_comp() const; // line 585
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > begin(); // line 589
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > begin() const; // line 596
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > end(); // line 603
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > end() const; // line 607
        reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > rbegin(); // line 614
        reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > rbegin() const; // line 618
        reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > rend(); // line 622
        reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > rend() const; // line 626
        bool empty() const; // line 630
        size_t size() const; // line 634
        size_t max_size() const; // line 638
        void swap(/*anon struct*/ int &arg0); // line 867
        pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> >,bool> insert_unique(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg0); // line 913
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > insert_equal(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &__v); // line 850
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > insert_unique(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg1); // line 939
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > insert_unique(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg1); // line 996
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > insert_equal(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg1); // line 1053
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > insert_equal(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg1); // line 1107
        void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0); // line 1183
        void erase(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0); // line 1197
        size_t erase(const /*anon struct*/ int &arg0); // line 1211
        void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0, _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg1); // line 1275
        void erase(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0, _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg1); // line 1288
        void erase(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1); // line 1301
        void clear(); // line 690
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > find(const /*anon struct*/ int &__k); // line 1311
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > find(const /*anon struct*/ int &__k) const; // line 1332
        size_t count(const /*anon struct*/ int &arg0) const; // line 1354
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > lower_bound(const /*anon struct*/ int &arg0); // line 1365
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > lower_bound(const /*anon struct*/ int &arg0) const; // line 1383
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > upper_bound(const /*anon struct*/ int &arg0); // line 1401
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > upper_bound(const /*anon struct*/ int &arg0) const; // line 1419
        pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> >,std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > equal_range(const /*anon struct*/ int &arg0); // line 1440
        pair<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> >,std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > equal_range(const /*anon struct*/ int &arg0) const; // line 1450
        bool __rb_verify() const; // line 1461
    } _M_t;
    public:
    void multimap(); // line 169
    void multimap(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 178
    void multimap(const /*anon struct*/ int &arg0); // line 188
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 240
    /*anon struct*/ int get_allocator() const; // line 248
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > begin(); // line 258
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > begin() const; // line 267
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > end(); // line 276
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > end() const; // line 285
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > rbegin(); // line 294
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > rbegin() const; // line 303
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > rend(); // line 312
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > rend() const; // line 321
    bool empty() const; // line 327
    size_t size() const; // line 332
    size_t max_size() const; // line 337
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > insert(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg0); // line 354
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > insert(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,ZenConfPropertyTree> &arg1); // line 378
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0); // line 405
    size_t erase(const /*anon struct*/ int &arg0); // line 420
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg0, _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > arg1); // line 435
    void swap(/*anon struct*/ int &arg0); // line 450
    void clear(); // line 460
    /*anon struct*/ int key_comp() const; // line 469
    value_compare value_comp() const; // line 477
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > find(const /*anon struct*/ int &arg0); // line 493
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > find(const /*anon struct*/ int &arg0) const; // line 508
    size_t count(const /*anon struct*/ int &arg0) const; // line 517
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > lower_bound(const /*anon struct*/ int &arg0); // line 532
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > lower_bound(const /*anon struct*/ int &arg0) const; // line 547
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > upper_bound(const /*anon struct*/ int &arg0); // line 557
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > upper_bound(const /*anon struct*/ int &arg0) const; // line 567
    pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> >,std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > equal_range(const /*anon struct*/ int &arg0); // line 584
    pair<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> >,std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ZenConfPropertyTree> > > equal_range(const /*anon struct*/ int &arg0) const; // line 601
    void ~multimap();
} child_node_map_;
public:
ZenConfPropertyTree(); // line 59
~ZenConfPropertyTree(); // line 60
int get_child(const string &path_str, const PROPERTY_TREE_NODE *&const_child_data) const; // line 64
int get_child(const string &path_str, PROPERTY_TREE_NODE *&child_data); // line 68
int put_child(const string &path_str, const string &new_child_name, const PROPERTY_TREE_NODE &new_child_note); // line 73
int new_child(const string &path_str, const string &new_child_name); // line 77
int get_leaf_str(const string &path_str, const string &key_data, string &value_data) const; // line 85
int get_leaf_str(const string &path_str, const string &key_data, char *&str_data, size_t max_str_len) const; // line 91
int get_leaf_int(const string &path_str, const string &key_data, int32_t &value_int) const; // line 97
int get_leaf_uint(const string &path_str, const string &key_data, uint32_t &value_uint) const; // line 101
int get_leaf_bool(const string &path_str, const string &key_data, bool &value_bool) const; // line 106
int put_leaf_str(const string &path_str, const string &key_data, const string &value_data); // line 111
int put_leaf_int(const string &path_str, const string &key_data, int value_int); // line 116
int put_leaf_bool(const string &path_str, const string &key_data, bool value_bool); // line 121
ZenConfPropertyTree(const ZenConfPropertyTree &arg0);
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_CONFIG_PROPERTY_TREE_H_H_
