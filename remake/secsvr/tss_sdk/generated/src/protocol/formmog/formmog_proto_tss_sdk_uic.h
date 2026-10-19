// Auto-generated header stub from DWARF info
// Original path: src/protocol/formmog/formmog_proto_tss_sdk_uic.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_UIC_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_UIC_H_H_

namespace sec_proto {
    enum UserInputCode {
        USER_INPUT_CODE_GBK = 0,
        USER_INPUT_CODE_UTF8 = 1,
        USER_INPUT_CODE_GBK = 0,
        USER_INPUT_CODE_UTF8 = 1,
    };
    namespace UserInputCheckPkgBody {
        // sizeof = 1169
        union ._330 { // line 177
        public:
        // anonymous struct, sizeof = 1169
        struct {
            public:
            uint32_t uin_;
            uint32_t session_id_;
            uint32_t session_seq_;
            int8_t user_input_code_;
            uint16_t user_input_data_len_;
            uint8_t user_input_data_[];
            uint16_t user_game_data_len_;
            uint8_t user_game_data_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 105
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 107
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 108
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 110
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 111
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 114
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 115
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 116
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 117
        } req_user_input_check_;
        // anonymous struct, sizeof = 16
        struct {
            public:
            uint32_t uin_;
            uint32_t session_id_;
            uint32_t session_seq_;
            uint32_t check_result_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 144
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 146
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 147
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 149
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 150
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 153
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 154
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 155
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 156
        } rsp_user_input_check_;
        };
    } // namespace UserInputCheckPkgBody
} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_TSS_SDK_UIC_H_H_
