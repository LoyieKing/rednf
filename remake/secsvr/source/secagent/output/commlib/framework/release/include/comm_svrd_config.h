// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_svrd_config.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SVRD_CONFIG_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SVRD_CONFIG_H_H_

#include "output/protocol/common/release/include/comm_conf_framework.h"
#include "output/protocol/common/release/include/comm_conf_svcid.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "src/commlib/framework/comm_service_info.h"
#include <bits/stl_set.h>
#include <bits/stringfwd.h>

// sizeof = 166692
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
            tsf4g_tdr::TdrError::ErrorType construct(); // line 128
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 130
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 131
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 133
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 134
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 135
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 137
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 138
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 140
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 141
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 142
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 143
        } trans_info_;
        // anonymous struct, sizeof = 8
        struct {
            public:
            uint32_t task_thread_num_;
            uint32_t task_thread_stack_size_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 88
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 90
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 91
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 93
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 94
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 95
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 97
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 98
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 100
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 101
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 102
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 103
        } task_info_;
        // anonymous struct, sizeof = 436
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
            uint32_t monitor_uin_count_;
            uint32_t monitor_uin_list_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 49
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 51
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 52
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 54
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 55
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 56
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 58
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 59
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 61
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 62
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 63
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 64
        } log_info_;
        tsf4g_tdr::TdrError::ErrorType construct(); // line 168
        tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 170
        tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 171
        tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 173
        tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 174
        tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 175
        tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 177
        tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 178
        tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 180
        tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 181
        tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 182
        tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 183
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
Comm_Svrd_Config::SVCID_CONFIG svcid_config_;
string log_file_prefix_;
string zerg_cfg_file_;
string app_cfg_file_;
string framework_cfg_file_;
string svcid_cfg_file_;
string master_cfgsvr_ip_;
short unsigned int instance_id_;
bool is_use_cfgsvr_;
// anonymous struct, sizeof = 24
struct {
    private:
    // anonymous struct, sizeof = 24
    struct {
        protected:
        _Rb_tree_impl<std::less<unsigned int>,false> _M_impl;
        public:
        /*anon struct*/ int get_allocator() const; // line 352
        protected:
        _Rb_tree_node<unsigned int> * _M_get_node(); // line 357
        void _M_put_node(_Rb_tree_node<unsigned int> *arg0); // line 361
        _Rb_tree_node<unsigned int> * _M_create_node(const unsigned int &arg0); // line 365
        _Rb_tree_node<unsigned int> * _M_clone_node(const _Rb_tree_node<unsigned int> *arg0); // line 379
        void destroy_node(_Rb_tree_node<unsigned int> *arg0); // line 389
        /*anon struct*/ int *& _M_root(); // line 441
        const /*anon struct*/ int * _M_root() const; // line 445
        /*anon struct*/ int *& _M_leftmost(); // line 449
        const /*anon struct*/ int * _M_leftmost() const; // line 453
        /*anon struct*/ int *& _M_rightmost(); // line 457
        const /*anon struct*/ int * _M_rightmost() const; // line 461
        _Rb_tree_node<unsigned int> * _M_begin(); // line 465
        const _Rb_tree_node<unsigned int> * _M_begin() const; // line 469
        _Rb_tree_node<unsigned int> * _M_end(); // line 476
        const _Rb_tree_node<unsigned int> * _M_end() const; // line 480
        static const unsigned int & _S_value(const _Rb_tree_node<unsigned int> *arg0); // line 484
        static const unsigned int & _S_key(const _Rb_tree_node<unsigned int> *arg0); // line 488
        static _Rb_tree_node<unsigned int> * _S_left(/*anon struct*/ int *arg0); // line 492
        static const _Rb_tree_node<unsigned int> * _S_left(const /*anon struct*/ int *arg0); // line 496
        static _Rb_tree_node<unsigned int> * _S_right(/*anon struct*/ int *arg0); // line 500
        static const _Rb_tree_node<unsigned int> * _S_right(const /*anon struct*/ int *arg0); // line 504
        static const unsigned int & _S_value(const /*anon struct*/ int *arg0); // line 508
        static const unsigned int & _S_key(const /*anon struct*/ int *arg0); // line 512
        static /*anon struct*/ int * _S_minimum(/*anon struct*/ int *arg0); // line 516
        static const /*anon struct*/ int * _S_minimum(const /*anon struct*/ int *arg0); // line 520
        static /*anon struct*/ int * _S_maximum(/*anon struct*/ int *arg0); // line 524
        static const /*anon struct*/ int * _S_maximum(const /*anon struct*/ int *arg0); // line 528
        private:
        _Rb_tree_iterator<unsigned int> _M_insert(/*anon struct*/ int *__x, /*anon struct*/ int *__p, const unsigned int &__v); // line 813
        _Rb_tree_const_iterator<unsigned int> _M_insert(const /*anon struct*/ int *arg0, const /*anon struct*/ int *arg1, const unsigned int &arg2); // line 831
        _Rb_tree_node<unsigned int> * _M_copy(const _Rb_tree_node<unsigned int> *arg0, _Rb_tree_node<unsigned int> *arg1); // line 1223
        void _M_erase(_Rb_tree_node<unsigned int> *__x); // line 1259
        public:
        void _Rb_tree(); // line 554
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 557
        void _Rb_tree(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 561
        void _Rb_tree(const /*anon struct*/ int &arg0); // line 565
        void ~_Rb_tree(); // line 577
        /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 791
        /*anon struct*/ int key_comp() const; // line 585
        _Rb_tree_iterator<unsigned int> begin(); // line 589
        _Rb_tree_const_iterator<unsigned int> begin() const; // line 596
        _Rb_tree_iterator<unsigned int> end(); // line 603
        _Rb_tree_const_iterator<unsigned int> end() const; // line 607
        reverse_iterator<std::_Rb_tree_iterator<unsigned int> > rbegin(); // line 614
        reverse_iterator<std::_Rb_tree_const_iterator<unsigned int> > rbegin() const; // line 618
        reverse_iterator<std::_Rb_tree_iterator<unsigned int> > rend(); // line 622
        reverse_iterator<std::_Rb_tree_const_iterator<unsigned int> > rend() const; // line 626
        bool empty() const; // line 630
        size_t size() const; // line 634
        size_t max_size() const; // line 638
        void swap(/*anon struct*/ int &arg0); // line 867
        pair<std::_Rb_tree_iterator<unsigned int>,bool> insert_unique(const unsigned int &__v); // line 913
        _Rb_tree_iterator<unsigned int> insert_equal(const unsigned int &arg0); // line 850
        _Rb_tree_iterator<unsigned int> insert_unique(_Rb_tree_iterator<unsigned int> arg0, const unsigned int &arg1); // line 939
        _Rb_tree_const_iterator<unsigned int> insert_unique(_Rb_tree_const_iterator<unsigned int> arg0, const unsigned int &arg1); // line 996
        _Rb_tree_iterator<unsigned int> insert_equal(_Rb_tree_iterator<unsigned int> arg0, const unsigned int &arg1); // line 1053
        _Rb_tree_const_iterator<unsigned int> insert_equal(_Rb_tree_const_iterator<unsigned int> arg0, const unsigned int &arg1); // line 1107
        void erase(_Rb_tree_iterator<unsigned int> arg0); // line 1183
        void erase(_Rb_tree_const_iterator<unsigned int> arg0); // line 1197
        size_t erase(const unsigned int &arg0); // line 1211
        void erase(_Rb_tree_iterator<unsigned int> arg0, _Rb_tree_iterator<unsigned int> arg1); // line 1275
        void erase(_Rb_tree_const_iterator<unsigned int> arg0, _Rb_tree_const_iterator<unsigned int> arg1); // line 1288
        void erase(const unsigned int *arg0, const unsigned int *arg1); // line 1301
        void clear(); // line 690
        _Rb_tree_iterator<unsigned int> find(const unsigned int &__k); // line 1311
        _Rb_tree_const_iterator<unsigned int> find(const unsigned int &arg0) const; // line 1332
        size_t count(const unsigned int &arg0) const; // line 1354
        _Rb_tree_iterator<unsigned int> lower_bound(const unsigned int &arg0); // line 1365
        _Rb_tree_const_iterator<unsigned int> lower_bound(const unsigned int &arg0) const; // line 1383
        _Rb_tree_iterator<unsigned int> upper_bound(const unsigned int &arg0); // line 1401
        _Rb_tree_const_iterator<unsigned int> upper_bound(const unsigned int &arg0) const; // line 1419
        pair<std::_Rb_tree_iterator<unsigned int>,std::_Rb_tree_iterator<unsigned int> > equal_range(const unsigned int &arg0); // line 1440
        pair<std::_Rb_tree_const_iterator<unsigned int>,std::_Rb_tree_const_iterator<unsigned int> > equal_range(const unsigned int &arg0) const; // line 1450
        bool __rb_verify() const; // line 1461
    } _M_t;
    public:
    void set(); // line 154
    void set(const /*anon struct*/ int &arg0, const /*anon struct*/ int &arg1); // line 165
    void set(const /*anon struct*/ int &arg0); // line 207
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 218
    /*anon struct*/ int key_comp() const; // line 228
    /*anon struct*/ int value_comp() const; // line 232
    /*anon struct*/ int get_allocator() const; // line 236
    _Rb_tree_const_iterator<unsigned int> begin() const; // line 244
    _Rb_tree_const_iterator<unsigned int> end() const; // line 252
    reverse_iterator<std::_Rb_tree_const_iterator<unsigned int> > rbegin() const; // line 261
    reverse_iterator<std::_Rb_tree_const_iterator<unsigned int> > rend() const; // line 270
    bool empty() const; // line 275
    size_t size() const; // line 280
    size_t max_size() const; // line 285
    void swap(/*anon struct*/ int &arg0); // line 300
    pair<std::_Rb_tree_const_iterator<unsigned int>,bool> insert(const unsigned int &arg0); // line 318
    _Rb_tree_const_iterator<unsigned int> insert(_Rb_tree_const_iterator<unsigned int> arg0, const unsigned int &arg1); // line 345
    void erase(_Rb_tree_const_iterator<unsigned int> arg0); // line 371
    size_t erase(const unsigned int &arg0); // line 386
    void erase(_Rb_tree_const_iterator<unsigned int> arg0, _Rb_tree_const_iterator<unsigned int> arg1); // line 401
    void clear(); // line 411
    size_t count(const unsigned int &arg0) const; // line 425
    _Rb_tree_const_iterator<unsigned int> find(const unsigned int &arg0); // line 443
    _Rb_tree_const_iterator<unsigned int> find(const unsigned int &arg0) const; // line 447
    _Rb_tree_const_iterator<unsigned int> lower_bound(const unsigned int &arg0); // line 464
    _Rb_tree_const_iterator<unsigned int> lower_bound(const unsigned int &arg0) const; // line 468
    _Rb_tree_const_iterator<unsigned int> upper_bound(const unsigned int &arg0); // line 480
    _Rb_tree_const_iterator<unsigned int> upper_bound(const unsigned int &arg0) const; // line 484
    pair<std::_Rb_tree_const_iterator<unsigned int>,std::_Rb_tree_const_iterator<unsigned int> > equal_range(const unsigned int &arg0); // line 505
    pair<std::_Rb_tree_const_iterator<unsigned int>,std::_Rb_tree_const_iterator<unsigned int> > equal_range(const unsigned int &arg0) const; // line 509
    void ~set();
} monitor_uin_set_;
protected:
static Comm_Svrd_Config *instance_;
public:
Comm_Svrd_Config(const Comm_Svrd_Config &arg0);
int init(int argc, const char **argv); // line 46
int reload(); // line 288
int proc_start_arg(int argc, const char **argv); // line 68
int load_config(); // line 234
bool is_monitor_uin(unsigned int uin); // line 295
private:
int usage(const char *program_name); // line 194
public:
static Comm_Svrd_Config * instance(); // line 215
static void clean_instance(); // line 226
private:
Comm_Svrd_Config(); // line 30
virtual ~Comm_Svrd_Config(); // line 41
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_SVRD_CONFIG_H_H_
