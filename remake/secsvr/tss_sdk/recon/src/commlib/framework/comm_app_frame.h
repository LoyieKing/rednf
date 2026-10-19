// Tag-394 Comm_App_Frame / _APP_FRAME_OPTION for reconstructed TUs.
//
// This header shadows the reuse-tree copies of
// src/commlib/framework/comm_app_frame.h for TUs compiled with `-I recon`.
// The reuse copies pull output/commlib/zenlib/.../zen_trace_log_basic.h and
// therefore the vendor libstdc++/TDR world; that cannot be co-included with
// the recon zenlib/protocol headers.  The declarations below are the ORIG
// tag-394 layout and signatures, byte-for-byte (see audit/tu_report.py):
//
//   _APP_FRAME_OPTION  size 4: inner_option_ : 24, frame_version_ : 8
//   Comm_App_Frame     size 50:
//     +0  frame_length_ unsigned int
//     +4  union { frame_option_; _APP_FRAME_OPTION inner_option_; }
//     +8  frame_command_   +12 frame_uin_
//     +16 send_service_ (SERVICES_ID 6) +22 recv_service_ +28 proxy_service_
//     +34 transaction_id_  +38 backfill_trans_id_ +42 app_id_
//     +46 union { send_serial_number_; send_ip_address_; }
//     +50 char frame_appdata_[0] (flexible array, ORIG DW_AT_byte_size 0)
//
// The template bodies only use LEN_OF_APPFRAME_HEAD, frame_length_ and
// frame_appdata_; they are the same weak instantiations the ORIG emits.
#ifndef TSS_RECON_COMM_APP_FRAME_H_
#define TSS_RECON_COMM_APP_FRAME_H_

#include <stddef.h>
#include <sstream>

#include "src/commlib/framework/comm_service_info.h"  // SERVICES_ID (pack(1), 6)
#include "src/commlib/zenlib/zen_recon_support.h"     // ZEN_LOG_PRIORITY

// sizeof = 4 (ORIG DWARF bitfield layout: 24 + 8 bits).
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
    // ORIG DWARF DW_AT_const_value of the four class statics (CU
    // comm_cfgsvr_sdk.cpp and every other CU that declares them):
    //   MAX_LEN_OF_APPFRAME      = 0x10000 (65536)  <- stack frame buffer
    //   LEN_OF_TEA_REMAIN_ROOM   = 0x11    (17)
    //   MAX_LEN_OF_APPFRAME_DATA = 0xFFBD  (65469)
    //   MAX_LEN_OF_TEA_APPDATA   = 0xFFCE  (65486)
    static const size_t MAX_LEN_OF_APPFRAME = 0x10000;
    static const size_t LEN_OF_TEA_REMAIN_ROOM = 0x11;
    static const size_t MAX_LEN_OF_APPFRAME_DATA = 0xFFBD;
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

    Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int option);
    Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int uin,
                   unsigned short sndtype, unsigned short rectype,
                   unsigned int option);
    Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int uin,
                   SERVICES_ID sndinfo, SERVICES_ID rcvsinfo,
                   unsigned int option);
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
    void init_framehead(unsigned int lenframe, unsigned int option,
                        unsigned int cmd);
    void set_send_svcid(unsigned short svrtype, unsigned int svrid);
    void set_recv_svcid(unsigned short svrtype, unsigned int svrid);
    void set_proxy_svcid(unsigned short svrtype, unsigned int svrid);
    void set_all_svcid(const SERVICES_ID &rcvinfo,
                       const SERVICES_ID &sndinfo,
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
    void dumpoutput_framehead(const char *outstr,
                              ZEN_LOG_PRIORITY log_priority) const;
    void dumpoutput_frameInfo(const char *outstr,
                              ZEN_LOG_PRIORITY log_priority) const;
    size_t get_appframe_len();
    size_t get_frame_datalen();
    unsigned int get_send_ip();
    int appframe_encrypt(const char *key, size_t key_len);
    int appframe_decrypt(const char *key, size_t key_len);
    int appframe_encrypt(const char *key, Comm_App_Frame *dest, size_t key_len);
    int appframe_decrypt(const char *key, Comm_App_Frame *dest, size_t key_len);

    template <typename T>
    int appdata_encode(unsigned int szframe_appdata, const T &info,
                       unsigned int data_start, size_t *sz_code) {
        size_t use_len = 0;
        int ret = info.pack(frame_appdata_ + data_start,
                            szframe_appdata - data_start, &use_len, 0);
        if (ret != 0) {
            return 0x13a0;
        }
        if (sz_code) {
            *sz_code = use_len;
        }
        frame_length_ = data_start + use_len + LEN_OF_APPFRAME_HEAD;
        return 0;
    }

    template <typename T>
    int appdata_decode(T &info, unsigned int data_start,
                       size_t *sz_code) const {
        size_t use_len = 0;
        int ret = info.unpack(frame_appdata_ + data_start,
                              frame_length_ - data_start -
                                  LEN_OF_APPFRAME_HEAD,
                              &use_len, 0);
        if (ret != 0) {
            return 0x13a0;
        }
        if (sz_code) {
            *sz_code = use_len;
        }
        return 0;
    }

    static Comm_App_Frame *FillDatainAppFrame(size_t szdata,
                                              const char *vardata);
    static void *operator new(size_t size, size_t lenframe);
    static void operator delete(void *ptrframe, size_t arg1);
    static void dumpoutput_framehead(const Comm_App_Frame *proc_frame,
                                     const char *outstr,
                                     ZEN_LOG_PRIORITY log_priority);
    static void dumpoutput_frameInfo(const Comm_App_Frame *proc_frame,
                                     const char *outstr,
                                     ZEN_LOG_PRIORITY log_priority);
    static void signature_construct(Comm_App_Frame *&frame, unsigned int cmd,
                                    const char *data, size_t len);
};
#pragma pack(pop)

typedef char tss_recon_app_frame_option_size[
    sizeof(_APP_FRAME_OPTION) == 4 ? 1 : -1];
typedef char tss_recon_comm_app_frame_size[
    sizeof(Comm_App_Frame) == 50 ? 1 : -1];

#endif  // TSS_RECON_COMM_APP_FRAME_H_
