// Auto-generated header stub from DWARF info
// Original path: output/commsvr/gunnersvr/release/include/gunner_channel_mgr.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_CHANNEL_MGR_H_H_
#define SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_CHANNEL_MGR_H_H_

#include "output/protocol/common/release/include/comm_conf_gunnersvr.h"
#include "output/protocol/common/release/include/comm_proto_public_monitorsvr.h"

// sizeof = 7804
struct GunnerChannelMgr { // line 34
private:
static const unsigned int MAX_CHANNEL_NUM;
GunnerChannel default_channel_;
unsigned int channel_num_;
GunnerChannel gunner_channel_[];
public:
GunnerChannelMgr(); // line 36
~GunnerChannelMgr(); // line 37
int init_channels(GunnerChannelMgr::LPCONFIG config); // line 39
void close_channels(); // line 41
GunnerChannel * get_channel(unsigned int appid); // line 43
};
// sizeof = 120
struct GunnerChannel { // line 17
private:
// anonymous struct, sizeof = 38
struct {
    public:
    // anonymous struct, sizeof = 34
    struct {
        public:
        char ip_[];
        uint16_t port_;
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
    } net_addr_;
    uint32_t app_id_;
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
} monitor_info_;
Zulu_SendRecv_Package zulu_;
public:
int init(const /*anon struct*/ int &monitor_info); // line 19
void close(); // line 21
int send_to_monitor(/*anon struct*/ int pkg); // line 23
unsigned int get_appid(); // line 25
private:
int connect_monitor(); // line 27
public:
GunnerChannel();
~GunnerChannel();
};
extern const unsigned int MAX_CHANNEL_NUM; // line 45

#endif // SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_CHANNEL_MGR_H_H_
