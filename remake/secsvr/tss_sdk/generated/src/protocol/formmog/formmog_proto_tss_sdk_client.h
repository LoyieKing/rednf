// Auto-generated header stub from DWARF info
// Original path: src/protocol/formmog/formmog_proto_tss_sdk_client.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_CLIENT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_CLIENT_H_H_

namespace sec_proto {
    namespace DPInterDataBody {
        // sizeof = 1580
        union ._310 { // line 1116
        public:
        // anonymous struct, sizeof = 4
        struct {
            public:
            uint32_t code_id_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1009
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1011
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1012
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1014
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1015
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1018
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1019
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1020
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1021
        } client_code_;
        // anonymous struct, sizeof = 8
        struct {
            public:
            uint32_t code_id_;
            uint32_t crc_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 866
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 868
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 869
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 871
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 872
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 875
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 876
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 877
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 878
        } dynamic_algorithm_data_;
        // anonymous struct, sizeof = 4
        struct {
            public:
            uint16_t length;
            uint16_t bin_code_id_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 939
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 941
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 942
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 944
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 945
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 948
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 949
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 950
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 951
        } bin_file_;
        // anonymous struct, sizeof = 1028
        struct {
            public:
            uint16_t up_cmd_count_;
            uint16_t down_cmd_count_;
            uint32_t up_cmd_list_[];
            uint32_t down_cmd_list_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1083
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1085
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1086
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1088
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1089
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1092
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1093
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1094
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1095
        } cmd_list;
        // anonymous struct, sizeof = 2
        struct {
            public:
            uint16_t bin_count_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 974
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 976
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 977
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 979
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 980
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 983
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 984
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 985
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 986
        } bin_count_;
        // anonymous struct, sizeof = 20
        struct {
            public:
            uint8_t client_version_[];
            uint32_t code_id_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 1045
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 1047
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 1048
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 1050
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 1051
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1054
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1055
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 1056
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 1057
        } server_code_;
        // anonymous struct, sizeof = 12
        struct {
            public:
            uint16_t size;
            uint32_t encrypt_code_id_;
            uint32_t decrypt_code_id_;
            uint16_t bin_file_id_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 520
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 522
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 523
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 525
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 526
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 529
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 530
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 531
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 532
        } request_code_;
        // anonymous struct, sizeof = 36
        struct {
            public:
            uint8_t code_ver_[];
            uint32_t client_info_;
            uint32_t client_user_id_;
            uint32_t client_code_id_;
            uint32_t server_code_id_;
            uint16_t bin_code_id_;
            uint16_t bin_count_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 561
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 563
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 564
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 566
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 567
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 570
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 571
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 572
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 573
        } init_data_ptc_;
        // anonymous struct, sizeof = 1580
        struct {
            public:
            uint32_t internal_msg_send_count_;
            uint32_t internal_msg_recv_count_;
            uint32_t game_pkg_recv_count_;
            uint32_t fix_encrypt_count_;
            uint32_t game_fix_decrypt_count_;
            uint32_t game_dynamic_decrypt_count_;
            uint32_t seq_less_count_;
            uint32_t seq_greater_count_;
            uint32_t seq_discard_count_;
            uint32_t down_cmd_list_crc_;
            uint16_t need_encrypt_count_;
            /*anon struct*/ int need_encrypt_list_[];
            uint16_t client_error_count_;
            /*anon struct*/ int client_error_list_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 719
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 721
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 722
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 724
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 725
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 728
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 729
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 730
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 731
        } client_stat_ptc_;
        // anonymous struct, sizeof = 8
        struct {
            public:
            uint32_t decrypt_id_;
            uint32_t encrypt_id_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 755
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 757
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 758
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 760
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 761
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 764
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 765
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 766
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 767
        } decrypt_info_;
        // anonymous struct, sizeof = 134
        struct {
            public:
            uint32_t error_id_;
            uint8_t is_encrypt_;
            uint8_t error_msg_len_;
            uint8_t error_msg_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 599
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 601
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 602
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 604
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 605
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 608
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 609
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 610
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 611
        } client_log_ptc_;
        uint8_t cs_test_data_[];
        uint8_t sc_test_data_[];
        // anonymous struct, sizeof = 12
        struct {
            public:
            uint32_t key_id_;
            uint32_t algorithm_type_;
            uint32_t crc;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 792
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 794
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 795
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 797
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 798
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 801
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 802
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 803
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 804
        } fix_algorithm_data_;
        // anonymous struct, sizeof = 3
        struct {
            public:
            uint8_t proto_check_switch_;
            uint8_t seq_check_switch_;
            uint8_t raw_check_switch_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 903
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 905
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 906
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 908
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 909
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 912
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 913
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 914
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 915
        } config_data_;
        };
    } // namespace DPInterDataBody
    namespace TClientFrameBody {
        // sizeof = 51210
        union ._290 { // line 403
        public:
        // anonymous struct, sizeof = 51210
        struct {
            public:
            uint16_t totalPiece;
            uint16_t pieceSerial;
            uint32_t totalCrc;
            uint16_t pieceDataSize;
            uint8_t pieceData[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 333
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 335
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 336
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 338
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 339
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 342
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 343
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 344
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 345
        } framePiece;
        };
    } // namespace TClientFrameBody
    namespace TClientPkgBody {
        // sizeof = 13
        union ._285 { // line 250
        public:
        // anonymous struct, sizeof = 8
        struct {
            public:
            uint32_t cmdID;
            uint32_t isHint;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 143
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 145
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 146
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 148
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 149
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 152
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 153
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 154
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 155
        } reviseMouse;
        // anonymous struct, sizeof = 13
        struct {
            public:
            uint16_t shPacketSize;
            uint16_t shPacketType;
            uint8_t chDataType;
            uint32_t cmdid;
            uint32_t bufLen;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 182
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 184
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 185
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 187
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 188
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 191
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 192
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 193
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 194
        } specialControl;
        // anonymous struct, sizeof = 4
        struct {
            public:
            uint32_t iSafemodeLevel;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 217
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 219
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 220
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 222
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 223
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 226
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 227
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 228
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 229
        } safeMode;
        };
    } // namespace TClientPkgBody
} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_CLIENT_H_H_
