// Auto-generated header stub from DWARF info
// Original path: src/protocol/formmog/formmog_proto_captcha_client_old.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_OLD_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_OLD_H_H_

namespace sec_proto {
    namespace TSSCaptchaOldBody {
        // sizeof = 4827
        union ._277 { // line 440
        public:
        // anonymous struct, sizeof = 539
        struct {
            public:
            uint32_t app_id_;
            uint32_t antibot_bus_id_;
            uint32_t zone_id_;
            uint16_t max_pic_change_num_;
            uint16_t max_verify_num_;
            uint32_t op_type_;
            uint32_t timeout_;
            uint8_t busi_id_;
            uint16_t busi_info_len_;
            int8_t busi_info_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 321
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 323
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 324
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 326
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 327
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 330
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 331
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 332
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 333
        } busi_captcha_req_;
        // anonymous struct, sizeof = 24
        struct {
            public:
            uint32_t app_id_;
            uint32_t antibot_bus_id_;
            uint32_t zone_id_;
            uint16_t max_pic_change_num_;
            uint16_t max_verify_num_;
            uint32_t op_type_;
            uint32_t timeout_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 124
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 126
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 127
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 129
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 130
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 133
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 134
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 135
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 136
        } captcha_req_;
        // anonymous struct, sizeof = 267
        struct {
            public:
            uint8_t code_len_;
            uint8_t code_[];
            uint16_t session_len_;
            uint8_t session_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 162
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 164
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 165
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 167
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 168
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 171
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 172
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 173
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 174
        } verify_req_;
        // anonymous struct, sizeof = 142
        struct {
            public:
            uint32_t app_id_;
            uint32_t antibot_bus_id_;
            uint32_t zone_id_;
            uint8_t result_;
            uint8_t msg_len_;
            int8_t msg_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 202
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 204
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 205
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 207
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 208
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 211
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 212
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 213
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 214
        } verify_res_;
        // anonymous struct, sizeof = 130
        struct {
            public:
            uint8_t result_;
            uint8_t msg_len_;
            int8_t msg_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 239
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 241
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 242
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 244
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 245
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 248
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 249
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 250
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 251
        } s2c_verify_res_;
        // anonymous struct, sizeof = 4827
        struct {
            public:
            uint32_t timeout_;
            uint16_t verify_times_limit_;
            uint16_t verify_times_;
            uint16_t max_change_times_;
            uint16_t change_times_;
            uint16_t picture_len_;
            uint8_t picture_[];
            uint16_t session_len_;
            uint8_t session_[];
            uint8_t busi_id_;
            uint16_t busi_info_len_;
            int8_t busi_info_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 407
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 409
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 410
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 412
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 413
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 416
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 417
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 418
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 419
        } busi_captcha_pic_;
        // anonymous struct, sizeof = 518
        struct {
            public:
            uint16_t log_src_;
            uint16_t log_type_;
            uint16_t log_len_;
            int8_t log_msg_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 277
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 279
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 280
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 282
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 283
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 286
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 287
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 288
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 289
        } report_log_;
        // anonymous struct, sizeof = 527
        struct {
            public:
            uint32_t app_id_;
            uint32_t antibot_bus_id_;
            uint32_t zone_id_;
            uint8_t busi_id_;
            uint16_t busi_info_len_;
            int8_t busi_info_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 361
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 363
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 364
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 366
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 367
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 370
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 371
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 372
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 373
        } captcha_busi_finish_;
        };
    } // namespace TSSCaptchaOldBody
} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_CAPTCHA_CLIENT_OLD_H_H_
