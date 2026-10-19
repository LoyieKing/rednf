// Auto-generated header stub from DWARF info
// Original path: output/protocol/formmog/release/include/formmog_proto_captcha_client.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_CAPTCHA_CLIENT_H_H_
#define SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_CAPTCHA_CLIENT_H_H_

namespace sec_proto {
    namespace TSSCaptchaBody {
        // sizeof = 4625
        union ._264 { // line 264
        public:
        // anonymous struct, sizeof = 531
        struct {
            public:
            uint32_t client_ip_;
            uint32_t platform_busid_;
            uint16_t max_pic_change_num_;
            uint16_t max_verify_num_;
            uint32_t timeout_;
            uint8_t busi_id_;
            uint16_t busi_info_len_;
            int8_t busi_info_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 114
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 116
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 117
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 119
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 120
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 123
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 124
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 125
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 126
        } set_captcha_;
        // anonymous struct, sizeof = 4625
        struct {
            public:
            uint32_t timeout_;
            uint16_t verify_times_limit_;
            uint16_t verify_times_;
            uint16_t change_times_limit_;
            uint16_t change_times_;
            uint16_t picture_len_;
            uint8_t picture_[];
            uint8_t busi_id_;
            uint16_t busi_info_len_;
            int8_t busi_info_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 158
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 160
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 161
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 163
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 164
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 167
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 168
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 169
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 170
        } captcha_pic_;
        // anonymous struct, sizeof = 65
        struct {
            public:
            uint8_t answer_len_;
            uint8_t answer_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 194
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 196
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 197
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 199
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 200
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 203
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 204
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 205
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 206
        } verify_req_;
        // anonymous struct, sizeof = 130
        struct {
            public:
            uint8_t result_;
            uint8_t msg_len_;
            int8_t msg_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 231
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 233
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 234
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 236
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 237
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 240
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 241
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 242
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 243
        } captcha_res_;
        };
    } // namespace TSSCaptchaBody
} // namespace sec_proto

#endif // SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_CAPTCHA_CLIENT_H_H_
