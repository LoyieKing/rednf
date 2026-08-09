// Auto-generated header stub from DWARF info
// Original path: output/protocol/formmog/release/include/formmog_proto_antibot_client.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_
#define SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_

#include <stdint.h>

typedef struct {
    public:
    uint8_t msg_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 760
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 762
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 763
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 765
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 766
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 769
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 770
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 771
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 772
} TAB_CS_TP_INFO_MSG;
typedef struct {
    public:
    uint32_t process_type_;
    uint8_t virus_name_[];
    uint8_t module_name_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 873
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 875
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 876
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 878
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 879
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 882
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 883
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 884
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 885
} TQD_CS_VIRUSINFO;
typedef struct {
    public:
    uint32_t virus_id_;
    uint32_t mod_size_;
    char path_name_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 951
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 953
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 954
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 956
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 957
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 960
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 961
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 962
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 963
} TROJANINFO;
typedef struct {
    public:
    sec_proto::TROJAN_REPORT::._230 _unnamed_;
    tsf4g_tdr::TdrError::ErrorType construct(int64_t arg0); // line 1026
    tsf4g_tdr::TdrError::ErrorType pack(int64_t arg0, /*anon struct*/ int &arg1, unsigned int arg2) const; // line 1028
    tsf4g_tdr::TdrError::ErrorType pack(int64_t arg0, char *arg1, size_t arg2, size_t *arg3, unsigned int arg4) const; // line 1030
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t arg0, /*anon struct*/ int &arg1, unsigned int arg2); // line 1032
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t arg0, const char *arg1, size_t arg2, size_t *arg3, unsigned int arg4); // line 1034
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t arg0, /*anon struct*/ int &arg1, int arg2, char arg3) const; // line 1037
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t arg0, char *arg1, size_t arg2, size_t *arg3, int arg4, char arg5) const; // line 1039
} TROJAN_REPORT;
typedef struct {
    public:
    uint8_t file_id_;
    uint8_t file_hash_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 1100
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1102
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1103
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1105
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1106
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1109
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1110
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1111
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1112
} FILE_HASH_VAL;
typedef struct {
    public:
    sec_proto::SAB_CS_SLN_DATA::._269 _unnamed_;
    tsf4g_tdr::TdrError::ErrorType construct(int64_t arg0); // line 2482
    tsf4g_tdr::TdrError::ErrorType pack(int64_t arg0, /*anon struct*/ int &arg1, unsigned int arg2) const; // line 2484
    tsf4g_tdr::TdrError::ErrorType pack(int64_t arg0, char *arg1, size_t arg2, size_t *arg3, unsigned int arg4) const; // line 2486
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t arg0, /*anon struct*/ int &arg1, unsigned int arg2); // line 2488
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t arg0, const char *arg1, size_t arg2, size_t *arg3, unsigned int arg4); // line 2490
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t arg0, /*anon struct*/ int &arg1, int arg2, char arg3) const; // line 2493
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t arg0, char *arg1, size_t arg2, size_t *arg3, int arg4, char arg5) const; // line 2495
} SAB_CS_SLN_DATA;
typedef struct {
    public:
    uint8_t cfg_id_;
    uint8_t cmd_size_;
    char cmd_data_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 3453
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3455
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3456
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3458
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3459
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3462
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3463
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3464
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3465
} SSC_SE_SCAN_DATA_ITEM;
typedef struct {
    public:
    sec_proto::SC_SCHEMEDATA::._299 _unnamed_;
    tsf4g_tdr::TdrError::ErrorType construct(int64_t arg0); // line 3567
    tsf4g_tdr::TdrError::ErrorType pack(int64_t arg0, /*anon struct*/ int &arg1, unsigned int arg2) const; // line 3569
    tsf4g_tdr::TdrError::ErrorType pack(int64_t arg0, char *arg1, size_t arg2, size_t *arg3, unsigned int arg4) const; // line 3571
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t arg0, /*anon struct*/ int &arg1, unsigned int arg2); // line 3573
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t arg0, const char *arg1, size_t arg2, size_t *arg3, unsigned int arg4); // line 3575
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t arg0, /*anon struct*/ int &arg1, int arg2, char arg3) const; // line 3578
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t arg0, char *arg1, size_t arg2, size_t *arg3, int arg4, char arg5) const; // line 3580
} SC_SCHEMEDATA;
typedef struct {
    public:
    uint8_t name_len_;
    uint8_t module_name_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 4009
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4011
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4012
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4014
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4015
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4018
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4019
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4020
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4021
} TMODULE_NAME;
typedef struct {
    public:
    uint8_t id_;
    TMODULE_NAME file_name_;
    uint8_t file_type_;
    uint32_t offset_;
    uint32_t len_;
    tsf4g_tdr::TdrError::ErrorType construct(); // line 4048
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4050
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4051
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4053
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4054
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4057
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4058
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4059
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4060
} TFILE_HASH_INFO_ITEM;
typedef struct {
    public:
    uint16_t count_;
    TFILE_HASH_INFO_ITEM file_hash_item_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 4084
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4086
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4087
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4089
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4090
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4093
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4094
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4095
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4096
} TFILE_HASH_INFO;
typedef struct {
    public:
    sec_proto::TORDINAL_OR_NAME::._317 _unnamed_;
    tsf4g_tdr::TdrError::ErrorType construct(int64_t arg0); // line 4195
    tsf4g_tdr::TdrError::ErrorType pack(int64_t arg0, /*anon struct*/ int &arg1, unsigned int arg2) const; // line 4197
    tsf4g_tdr::TdrError::ErrorType pack(int64_t arg0, char *arg1, size_t arg2, size_t *arg3, unsigned int arg4) const; // line 4199
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t arg0, /*anon struct*/ int &arg1, unsigned int arg2); // line 4201
    tsf4g_tdr::TdrError::ErrorType unpack(int64_t arg0, const char *arg1, size_t arg2, size_t *arg3, unsigned int arg4); // line 4203
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t arg0, /*anon struct*/ int &arg1, int arg2, char arg3) const; // line 4206
    tsf4g_tdr::TdrError::ErrorType visualize(int64_t arg0, char *arg1, size_t arg2, size_t *arg3, int arg4, char arg5) const; // line 4208
} TORDINAL_OR_NAME;
typedef struct {
    public:
    uint8_t id_;
    TORDINAL_OR_NAME name_or_ordinal_;
    uint8_t mem_type_;
    uint32_t cur_times_;
    uint32_t total_times_;
    uint32_t offset_;
    uint32_t len_;
    uint8_t mem_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 4238
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4240
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4241
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4243
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4244
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4247
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4248
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4249
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4250
} TMEM_RECOVER_ITEM;
typedef struct {
    public:
    uint16_t count_;
    TMEM_RECOVER_ITEM mem_item_[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 4274
    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4276
    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4277
    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4279
    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4280
    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4283
    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4284
    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4285
    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4286
} TMEM_RECOVER;
namespace sec_proto {
    namespace AntibotPkgBody {
        // sizeof = 38541
        union ._323 { // line 4379
            typedef struct {
                public:
                uint32_t init_crc_value_;
                uint8_t send_ver_[];
                uint8_t data_flag_;
                uint16_t rcid_;
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2752
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2754
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2755
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2757
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2758
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2761
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2762
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2763
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2764
            } TAB_SC_SENDANTIBOT;
            typedef struct {
                public:
                uint8_t rand_data_[];
                uint8_t type_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2788
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2790
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2791
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2793
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2794
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2797
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2798
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2799
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2800
            } TAB_SC_RANDCHECK;
            typedef struct {
                public:
                uint8_t is_init_;
                uint8_t sig_flag_;
                uint8_t rccp_ver_[];
                uint32_t module_handle_;
                uint32_t lib_base_;
                uint8_t os_ver_;
                uint8_t st_ver_[];
                uint32_t proc_address_;
                uint8_t sf_ver_[];
                uint16_t rc_id_;
                uint32_t client_ver_;
                uint8_t user_id_[];
                uint8_t admin_;
                uint8_t dib_id_;
                uint32_t key_offset_;
                uint8_t key_len_;
                uint8_t rp_data_info_size_;
                char rp_data_info_[];
                uint8_t cs_data_;
                uint8_t len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 600
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 602
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 603
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 605
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 606
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 609
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 610
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 611
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 612
            } TAB_CS_INITINFO;
            typedef struct {
                public:
                uint16_t ext_size_;
                uint32_t uin_;
                uint8_t stamp_[];
                uint8_t module_[];
                uint8_t out_proc_[];
                uint8_t trojan_[];
                uint8_t err_msg_size_;
                uint8_t game_id_;
                uint8_t bot_id_;
                char err_msg_[];
                uint8_t client_passitive_;
                uint32_t client_rand_value_;
                uint8_t send_data_len_;
                uint8_t send_data_[];
                uint8_t rand_[];
                uint8_t local_check_flag_;
                char rc_check_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 651
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 653
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 654
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 656
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 657
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 660
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 661
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 662
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 663
            } TAB_CS_VERIFYRESULT;
            typedef struct {
                public:
                uint8_t stop_rand_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2901
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2903
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2904
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2906
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2907
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2910
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2911
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2912
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2913
            } TAB_SC_STOPLCHECK;
            typedef struct {
                public:
                uint8_t data_flag_;
                uint8_t re_send_reason_;
                uint32_t resend_data_info_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 545
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 547
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 548
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 550
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 551
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 554
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 555
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 556
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 557
            } TAB_CS_RESENDDATA;
            typedef struct {
                public:
                uint16_t data_proto_id_;
                uint8_t data_type_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2937
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2939
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2940
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2942
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2943
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2946
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2947
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2948
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2949
            } TAB_SC_RESENDDATA;
            typedef struct {
                public:
                int32_t crc_;
                uint16_t serial_;
                uint8_t end_flag_;
                uint16_t size_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 689
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 691
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 692
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 694
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 695
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 698
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 699
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 700
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 701
            } TAB_CS_P2PSTATINFO;
            typedef struct {
                public:
                uint8_t type_;
                uint8_t len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2974
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2976
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2977
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2979
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2980
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2983
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2984
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2985
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2986
            } TAB_SC_CFGDATA;
            typedef struct {
                public:
                uint8_t rand_data_[];
                uint8_t type_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3010
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3012
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3013
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3015
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3016
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3019
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3020
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3021
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3022
            } TAB_SC_REQCLIRESENDDATA;
            typedef struct {
                public:
                uint16_t client_info_msg_size_;
                uint8_t client_info_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 725
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 727
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 728
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 730
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 731
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 734
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 735
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 736
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 737
            } TAB_CS_CLIENTINFO;
            typedef struct {
                public:
                uint16_t tp_info_msg_size_;
                TAB_CS_TP_INFO_MSG tp_info_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 796
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 798
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 799
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 801
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 802
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 805
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 806
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 807
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 808
            } TAB_CS_TPINFO;
            typedef struct {
                public:
                uint32_t crc_;
                uint16_t len_;
                uint8_t scan_type_;
                uint8_t prompt_;
                uint8_t action_on_memory_;
                uint8_t action_on_file_;
                uint8_t scan_sub_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2829
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2831
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2832
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2834
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2835
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2838
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2839
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2840
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2841
            } TQD_SC_CFGDATA;
            typedef struct {
                public:
                uint32_t crc_;
                uint16_t len_;
                uint8_t hash_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2866
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2868
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2869
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2871
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2872
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2875
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2876
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2877
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2878
            } TQD_SC_CMDDATA;
            typedef struct {
                public:
                uint32_t crc_;
                uint16_t data_len_;
                uint16_t data_len_temp_;
                uint8_t data_temp_[];
                uint32_t uin_;
                uint16_t virus_cnt_;
                TQD_CS_VIRUSINFO virus_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 914
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 916
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 917
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 919
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 920
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 923
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 924
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 925
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 926
            } TQD_CS_SCANRESULT;
            typedef struct {
                public:
                uint32_t crc_;
                uint16_t data_len_;
                uint8_t mem_sign_ver_[];
                uint8_t file_sign_ver_[];
                uint8_t scan_engine_ver_[];
                uint8_t clean_engine_ver_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 836
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 838
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 839
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 841
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 842
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 845
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 846
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 847
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 848
            } TQD_CS_INITDATA;
            typedef struct {
                public:
                uint8_t type_;
                uint16_t len_;
                TROJAN_REPORT trojan_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1064
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1066
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1067
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1069
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1070
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1073
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1074
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1075
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1076
            } TSANLIX_CS_REPORT;
            typedef struct {
                public:
                uint16_t tenpary_data_len_;
                uint8_t tenpary_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3046
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3048
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3049
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3051
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3052
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3055
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3056
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3057
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3058
            } TAB_SC_TENPRTYDATA;
            typedef struct {
                public:
                uint16_t buf_len_;
                TFILE_HASH_INFO file_hash_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 4120
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4122
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4123
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4125
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4126
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4129
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4130
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4131
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4132
            } TAB_SC_FILE_HASH;
            typedef struct {
                public:
                uint8_t file_num_;
                FILE_HASH_VAL file_hash_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1136
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1138
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1139
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1141
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1142
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1145
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1146
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1147
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1148
            } TAB_CS_CLINETINFO;
            typedef struct {
                public:
                uint16_t buf_len_;
                TMEM_RECOVER mem_info_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 4310
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4312
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4313
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4315
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4316
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4319
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4320
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4321
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4322
            } TAB_SC_MEM_RECOVER;
            typedef struct {
                public:
                uint16_t ext_info_len_;
                uint8_t ext_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1172
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1174
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1175
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1177
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1178
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1181
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1182
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1183
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1184
            } TAB_CS_EXTINFO;
            typedef struct {
                public:
                uint16_t local_check_data_len_;
                uint8_t local_check_data_[];
                uint8_t num_msg_size_;
                char num_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1210
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1212
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1213
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1215
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1216
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1219
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1220
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1221
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1222
            } TAB_CS_LOCALCHECK;
            typedef struct {
                public:
                uint16_t num_info_size_;
                char num_info_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1246
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1248
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1249
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1251
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1252
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1255
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1256
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1257
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1258
            } TAB_CS_NUMINFO;
            typedef struct {
                public:
                uint8_t cmd_count_;
                uint16_t cmd_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3082
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3084
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3085
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3087
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3088
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3091
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3092
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3093
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3094
            } TAB_SC_DE_CMDID_LIST;
            typedef struct {
                public:
                uint8_t id_;
                uint8_t dib_;
                uint16_t dib_data_len_;
                uint8_t dib_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3120
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3122
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3123
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3125
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3126
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3129
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3130
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3131
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3132
            } TAB_SC_DEDIBDATA;
            typedef struct {
                public:
                uint8_t pad_data_size_;
                char pad_data_[];
                uint32_t key_crc_;
                uint8_t key_len_;
                uint32_t key_offset_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3159
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3161
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3162
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3164
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3165
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3168
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3169
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3170
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3171
            } TAB_SC_DEKEY;
            typedef struct {
                public:
                uint8_t id_;
                uint32_t key_crc_;
                uint8_t err_msg_size_;
                char err_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1284
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1286
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1287
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1289
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1290
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1293
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1294
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1295
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1296
            } TAB_CS_DEREADY;
            typedef struct {
                public:
                uint32_t test_pkg_crc_;
                uint8_t content_size_;
                char content_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1321
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1323
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1324
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1326
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1327
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1330
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1331
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1332
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1333
            } TAB_CS_DETESTPKG;
            typedef struct {
                public:
                uint32_t test_pkg_crc_;
                uint8_t content_size_;
                char content_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3196
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3198
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3199
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3201
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3202
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3205
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3206
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3207
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3208
            } TAB_SC_TESTPKG;
            typedef struct {
                public:
                uint8_t error_id_;
                uint32_t error_;
                uint8_t pad_size_;
                char buf_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1359
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1361
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1362
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1364
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1365
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1368
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1369
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1370
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1371
            } TAB_CS_DELOG;
            typedef struct {
                public:
                uint32_t test_pkg_crc_;
                uint16_t pkg_size_;
                uint8_t pkg_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3233
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3235
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3236
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3238
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3239
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3242
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3243
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3244
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3245
            } TAB_SC_ENCRYPT;
            typedef struct {
                public:
                uint32_t test_pkg_crc_;
                uint16_t pkg_len_;
                uint8_t pkg_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1396
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1398
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1399
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1401
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1402
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1405
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1406
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1407
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1408
            } TAB_CS_ENCRYPT;
            typedef struct {
                public:
                uint16_t snd_proto_id_;
                uint32_t snd_size_;
                uint8_t err_info_size_;
                char err_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1434
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1436
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1437
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1439
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1440
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1443
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1444
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1445
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1446
            } TAB_CS_ERRPACKET;
            typedef struct {
                public:
                uint16_t system_error_len_;
                uint8_t system_error_data_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1472
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1474
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1475
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1477
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1478
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1481
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1482
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1483
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1484
            } TAB_CS_SYSMEM;
            typedef struct {
                public:
                uint8_t system_log_size_;
                char system_log_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1510
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1512
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1513
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1515
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1516
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1519
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1520
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1521
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1522
            } TAB_CS_SYSMEMLOG;
            typedef struct {
                public:
                uint32_t ban_time_;
                uint8_t pad_data_size_;
                char pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3270
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3272
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3273
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3275
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3276
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3279
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3280
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3281
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3282
            } TAB_SC_RESTRICTUIN;
            typedef struct {
                public:
                uint8_t pad_len_;
                uint8_t pad_[];
                uint8_t iat_check_result_;
                uint8_t iat_info_size_;
                char iat_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1549
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1551
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1552
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1554
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1555
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1558
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1559
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1560
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1561
            } TAB_CS_IATCHECK;
            typedef struct {
                public:
                uint8_t encrypt_key_[];
                uint16_t rpcs_data_len_;
                uint8_t rpcs_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3307
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3309
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3310
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3312
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3313
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3316
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3317
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3318
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3319
            } TAB_SC_CSCFGDATA;
            typedef struct {
                public:
                uint16_t rpcs_result_len_;
                uint8_t rpcs_result_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1585
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1587
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1588
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1590
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1591
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1594
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1595
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1596
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1597
            } TAB_CS_CSCHECK;
            typedef struct {
                public:
                uint16_t sus_addr_data_len_;
                uint8_t sus_addr_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3343
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3345
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3346
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3348
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3349
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3352
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3353
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3354
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3355
            } TAB_SC_SUSMODQUEST;
            typedef struct {
                public:
                uint16_t sus_mod_info_len_;
                uint8_t sus_mod_info_data_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1623
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1625
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1626
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1628
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1629
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1632
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1633
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1634
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1635
            } TAB_CS_SUSMODINFO;
            typedef struct {
                public:
                uint32_t key_crc_;
                uint32_t main_code_offset_;
                uint32_t main_code_size_;
                uint8_t tv_flag_;
                uint32_t thread_num_;
                /*anon struct*/ int thread_verify_infos_[];
                uint8_t pad_size_;
                uint8_t pad_buf_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 314
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 316
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 317
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 319
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 320
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 323
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 324
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 325
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 326
            } TAB_CS_TVINFO;
            typedef struct {
                public:
                uint32_t tv_gap_;
                uint8_t tv_info_key_[];
                uint8_t tv_check_flag_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3380
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3382
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3383
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3385
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3386
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3389
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3390
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3391
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3392
            } TAB_SC_TVCHECK;
            typedef struct {
                public:
                uint16_t slx_data_len_;
                uint8_t slx_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3416
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3418
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3419
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3421
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3422
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3425
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3426
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3427
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3428
            } TAB_SC_SLXSHAREINFO;
            typedef struct {
                public:
                uint16_t scheme_id_;
                uint16_t sub_protocol_id_;
                SC_SCHEMEDATA data_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3605
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3607
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3608
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3610
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3611
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3614
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3615
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3616
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3617
            } TAB_SC_SCHEMEDATA;
            typedef struct {
                public:
                uint16_t scheme_id_;
                uint16_t sub_protocol_id_;
                SAB_CS_SLN_DATA data_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2520
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2522
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2523
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2525
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2526
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2529
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2530
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2531
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2532
            } TAB_CS_SCHEMEDATA;
            typedef struct {
                public:
                uint8_t clr_flag_;
                uint16_t rpfmc_scan_cmd_len_;
                uint8_t rpfmc_scan_cmd_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3789
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3791
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3792
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3794
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3795
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3798
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3799
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3800
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3801
            } TAB_SC_RPFMCSCANCMD;
            typedef struct {
                public:
                uint32_t hash_;
                uint16_t rpfmc_scan_result_len_;
                uint8_t rpfmc_scan_result_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 508
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 510
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 511
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 513
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 514
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 517
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 518
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 519
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 520
            } TAB_CS_RPFMCSCANRESULT;
            typedef struct {
                public:
                uint16_t la_cmd_size_;
                uint8_t la_cmd_data_[];
                uint8_t pad_data_size_;
                char pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3862
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3864
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3865
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3867
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3868
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3871
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3872
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3873
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3874
            } TAB_SC_LAPKGCMD;
            typedef struct {
                public:
                uint32_t hash_;
                uint16_t la_result_len_;
                uint8_t aucLaResultData[];
                uint32_t serial_;
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 471
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 473
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 474
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 476
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 477
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 480
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 481
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 482
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 483
            } TAB_CS_LAPKGRESULT;
            typedef struct {
                public:
                uint16_t dumb_;
                uint16_t fms_scn_cmd_len_;
                uint8_t fms_scn_cmd_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3899
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3901
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3902
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3904
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3905
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3908
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3909
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3910
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3911
            } TAB_SC_FMSCMD;
            typedef struct {
                public:
                uint16_t fms_result_len_;
                char fms_result_data_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 431
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 433
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 434
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 436
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 437
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 440
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 441
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 442
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 443
            } TAB_CS_FMCRESULT;
            typedef struct {
                public:
                uint32_t anti_scan_gap_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3824
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3826
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3827
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3829
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3830
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3833
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3834
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3835
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3836
            } TAB_SC_ANTICFGINFO;
            typedef struct {
                public:
                uint32_t uin_;
                uint32_t time_stamp_;
                uint32_t seq_;
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 393
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 395
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 396
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 398
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 399
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 402
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 403
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 404
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 405
            } TAB_CS_ANTITIME;
            typedef struct {
                public:
                uint16_t flag_;
                uint32_t uin_;
                uint32_t ip_;
                uint8_t signature_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 354
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 356
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 357
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 359
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 360
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 363
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 364
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 365
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 366
            } TAB_CS_ANTICLTKEY;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2599
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2601
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2602
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2604
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2605
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2608
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2609
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2610
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2611
            } TAB_CS_CAPTCHARESULT;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2635
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2637
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2638
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2640
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2641
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2644
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2645
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2646
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2647
            } TAB_CS_CAPTCHAREQ;
            typedef struct {
                public:
                uint16_t error_func_id_;
                uint8_t error_type_;
                uint32_t error_index_;
                uint32_t error_code_;
                uint32_t eip_;
                uint8_t error_env_[];
                uint32_t reserved1_;
                uint32_t reserved2_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2677
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2679
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2680
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2682
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2683
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2686
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2687
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2688
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2689
            } AB_CS_REPORT_ERROR;
            typedef struct {
                public:
                uint16_t rp_id_;
                uint16_t rpcode_data_size_;
                uint8_t rpcode_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3642
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3644
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3645
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3647
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3648
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3651
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3652
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3653
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3654
            } TAB_SC_PKGCODE;
            typedef struct {
                public:
                uint16_t rpcode_result_data_size_;
                uint32_t rpcode_data_hash_;
                uint8_t rpcode_result_data_[];
                uint8_t rpcode_func_id_;
                uint32_t serial_;
                uint16_t hash_idx_;
                uint32_t hash_;
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2563
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2565
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2566
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2568
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2569
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2572
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2573
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2574
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2575
            } TAB_CS_RPPKGRESULT;
            typedef struct {
                public:
                uint8_t key_info_len_;
                uint8_t key_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3678
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3680
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3681
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3683
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3684
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3687
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3688
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3689
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3690
            } TAB_SC_PKGCODE_KEY;
            typedef struct {
                public:
                uint16_t channel_id_;
                uint16_t world_id_;
                uint32_t service_id_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3752
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3754
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3755
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3757
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3758
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3761
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3762
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3763
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3764
            } TAB_SC_CHANNEL_SERVER_ID;
            typedef struct {
                public:
                uint32_t error_code_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2712
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2714
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2715
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2717
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2718
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2721
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2722
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2723
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2724
            } TAB_CS_CSCHANNEL_RESULT;
            typedef struct {
                public:
                uint64_t secsvr_down_time_;
                uint64_t sdk_down_time_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3973
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3975
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3976
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3978
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3979
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3982
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3983
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3984
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3985
            } TAB_SC_DELAY_CHECK;
            typedef struct {
                public:
                uint64_t secsvr_down_time_;
                uint64_t sdk_down_time_;
                uint64_t sdk_up_time_;
                uint32_t client_up_time_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3937
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3939
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3940
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3942
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3943
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3946
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3947
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3948
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3949
            } TAB_CS_DELAY_CHECK;
        public:
        sec_proto::AntibotPkgBody::._323::TAB_SC_SENDANTIBOT sc_send_antibot_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_RANDCHECK sc_rand_check_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_INITINFO cs_init_info_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_VERIFYRESULT cs_verify_result_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_STOPLCHECK sc_stop_l_check_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_RESENDDATA cs_resend_data_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_RESENDDATA sc_resend_data_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_P2PSTATINFO cs_p2p_stat_info_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_CFGDATA sc_cfg_data_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_REQCLIRESENDDATA sc_req_cli_resend_data_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_CLIENTINFO cs_client_info_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_TPINFO cs_tp_info_;
        sec_proto::AntibotPkgBody::._323::TQD_SC_CFGDATA sc_qd_cfg_data_;
        sec_proto::AntibotPkgBody::._323::TQD_SC_CMDDATA sc_qd_cmd_data_;
        sec_proto::AntibotPkgBody::._323::TQD_CS_SCANRESULT cs_qd_scan_result_;
        sec_proto::AntibotPkgBody::._323::TQD_CS_INITDATA cs_qd_init_data_;
        sec_proto::AntibotPkgBody::._323::TSANLIX_CS_REPORT cs_sanlix_report_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_TENPRTYDATA sc_tenparty_data_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_FILE_HASH sc_file_hash_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_CLINETINFO cs_cli_net_info_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_MEM_RECOVER sc_mem_recover_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_EXTINFO cs_ext_info_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_LOCALCHECK cs_local_check_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_NUMINFO cs_num_info_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_DE_CMDID_LIST sc_de_cmd_id_list_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_DEDIBDATA sc_de_dib_data_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_DEKEY sc_de_key_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_DEREADY cs_de_ready_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_DETESTPKG cs_de_test_pkg_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_TESTPKG sc_test_pkg_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_DELOG cs_de_log_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_ENCRYPT sc_encrypt_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_ENCRYPT cs_encrypt_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_ERRPACKET cs_err_packet_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_SYSMEM cs_sys_mem_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_SYSMEMLOG cs_sys_mem_log_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_RESTRICTUIN sc_restrict_uin_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_IATCHECK cs_iat_check_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_CSCFGDATA sc_cs_cfg_data_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_CSCHECK cs_cs_check_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_SUSMODQUEST sc_susmod_quest_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_SUSMODINFO cs_sus_mod_info_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_TVINFO cs_tv_info_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_TVCHECK sc_tv_check_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_SLXSHAREINFO sc_slx_share_info_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_SCHEMEDATA sc_scheme_data_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_SCHEMEDATA cs_scheme_data_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_SLXSHAREINFO sc_slx_te_share_info_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_RPFMCSCANCMD sc_rpfmc_scan_cmd_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_RPFMCSCANRESULT cs_rp_fmc_scan_result_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_LAPKGCMD sc_la_pkg_cmd_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_LAPKGRESULT cs_la_pkg_result_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_FMSCMD sc_fms_cmd_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_FMCRESULT cs_fmc_result_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_ANTICFGINFO sc_anti_cfg_info_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_ANTITIME cs_anti_time_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_ANTICLTKEY cs_anti_clt_key_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_CAPTCHARESULT cs_captcha_result_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_CAPTCHAREQ cs_captcha_req_;
        // anonymous struct, sizeof = 32
        struct {
            public:
            uint8_t guid_[];
            uint8_t reserve_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1659
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1661
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1662
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1664
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1665
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1668
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1669
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1670
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1671
        } cs_report_client_guid_;
        sec_proto::AntibotPkgBody::._323::AB_CS_REPORT_ERROR cs_report_error_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_PKGCODE sc_pkg_code_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_RPPKGRESULT cs_rp_pkg_result_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_PKGCODE_KEY sc_pkg_code_key_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_CHANNEL_SERVER_ID sc_channel_server_id_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_CSCHANNEL_RESULT cs_cschannel_result_;
        sec_proto::AntibotPkgBody::._323::TAB_SC_DELAY_CHECK sc_channel_delay_check_;
        sec_proto::AntibotPkgBody::._323::TAB_CS_DELAY_CHECK cs_channel_delay_check_;
        };
        namespace ._323 {
            typedef struct {
                public:
                uint32_t init_crc_value_;
                uint8_t send_ver_[];
                uint8_t data_flag_;
                uint16_t rcid_;
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2752
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2754
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2755
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2757
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2758
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2761
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2762
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2763
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2764
            } TAB_SC_SENDANTIBOT;
            typedef struct {
                public:
                uint8_t rand_data_[];
                uint8_t type_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2788
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2790
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2791
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2793
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2794
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2797
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2798
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2799
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2800
            } TAB_SC_RANDCHECK;
            typedef struct {
                public:
                uint8_t is_init_;
                uint8_t sig_flag_;
                uint8_t rccp_ver_[];
                uint32_t module_handle_;
                uint32_t lib_base_;
                uint8_t os_ver_;
                uint8_t st_ver_[];
                uint32_t proc_address_;
                uint8_t sf_ver_[];
                uint16_t rc_id_;
                uint32_t client_ver_;
                uint8_t user_id_[];
                uint8_t admin_;
                uint8_t dib_id_;
                uint32_t key_offset_;
                uint8_t key_len_;
                uint8_t rp_data_info_size_;
                char rp_data_info_[];
                uint8_t cs_data_;
                uint8_t len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 600
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 602
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 603
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 605
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 606
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 609
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 610
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 611
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 612
            } TAB_CS_INITINFO;
            typedef struct {
                public:
                uint16_t ext_size_;
                uint32_t uin_;
                uint8_t stamp_[];
                uint8_t module_[];
                uint8_t out_proc_[];
                uint8_t trojan_[];
                uint8_t err_msg_size_;
                uint8_t game_id_;
                uint8_t bot_id_;
                char err_msg_[];
                uint8_t client_passitive_;
                uint32_t client_rand_value_;
                uint8_t send_data_len_;
                uint8_t send_data_[];
                uint8_t rand_[];
                uint8_t local_check_flag_;
                char rc_check_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 651
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 653
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 654
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 656
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 657
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 660
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 661
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 662
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 663
            } TAB_CS_VERIFYRESULT;
            typedef struct {
                public:
                uint8_t stop_rand_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2901
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2903
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2904
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2906
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2907
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2910
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2911
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2912
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2913
            } TAB_SC_STOPLCHECK;
            typedef struct {
                public:
                uint8_t data_flag_;
                uint8_t re_send_reason_;
                uint32_t resend_data_info_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 545
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 547
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 548
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 550
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 551
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 554
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 555
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 556
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 557
            } TAB_CS_RESENDDATA;
            typedef struct {
                public:
                uint16_t data_proto_id_;
                uint8_t data_type_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2937
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2939
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2940
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2942
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2943
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2946
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2947
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2948
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2949
            } TAB_SC_RESENDDATA;
            typedef struct {
                public:
                int32_t crc_;
                uint16_t serial_;
                uint8_t end_flag_;
                uint16_t size_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 689
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 691
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 692
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 694
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 695
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 698
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 699
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 700
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 701
            } TAB_CS_P2PSTATINFO;
            typedef struct {
                public:
                uint8_t type_;
                uint8_t len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2974
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2976
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2977
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2979
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2980
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2983
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2984
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2985
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2986
            } TAB_SC_CFGDATA;
            typedef struct {
                public:
                uint8_t rand_data_[];
                uint8_t type_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3010
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3012
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3013
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3015
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3016
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3019
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3020
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3021
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3022
            } TAB_SC_REQCLIRESENDDATA;
            typedef struct {
                public:
                uint16_t client_info_msg_size_;
                uint8_t client_info_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 725
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 727
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 728
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 730
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 731
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 734
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 735
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 736
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 737
            } TAB_CS_CLIENTINFO;
            typedef struct {
                public:
                uint16_t tp_info_msg_size_;
                TAB_CS_TP_INFO_MSG tp_info_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 796
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 798
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 799
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 801
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 802
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 805
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 806
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 807
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 808
            } TAB_CS_TPINFO;
            typedef struct {
                public:
                uint32_t crc_;
                uint16_t len_;
                uint8_t scan_type_;
                uint8_t prompt_;
                uint8_t action_on_memory_;
                uint8_t action_on_file_;
                uint8_t scan_sub_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2829
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2831
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2832
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2834
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2835
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2838
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2839
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2840
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2841
            } TQD_SC_CFGDATA;
            typedef struct {
                public:
                uint32_t crc_;
                uint16_t len_;
                uint8_t hash_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2866
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2868
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2869
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2871
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2872
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2875
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2876
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2877
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2878
            } TQD_SC_CMDDATA;
            typedef struct {
                public:
                uint32_t crc_;
                uint16_t data_len_;
                uint16_t data_len_temp_;
                uint8_t data_temp_[];
                uint32_t uin_;
                uint16_t virus_cnt_;
                TQD_CS_VIRUSINFO virus_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 914
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 916
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 917
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 919
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 920
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 923
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 924
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 925
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 926
            } TQD_CS_SCANRESULT;
            typedef struct {
                public:
                uint32_t crc_;
                uint16_t data_len_;
                uint8_t mem_sign_ver_[];
                uint8_t file_sign_ver_[];
                uint8_t scan_engine_ver_[];
                uint8_t clean_engine_ver_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 836
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 838
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 839
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 841
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 842
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 845
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 846
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 847
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 848
            } TQD_CS_INITDATA;
            typedef struct {
                public:
                uint8_t type_;
                uint16_t len_;
                TROJAN_REPORT trojan_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1064
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1066
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1067
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1069
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1070
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1073
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1074
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1075
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1076
            } TSANLIX_CS_REPORT;
            typedef struct {
                public:
                uint16_t tenpary_data_len_;
                uint8_t tenpary_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3046
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3048
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3049
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3051
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3052
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3055
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3056
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3057
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3058
            } TAB_SC_TENPRTYDATA;
            typedef struct {
                public:
                uint16_t buf_len_;
                TFILE_HASH_INFO file_hash_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 4120
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4122
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4123
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4125
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4126
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4129
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4130
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4131
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4132
            } TAB_SC_FILE_HASH;
            typedef struct {
                public:
                uint8_t file_num_;
                FILE_HASH_VAL file_hash_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1136
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1138
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1139
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1141
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1142
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1145
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1146
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1147
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1148
            } TAB_CS_CLINETINFO;
            typedef struct {
                public:
                uint16_t buf_len_;
                TMEM_RECOVER mem_info_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 4310
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4312
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4313
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4315
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4316
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4319
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4320
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4321
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4322
            } TAB_SC_MEM_RECOVER;
            typedef struct {
                public:
                uint16_t ext_info_len_;
                uint8_t ext_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1172
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1174
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1175
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1177
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1178
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1181
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1182
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1183
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1184
            } TAB_CS_EXTINFO;
            typedef struct {
                public:
                uint16_t local_check_data_len_;
                uint8_t local_check_data_[];
                uint8_t num_msg_size_;
                char num_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1210
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1212
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1213
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1215
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1216
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1219
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1220
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1221
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1222
            } TAB_CS_LOCALCHECK;
            typedef struct {
                public:
                uint16_t num_info_size_;
                char num_info_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1246
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1248
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1249
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1251
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1252
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1255
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1256
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1257
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1258
            } TAB_CS_NUMINFO;
            typedef struct {
                public:
                uint8_t cmd_count_;
                uint16_t cmd_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3082
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3084
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3085
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3087
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3088
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3091
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3092
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3093
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3094
            } TAB_SC_DE_CMDID_LIST;
            typedef struct {
                public:
                uint8_t id_;
                uint8_t dib_;
                uint16_t dib_data_len_;
                uint8_t dib_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3120
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3122
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3123
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3125
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3126
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3129
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3130
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3131
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3132
            } TAB_SC_DEDIBDATA;
            typedef struct {
                public:
                uint8_t pad_data_size_;
                char pad_data_[];
                uint32_t key_crc_;
                uint8_t key_len_;
                uint32_t key_offset_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3159
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3161
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3162
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3164
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3165
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3168
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3169
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3170
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3171
            } TAB_SC_DEKEY;
            typedef struct {
                public:
                uint8_t id_;
                uint32_t key_crc_;
                uint8_t err_msg_size_;
                char err_msg_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1284
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1286
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1287
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1289
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1290
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1293
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1294
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1295
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1296
            } TAB_CS_DEREADY;
            typedef struct {
                public:
                uint32_t test_pkg_crc_;
                uint8_t content_size_;
                char content_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1321
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1323
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1324
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1326
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1327
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1330
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1331
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1332
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1333
            } TAB_CS_DETESTPKG;
            typedef struct {
                public:
                uint32_t test_pkg_crc_;
                uint8_t content_size_;
                char content_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3196
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3198
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3199
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3201
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3202
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3205
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3206
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3207
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3208
            } TAB_SC_TESTPKG;
            typedef struct {
                public:
                uint8_t error_id_;
                uint32_t error_;
                uint8_t pad_size_;
                char buf_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1359
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1361
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1362
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1364
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1365
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1368
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1369
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1370
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1371
            } TAB_CS_DELOG;
            typedef struct {
                public:
                uint32_t test_pkg_crc_;
                uint16_t pkg_size_;
                uint8_t pkg_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3233
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3235
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3236
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3238
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3239
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3242
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3243
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3244
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3245
            } TAB_SC_ENCRYPT;
            typedef struct {
                public:
                uint32_t test_pkg_crc_;
                uint16_t pkg_len_;
                uint8_t pkg_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1396
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1398
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1399
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1401
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1402
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1405
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1406
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1407
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1408
            } TAB_CS_ENCRYPT;
            typedef struct {
                public:
                uint16_t snd_proto_id_;
                uint32_t snd_size_;
                uint8_t err_info_size_;
                char err_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1434
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1436
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1437
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1439
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1440
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1443
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1444
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1445
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1446
            } TAB_CS_ERRPACKET;
            typedef struct {
                public:
                uint16_t system_error_len_;
                uint8_t system_error_data_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1472
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1474
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1475
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1477
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1478
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1481
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1482
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1483
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1484
            } TAB_CS_SYSMEM;
            typedef struct {
                public:
                uint8_t system_log_size_;
                char system_log_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1510
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1512
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1513
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1515
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1516
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1519
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1520
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1521
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1522
            } TAB_CS_SYSMEMLOG;
            typedef struct {
                public:
                uint32_t ban_time_;
                uint8_t pad_data_size_;
                char pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3270
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3272
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3273
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3275
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3276
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3279
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3280
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3281
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3282
            } TAB_SC_RESTRICTUIN;
            typedef struct {
                public:
                uint8_t pad_len_;
                uint8_t pad_[];
                uint8_t iat_check_result_;
                uint8_t iat_info_size_;
                char iat_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1549
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1551
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1552
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1554
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1555
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1558
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1559
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1560
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1561
            } TAB_CS_IATCHECK;
            typedef struct {
                public:
                uint8_t encrypt_key_[];
                uint16_t rpcs_data_len_;
                uint8_t rpcs_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3307
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3309
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3310
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3312
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3313
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3316
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3317
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3318
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3319
            } TAB_SC_CSCFGDATA;
            typedef struct {
                public:
                uint16_t rpcs_result_len_;
                uint8_t rpcs_result_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1585
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1587
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1588
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1590
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1591
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1594
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1595
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1596
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1597
            } TAB_CS_CSCHECK;
            typedef struct {
                public:
                uint16_t sus_addr_data_len_;
                uint8_t sus_addr_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3343
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3345
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3346
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3348
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3349
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3352
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3353
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3354
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3355
            } TAB_SC_SUSMODQUEST;
            typedef struct {
                public:
                uint16_t sus_mod_info_len_;
                uint8_t sus_mod_info_data_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1623
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1625
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1626
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1628
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1629
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1632
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1633
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1634
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1635
            } TAB_CS_SUSMODINFO;
            typedef struct {
                public:
                uint32_t key_crc_;
                uint32_t main_code_offset_;
                uint32_t main_code_size_;
                uint8_t tv_flag_;
                uint32_t thread_num_;
                /*anon struct*/ int thread_verify_infos_[];
                uint8_t pad_size_;
                uint8_t pad_buf_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 314
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 316
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 317
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 319
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 320
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 323
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 324
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 325
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 326
            } TAB_CS_TVINFO;
            typedef struct {
                public:
                uint32_t tv_gap_;
                uint8_t tv_info_key_[];
                uint8_t tv_check_flag_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3380
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3382
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3383
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3385
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3386
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3389
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3390
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3391
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3392
            } TAB_SC_TVCHECK;
            typedef struct {
                public:
                uint16_t slx_data_len_;
                uint8_t slx_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3416
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3418
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3419
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3421
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3422
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3425
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3426
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3427
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3428
            } TAB_SC_SLXSHAREINFO;
            typedef struct {
                public:
                uint16_t scheme_id_;
                uint16_t sub_protocol_id_;
                SC_SCHEMEDATA data_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3605
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3607
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3608
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3610
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3611
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3614
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3615
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3616
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3617
            } TAB_SC_SCHEMEDATA;
            typedef struct {
                public:
                uint16_t scheme_id_;
                uint16_t sub_protocol_id_;
                SAB_CS_SLN_DATA data_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2520
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2522
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2523
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2525
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2526
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2529
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2530
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2531
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2532
            } TAB_CS_SCHEMEDATA;
            typedef struct {
                public:
                uint8_t clr_flag_;
                uint16_t rpfmc_scan_cmd_len_;
                uint8_t rpfmc_scan_cmd_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3789
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3791
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3792
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3794
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3795
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3798
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3799
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3800
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3801
            } TAB_SC_RPFMCSCANCMD;
            typedef struct {
                public:
                uint32_t hash_;
                uint16_t rpfmc_scan_result_len_;
                uint8_t rpfmc_scan_result_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 508
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 510
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 511
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 513
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 514
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 517
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 518
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 519
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 520
            } TAB_CS_RPFMCSCANRESULT;
            typedef struct {
                public:
                uint16_t la_cmd_size_;
                uint8_t la_cmd_data_[];
                uint8_t pad_data_size_;
                char pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3862
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3864
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3865
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3867
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3868
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3871
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3872
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3873
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3874
            } TAB_SC_LAPKGCMD;
            typedef struct {
                public:
                uint32_t hash_;
                uint16_t la_result_len_;
                uint8_t aucLaResultData[];
                uint32_t serial_;
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 471
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 473
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 474
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 476
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 477
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 480
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 481
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 482
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 483
            } TAB_CS_LAPKGRESULT;
            typedef struct {
                public:
                uint16_t dumb_;
                uint16_t fms_scn_cmd_len_;
                uint8_t fms_scn_cmd_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3899
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3901
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3902
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3904
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3905
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3908
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3909
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3910
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3911
            } TAB_SC_FMSCMD;
            typedef struct {
                public:
                uint16_t fms_result_len_;
                char fms_result_data_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 431
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 433
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 434
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 436
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 437
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 440
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 441
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 442
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 443
            } TAB_CS_FMCRESULT;
            typedef struct {
                public:
                uint32_t anti_scan_gap_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3824
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3826
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3827
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3829
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3830
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3833
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3834
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3835
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3836
            } TAB_SC_ANTICFGINFO;
            typedef struct {
                public:
                uint32_t uin_;
                uint32_t time_stamp_;
                uint32_t seq_;
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 393
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 395
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 396
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 398
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 399
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 402
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 403
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 404
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 405
            } TAB_CS_ANTITIME;
            typedef struct {
                public:
                uint16_t flag_;
                uint32_t uin_;
                uint32_t ip_;
                uint8_t signature_[];
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 354
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 356
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 357
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 359
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 360
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 363
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 364
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 365
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 366
            } TAB_CS_ANTICLTKEY;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2599
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2601
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2602
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2604
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2605
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2608
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2609
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2610
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2611
            } TAB_CS_CAPTCHARESULT;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2635
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2637
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2638
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2640
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2641
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2644
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2645
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2646
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2647
            } TAB_CS_CAPTCHAREQ;
            typedef struct {
                public:
                uint16_t error_func_id_;
                uint8_t error_type_;
                uint32_t error_index_;
                uint32_t error_code_;
                uint32_t eip_;
                uint8_t error_env_[];
                uint32_t reserved1_;
                uint32_t reserved2_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2677
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2679
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2680
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2682
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2683
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2686
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2687
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2688
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2689
            } AB_CS_REPORT_ERROR;
            typedef struct {
                public:
                uint16_t rp_id_;
                uint16_t rpcode_data_size_;
                uint8_t rpcode_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3642
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3644
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3645
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3647
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3648
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3651
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3652
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3653
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3654
            } TAB_SC_PKGCODE;
            typedef struct {
                public:
                uint16_t rpcode_result_data_size_;
                uint32_t rpcode_data_hash_;
                uint8_t rpcode_result_data_[];
                uint8_t rpcode_func_id_;
                uint32_t serial_;
                uint16_t hash_idx_;
                uint32_t hash_;
                uint8_t pad_len_;
                uint8_t pad_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2563
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2565
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2566
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2568
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2569
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2572
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2573
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2574
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2575
            } TAB_CS_RPPKGRESULT;
            typedef struct {
                public:
                uint8_t key_info_len_;
                uint8_t key_info_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3678
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3680
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3681
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3683
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3684
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3687
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3688
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3689
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3690
            } TAB_SC_PKGCODE_KEY;
            typedef struct {
                public:
                uint16_t channel_id_;
                uint16_t world_id_;
                uint32_t service_id_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3752
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3754
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3755
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3757
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3758
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3761
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3762
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3763
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3764
            } TAB_SC_CHANNEL_SERVER_ID;
            typedef struct {
                public:
                uint32_t error_code_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2712
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2714
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2715
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2717
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2718
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2721
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2722
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2723
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2724
            } TAB_CS_CSCHANNEL_RESULT;
            typedef struct {
                public:
                uint64_t secsvr_down_time_;
                uint64_t sdk_down_time_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3973
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3975
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3976
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3978
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3979
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3982
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3983
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3984
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3985
            } TAB_SC_DELAY_CHECK;
            typedef struct {
                public:
                uint64_t secsvr_down_time_;
                uint64_t sdk_down_time_;
                uint64_t sdk_up_time_;
                uint32_t client_up_time_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3937
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3939
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3940
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3942
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3943
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3946
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3947
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3948
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3949
            } TAB_CS_DELAY_CHECK;
        } // namespace ._323
    } // namespace AntibotPkgBody
    namespace SAB_CS_SLN_DATA {
        // sizeof = 32731
        union ._269 { // line 2458
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t error_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2039
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2041
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2042
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2044
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2045
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2048
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2049
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2050
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2051
            } SSE_ERROR_INFO;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1966
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1968
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1969
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1971
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1972
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1975
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1976
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1977
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1978
            } SCS_SLN_DATA_BUF;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t type_;
                uint32_t number_;
                uint32_t reserved_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2077
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2079
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2080
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2082
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2083
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2086
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2087
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2088
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2089
            } STP_ERROR_INFO;
            typedef struct {
                public:
                uint16_t total_len_;
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2003
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2005
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2006
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2008
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2009
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2012
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2013
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2014
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2015
            } STP_CUSTOM_STRING;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t ipcs_status_;
                uint8_t read_flag_;
                uint32_t read_status_;
                uint32_t qq_login_mac_crc_;
                uint32_t game_mac_crc_;
                uint32_t qq_login_uin_;
                uint32_t game_uin_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2119
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2121
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2122
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2124
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2125
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2128
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2129
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2130
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2131
            } SAB_MAC_INFO;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t mac_id_;
                uint32_t sig_size_;
                char sig_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2157
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2159
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2160
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2162
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2163
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2166
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2167
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2168
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2169
            } SAB_SIGN_INFO;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t eip_;
                uint32_t exception_code_;
                uint8_t except_code_array_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2195
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2197
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2198
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2200
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2201
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2204
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2205
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2206
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2207
            } SRP_THREAD_EXP;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t fun_id_;
                uint8_t pad_data_len_;
                uint8_t pad_[];
                uint8_t hash_count_;
                uint32_t hash_[];
                uint32_t times_;
                uint32_t send_func_addr_;
                uint32_t no_die_value_;
                uint8_t garbage_data_len_;
                uint8_t garbage_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2240
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2242
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2243
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2245
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2246
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2249
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2250
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2251
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2252
            } SRP_RPCODE_HASH;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t func_call_info_size_;
                /*anon struct*/ int func_call_infos_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2389
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2391
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2392
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2394
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2395
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2398
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2399
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2400
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2401
            } STVMP_REPORT_CALL_INFO;
            typedef struct {
                public:
                uint16_t send_data_len_;
                // anonymous struct, sizeof = 8
                struct {
                    public:
                    uint32_t addr_;
                    uint32_t hash_;
                    tsf4g_tdr::TdrError::ErrorType construct(); // line 2352
                    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2354
                    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2355
                    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2357
                    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2358
                    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2361
                    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2362
                    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2363
                    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2364
                } func_hash_info_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2425
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2427
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2428
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2430
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2431
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2434
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2435
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2436
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2437
            } STVMP_REPORT_HASH_INFO;
        public:
        sec_proto::SAB_CS_SLN_DATA::._269::SSE_ERROR_INFO se_error_info_;
        sec_proto::SAB_CS_SLN_DATA::._269::SCS_SLN_DATA_BUF se_data_;
        // anonymous struct, sizeof = 22
        struct {
            public:
            uint16_t data_len_;
            uint32_t verify_type_;
            uint32_t verify_start_ptr_;
            uint32_t verify_len_;
            uint32_t load_hash_;
            uint32_t verify_hash_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1850
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1852
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1853
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1855
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1856
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1859
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1860
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1861
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1862
        } shell_module_verify_;
        // anonymous struct, sizeof = 595
        struct {
            public:
            uint16_t data_len_;
            int32_t scan_result_;
            uint32_t sanlix_dat_version_low_;
            uint32_t sanlix_dat_version_high_;
            uint32_t sanlix_id_;
            uint8_t flag_;
            char virus_name_[];
            char scan_module_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1892
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1894
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1895
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1897
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1898
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1901
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1902
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1903
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1904
        } sanlix_scan_result_;
        sec_proto::SAB_CS_SLN_DATA::._269::STP_ERROR_INFO tp_error_info_;
        sec_proto::SAB_CS_SLN_DATA::._269::STP_CUSTOM_STRING tp_custom_string_;
        // anonymous struct, sizeof = 1299
        struct {
            public:
            uint16_t send_data_len_;
            int32_t data_source_;
            int32_t game_id_;
            uint32_t uin_;
            uint32_t log_type_;
            uint8_t int_count_;
            int32_t int_values_[];
            char comment_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1737
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1739
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1740
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1742
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1743
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1746
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1747
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1748
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1749
        } tp_bin_log_report_;
        // anonymous struct, sizeof = 34
        struct {
            public:
            uint16_t send_data_len_;
            // anonymous struct, sizeof = 32
            struct {
                public:
                uint8_t guid_[];
                uint8_t reserve_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1659
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1661
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1662
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1664
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1665
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1668
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1669
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1670
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1671
            } guid_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1695
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1697
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1698
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1700
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1701
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1704
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1705
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1706
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1707
        } client_guid_info_;
        // anonymous struct, sizeof = 14
        struct {
            public:
            uint16_t send_data_len_;
            uint32_t major_code_;
            uint32_t minor_code_;
            int32_t status_code_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1775
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1777
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1778
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1780
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1781
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1784
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1785
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1786
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1787
        } tp_kr_report_info_;
        // anonymous struct, sizeof = 1024
        struct {
            public:
            char info_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1810
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1812
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1813
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1815
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1816
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1819
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1820
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1821
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1822
        } tp_apc_report_info_;
        sec_proto::SAB_CS_SLN_DATA::._269::SCS_SLN_DATA_BUF ab_auth_code_;
        sec_proto::SAB_CS_SLN_DATA::._269::SAB_MAC_INFO ab_mac_info_;
        sec_proto::SAB_CS_SLN_DATA::._269::SAB_SIGN_INFO ab_sign_info_;
        sec_proto::SAB_CS_SLN_DATA::._269::SCS_SLN_DATA_BUF ab_exp_data_;
        sec_proto::SAB_CS_SLN_DATA::._269::SCS_SLN_DATA_BUF rp_msg_result_;
        sec_proto::SAB_CS_SLN_DATA::._269::SRP_THREAD_EXP rp_thread_exp_;
        sec_proto::SAB_CS_SLN_DATA::._269::SCS_SLN_DATA_BUF rp_func_exp_;
        sec_proto::SAB_CS_SLN_DATA::._269::SRP_RPCODE_HASH rp_rpcode_hash_;
        sec_proto::SAB_CS_SLN_DATA::._269::STVMP_REPORT_CALL_INFO tvmp_report_call_info_;
        sec_proto::SAB_CS_SLN_DATA::._269::STVMP_REPORT_HASH_INFO tvmp_report_hash_info_;
        };
        namespace ._269 {
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t error_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2039
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2041
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2042
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2044
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2045
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2048
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2049
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2050
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2051
            } SSE_ERROR_INFO;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 1966
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1968
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1969
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1971
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1972
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1975
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1976
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1977
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1978
            } SCS_SLN_DATA_BUF;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t type_;
                uint32_t number_;
                uint32_t reserved_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2077
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2079
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2080
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2082
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2083
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2086
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2087
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2088
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2089
            } STP_ERROR_INFO;
            typedef struct {
                public:
                uint16_t total_len_;
                uint16_t send_data_len_;
                uint8_t send_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2003
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2005
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2006
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2008
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2009
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2012
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2013
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2014
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2015
            } STP_CUSTOM_STRING;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t ipcs_status_;
                uint8_t read_flag_;
                uint32_t read_status_;
                uint32_t qq_login_mac_crc_;
                uint32_t game_mac_crc_;
                uint32_t qq_login_uin_;
                uint32_t game_uin_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2119
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2121
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2122
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2124
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2125
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2128
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2129
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2130
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2131
            } SAB_MAC_INFO;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t mac_id_;
                uint32_t sig_size_;
                char sig_data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2157
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2159
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2160
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2162
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2163
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2166
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2167
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2168
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2169
            } SAB_SIGN_INFO;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t eip_;
                uint32_t exception_code_;
                uint8_t except_code_array_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2195
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2197
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2198
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2200
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2201
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2204
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2205
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2206
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2207
            } SRP_THREAD_EXP;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint8_t fun_id_;
                uint8_t pad_data_len_;
                uint8_t pad_[];
                uint8_t hash_count_;
                uint32_t hash_[];
                uint32_t times_;
                uint32_t send_func_addr_;
                uint32_t no_die_value_;
                uint8_t garbage_data_len_;
                uint8_t garbage_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2240
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2242
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2243
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2245
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2246
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2249
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2250
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2251
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2252
            } SRP_RPCODE_HASH;
            typedef struct {
                public:
                uint16_t send_data_len_;
                uint32_t func_call_info_size_;
                /*anon struct*/ int func_call_infos_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2389
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2391
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2392
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2394
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2395
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2398
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2399
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2400
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2401
            } STVMP_REPORT_CALL_INFO;
            typedef struct {
                public:
                uint16_t send_data_len_;
                // anonymous struct, sizeof = 8
                struct {
                    public:
                    uint32_t addr_;
                    uint32_t hash_;
                    tsf4g_tdr::TdrError::ErrorType construct(); // line 2352
                    tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2354
                    tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2355
                    tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2357
                    tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2358
                    tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2361
                    tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2362
                    const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2363
                    const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2364
                } func_hash_info_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 2425
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 2427
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 2428
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 2430
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 2431
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2434
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2435
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 2436
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 2437
            } STVMP_REPORT_HASH_INFO;
        } // namespace ._269
    } // namespace SAB_CS_SLN_DATA
    namespace SC_SCHEMEDATA {
        // sizeof = 32729
        union ._299 { // line 3558
            typedef struct {
                public:
                uint16_t temp_data_len_;
                SSC_SE_SCAN_DATA_ITEM se_scan_data_item_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3489
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3491
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3492
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3494
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3495
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3498
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3499
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3500
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3501
            } SSC_SE_SCAN_DATA;
            typedef struct {
                public:
                uint16_t data_len_;
                uint8_t data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3525
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3527
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3528
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3530
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3531
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3534
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3535
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3536
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3537
            } SSC_SLN_DATA;
        public:
        sec_proto::SC_SCHEMEDATA::._299::SSC_SE_SCAN_DATA se_scan_data_;
        // anonymous struct, sizeof = 14
        struct {
            public:
            uint16_t data_len_;
            uint32_t sanlix_id_;
            uint32_t result_;
            uint32_t duration_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1930
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1932
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1933
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1935
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1936
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1939
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1940
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1941
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1942
        } sx_judge_result_;
        sec_proto::SC_SCHEMEDATA::._299::SSC_SLN_DATA tp_svr_config_;
        sec_proto::SC_SCHEMEDATA::._299::SSC_SLN_DATA rp_cp_data_;
        sec_proto::SC_SCHEMEDATA::._299::SSC_SLN_DATA sx_cfg_info_;
        };
        namespace ._299 {
            typedef struct {
                public:
                uint16_t temp_data_len_;
                SSC_SE_SCAN_DATA_ITEM se_scan_data_item_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3489
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3491
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3492
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3494
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3495
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3498
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3499
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3500
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3501
            } SSC_SE_SCAN_DATA;
            typedef struct {
                public:
                uint16_t data_len_;
                uint8_t data_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 3525
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 3527
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 3528
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 3530
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 3531
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3534
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3535
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 3536
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 3537
            } SSC_SLN_DATA;
        } // namespace ._299
    } // namespace SC_SCHEMEDATA
    namespace TORDINAL_OR_NAME {
        // sizeof = 257
        union ._317 { // line 4189
            typedef struct {
                public:
                uint8_t count_;
                uint32_t ordinal_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 4156
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4158
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4159
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4161
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4162
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4165
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4166
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4167
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4168
            } TORDINAL;
        public:
        TMODULE_NAME name_;
        sec_proto::TORDINAL_OR_NAME::._317::TORDINAL ordinal_;
        };
        namespace ._317 {
            typedef struct {
                public:
                uint8_t count_;
                uint32_t ordinal_;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 4156
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 4158
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 4159
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 4161
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 4162
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4165
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4166
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 4167
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 4168
            } TORDINAL;
        } // namespace ._317
    } // namespace TORDINAL_OR_NAME
    namespace TROJAN_REPORT {
        // sizeof = 16644
        union ._230 { // line 1020
            typedef struct {
                public:
                uint32_t trojan_count_;
                TROJANINFO trojans_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 987
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 989
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 990
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 992
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 993
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 996
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 997
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 998
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 999
            } SLX_TROJAN_INFO;
        public:
        sec_proto::TROJAN_REPORT::._230::SLX_TROJAN_INFO trojans_;
        uint32_t trojan_count_;
        };
        namespace ._230 {
            typedef struct {
                public:
                uint32_t trojan_count_;
                TROJANINFO trojans_[];
                tsf4g_tdr::TdrError::ErrorType construct(); // line 987
                tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 989
                tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 990
                tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 992
                tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 993
                tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 996
                tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 997
                const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 998
                const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 999
            } SLX_TROJAN_INFO;
        } // namespace ._230
    } // namespace TROJAN_REPORT
} // namespace sec_proto

#endif // SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_
