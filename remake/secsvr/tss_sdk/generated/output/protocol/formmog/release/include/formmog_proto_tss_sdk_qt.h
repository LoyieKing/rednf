// Auto-generated header stub from DWARF info
// Original path: output/protocol/formmog/release/include/formmog_proto_tss_sdk_qt.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_TSS_SDK_QT_H_H_
#define SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_TSS_SDK_QT_H_H_

namespace sec_proto {
    namespace Dp2QtMsgBody {
        // sizeof = 265
        union ._505 { // line 434
        public:
        // anonymous struct, sizeof = 265
        struct {
            public:
            uint32_t uin;
            uint32_t server_ip;
            uint8_t name_len;
            uint8_t name[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 78
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 80
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 81
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 83
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 84
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 87
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 88
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 89
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 90
        } login;
        // anonymous struct, sizeof = 1
        struct {
            public:
            uint8_t channel_id;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 113
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 115
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 116
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 118
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 119
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 122
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 123
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 124
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 125
        } enter_channel;
        // anonymous struct, sizeof = 2
        struct {
            public:
            uint8_t room_type;
            uint8_t visitor_count;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 149
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 151
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 152
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 154
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 155
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 158
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 159
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 160
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 161
        } create_room;
        // anonymous struct, sizeof = 2
        struct {
            public:
            uint16_t room_id;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 184
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 186
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 187
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 189
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 190
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 193
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 194
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 195
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 196
        } create_room_res;
        // anonymous struct, sizeof = 6
        struct {
            public:
            uint16_t room_id;
            uint8_t room_type;
            uint8_t visitor_count;
            uint8_t role_type;
            uint8_t role_position;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 223
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 225
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 226
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 228
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 229
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 232
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 233
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 234
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 235
        } enter_room;
        // anonymous struct, sizeof = 1
        struct {
            public:
            uint8_t role_position;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 258
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 260
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 261
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 263
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 264
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 267
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 268
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 269
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 270
        } change_role;
        // anonymous struct, sizeof = 2
        struct {
            public:
            uint8_t role_position;
            uint8_t role_type;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 294
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 296
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 297
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 299
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 300
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 303
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 304
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 305
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 306
        } change_position;
        // anonymous struct, sizeof = 5
        struct {
            public:
            uint8_t role_action;
            uint32_t revert;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 330
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 332
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 333
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 335
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 336
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 339
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 340
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 341
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 342
        } role_action;
        // anonymous struct, sizeof = 2
        struct {
            public:
            uint8_t master_role_type;
            uint8_t master_role_position;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 366
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 368
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 369
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 371
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 372
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 375
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 376
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 377
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 378
        } room_master;
        // anonymous struct, sizeof = 1
        struct {
            public:
            uint8_t room_type;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 401
            tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 403
            tsf4g_tdr::TdrError::ErrorType pack(char *arg0, size_t arg1, size_t *arg2, unsigned int arg3) const; // line 404
            tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &arg0, unsigned int arg1); // line 406
            tsf4g_tdr::TdrError::ErrorType unpack(const char *arg0, size_t arg1, size_t *arg2, unsigned int arg3); // line 407
            tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 410
            tsf4g_tdr::TdrError::ErrorType visualize(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 411
            const char * visualize_ex(/*anon struct*/ int &arg0, int arg1, char arg2) const; // line 412
            const char * visualize_ex(char *arg0, size_t arg1, size_t *arg2, int arg3, char arg4) const; // line 413
        } room_type;
        };
    } // namespace Dp2QtMsgBody
} // namespace sec_proto

#endif // SECSVR_OUTPUT_PROTOCOL_FORMMOG_RELEASE_INCLUDE_FORMMOG_PROTO_TSS_SDK_QT_H_H_
