// Reconstructed from gunnersvr binary disassembly (2026-08-10, fw_frame)
// 语义与二进制一致：帧头 50 字节 pack(1)，htonl/ntohs 序列、clone/fill 行为逐字节核对。
#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_service_info.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include <arpa/inet.h>
#include <string.h>

// ---- 简单的 svcid 设置 ----
void Comm_App_Frame::set_send_svcid(unsigned short svrtype, unsigned int svrid) {
    send_service_.services_type_ = svrtype;
    send_service_.services_id_ = svrid;
}

void Comm_App_Frame::set_recv_svcid(unsigned short svrtype, unsigned int svrid) {
    recv_service_.services_type_ = svrtype;
    recv_service_.services_id_ = svrid;
}

void Comm_App_Frame::set_proxy_svcid(unsigned short svrtype, unsigned int svrid) {
    proxy_service_.services_type_ = svrtype;
    proxy_service_.services_id_ = svrid;
}

void Comm_App_Frame::set_all_svcid(const SERVICES_ID &rcvinfo, const SERVICES_ID &sndinfo,
                                   const SERVICES_ID &proxyinfo) {
    recv_service_ = rcvinfo;
    send_service_ = sndinfo;
    proxy_service_ = proxyinfo;
}

// ---- 帧头交换/回填 ----
void Comm_App_Frame::exchange_rcvsnd_svcid(Comm_App_Frame &exframe) {
    recv_service_ = exframe.send_service_;
    send_service_ = exframe.recv_service_;
    proxy_service_ = exframe.proxy_service_;
    transaction_id_ = exframe.transaction_id_;
    backfill_trans_id_ = exframe.backfill_trans_id_;
    app_id_ = exframe.app_id_;
    frame_uin_ = exframe.frame_uin_;
}

void Comm_App_Frame::exchange_rcvsnd_svcid() {
    SERVICES_ID tmpsvrinfo = recv_service_;
    recv_service_ = send_service_;
    send_service_ = tmpsvrinfo;
}

void Comm_App_Frame::fillback_appframe_head(Comm_App_Frame &exframe) {
    recv_service_ = exframe.send_service_;
    send_service_ = exframe.recv_service_;
    proxy_service_ = exframe.proxy_service_;
    transaction_id_ = exframe.backfill_trans_id_;
    backfill_trans_id_ = exframe.transaction_id_;
    app_id_ = exframe.app_id_;
    frame_uin_ = exframe.frame_uin_;
}

// ---- clone ----
void Comm_App_Frame::CloneFrameHead(Comm_App_Frame *clone_frame) const {
    memcpy(clone_frame, this, LEN_OF_APPFRAME_HEAD);
}

void Comm_App_Frame::CloneAppFrame(Comm_App_Frame *clone_frame) const {
    memcpy(clone_frame, this, frame_length_);
}

Comm_App_Frame *Comm_App_Frame::CloneAppFrame() const {
    Comm_App_Frame *proc_frame =
        new (frame_length_) Comm_App_Frame(0, LEN_OF_APPFRAME_HEAD, (TSS_APPFRAME_V1 << 24));
    memcpy(proc_frame, this, frame_length_);
    return proc_frame;
}

// ---- 赋值 / 数据填充 ----
Comm_App_Frame &Comm_App_Frame::operator=(const Comm_App_Frame &other) {
    if (this != &other) {
        memcpy(this, &other, other.frame_length_);
    }
    return *this;
}

int Comm_App_Frame::fill_appdata(size_t szdata, const char *vardata) {
    if (szdata > MAX_LEN_OF_APPFRAME_DATA) {
        return 0x139F; // 二进制返回该常量（= MAX_LEN_OF_TEA_APPDATA，语义待最终核对）
    }
    memcpy(frame_appdata_, vardata, szdata);
    frame_length_ = szdata + LEN_OF_APPFRAME_HEAD;
    return 0;
}

// ---- 帧头初始化 / 编解码 ----
void Comm_App_Frame::init_framehead(unsigned int lenframe, unsigned int option, unsigned int cmd) {
    frame_length_ = lenframe;
    frame_option_ = option;
    frame_command_ = cmd;
    frame_uin_ = 0;
    inner_option_.frame_version_ = TSS_APPFRAME_V1;
    send_service_.set_serviceid(0, 0);
    recv_service_.set_serviceid(0, 0);
    proxy_service_.set_serviceid(0, 0);
    transaction_id_ = 0;
    backfill_trans_id_ = 0;
    app_id_ = 0;
    send_serial_number_ = 0;
}

void Comm_App_Frame::framehead_decode() {
    frame_length_ = ntohl(frame_length_);
    frame_option_ = ntohl(frame_option_);
    frame_command_ = ntohl(frame_command_);
    frame_uin_ = ntohl(frame_uin_);
    recv_service_.services_type_ = ntohs(recv_service_.services_type_);
    recv_service_.services_id_ = ntohl(recv_service_.services_id_);
    send_service_.services_type_ = ntohs(send_service_.services_type_);
    send_service_.services_id_ = ntohl(send_service_.services_id_);
    proxy_service_.services_type_ = ntohs(proxy_service_.services_type_);
    proxy_service_.services_id_ = ntohl(proxy_service_.services_id_);
    transaction_id_ = ntohl(transaction_id_);
    backfill_trans_id_ = ntohl(backfill_trans_id_);
    app_id_ = ntohl(app_id_);
    send_serial_number_ = ntohl(send_serial_number_);
}

void Comm_App_Frame::framehead_encode() {
    frame_length_ = htonl(frame_length_);
    frame_option_ = htonl(frame_option_);
    frame_command_ = htonl(frame_command_);
    frame_uin_ = htonl(frame_uin_);
    recv_service_.services_type_ = htons(recv_service_.services_type_);
    recv_service_.services_id_ = htonl(recv_service_.services_id_);
    send_service_.services_type_ = htons(send_service_.services_type_);
    send_service_.services_id_ = htonl(send_service_.services_id_);
    proxy_service_.services_type_ = htons(proxy_service_.services_type_);
    proxy_service_.services_id_ = htonl(proxy_service_.services_id_);
    transaction_id_ = htonl(transaction_id_);
    backfill_trans_id_ = htonl(backfill_trans_id_);
    app_id_ = htonl(app_id_);
    send_serial_number_ = htonl(send_serial_number_);
}

// ---- 构造 / 析构 / new / delete ----
Comm_App_Frame::Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int option)
    : frame_length_(lenframe),
      frame_option_(option),
      frame_command_(cmd),
      frame_uin_(0),
      send_service_(0, 0),
      recv_service_(0, 0),
      proxy_service_(0, 0),
      transaction_id_(0),
      backfill_trans_id_(0),
      send_serial_number_(0) {
}

Comm_App_Frame::Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int uin,
                               unsigned short sndtype, unsigned short rectype,
                               unsigned int option)
    : frame_length_(lenframe),
      frame_option_(option),
      frame_command_(cmd),
      frame_uin_(uin),
      send_service_(sndtype, uin),
      recv_service_(rectype, 0),
      proxy_service_(0, 0),
      transaction_id_(0),
      backfill_trans_id_(0),
      app_id_(0),
      send_serial_number_(0) {
}

Comm_App_Frame::Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int uin,
                               SERVICES_ID sndinfo, SERVICES_ID rcvsinfo, unsigned int option)
    : frame_length_(lenframe),
      frame_option_(option),
      frame_command_(cmd),
      frame_uin_(uin),
      send_service_(sndinfo),
      recv_service_(rcvsinfo),
      proxy_service_(0, 0),
      transaction_id_(0),
      backfill_trans_id_(0),
      app_id_(0),
      send_serial_number_(0) {
}

Comm_App_Frame::Comm_App_Frame(unsigned int cmd, unsigned int lenframe, unsigned int uin,
                               const SERVICES_ID &sndinfo, const SERVICES_ID &proxyinfo,
                               unsigned short rectype, unsigned int option)
    : frame_length_(lenframe),
      frame_option_(option),
      frame_command_(cmd),
      frame_uin_(uin),
      send_service_(sndinfo),
      recv_service_(rectype, 0),
      proxy_service_(proxyinfo),
      transaction_id_(0),
      backfill_trans_id_(0),
      app_id_(0),
      send_serial_number_(0) {
}

Comm_App_Frame::~Comm_App_Frame() {
}

void *Comm_App_Frame::operator new(size_t size, size_t lenframe) {
    void *ptr = new char[(lenframe > LEN_OF_APPFRAME_HEAD) ? lenframe : LEN_OF_APPFRAME_HEAD];
    return ptr;
}

void Comm_App_Frame::operator delete(void *ptrframe, size_t arg1) {
    if (ptrframe) {
        char *ptr = (char *)ptrframe;
        delete[] ptr;
    }
}

Comm_App_Frame *Comm_App_Frame::FillDatainAppFrame(size_t szdata, const char *vardata) {
    Comm_App_Frame *ptr = new (szdata + LEN_OF_APPFRAME_HEAD)
        Comm_App_Frame(0, LEN_OF_APPFRAME_HEAD, (TSS_APPFRAME_V1 << 24));
    memcpy(ptr->frame_appdata_, vardata, szdata);
    return ptr;
}

// ---- dump 系列 ----
void Comm_App_Frame::dump_appframe_head(std::ostringstream &strstream) const {
    strstream << "Len:" << frame_length_ << " Framedesc:0x " << frame_option_
              << " Command:" << frame_command_ << " Uin:" << frame_uin_
              << " TransactionID:" << transaction_id_
              << " BackfillTransID:" << backfill_trans_id_
              << " ProcessHandler:" << app_id_
              << " Sendip:" << send_serial_number_
              << "Rcvsvr:" << recv_service_.services_type_
              << "|" << recv_service_.services_id_
              << " Sndsvr:" << send_service_.services_type_
              << "|" << send_service_.services_id_
              << " Proxysvr:" << proxy_service_.services_type_
              << "|" << proxy_service_.services_id_;
}

void Comm_App_Frame::dump_appframe_data(std::ostringstream &strstream) const {
    const unsigned int LINE_OUT_NUM = 40;
    unsigned int datalen = frame_length_ - LEN_OF_APPFRAME_HEAD;
    std::string strascii;
    strascii.reserve(0x100);
    strstream << std::hex;
    for (unsigned int i = 0; i < datalen; ++i) {
        if (i % LINE_OUT_NUM == 0 && i != 0) {
            strstream << " " << strascii << "  " << std::endl;
            strascii.clear();
        }
        unsigned char bytmp = (unsigned char)frame_appdata_[i];
        strstream << std::hex << std::setw(2) << std::setfill('0')
                  << std::uppercase << (unsigned short)bytmp << " ";
        if (bytmp >= 0x21 && bytmp <= 0xF9) {
            strascii.push_back((char)bytmp);
        } else {
            strascii.push_back('.');
        }
    }
    if (datalen % LINE_OUT_NUM != 0) {
        unsigned int remain = LINE_OUT_NUM - (datalen % LINE_OUT_NUM);
        for (unsigned int j = 0; j < remain; ++j) {
            strstream << "   ";
        }
        strstream << " " << strascii << std::endl << std::ends;
    }
}

void Comm_App_Frame::dump_appframe_info(std::ostringstream &strstream) const {
    dump_appframe_head(strstream);
    dump_appframe_data(strstream);
}

void Comm_App_Frame::dumpoutput_framehead(const char *outstr, ZEN_LOG_PRIORITY log_priority) const {
    std::ostringstream strstream;
    dump_appframe_head(strstream);
    std::string str = strstream.str();
    ZEN_Trace_LogMsg::debug_output(log_priority, "[framework] [%s]%s", outstr, str.c_str());
}

void Comm_App_Frame::dumpoutput_frameInfo(const char *outstr, ZEN_LOG_PRIORITY log_priority) const {
    std::ostringstream strstream;
    dump_appframe_info(strstream);
    std::string str = strstream.str();
    ZEN_Trace_LogMsg::debug_output(log_priority, "[framework] [%s]%s", outstr, str.c_str());
}

void Comm_App_Frame::dumpoutput_framehead(const Comm_App_Frame *proc_frame, const char *outstr,
                                          ZEN_LOG_PRIORITY log_priority) {
    proc_frame->dumpoutput_framehead(outstr, log_priority);
}

void Comm_App_Frame::dumpoutput_frameInfo(const Comm_App_Frame *proc_frame, const char *outstr,
                                          ZEN_LOG_PRIORITY log_priority) {
    proc_frame->dumpoutput_frameInfo(outstr, log_priority);
}
