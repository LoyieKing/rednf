// Auto-generated header stub from DWARF info
// Original path: output/protocol/common/release/include/comm_conf_zerg.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_PROTOCOL_COMMON_RELEASE_INCLUDE_COMM_CONF_ZERG_H_H_
#define SECSVR_OUTPUT_PROTOCOL_COMMON_RELEASE_INCLUDE_COMM_CONF_ZERG_H_H_

namespace Comm_Svrd_Config {
    typedef struct {
        public:
        // anonymous struct, sizeof = 29
        struct {
            public:
            // anonymous struct, sizeof = 7
            struct {
                public:
                uint16_t svr_type;
                uint32_t svr_id;
                uint8_t use_encrypt;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 42
                tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 44
                tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 45
                tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 47
                tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 48
                tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 49
                tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 51
                tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 52
                tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 54
                tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 55
                tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 56
                tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 57
            } self_svr_info;
            uint8_t slave_svr_count;
            /*anon struct*/ int slave_svrs[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 82
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 84
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 85
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 87
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 88
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 89
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 91
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 92
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 94
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 95
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 96
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 97
        } self_cfg;
        // anonymous struct, sizeof = 32
        struct {
            public:
            char log_level[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 120
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 122
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 123
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 125
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 126
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 127
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 129
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 130
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 132
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 133
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 134
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 135
        } log_cfg;
        // anonymous struct, sizeof = 361
        struct {
            public:
            uint32_t max_frame_len;
            uint32_t accept_send_buf_size;
            uint32_t connect_send_deque_size;
            uint32_t connect_timeout;
            uint32_t recv_timeout;
            uint8_t is_proxy;
            uint32_t max_accept_svr;
            uint8_t is_lock_pipe;
            uint8_t retry_error;
            uint32_t recv_pipe_len;
            uint32_t send_pipe_len;
            uint32_t error_pipe_len;
            uint8_t check_pthread;
            uint8_t insurance;
            char opt_key1[];
            char opt_key2[];
            char get_svr_info_type[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 174
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 176
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 177
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 179
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 180
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 181
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 183
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 184
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 186
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 187
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 188
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 189
        } comm_cfg;
        // anonymous struct, sizeof = 25
        struct {
            public:
            // anonymous struct, sizeof = 7
            struct {
                public:
                uint16_t svr_type;
                uint32_t svr_id;
                uint8_t use_encrypt;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 42
                tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 44
                tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 45
                tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 47
                tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 48
                tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 49
                tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 51
                tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 52
                tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 54
                tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 55
                tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 56
                tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 57
            } svr_info;
            char svr_ip[];
            uint16_t svr_port;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 214
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 216
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 217
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 219
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 220
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 221
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 223
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 224
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 226
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 227
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 228
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 229
        } cfgsvr_cfg;
        // anonymous struct, sizeof = 19
        struct {
            public:
            uint8_t use_console;
            char console_ip[];
            uint16_t console_port;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 254
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 256
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 257
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 259
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 260
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 261
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 263
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 264
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 266
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 267
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 268
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 269
        } console_cfg;
        // anonymous struct, sizeof = 43
        struct {
            public:
            uint8_t check_frame;
            uint8_t check_rcvid;
            uint8_t num_snd_type;
            uint32_t allow_snd_types[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 295
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 297
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 298
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 300
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 301
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 302
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 304
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 305
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 307
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 308
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 309
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 310
        } check_cfg;
        // anonymous struct, sizeof = 1404
        struct {
            public:
            uint32_t auto_connect_num;
            /*anon struct*/ int auto_connect_svrs[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 373
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 375
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 376
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 378
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 379
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 380
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 382
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 383
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 385
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 386
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 387
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 388
        } auto_connect_cfg;
        // anonymous struct, sizeof = 40960
        struct {
            public:
            char allow_ips[];
            char reject_ips[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 412
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 414
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 415
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 417
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 418
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 419
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 421
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 422
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 424
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 425
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 426
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 427
        } restrict_cfg;
        // anonymous struct, sizeof = 404
        struct {
            public:
            uint32_t monitor_num;
            uint32_t monitor_cmds[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 451
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 453
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 454
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 456
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 457
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 458
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 460
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 461
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 463
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 464
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 465
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 466
        } monitor_cfg;
        tsf4g_tdr::TdrError::ErrorType construct(); // line 497
        tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 499
        tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 500
        tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 502
        tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 503
        tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 504
        tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 506
        tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 507
        tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 509
        tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 510
        tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 511
        tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 512
    } ZERG_CONFIG;
} // namespace Comm_Svrd_Config
namespace MML_Console_Handler {
    typedef struct {
        public:
        // anonymous struct, sizeof = 29
        struct {
            public:
            // anonymous struct, sizeof = 7
            struct {
                public:
                uint16_t svr_type;
                uint32_t svr_id;
                uint8_t use_encrypt;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 42
                tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 44
                tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 45
                tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 47
                tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 48
                tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 49
                tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 51
                tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 52
                tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 54
                tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 55
                tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 56
                tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 57
            } self_svr_info;
            uint8_t slave_svr_count;
            /*anon struct*/ int slave_svrs[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 82
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 84
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 85
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 87
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 88
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 89
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 91
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 92
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 94
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 95
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 96
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 97
        } self_cfg;
        // anonymous struct, sizeof = 32
        struct {
            public:
            char log_level[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 120
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 122
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 123
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 125
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 126
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 127
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 129
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 130
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 132
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 133
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 134
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 135
        } log_cfg;
        // anonymous struct, sizeof = 361
        struct {
            public:
            uint32_t max_frame_len;
            uint32_t accept_send_buf_size;
            uint32_t connect_send_deque_size;
            uint32_t connect_timeout;
            uint32_t recv_timeout;
            uint8_t is_proxy;
            uint32_t max_accept_svr;
            uint8_t is_lock_pipe;
            uint8_t retry_error;
            uint32_t recv_pipe_len;
            uint32_t send_pipe_len;
            uint32_t error_pipe_len;
            uint8_t check_pthread;
            uint8_t insurance;
            char opt_key1[];
            char opt_key2[];
            char get_svr_info_type[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 174
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 176
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 177
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 179
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 180
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 181
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 183
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 184
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 186
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 187
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 188
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 189
        } comm_cfg;
        // anonymous struct, sizeof = 25
        struct {
            public:
            // anonymous struct, sizeof = 7
            struct {
                public:
                uint16_t svr_type;
                uint32_t svr_id;
                uint8_t use_encrypt;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 42
                tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 44
                tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 45
                tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 47
                tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 48
                tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 49
                tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 51
                tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 52
                tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 54
                tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 55
                tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 56
                tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 57
            } svr_info;
            char svr_ip[];
            uint16_t svr_port;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 214
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 216
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 217
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 219
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 220
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 221
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 223
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 224
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 226
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 227
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 228
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 229
        } cfgsvr_cfg;
        // anonymous struct, sizeof = 19
        struct {
            public:
            uint8_t use_console;
            char console_ip[];
            uint16_t console_port;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 254
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 256
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 257
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 259
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 260
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 261
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 263
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 264
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 266
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 267
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 268
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 269
        } console_cfg;
        // anonymous struct, sizeof = 43
        struct {
            public:
            uint8_t check_frame;
            uint8_t check_rcvid;
            uint8_t num_snd_type;
            uint32_t allow_snd_types[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 295
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 297
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 298
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 300
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 301
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 302
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 304
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 305
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 307
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 308
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 309
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 310
        } check_cfg;
        // anonymous struct, sizeof = 1404
        struct {
            public:
            uint32_t auto_connect_num;
            /*anon struct*/ int auto_connect_svrs[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 373
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 375
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 376
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 378
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 379
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 380
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 382
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 383
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 385
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 386
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 387
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 388
        } auto_connect_cfg;
        // anonymous struct, sizeof = 40960
        struct {
            public:
            char allow_ips[];
            char reject_ips[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 412
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 414
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 415
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 417
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 418
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 419
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 421
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 422
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 424
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 425
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 426
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 427
        } restrict_cfg;
        // anonymous struct, sizeof = 404
        struct {
            public:
            uint32_t monitor_num;
            uint32_t monitor_cmds[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 451
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 453
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 454
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 456
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 457
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 458
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 460
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 461
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 463
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 464
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 465
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 466
        } monitor_cfg;
        tsf4g_tdr::TdrError::ErrorType construct(); // line 497
        tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 499
        tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 500
        tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 502
        tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 503
        tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 504
        tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 506
        tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 507
        tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 509
        tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 510
        tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 511
        tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 512
    } ZERG_CONFIG;
} // namespace MML_Console_Handler
namespace Zerg_MMAP_BusPipe {
    typedef struct {
        public:
        // anonymous struct, sizeof = 29
        struct {
            public:
            // anonymous struct, sizeof = 7
            struct {
                public:
                uint16_t svr_type;
                uint32_t svr_id;
                uint8_t use_encrypt;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 42
                tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 44
                tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 45
                tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 47
                tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 48
                tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 49
                tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 51
                tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 52
                tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 54
                tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 55
                tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 56
                tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 57
            } self_svr_info;
            uint8_t slave_svr_count;
            /*anon struct*/ int slave_svrs[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 82
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 84
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 85
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 87
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 88
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 89
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 91
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 92
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 94
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 95
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 96
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 97
        } self_cfg;
        // anonymous struct, sizeof = 32
        struct {
            public:
            char log_level[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 120
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 122
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 123
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 125
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 126
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 127
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 129
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 130
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 132
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 133
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 134
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 135
        } log_cfg;
        // anonymous struct, sizeof = 361
        struct {
            public:
            uint32_t max_frame_len;
            uint32_t accept_send_buf_size;
            uint32_t connect_send_deque_size;
            uint32_t connect_timeout;
            uint32_t recv_timeout;
            uint8_t is_proxy;
            uint32_t max_accept_svr;
            uint8_t is_lock_pipe;
            uint8_t retry_error;
            uint32_t recv_pipe_len;
            uint32_t send_pipe_len;
            uint32_t error_pipe_len;
            uint8_t check_pthread;
            uint8_t insurance;
            char opt_key1[];
            char opt_key2[];
            char get_svr_info_type[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 174
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 176
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 177
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 179
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 180
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 181
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 183
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 184
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 186
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 187
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 188
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 189
        } comm_cfg;
        // anonymous struct, sizeof = 25
        struct {
            public:
            // anonymous struct, sizeof = 7
            struct {
                public:
                uint16_t svr_type;
                uint32_t svr_id;
                uint8_t use_encrypt;
                tsf4g_tdr::TdrError::ErrorType construct(); // line 42
                tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 44
                tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 45
                tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 47
                tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 48
                tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 49
                tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 51
                tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 52
                tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 54
                tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 55
                tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 56
                tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 57
            } svr_info;
            char svr_ip[];
            uint16_t svr_port;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 214
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 216
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 217
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 219
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 220
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 221
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 223
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 224
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 226
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 227
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 228
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 229
        } cfgsvr_cfg;
        // anonymous struct, sizeof = 19
        struct {
            public:
            uint8_t use_console;
            char console_ip[];
            uint16_t console_port;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 254
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 256
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 257
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 259
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 260
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 261
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 263
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 264
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 266
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 267
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 268
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 269
        } console_cfg;
        // anonymous struct, sizeof = 43
        struct {
            public:
            uint8_t check_frame;
            uint8_t check_rcvid;
            uint8_t num_snd_type;
            uint32_t allow_snd_types[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 295
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 297
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 298
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 300
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 301
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 302
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 304
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 305
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 307
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 308
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 309
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 310
        } check_cfg;
        // anonymous struct, sizeof = 1404
        struct {
            public:
            uint32_t auto_connect_num;
            /*anon struct*/ int auto_connect_svrs[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 373
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 375
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 376
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 378
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 379
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 380
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 382
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 383
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 385
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 386
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 387
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 388
        } auto_connect_cfg;
        // anonymous struct, sizeof = 40960
        struct {
            public:
            char allow_ips[];
            char reject_ips[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 412
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 414
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 415
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 417
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 418
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 419
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 421
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 422
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 424
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 425
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 426
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 427
        } restrict_cfg;
        // anonymous struct, sizeof = 404
        struct {
            public:
            uint32_t monitor_num;
            uint32_t monitor_cmds[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 451
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 453
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 454
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 456
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 457
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 458
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 460
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 461
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 463
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 464
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 465
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 466
        } monitor_cfg;
        tsf4g_tdr::TdrError::ErrorType construct(); // line 497
        tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 499
        tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 500
        tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 502
        tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 503
        tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 504
        tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 506
        tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 507
        tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 509
        tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 510
        tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 511
        tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 512
    } ZERG_CONFIG;
} // namespace Zerg_MMAP_BusPipe

#endif // SECSVR_OUTPUT_PROTOCOL_COMMON_RELEASE_INCLUDE_COMM_CONF_ZERG_H_H_
