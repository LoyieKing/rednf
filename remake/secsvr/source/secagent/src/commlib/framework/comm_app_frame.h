// Rebuilt from gunnersvr DWARF + disassembly (2026-08-10, fw_frame)
// Layout: #pragma pack(1) — 成员偏移已用二进制 mov disp(%reg) 逐一核对
//   frame_length_@0 frame_option_@4 frame_command_@8 frame_uin_@0xc
//   send@0x10 recv@0x16 proxy@0x1c transaction_id_@0x22 backfill@0x26
//   app_id_@0x2a serial/ip@0x2e appdata@0x32   (LEN_OF_APPFRAME_HEAD = 0x32)
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_APP_FRAME_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_APP_FRAME_H_H_

#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "src/commlib/framework/comm_service_info.h"
#include <sstream>
#include <stddef.h>

// sizeof = 4
struct _APP_FRAME_OPTION {
    unsigned int inner_option_ : 24;
    unsigned int frame_version_ : 8;
};

#pragma pack(push, 1)
struct Comm_App_Frame {
    static const unsigned int INNER_OPTION_MASK = 0xFFFFFF;
    static const unsigned int OUTER_OPTION_MASK = 0xFF000000;
    static const size_t LEN_OF_APPFRAME_HEAD = 0x32;
    static const unsigned char TSS_APPFRAME_V1 = 1;
    static const size_t MAX_LEN_OF_APPFRAME = 0x10000;
    static const size_t LEN_OF_TEA_REMAIN_ROOM = 0x11;
    static const size_t MAX_LEN_OF_APPFRAME_DATA = 0xFFBD;   // fill_appdata 上限
    static const size_t MAX_LEN_OF_TEA_APPDATA = 0xFFCE;

    unsigned int frame_length_;
    union {
        unsigned int frame_option_;
        _APP_FRAME_OPTION inner_option_;
    };
    unsigned int frame_command_;
    unsigned int frame_uin_;
    SERVICES_ID send_service_;
    SERVICES_ID recv_service_;
    SERVICES_ID proxy_service_;
    unsigned int transaction_id_;
    unsigned int backfill_trans_id_;
    unsigned int app_id_;
    union {
        unsigned int send_serial_number_;
        unsigned int send_ip_address_;
    };
    char frame_appdata_[];

    // 参数顺序按二进制反汇编确认：p1→frame_command_, p2→frame_length_, p3→frame_option_
    Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int option);
    Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int uin,
                   unsigned short sndtype, unsigned short rectype, unsigned int option);
    Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int uin,
                   SERVICES_ID sndinfo, SERVICES_ID rcvsinfo, unsigned int option);
    Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int uin,
                   const SERVICES_ID &sndinfo, const SERVICES_ID &proxyinfo,
                   unsigned short rectype, unsigned int option);
    ~Comm_App_Frame();
    Comm_App_Frame &operator=(const Comm_App_Frame &other);

    bool is_internal_process(bool &is_zerg_process);
    bool is_zerg_processcmd();
    void clear_inner_option();
    void clear_all_option();
    void framehead_encode();
    void framehead_decode();
    void init_framehead(unsigned int lenframe, unsigned int option, unsigned int cmd);
    void set_send_svcid(unsigned short svrtype, unsigned int svrid);
    void set_recv_svcid(unsigned short svrtype, unsigned int svrid);
    void set_proxy_svcid(unsigned short svrtype, unsigned int svrid);
    void set_all_svcid(const SERVICES_ID &rcvinfo, const SERVICES_ID &sndinfo,
                       const SERVICES_ID &proxyinfo);
    int fill_appdata(size_t szdata, const char *vardata);
    void exchange_rcvsnd_svcid();
    void exchange_rcvsnd_svcid(Comm_App_Frame &exframe);
    void fillback_appframe_head(Comm_App_Frame &exframe);
    Comm_App_Frame *CloneAppFrame() const;
    void CloneAppFrame(Comm_App_Frame *clone_frame) const;
    void CloneFrameHead(Comm_App_Frame *clone_frame) const;
    void dump_appframe_info(std::ostringstream &strstream) const;
    void dump_appframe_data(std::ostringstream &strstream) const;
    void dump_appframe_head(std::ostringstream &strstream) const;
    void dumpoutput_framehead(const char *outstr, ZEN_LOG_PRIORITY log_priority) const;
    void dumpoutput_frameInfo(const char *outstr, ZEN_LOG_PRIORITY log_priority) const;
    size_t get_appframe_len();
    size_t get_frame_datalen();
    unsigned int get_send_ip();
    int appframe_encrypt(const char *key, size_t key_len);
    int appframe_decrypt(const char *key, size_t key_len);
    int appframe_encrypt(const char *key, Comm_App_Frame *dest, size_t key_len);
    int appframe_decrypt(const char *key, Comm_App_Frame *dest, size_t key_len);

    // 头文件模板（gunnersvr 弱符号）：T 为 tsf4g TDR 结构（pack/unpack）。
    template<typename T>
    int appdata_encode(unsigned int szframe_appdata, const T &info, unsigned int data_start,
                       size_t *sz_code) {
        size_t use_len = 0;
        int ret = info.pack(frame_appdata_ + data_start, szframe_appdata - data_start,
                            &use_len, 0);
        if (ret != 0) {
            return 0x13a0;
        }
        if (sz_code) {
            *sz_code = use_len;
        }
        frame_length_ = data_start + use_len + LEN_OF_APPFRAME_HEAD;
        return 0;
    }

    template<typename T>
    int appdata_decode(T &info, unsigned int data_start, size_t *sz_code) const {
        size_t use_len = 0;
        int ret = info.unpack(frame_appdata_ + data_start,
                              frame_length_ - data_start - LEN_OF_APPFRAME_HEAD,
                              &use_len, 0);
        if (ret != 0) {
            return 0x13a0;
        }
        if (sz_code) {
            *sz_code = use_len;
        }
        return 0;
    }

    static Comm_App_Frame *FillDatainAppFrame(size_t szdata, const char *vardata);
    static void *operator new(size_t size, size_t lenframe);
    static void operator delete(void *ptrframe, size_t arg1);
    static void dumpoutput_framehead(const Comm_App_Frame *proc_frame, const char *outstr,
                                     ZEN_LOG_PRIORITY log_priority);
    static void dumpoutput_frameInfo(const Comm_App_Frame *proc_frame, const char *outstr,
                                     ZEN_LOG_PRIORITY log_priority);
    static void signature_construct(Comm_App_Frame *&frame, unsigned int cmd,
                                    const char *data, size_t len);
};
#pragma pack(pop)

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_APP_FRAME_H_H_
