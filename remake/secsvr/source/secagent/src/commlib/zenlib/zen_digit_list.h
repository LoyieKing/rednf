// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_digit_list.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DIGIT_LIST_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DIGIT_LIST_H_H_

#include <bits/stl_set.h>

// sizeof = 24
struct DigitList { // line 14
    enum EDigitType {
        EDT_10 = 0,
        EDT_16 = 1,
    };
private:
// anonymous struct, sizeof = 24
struct {
    private:
    // anonymous struct, sizeof = 24
    struct {
        protected:
        _Rb_tree_impl<std::less<unsigned int>,false> _M_impl;
        public:
        /*anon struct*/ int get_allocator() const; // line 352
        protected:
        _Rb_tree_node<unsigned int> * _M_get_node(); // line 357
        void _M_put_node(_Rb_tree_node<unsigned int> *arg0); // line 361
        _Rb_tree_node<unsigned int> * _M_create_node(const unsigned int &arg0); // line 365
        _Rb_tree_node<unsigned int> * _M_clone_node(const _Rb_tree_node<unsigned int> *arg0); // line 379
        void destroy_node(_Rb_tree_node<unsigned int> *arg0); // line 389
        /*anon struct*/ int *& _M_root(); // line 441
        const /*anon struct*/ int * _M_root() const; // line 445
        /*anon struct*/ int *& _M_leftmost(); // line 449
        const /*anon struct*/ int * _M_leftmost() const; // line 453
        /*anon struct*/ int *& _M_rightmost(); // line 457
        const /*anon struct*/ int * _M_rightmost() const; // line 461
        _Rb_tree_node<unsigned int> * _M_begin(); // line 465
        const _Rb_tree_node<unsigned int> * _M_begin() const; // line 469
        _Rb_tree_node<unsigned int> * _M_end(); // line 476
        const _Rb_tree_node<unsigned int> * _M_end() const; // line 480
        static const unsigned int & _S_value(const _Rb_tree_node<unsigned int> *arg0); // line 484
        static const unsigned int & _S_key(const _Rb_tree_node<unsigned int> *arg0); // line 488
        static _Rb_tree_node<unsigned int> * _S_left(/*anon struct*/ int *arg0); // line 492
        static const _Rb_tree_node<unsigned int> * _S_left(const /*anon struct*/ int *arg0); // line 496
        static _Rb_tree_node<unsigned int> * _S_right(/*anon struct*/ int *arg0); // line 500
        static const _Rb_tree_node<unsigned int> * _S_right(const /*anon struct*/ int *arg0); // line 504
        static const unsigned int & _S_value(const /*anon struct*/ int *arg0); // line 508
        static const unsigned int & _S_key(const /*anon struct*/ int *arg0); // line 512
        static /*anon struct*/ int * _S_minimum(/*anon struct*/ int *arg0); // line 516
        static const /*anon struct*/ int * _S_minimum(const /*anon struct*/ int *arg0); // line 520
        static /*anon struct*/ int * _S_maximum(/*anon struct*/ int *arg0); // line 524
        static const /*anon struct*/ int * _S_maximum(const /*anon struct*/ int *arg0); // line 528
        private:
        _Rb_tree_iterator<unsigned int> _M_insert(/*anon struct*/ int *arg0, /*anon struct*/ int *arg1, const unsigned int &arg2); // line 813
        _Rb_tree_const_iterator<unsigned int> _M_insert(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1, const unsigned int &arg2); // line 831
        _Rb_tree_node<unsigned int> * _M_copy(const _Rb_tree_node<unsigned int> *arg0, _Rb_tree_node<unsigned int> *arg1); // line 1223
        void _M_erase(_Rb_tree_node<unsigned int> *arg0); // line 1259
        public:
        void _Rb_tree(); // line 554
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 557
        void _Rb_tree(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 561
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 565
        void ~_Rb_tree(); // line 577
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 791
        /*anon struct*/ int key_comp() const; // line 585
        _Rb_tree_iterator<unsigned int> begin(); // line 589
        _Rb_tree_const_iterator<unsigned int> begin() const; // line 596
        _Rb_tree_iterator<unsigned int> end(); // line 603
        _Rb_tree_const_iterator<unsigned int> end() const; // line 607
        reverse_iterator<std::_Rb_tree_iterator<unsigned int> > rbegin(); // line 614
        reverse_iterator<std::_Rb_tree_const_iterator<unsigned int> > rbegin() const; // line 618
        reverse_iterator<std::_Rb_tree_iterator<unsigned int> > rend(); // line 622
        reverse_iterator<std::_Rb_tree_const_iterator<unsigned int> > rend() const; // line 626
        bool empty() const; // line 630
        size_t size() const; // line 634
        size_t max_size() const; // line 638
        void swap(/*anon struct*/ int &arg0); // line 867
        pair<std::_Rb_tree_iterator<unsigned int>,bool> insert_unique(const unsigned int &arg0); // line 913
        _Rb_tree_iterator<unsigned int> insert_equal(const unsigned int &arg0); // line 850
        _Rb_tree_iterator<unsigned int> insert_unique(_Rb_tree_iterator<unsigned int> arg0, const unsigned int &arg1); // line 939
        _Rb_tree_const_iterator<unsigned int> insert_unique(_Rb_tree_const_iterator<unsigned int> arg0, const unsigned int &arg1); // line 996
        _Rb_tree_iterator<unsigned int> insert_equal(_Rb_tree_iterator<unsigned int> arg0, const unsigned int &arg1); // line 1053
        _Rb_tree_const_iterator<unsigned int> insert_equal(_Rb_tree_const_iterator<unsigned int> arg0, const unsigned int &arg1); // line 1107
        void erase(_Rb_tree_iterator<unsigned int> arg0); // line 1183
        void erase(_Rb_tree_const_iterator<unsigned int> arg0); // line 1197
        size_t erase(const unsigned int &arg0); // line 1211
        void erase(_Rb_tree_iterator<unsigned int> arg0, _Rb_tree_iterator<unsigned int> arg1); // line 1275
        void erase(_Rb_tree_const_iterator<unsigned int> arg0, _Rb_tree_const_iterator<unsigned int> arg1); // line 1288
        void erase(const unsigned int *arg0, const unsigned int *arg1); // line 1301
        void clear(); // line 690
        _Rb_tree_iterator<unsigned int> find(const unsigned int &arg0); // line 1311
        _Rb_tree_const_iterator<unsigned int> find(const unsigned int &arg0) const; // line 1332
        size_t count(const unsigned int &arg0) const; // line 1354
        _Rb_tree_iterator<unsigned int> lower_bound(const unsigned int &arg0); // line 1365
        _Rb_tree_const_iterator<unsigned int> lower_bound(const unsigned int &arg0) const; // line 1383
        _Rb_tree_iterator<unsigned int> upper_bound(const unsigned int &arg0); // line 1401
        _Rb_tree_const_iterator<unsigned int> upper_bound(const unsigned int &arg0) const; // line 1419
        pair<std::_Rb_tree_iterator<unsigned int>,std::_Rb_tree_iterator<unsigned int> > equal_range(const unsigned int &arg0); // line 1440
        pair<std::_Rb_tree_const_iterator<unsigned int>,std::_Rb_tree_const_iterator<unsigned int> > equal_range(const unsigned int &arg0) const; // line 1450
        bool __rb_verify() const; // line 1461
    } _M_t;
    public:
    void set(); // line 154
    void set(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 165
    void set(const /*anon struct*/ int &arg0); // line 207
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 218
    /*anon struct*/ int key_comp() const; // line 228
    /*anon struct*/ int value_comp() const; // line 232
    /*anon struct*/ int get_allocator() const; // line 236
    _Rb_tree_const_iterator<unsigned int> begin() const; // line 244
    _Rb_tree_const_iterator<unsigned int> end() const; // line 252
    reverse_iterator<std::_Rb_tree_const_iterator<unsigned int> > rbegin() const; // line 261
    reverse_iterator<std::_Rb_tree_const_iterator<unsigned int> > rend() const; // line 270
    bool empty() const; // line 275
    size_t size() const; // line 280
    size_t max_size() const; // line 285
    void swap(/*anon struct*/ int &arg0); // line 300
    pair<std::_Rb_tree_const_iterator<unsigned int>,bool> insert(const unsigned int &arg0); // line 318
    _Rb_tree_const_iterator<unsigned int> insert(_Rb_tree_const_iterator<unsigned int> arg0, const unsigned int &arg1); // line 345
    void erase(_Rb_tree_const_iterator<unsigned int> arg0); // line 371
    size_t erase(const unsigned int &arg0); // line 386
    void erase(_Rb_tree_const_iterator<unsigned int> arg0, _Rb_tree_const_iterator<unsigned int> arg1); // line 401
    void clear(); // line 411
    size_t count(const unsigned int &arg0) const; // line 425
    _Rb_tree_const_iterator<unsigned int> find(const unsigned int &arg0); // line 443
    _Rb_tree_const_iterator<unsigned int> find(const unsigned int &arg0) const; // line 447
    _Rb_tree_const_iterator<unsigned int> lower_bound(const unsigned int &arg0); // line 464
    _Rb_tree_const_iterator<unsigned int> lower_bound(const unsigned int &arg0) const; // line 468
    _Rb_tree_const_iterator<unsigned int> upper_bound(const unsigned int &arg0); // line 480
    _Rb_tree_const_iterator<unsigned int> upper_bound(const unsigned int &arg0) const; // line 484
    pair<std::_Rb_tree_const_iterator<unsigned int>,std::_Rb_tree_const_iterator<unsigned int> > equal_range(const unsigned int &arg0); // line 505
    pair<std::_Rb_tree_const_iterator<unsigned int>,std::_Rb_tree_const_iterator<unsigned int> > equal_range(const unsigned int &arg0) const; // line 509
} values_;
public:
DigitList(); // line 16
~DigitList(); // line 17
int load(const char *arg0, DigitList::EDigitType arg1); // line 33
bool is_exist(unsigned int arg0) const; // line 43
const /*anon struct*/ int & get_list(); // line 53
private:
unsigned int str_to_digit(const char *arg0, DigitList::EDigitType arg1); // line 56
DigitList(const DigitList &arg0); // line 62
DigitList & operator=(const DigitList &arg0); // line 63
};
namespace DigitList {
    enum EDigitType {
        EDT_10 = 0,
        EDT_16 = 1,
    };
} // namespace DigitList

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DIGIT_LIST_H_H_
