// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_socket_channel.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SOCKET_CHANNEL_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SOCKET_CHANNEL_H_H_

#include <bits/stl_map.h>

// sizeof = 108
struct SocketChannel : public Zen_Thread_Task { // line 29
private:
static const int HEAVY_IDLE_INTERVAL_MICROSECOND;
volatile bool is_thread_run_;
// anonymous struct, sizeof = 24
struct {
    private:
    // anonymous struct, sizeof = 24
    struct {
        protected:
        _Rb_tree_impl<std::less<int>,false> _M_impl;
        public:
        /*anon struct*/ int get_allocator() const; // line 352
        protected:
        _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _M_get_node(); // line 357
        void _M_put_node(_Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > *arg0); // line 361
        _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _M_create_node(const pair<const int,SocketChannelBusinessInfo*> &arg0); // line 365
        _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _M_clone_node(const _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > *arg0); // line 379
        void destroy_node(_Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > *arg0); // line 389
        /*anon struct*/ int *& _M_root(); // line 441
        const /*anon struct*/ int * _M_root() const; // line 445
        /*anon struct*/ int *& _M_leftmost(); // line 449
        const /*anon struct*/ int * _M_leftmost() const; // line 453
        /*anon struct*/ int *& _M_rightmost(); // line 457
        const /*anon struct*/ int * _M_rightmost() const; // line 461
        _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _M_begin(); // line 465
        const _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _M_begin() const; // line 469
        _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _M_end(); // line 476
        const _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _M_end() const; // line 480
        static const pair<const int,SocketChannelBusinessInfo*> & _S_value(const _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > *arg0); // line 484
        static const int & _S_key(const _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > *arg0); // line 488
        static _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _S_left(/*anon struct*/ int *arg0); // line 492
        static const _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _S_left(const /*anon struct*/ int *arg0); // line 496
        static _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _S_right(/*anon struct*/ int *arg0); // line 500
        static const _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _S_right(const /*anon struct*/ int *arg0); // line 504
        static const pair<const int,SocketChannelBusinessInfo*> & _S_value(const /*anon struct*/ int *arg0); // line 508
        static const int & _S_key(const /*anon struct*/ int *arg0); // line 512
        static /*anon struct*/ int * _S_minimum(/*anon struct*/ int *arg0); // line 516
        static const /*anon struct*/ int * _S_minimum(const /*anon struct*/ int *arg0); // line 520
        static /*anon struct*/ int * _S_maximum(/*anon struct*/ int *arg0); // line 524
        static const /*anon struct*/ int * _S_maximum(const /*anon struct*/ int *arg0); // line 528
        private:
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > _M_insert(/*anon struct*/ int *__x, /*anon struct*/ int *__p, const pair<const int,SocketChannelBusinessInfo*> &__v); // line 813
        _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > _M_insert(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1, const pair<const int,SocketChannelBusinessInfo*> &arg2); // line 831
        _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > * _M_copy(const _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > *arg0, _Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > *arg1); // line 1223
        void _M_erase(_Rb_tree_node<std::pair<const int, SocketChannelBusinessInfo*> > *__x); // line 1259
        public:
        void _Rb_tree(); // line 554
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 557
        void _Rb_tree(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 561
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 565
        void ~_Rb_tree(); // line 577
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 791
        /*anon struct*/ int key_comp() const; // line 585
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > begin(); // line 589
        _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > begin() const; // line 596
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > end(); // line 603
        _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > end() const; // line 607
        reverse_iterator<std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > rbegin(); // line 614
        reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > rbegin() const; // line 618
        reverse_iterator<std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > rend(); // line 622
        reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > rend() const; // line 626
        bool empty() const; // line 630
        size_t size() const; // line 634
        size_t max_size() const; // line 638
        void swap(/*anon struct*/ int &arg0); // line 867
        pair<std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> >,bool> insert_unique(const pair<const int,SocketChannelBusinessInfo*> &__v); // line 913
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > insert_equal(const pair<const int,SocketChannelBusinessInfo*> &arg0); // line 850
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > insert_unique(_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0, const pair<const int,SocketChannelBusinessInfo*> &arg1); // line 939
        _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > insert_unique(_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0, const pair<const int,SocketChannelBusinessInfo*> &arg1); // line 996
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > insert_equal(_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0, const pair<const int,SocketChannelBusinessInfo*> &arg1); // line 1053
        _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > insert_equal(_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0, const pair<const int,SocketChannelBusinessInfo*> &arg1); // line 1107
        void erase(_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0); // line 1183
        void erase(_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0); // line 1197
        size_t erase(const int &arg0); // line 1211
        void erase(_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0, _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg1); // line 1275
        void erase(_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0, _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg1); // line 1288
        void erase(const int *arg0, const int *arg1); // line 1301
        void clear(); // line 690
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > find(const int &__k); // line 1311
        _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > find(const int &arg0) const; // line 1332
        size_t count(const int &arg0) const; // line 1354
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > lower_bound(const int &arg0); // line 1365
        _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > lower_bound(const int &arg0) const; // line 1383
        _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > upper_bound(const int &arg0); // line 1401
        _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > upper_bound(const int &arg0) const; // line 1419
        pair<std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> >,std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > equal_range(const int &arg0); // line 1440
        pair<std::_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> >,std::_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > equal_range(const int &arg0) const; // line 1450
        bool __rb_verify() const; // line 1461
    } _M_t;
    public:
    void map(); // line 156
    void map(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 164
    void map(const /*anon struct*/ int &arg0); // line 174
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 225
    /*anon struct*/ int get_allocator() const; // line 233
    _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > begin(); // line 243
    _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > begin() const; // line 252
    _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > end(); // line 260
    _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > end() const; // line 269
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > rbegin(); // line 278
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > rbegin() const; // line 287
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > rend(); // line 296
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > rend() const; // line 305
    bool empty() const; // line 313
    size_t size() const; // line 318
    size_t max_size() const; // line 323
    SocketChannelBusinessInfo *& operator[](const int &arg0); // line 340
    SocketChannelBusinessInfo *& at(const int &arg0); // line 362
    SocketChannelBusinessInfo *const & at(const int &arg0) const; // line 371
    pair<std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> >,bool> insert(const pair<const int,SocketChannelBusinessInfo*> &arg0); // line 395
    _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > insert(_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0, const pair<const int,SocketChannelBusinessInfo*> &arg1); // line 419
    void erase(_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0); // line 445
    size_t erase(const int &arg0); // line 460
    void erase(_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg0, _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > arg1); // line 475
    void swap(/*anon struct*/ int &arg0); // line 490
    void clear(); // line 500
    /*anon struct*/ int key_comp() const; // line 509
    value_compare value_comp() const; // line 517
    _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > find(const int &arg0); // line 533
    _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > find(const int &arg0) const; // line 548
    size_t count(const int &arg0) const; // line 560
    _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > lower_bound(const int &arg0); // line 575
    _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > lower_bound(const int &arg0) const; // line 590
    _Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > upper_bound(const int &arg0); // line 600
    _Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > upper_bound(const int &arg0) const; // line 610
    pair<std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> >,std::_Rb_tree_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > equal_range(const int &arg0); // line 629
    pair<std::_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> >,std::_Rb_tree_const_iterator<std::pair<const int, SocketChannelBusinessInfo*> > > equal_range(const int &arg0) const; // line 648
    void ~map();
} business_info_map_;
ZEN_Thread_RW_Mutex business_info_map_rwlock_;
ZEN_Thread_Mutex start_thread_lock_;
static SocketChannel *instance_;
public:
SocketChannel(const SocketChannel &arg0);
virtual ~SocketChannel(); // line 36
int init_connection(SocketChannelInitData &addr); // line 78
int fini(); // line 51
int send_data(int business_id, const void *send_buf); // line 244
int recv_data(int *business_id, void *recv_buf, size_t *recv_buf_len); // line 280
int recv_business_data(int business_id, void *recv_buf, size_t *recv_buf_len); // line 330
static SocketChannel * instance(); // line 41
private:
SocketChannel(); // line 25
int start_thread(); // line 184
int stop_thread(); // line 212
virtual int svc(); // line 219
int send_to_net(); // line 382
SocketChannelBusinessInfo * get_business_info(int business_id); // line 169
};
extern const int HEAVY_IDLE_INTERVAL_MICROSECOND; // line 80

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SOCKET_CHANNEL_H_H_
