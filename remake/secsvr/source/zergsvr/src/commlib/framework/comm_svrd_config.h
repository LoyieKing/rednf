// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_svrd_config.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_CONFIG_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_CONFIG_H_H_

#include "output/protocol/common/release/include/comm_conf_framework.h"
#include "output/protocol/common/release/include/comm_conf_svcid.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "src/commlib/framework/comm_service_info.h"
#include <bits/stringfwd.h>

// sizeof = 166264
struct Comm_Svrd_Config { // line 27
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
    typedef struct {
        public:
        // anonymous struct, sizeof = 12
        struct {
            public:
            uint32_t trans_num_;
            uint32_t trans_cmd_num_;
            uint32_t func_cmd_num_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 126
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 128
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 129
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 131
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 132
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 133
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 135
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 136
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 138
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 139
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 140
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 141
        } trans_info_;
        // anonymous struct, sizeof = 8
        struct {
            public:
            uint32_t task_thread_num_;
            uint32_t task_thread_stack_size_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 86
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 88
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 89
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 91
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 92
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 93
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 95
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 96
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 98
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 99
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 100
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 101
        } task_info_;
        // anonymous struct, sizeof = 32
        struct {
            public:
            uint32_t log_output_;
            uint32_t log_div_type_;
            uint32_t max_log_file_num_;
            uint32_t max_log_file_size_;
            uint32_t bill_output_;
            uint32_t bill_div_type_;
            uint32_t max_bill_file_num_;
            uint32_t max_bill_file_size_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 47
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 49
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 50
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 52
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 53
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 54
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 56
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 57
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 59
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 60
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 61
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 62
        } log_info_;
        tsf4g_tdr::TdrError::ErrorType construct(); // line 166
        tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 168
        tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 169
        tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 171
        tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 172
        tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 173
        tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 175
        tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 176
        tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 178
        tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 179
        tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 180
        tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 181
    } FRAMEWORK_CONFIG;
    typedef struct {
        public:
        uint32_t service_num_;
        /*anon struct*/ int service_info_[];
        tsf4g_tdr::TdrError::ErrorType construct(); // line 90
        tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 92
        tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 93
        tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 95
        tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 96
        tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 97
        tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 99
        tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 100
        tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 102
        tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 103
        tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 104
        tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 105
    } SVCID_CONFIG;
public:
int (***_vptr.Comm_Svrd_Config)(...);
SERVICES_ID self_svr_id_;
string app_run_dir_;
bool if_restore_pipe_;
bool app_run_daemon_;
bool app_install_service_;
bool app_uninstall_service_;
Zerg_MMAP_BusPipe::ZERG_CONFIG zerg_config_;
Comm_Svrd_Config::FRAMEWORK_CONFIG framework_config_;
ServicesConfig::SVCID_CONFIG svcid_config_;
string log_file_prefix_;
string zerg_cfg_file_;
string app_cfg_file_;
string framework_cfg_file_;
string svcid_cfg_file_;
string master_cfgsvr_ip_;
short unsigned int instance_id_;
bool is_use_cfgsvr_;
protected:
static Comm_Svrd_Config *instance_;
public:
Comm_Svrd_Config(const Comm_Svrd_Config &arg0);
int init(int argc, const char **argv); // line 46
int reload(); // line 278
int proc_start_arg(int argc, const char **argv); // line 68
int load_config(); // line 234
private:
int usage(const char *program_name); // line 194
public:
static Comm_Svrd_Config * instance(); // line 215
static void clean_instance(); // line 226
private:
Comm_Svrd_Config(); // line 30
virtual ~Comm_Svrd_Config(); // line 41
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_CONFIG_H_H_
