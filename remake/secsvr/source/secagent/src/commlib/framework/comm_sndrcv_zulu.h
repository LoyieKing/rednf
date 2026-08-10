// Restored from gunnersvr DWARF + disassembly (2026-08-10)
// Zulu_SendRecv_Package：comm_sndrcv_zulu.cpp 全部 11 符号已还原；
// send_svc_package<T>/receive_svc_package<T> 为头文件内联模板（二进制 W 符号）。
// 布局：基类 44 + zulu_connected_(0x2c) + zulu_svc_ip_(0x30) + zulu_stream_(0x4c) = 80
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SNDRCV_ZULU_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SNDRCV_ZULU_H_H_

#include <stddef.h>

#include "output/commlib/zenlib/release/include/zen_socket_stream.h"
#include "output/commlib/zenlib/release/include/zen_socket_connector.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "src/commlib/framework/comm_sndrcv_base.h"
#include "src/commlib/framework/comm_app_frame.h"

// sizeof = 80
struct Zulu_SendRecv_Package : public Tibetan_SendRecv_Package { // line 29
protected:
    bool zulu_connected_;              // 0x2c
    ZEN_Sockaddr_In zulu_svc_ip_;      // 0x30
    ZEN_Socket_Stream zulu_stream_;    // 0x4c
public:
    Zulu_SendRecv_Package(); // line 16
    virtual ~Zulu_SendRecv_Package(); // line 23
protected:
    int receive_svc_package(ZEN_Time_Value *time_wait); // line 116
    int send_svc_package(ZEN_Time_Value *time_wait); // line 265
public:
    int set_zulu_svcinfo(const char *svc_ip, unsigned short svc_port,
                         const SERVICES_ID &recv_service, const SERVICES_ID &send_service,
                         const SERVICES_ID &proxy_service, size_t frame_len); // line 55
    int connect_zulu_server(ZEN_Time_Value *time_wait); // line 72
    void close(); // line 105
    int receive_svc_package(unsigned int &recv_cmd, ZEN_Time_Value *time_out); // line 307

    // 头文件内联模板（gunnersvr 弱符号，见 comm_cfgsvr_sdk.cpp 实例化）
    template<typename T>
    int send_svc_package(unsigned int qquin, unsigned int cmd, const T &info,
                         ZEN_Time_Value *time_wait, unsigned int app_id,
                         unsigned int backfill_trans_id) {
        int ret = 0;
        if (!zulu_connected_) {
            ret = connect_zulu_server(time_wait);
            if (ret != 0) {
                return ret;
            }
        }
        tibetan_send_appframe_->frame_command_ = cmd;
        tibetan_send_appframe_->backfill_trans_id_ = backfill_trans_id;
        tibetan_send_appframe_->app_id_ = app_id;
        tibetan_send_appframe_->frame_uin_ = qquin;
        ret = tibetan_send_appframe_->appdata_encode<T>(
            0xffbd /*MAX_LEN_OF_APPFRAME_DATA*/, info, 0, 0);
        if (ret != 0) {
            return ret;
        }
        ret = send_svc_package(time_wait);
        if (ret != 0) {
            zulu_stream_.close();
            zulu_connected_ = false;
        }
        return ret;
    }

    template<typename T>
    int receive_svc_package(unsigned int cmd, T &info, bool if_loop,
                            ZEN_Time_Value *time_out) {
        int ret = 0;
        for (;;) {
            ret = receive_svc_package(time_out);
            if (ret != 0) {
                zulu_stream_.close();
                zulu_connected_ = false;
                return ret;
            }
            unsigned int recv_cmd = tibetan_recv_appframe_->frame_command_;
            if (recv_cmd != cmd) {
                if (if_loop) {
                    continue;
                }
                ZEN_Trace_LogMsg::debug_errorex(
                    "[framework] recv a error or unexpect frame,expect cmd =%u,recv cmd =%u.",
                    cmd, recv_cmd);
                zulu_stream_.close();
                zulu_connected_ = false;
                return 0x13e7;
            }
            if (tibetan_recv_appframe_->frame_length_ > 0x32 /*LEN_OF_APPFRAME_HEAD*/) {
                ret = tibetan_recv_appframe_->appdata_decode<T>(info, 0, 0);
            }
            return ret;
        }
    }
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SNDRCV_ZULU_H_H_
