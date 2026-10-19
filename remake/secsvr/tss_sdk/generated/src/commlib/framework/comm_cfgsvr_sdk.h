// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_cfgsvr_sdk.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_CFGSVR_SDK_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_CFGSVR_SDK_H_H_

#include "output/protocol/common/release/include/comm_conf_cfgsdk.h"
#include "src/commlib/framework/comm_service_info.h"
#include <bits/stl_map.h>
#include <bits/stl_vector.h>
#include <bits/stringfwd.h>

// sizeof = 344
struct CfgSvrSdk : public Zen_Thread_Task { // line 14
    // sizeof = 16
    struct CFG_MD5 {
    public:
    unsigned char md5_value_[];
    };
    enum GET_CONFIG_RESULT {
        CONFIG_IS_NEWEST = 1,
        CONFIG_IS_GET_OVER = 2,
        CONFIG_NEED_GET_MORE = 3,
    };
private:
static const unsigned int MAX_FILE_LENGTH;
static const short int MASTER_CFGSVR_PORT;
static const unsigned int MASTER_CFGSVR_ID;
static const int CHECK_UPDATE_TIME;
static CfgSvrSdk *instance_;
SERVICES_ID self_info_;
SERVICES_ID master_cfgsvr_info_;
SERVICES_ID slave_cfgsvr_info_;
short unsigned int svc_index_;
ZEN_Sockaddr_In master_cfgsvr_ip_;
ZEN_Sockaddr_In slave_cfgsvr_ip_;
// anonymous struct, sizeof = 154
struct {
    public:
    uint16_t slave_cfgsvr_type_;
    uint32_t slave_cfgsvr_id_;
    char slave_cfgsvr_ip_[];
    uint16_t slave_cfgsvr_port_;
    uint32_t game_id_;
    uint32_t idc_no_;
    uint16_t world_;
    uint32_t deploy_info_num_;
    /*anon struct*/ int deploy_info_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 94
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 96
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 97
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 99
    tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 100
    tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 101
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 103
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 104
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 106
    tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 107
    tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 108
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 109
} cfgsdk_config_;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, const /*anon struct*/ int &arg1); // line 298
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > begin(); // line 332
    __normal_iterator<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > begin() const; // line 341
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > end(); // line 350
    __normal_iterator<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, /*anon struct*/ int arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    /*anon struct*/ int & operator[](unsigned int arg0); // line 479
    const /*anon struct*/ int & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    /*anon struct*/ int & at(unsigned int arg0); // line 519
    const /*anon struct*/ int & at(unsigned int arg0) const; // line 537
    /*anon struct*/ int & front(); // line 548
    const /*anon struct*/ int & front() const; // line 556
    /*anon struct*/ int & back(); // line 564
    const /*anon struct*/ int & back() const; // line 572
    /*anon struct*/ int * data(); // line 583
    const /*anon struct*/ int * data() const; // line 587
    void push_back(const /*anon struct*/ int &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > insert(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0, const /*anon struct*/ int &arg1); // line 93
    void insert(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0, unsigned int arg1, const /*anon struct*/ int &arg2); // line 657
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > erase(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0); // line 110
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > erase(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0, __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg1); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, const /*anon struct*/ int &arg1); // line 171
    void _M_fill_insert(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0, unsigned int arg1, const /*anon struct*/ int &arg2); // line 311
    void _M_insert_aux(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > __position, const /*anon struct*/ int &__x); // line 249
} cfg_name_list_;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, const /*anon struct*/ int &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, const /*anon struct*/ int &arg1); // line 298
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > begin(); // line 332
    __normal_iterator<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > begin() const; // line 341
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > end(); // line 350
    __normal_iterator<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, /*anon struct*/ int arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    /*anon struct*/ int & operator[](unsigned int arg0); // line 479
    const /*anon struct*/ int & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    /*anon struct*/ int & at(unsigned int arg0); // line 519
    const /*anon struct*/ int & at(unsigned int arg0) const; // line 537
    /*anon struct*/ int & front(); // line 548
    const /*anon struct*/ int & front() const; // line 556
    /*anon struct*/ int & back(); // line 564
    const /*anon struct*/ int & back() const; // line 572
    /*anon struct*/ int * data(); // line 583
    const /*anon struct*/ int * data() const; // line 587
    void push_back(const /*anon struct*/ int &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > insert(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0, const /*anon struct*/ int &arg1); // line 93
    void insert(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0, unsigned int arg1, const /*anon struct*/ int &arg2); // line 657
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > erase(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0); // line 110
    __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > erase(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0, __normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg1); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, const /*anon struct*/ int &arg1); // line 171
    void _M_fill_insert(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > arg0, unsigned int arg1, const /*anon struct*/ int &arg2); // line 311
    void _M_insert_aux(__normal_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char> >*,std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > __position, const /*anon struct*/ int &__x); // line 249
} config_update_list_;
unsigned char *file_buf_;
unsigned int recv_len_;
string curr_cfg_name_;
CfgSvrSdk::CFG_MD5 local_cfg_md5_;
CfgSvrSdk::CFG_MD5 recv_cfg_md5_;
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
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _M_get_node(); // line 357
        void _M_put_node(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > *arg0); // line 361
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _M_create_node(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg0); // line 365
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _M_clone_node(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > *arg0); // line 379
        void destroy_node(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > *arg0); // line 389
        /*anon struct*/ int *& _M_root(); // line 441
        const /*anon struct*/ int * _M_root() const; // line 445
        /*anon struct*/ int *& _M_leftmost(); // line 449
        const /*anon struct*/ int * _M_leftmost() const; // line 453
        /*anon struct*/ int *& _M_rightmost(); // line 457
        const /*anon struct*/ int * _M_rightmost() const; // line 461
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _M_begin(); // line 465
        const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _M_begin() const; // line 469
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _M_end(); // line 476
        const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _M_end() const; // line 480
        static const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> & _S_value(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > *arg0); // line 484
        static const /*anon struct*/ int & _S_key(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > *arg0); // line 488
        static _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _S_left(/*anon struct*/ int *arg0); // line 492
        static const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _S_left(const /*anon struct*/ int *arg0); // line 496
        static _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _S_right(/*anon struct*/ int *arg0); // line 500
        static const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _S_right(const /*anon struct*/ int *arg0); // line 504
        static const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> & _S_value(const /*anon struct*/ int *arg0); // line 508
        static const /*anon struct*/ int & _S_key(const /*anon struct*/ int *arg0); // line 512
        static /*anon struct*/ int * _S_minimum(/*anon struct*/ int *arg0); // line 516
        static const /*anon struct*/ int * _S_minimum(const /*anon struct*/ int *arg0); // line 520
        static /*anon struct*/ int * _S_maximum(/*anon struct*/ int *arg0); // line 524
        static const /*anon struct*/ int * _S_maximum(const /*anon struct*/ int *arg0); // line 528
        private:
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > _M_insert(/*anon struct*/ int *__x, /*anon struct*/ int *__p, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &__v); // line 813
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > _M_insert(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg2); // line 831
        _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > * _M_copy(const _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > *arg0, _Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > *arg1); // line 1223
        void _M_erase(_Rb_tree_node<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > *__x); // line 1259
        public:
        void _Rb_tree(); // line 554
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 557
        void _Rb_tree(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 561
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 565
        void ~_Rb_tree(); // line 577
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 791
        /*anon struct*/ int key_comp() const; // line 585
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > begin(); // line 589
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > begin() const; // line 596
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > end(); // line 603
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > end() const; // line 607
        reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > rbegin(); // line 614
        reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > rbegin() const; // line 618
        reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > rend(); // line 622
        reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > rend() const; // line 626
        bool empty() const; // line 630
        size_t size() const; // line 634
        size_t max_size() const; // line 638
        void swap(/*anon struct*/ int &arg0); // line 867
        pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> >,bool> insert_unique(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &__v); // line 913
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > insert_equal(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg0); // line 850
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > insert_unique(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg1); // line 939
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > insert_unique(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg1); // line 996
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > insert_equal(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg1); // line 1053
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > insert_equal(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg1); // line 1107
        void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0); // line 1183
        void erase(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0); // line 1197
        size_t erase(const /*anon struct*/ int &__x); // line 1211
        void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > __first, _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > __last); // line 1275
        void erase(_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0, _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg1); // line 1288
        void erase(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1); // line 1301
        void clear(); // line 690
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > find(const /*anon struct*/ int &__k); // line 1311
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > find(const /*anon struct*/ int &arg0) const; // line 1332
        size_t count(const /*anon struct*/ int &arg0) const; // line 1354
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > lower_bound(const /*anon struct*/ int &__k); // line 1365
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > lower_bound(const /*anon struct*/ int &arg0) const; // line 1383
        _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > upper_bound(const /*anon struct*/ int &__k); // line 1401
        _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > upper_bound(const /*anon struct*/ int &arg0) const; // line 1419
        pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> >,std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > equal_range(const /*anon struct*/ int &arg0); // line 1440
        pair<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> >,std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > equal_range(const /*anon struct*/ int &arg0) const; // line 1450
        bool __rb_verify() const; // line 1461
    } _M_t;
    public:
    void map(); // line 156
    void map(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 164
    void map(const /*anon struct*/ int &arg0); // line 174
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 225
    /*anon struct*/ int get_allocator() const; // line 233
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > begin(); // line 243
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > begin() const; // line 252
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > end(); // line 260
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > end() const; // line 269
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > rbegin(); // line 278
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > rbegin() const; // line 287
    reverse_iterator<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > rend(); // line 296
    reverse_iterator<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > rend() const; // line 305
    bool empty() const; // line 313
    size_t size() const; // line 318
    size_t max_size() const; // line 323
    CfgSvrSdk::CFG_MD5 & operator[](const /*anon struct*/ int &arg0); // line 340
    CfgSvrSdk::CFG_MD5 & at(const /*anon struct*/ int &arg0); // line 362
    const CfgSvrSdk::CFG_MD5 & at(const /*anon struct*/ int &arg0) const; // line 371
    pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> >,bool> insert(const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg0); // line 395
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > insert(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0, const pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >,CfgSvrSdk::CFG_MD5> &arg1); // line 419
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0); // line 445
    size_t erase(const /*anon struct*/ int &arg0); // line 460
    void erase(_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg0, _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > arg1); // line 475
    void swap(/*anon struct*/ int &arg0); // line 490
    void clear(); // line 500
    /*anon struct*/ int key_comp() const; // line 509
    value_compare value_comp() const; // line 517
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > find(const /*anon struct*/ int &arg0); // line 533
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > find(const /*anon struct*/ int &arg0) const; // line 548
    size_t count(const /*anon struct*/ int &arg0) const; // line 560
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > lower_bound(const /*anon struct*/ int &arg0); // line 575
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > lower_bound(const /*anon struct*/ int &arg0) const; // line 590
    _Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > upper_bound(const /*anon struct*/ int &arg0); // line 600
    _Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > upper_bound(const /*anon struct*/ int &arg0) const; // line 610
    pair<std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> >,std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > equal_range(const /*anon struct*/ int &arg0); // line 629
    pair<std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> >,std::_Rb_tree_const_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, CfgSvrSdk::CFG_MD5> > > equal_range(const /*anon struct*/ int &arg0) const; // line 648
    void ~map();
} cfg_md5_map_;
bool is_pthread_start_;
bool is_updade_config_;
public:
CfgSvrSdk(const CfgSvrSdk &arg0);
private:
CfgSvrSdk(); // line 24
virtual ~CfgSvrSdk(); // line 40
public:
static CfgSvrSdk * instance(); // line 49
static void clean_instance(); // line 59
void add_config(const char *cfg_name); // line 604
void set_service_type(short unsigned int svc_type); // line 178
int init(); // line 74
unsigned int get_game_id(); // line 408
int get_idc_no(); // line 419
short unsigned int get_world_id(); // line 430
void get_deploy_world_list(/*anon struct*/ int &world_list); // line 515
int get_ip_info(const SERVICES_ID &req_info, ZEN_Sockaddr_In &svr_inetadd, unsigned int &svr_idc_no); // line 536
int start_task(); // line 1143
bool is_need_update_file(); // line 1289
void clear_file_update_flags(); // line 1300
private:
int get_slave_cfgsvr_info(); // line 242
int init_zulu_ex(Zulu_SendRecv_Package &zulu_ex, const ZEN_Sockaddr_In &ip_info, const SERVICES_ID &recv_info); // line 194
int get_svc_id(); // line 332
int get_svc_deploy_info(); // line 442
int get_config_file(); // line 615
int get_config_file(Zulu_SendRecv_Package &zulu_ex, const char *cfg_name); // line 651
void insert_config_md5(const string &cfg_name, const CfgSvrSdk::CFG_MD5 &cfg_md5); // line 776
void caculate_config_md5(); // line 810
void get_config_md5(const string &cfg_name, CfgSvrSdk::CFG_MD5 &cfg_md5); // line 791
int send_config_file_req(Zulu_SendRecv_Package &zulu_ex); // line 856
int recv_config_file_rsp(Zulu_SendRecv_Package &zulu_ex, CfgSvrSdk::GET_CONFIG_RESULT &get_config_result); // line 895
int read_local_file(int &length); // line 741
bool check_config_md5(); // line 832
int write_local_file(); // line 983
int uncompress_file(const char *cfg_name); // line 1313
int get_config_update_list(Zulu_SendRecv_Package &zulu_ex); // line 1018
int update_config_file(Zulu_SendRecv_Package &zulu_ex); // line 1120
void update_config_name_list(); // line 1083
virtual int svc(); // line 1171
};
extern const unsigned int MAX_FILE_LENGTH; // line 39
extern const short int MASTER_CFGSVR_PORT; // line 42
extern const unsigned int MASTER_CFGSVR_ID; // line 45
extern const int CHECK_UPDATE_TIME; // line 48
namespace CfgSvrSdk {
    // sizeof = 16
    struct CFG_MD5 { // line 18
    public:
    unsigned char md5_value_[];
    };
    enum GET_CONFIG_RESULT {
        CONFIG_IS_NEWEST = 1,
        CONFIG_IS_GET_OVER = 2,
        CONFIG_NEED_GET_MORE = 3,
    };
} // namespace CfgSvrSdk

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_CFGSVR_SDK_H_H_
