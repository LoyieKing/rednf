// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_mml_command.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_COMMAND_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_COMMAND_H_H_

#include <bits/stl_map.h>
#include <bits/stl_set.h>
#include <bits/stringfwd.h>

// sizeof = 1
struct LessofCaseString { // line 15
public:
bool operator()(const string &arg0, const string &arg1) const; // line 17
};
// sizeof = 56
struct MML_Console_Command { // line 33
protected:
string mml_string_;
string mml_command_;
// anonymous struct, sizeof = 24
struct {
    private:
    // anonymous struct, sizeof = 24
    struct {
        protected:
        _Rb_tree_impl<LessofCaseString,false> _M_impl;
        public:
        /*anon struct*/ int get_allocator() const; // line 352
        protected:
        _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _M_get_node(); // line 357
        void _M_put_node(_Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > *arg0); // line 361
        _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _M_create_node(const /*anon struct*/ int &arg0); // line 365
        _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _M_clone_node(const _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > *arg0); // line 379
        void destroy_node(_Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > *arg0); // line 389
        /*anon struct*/ int *& _M_root(); // line 441
        const /*anon struct*/ int * _M_root() const; // line 445
        /*anon struct*/ int *& _M_leftmost(); // line 449
        const /*anon struct*/ int * _M_leftmost() const; // line 453
        /*anon struct*/ int *& _M_rightmost(); // line 457
        const /*anon struct*/ int * _M_rightmost() const; // line 461
        _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _M_begin(); // line 465
        const _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _M_begin() const; // line 469
        _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _M_end(); // line 476
        const _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _M_end() const; // line 480
        static const /*anon struct*/ int & _S_value(const _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > *arg0); // line 484
        static const /*anon struct*/ int & _S_key(const _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > *arg0); // line 488
        static _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _S_left(/*anon struct*/ int *arg0); // line 492
        static const _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _S_left(const /*anon struct*/ int *arg0); // line 496
        static _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _S_right(/*anon struct*/ int *arg0); // line 500
        static const _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _S_right(const /*anon struct*/ int *arg0); // line 504
        static const /*anon struct*/ int & _S_value(const /*anon struct*/ int *arg0); // line 508
        static const /*anon struct*/ int & _S_key(const /*anon struct*/ int *arg0); // line 512
        static /*anon struct*/ int * _S_minimum(/*anon struct*/ int *arg0); // line 516
        static const /*anon struct*/ int * _S_minimum(const /*anon struct*/ int *arg0); // line 520
        static /*anon struct*/ int * _S_maximum(/*anon struct*/ int *arg0); // line 524
        static const /*anon struct*/ int * _S_maximum(const /*anon struct*/ int *arg0); // line 528
        private:
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > _M_insert(/*anon struct*/ int *__x, /*anon struct*/ int *__p, const /*anon struct*/ int &__v); // line 813
        _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > _M_insert(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1, const /*anon struct*/ int &arg2); // line 831
        _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > * _M_copy(const _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > *arg0, _Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > *arg1); // line 1223
        void _M_erase(_Rb_tree_node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > *__x); // line 1259
        public:
        void _Rb_tree(); // line 554
        void _Rb_tree(const LessofCaseString &arg0); // line 557
        void _Rb_tree(const LessofCaseString &arg0, const /*anon struct*/ int &arg1); // line 561
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 565
        void ~_Rb_tree(); // line 577
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 791
        LessofCaseString key_comp() const; // line 585
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > begin(); // line 589
        _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > begin() const; // line 596
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > end(); // line 603
        _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > end() const; // line 607
        reverse_iterator<std::_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > rbegin(); // line 614
        reverse_iterator<std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > rbegin() const; // line 618
        reverse_iterator<std::_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > rend(); // line 622
        reverse_iterator<std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > rend() const; // line 626
        bool empty() const; // line 630
        size_t size() const; // line 634
        size_t max_size() const; // line 638
        void swap(/*anon struct*/ int &arg0); // line 867
        pair<std::_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,bool> insert_unique(const /*anon struct*/ int &__v); // line 913
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > insert_equal(const /*anon struct*/ int &arg0); // line 850
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > insert_unique(_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0, const /*anon struct*/ int &arg1); // line 939
        _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > insert_unique(_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0, const /*anon struct*/ int &arg1); // line 996
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > insert_equal(_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0, const /*anon struct*/ int &arg1); // line 1053
        _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > insert_equal(_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0, const /*anon struct*/ int &arg1); // line 1107
        void erase(_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0); // line 1183
        void erase(_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0); // line 1197
        size_t erase(const /*anon struct*/ int &arg0); // line 1211
        void erase(_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0, _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg1); // line 1275
        void erase(_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0, _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg1); // line 1288
        void erase(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1); // line 1301
        void clear(); // line 690
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > find(const /*anon struct*/ int &arg0); // line 1311
        _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > find(const /*anon struct*/ int &__k) const; // line 1332
        size_t count(const /*anon struct*/ int &arg0) const; // line 1354
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > lower_bound(const /*anon struct*/ int &arg0); // line 1365
        _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > lower_bound(const /*anon struct*/ int &arg0) const; // line 1383
        _Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > upper_bound(const /*anon struct*/ int &arg0); // line 1401
        _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > upper_bound(const /*anon struct*/ int &arg0) const; // line 1419
        pair<std::_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,std::_Rb_tree_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > equal_range(const /*anon struct*/ int &arg0); // line 1440
        pair<std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > equal_range(const /*anon struct*/ int &arg0) const; // line 1450
        bool __rb_verify() const; // line 1461
    } _M_t;
    public:
    void set(); // line 154
    void set(const LessofCaseString &arg0, const /*anon struct*/ int &arg1); // line 165
    void set(const /*anon struct*/ int &arg0); // line 207
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 218
    LessofCaseString key_comp() const; // line 228
    LessofCaseString value_comp() const; // line 232
    /*anon struct*/ int get_allocator() const; // line 236
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > begin() const; // line 244
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > end() const; // line 252
    reverse_iterator<std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > rbegin() const; // line 261
    reverse_iterator<std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > rend() const; // line 270
    bool empty() const; // line 275
    size_t size() const; // line 280
    size_t max_size() const; // line 285
    void swap(/*anon struct*/ int &arg0); // line 300
    pair<std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,bool> insert(const /*anon struct*/ int &arg0); // line 318
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > insert(_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0, const /*anon struct*/ int &arg1); // line 345
    void erase(_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0); // line 371
    size_t erase(const /*anon struct*/ int &arg0); // line 386
    void erase(_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg0, _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > arg1); // line 401
    void clear(); // line 411
    size_t count(const /*anon struct*/ int &arg0) const; // line 425
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > find(const /*anon struct*/ int &arg0); // line 443
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > find(const /*anon struct*/ int &arg0) const; // line 447
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > lower_bound(const /*anon struct*/ int &arg0); // line 464
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > lower_bound(const /*anon struct*/ int &arg0) const; // line 468
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > upper_bound(const /*anon struct*/ int &arg0); // line 480
    _Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > upper_bound(const /*anon struct*/ int &arg0) const; // line 484
    pair<std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > equal_range(const /*anon struct*/ int &arg0); // line 505
    pair<std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > equal_range(const /*anon struct*/ int &arg0) const; // line 509
    void ~set();
} mml_cmd_option_;
// anonymous struct, sizeof = 24
struct {
    private:
    // anonymous struct, sizeof = 24
    struct {
        protected:
        _Rb_tree_impl<LessofCaseString,false> _M_impl;
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
        void _Rb_tree(const LessofCaseString &arg0); // line 557
        void _Rb_tree(const LessofCaseString &arg0, const /*anon struct*/ int &arg1); // line 561
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 565
        void ~_Rb_tree(); // line 577
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 791
        LessofCaseString key_comp() const; // line 585
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
    void map(const LessofCaseString &arg0, const /*anon struct*/ int &arg1); // line 164
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
    /*anon struct*/ int & operator[](const /*anon struct*/ int &__k); // line 340
    /*anon struct*/ int & at(const /*anon struct*/ int &arg0); // line 362
    const /*anon struct*/ int & at(const /*anon struct*/ int &arg0) const; // line 371
    pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >,bool> insert(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg0); // line 395
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > insert(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> > > &arg1); // line 419
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0); // line 445
    size_t erase(const /*anon struct*/ int &arg0); // line 460
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg0, _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > arg1); // line 475
    void swap(/*anon struct*/ int &arg0); // line 490
    void clear(); // line 500
    LessofCaseString key_comp() const; // line 509
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
} mml_cmd_parameter_;
public:
MML_Console_Command(const char *arg0); // line 53
MML_Console_Command(); // line 54
~MML_Console_Command(); // line 55
void GetMMLString(string &mml_string) const; // line 60
const char * GetMMLString() const; // line 61
int GetMMLCommand(string &mml_cmd) const; // line 64
const char * GetMMLCommand() const; // line 65
int IsHaveCmdOption(const string &mml_option) const; // line 68
int GetCmdParameter(const string &para_key, string &para_value) const; // line 71
int GetCmdParameter(const string &arg0, char *arg1) const; // line 73
int ParseMMLCommandString(const char *mml_cmd_string); // line 79
int ParseMMLCommandString2(const char *mml_cmd_string); // line 82
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_COMMAND_H_H_
