// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_app_frame.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_APP_FRAME_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_APP_FRAME_H_H_

#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_service_info.h"
#include <stddef.h>

// sizeof = 4
struct _APP_FRAME_OPTION { // line 17
public:
// bitfield 24
unsigned int inner_option_ : 24;
// bitfield 8
unsigned int frame_version_ : 8;
};
// sizeof = 50
struct Comm_App_Frame { // line 33
    // sizeof = 4
    union ._206 {
    public:
    unsigned int frame_option_;
    _APP_FRAME_OPTION inner_option_;
    };
    // sizeof = 4
    union ._207 {
    public:
    unsigned int send_serial_number_;
    unsigned int send_ip_address_;
    };
public:
static const unsigned int INNER_OPTION_MASK;
static const unsigned int OUTER_OPTION_MASK;
static const size_t LEN_OF_APPFRAME_HEAD;
static const unsigned char TSS_APPFRAME_V1;
static const size_t MAX_LEN_OF_APPFRAME;
static const size_t LEN_OF_TEA_REMAIN_ROOM;
static const size_t MAX_LEN_OF_APPFRAME_DATA;
static const size_t MAX_LEN_OF_TEA_APPDATA;
unsigned int frame_length_;
Comm_App_Frame::._206 _unnamed_;
unsigned int frame_command_;
unsigned int frame_uin_;
SERVICES_ID send_service_;
SERVICES_ID recv_service_;
SERVICES_ID proxy_service_;
unsigned int transaction_id_;
unsigned int backfill_trans_id_;
unsigned int app_id_;
Comm_App_Frame::._207 _unnamed_;
char frame_appdata_[];
Comm_App_Frame(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 194
Comm_App_Frame(unsigned int arg0, unsigned int arg1, unsigned int arg2, short unsigned int arg3, short unsigned int arg4, unsigned int arg5); // line 202
Comm_App_Frame(unsigned int arg0, unsigned int arg1, unsigned int arg2, SERVICES_ID arg3, SERVICES_ID arg4, unsigned int arg5); // line 210
Comm_App_Frame(unsigned int arg0, unsigned int arg1, unsigned int arg2, const SERVICES_ID &arg3, const SERVICES_ID &arg4, short unsigned int arg5, unsigned int arg6); // line 219
~Comm_App_Frame(); // line 222
Comm_App_Frame & operator=(const Comm_App_Frame &arg0); // line 225
bool is_internal_process(bool &arg0); // line 230
bool is_zerg_processcmd(); // line 232
void clear_inner_option(); // line 235
void clear_all_option(); // line 237
void framehead_encode(); // line 240
void framehead_decode(); // line 242
void init_framehead(unsigned int arg0, unsigned int arg1, unsigned int arg2); // line 247
void set_send_svcid(short unsigned int arg0, unsigned int arg1); // line 250
void set_recv_svcid(short unsigned int arg0, unsigned int arg1); // line 252
void set_proxy_svcid(short unsigned int arg0, unsigned int arg1); // line 254
void set_all_svcid(const SERVICES_ID &arg0, const SERVICES_ID &arg1, const SERVICES_ID &arg2); // line 257
int fill_appdata(size_t arg0, const char *arg1); // line 260
void exchange_rcvsnd_svcid(); // line 263
void exchange_rcvsnd_svcid(Comm_App_Frame &arg0); // line 265
void fillback_appframe_head(Comm_App_Frame &arg0); // line 267
Comm_App_Frame * CloneAppFrame() const; // line 270
void CloneAppFrame(Comm_App_Frame *arg0) const; // line 272
void CloneFrameHead(Comm_App_Frame *arg0) const; // line 274
void dump_appframe_info(ostringstream &arg0) const; // line 277
void dump_appframe_data(ostringstream &arg0) const; // line 279
void dump_appframe_head(ostringstream &arg0) const; // line 281
void dumpoutput_framehead(const char *arg0, ZEN_LOG_PRIORITY arg1) const; // line 285
void dumpoutput_frameInfo(const char *arg0, ZEN_LOG_PRIORITY arg1) const; // line 288
size_t get_appframe_len(); // line 291
size_t get_frame_datalen(); // line 293
unsigned int get_send_ip(); // line 315
int appframe_encrypt(const char *arg0, size_t arg1); // line 323
int appframe_decrypt(const char *arg0, size_t arg1); // line 326
int appframe_encrypt(const char *arg0, Comm_App_Frame *arg1, size_t arg2); // line 331
int appframe_decrypt(const char *arg0, Comm_App_Frame *arg1, size_t arg2); // line 335
static Comm_App_Frame * FillDatainAppFrame(size_t arg0, const char *arg1); // line 340
static void * operator new(size_t arg0, size_t arg1); // line 343
static void operator delete(void *arg0, size_t arg1); // line 345
static void dumpoutput_framehead(const Comm_App_Frame *arg0, const char *arg1, ZEN_LOG_PRIORITY arg2); // line 353
static void dumpoutput_frameInfo(const Comm_App_Frame *arg0, const char *arg1, ZEN_LOG_PRIORITY arg2); // line 357
static void signature_construct(Comm_App_Frame *&arg0, unsigned int arg1, const char *arg2, size_t arg3); // line 363
int appdata_encode<sec_proto::CheckCfgFileUpdateReq>(unsigned int szframe_appdata, const /*anon struct*/ int &info, unsigned int data_start, size_t *sz_code); // line 449
int appdata_decode<sec_proto::CheckCfgFileUpdateRsp>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) const; // line 490
int appdata_decode<sec_proto::CfgFileRsp>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) const; // line 490
int appdata_encode<sec_proto::CfgFileReq>(unsigned int szframe_appdata, const /*anon struct*/ int &info, unsigned int data_start, size_t *sz_code); // line 449
int appdata_encode<sec_proto::SvcIpInfoReq>(unsigned int szframe_appdata, const /*anon struct*/ int &info, unsigned int data_start, size_t *sz_code); // line 449
int appdata_decode<sec_proto::SvcIpInfoRsp>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) const; // line 490
int appdata_encode<sec_proto::DeployInfoReq>(unsigned int szframe_appdata, const /*anon struct*/ int &info, unsigned int data_start, size_t *sz_code); // line 449
int appdata_decode<sec_proto::DeployInfoRsp>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) const; // line 490
int appdata_encode<sec_proto::SvcIdReq>(unsigned int szframe_appdata, const /*anon struct*/ int &info, unsigned int data_start, size_t *sz_code); // line 449
int appdata_decode<sec_proto::SvcIdRsp>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) const; // line 490
int appdata_decode<sec_proto::CfgsvrInfoRsp>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) const; // line 490
};
extern int ret; // line 452
extern const unsigned int OUTER_OPTION_MASK; // line 93
extern const size_t LEN_OF_APPFRAME_HEAD; // line 98
extern const size_t MAX_LEN_OF_APPFRAME; // line 120
extern const size_t LEN_OF_TEA_REMAIN_ROOM; // line 124
extern const size_t MAX_LEN_OF_APPFRAME_DATA; // line 127
namespace Comm_App_Frame {
    // sizeof = 4
    union ._207 { // line 139
    public:
    unsigned int frame_option_;
    _APP_FRAME_OPTION inner_option_;
    };
    // sizeof = 4
    union ._208 { // line 171
    public:
    unsigned int send_serial_number_;
    unsigned int send_ip_address_;
    };
} // namespace Comm_App_Frame

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_APP_FRAME_H_H_
