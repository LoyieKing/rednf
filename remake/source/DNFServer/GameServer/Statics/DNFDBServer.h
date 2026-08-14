#ifndef DNF_DB_SERVER_H_
#define DNF_DB_SERVER_H_

#include <string>

#include "HWSpecResearcher.h"
#include "PacketHeader.h"

class CUdpHandler;

// ---- STSpecCount：0xe ----
#pragma pack(push, 1)
struct STSpecCount
{
    STSpecCount();
    unsigned short m_total;   // +0（log_hardware_ting.total）
    HWSpec m_spec;            // +2
};
#pragma pack(pop)

// ---- STErrorCount：0xa ----
#pragma pack(push, 1)
struct STErrorCount
{
    STErrorCount();
    unsigned int m_errorCode;   // +0（log_packet_dispatcher_error_line）
    unsigned short m_errorLine; // +4（log_packet_dispatcher_error_line.error_line）
    unsigned int m_cnt;         // +6（log_packet_dispatcher_error_line.cnt）
};
#pragma pack(pop)

// CDBServer：0x14
class CDBServer
{
public:
    CDBServer();
    ~CDBServer();
    void Init(unsigned char id, std::string& name, unsigned short port, unsigned char param);
    bool IsValidServer();
    bool IsHeartBeatTimeOver();
    void ResetHeartBeat();
    void SendToServer(char* buf, int len);
    void OnDisconnect();
    bool IsConnected();
    void SetConnFlag(bool flag);
    unsigned char m_id;             // +0（0xff=无效）
    std::string m_name;             // +4
    unsigned short m_port;          // +8
    bool m_connectFlag;             // +0xa（同 coserver：SetConnFlag 求值顺序对齐 ORIG）
    unsigned char m_heartBeatCount; // +0xb（0x14）
    unsigned char m_heartBeatOver;  // +0xc
    unsigned char m_reserved;       // +0xd
    CUdpHandler* m_udp;             // +0x10
};

// ---- 客户端 HW spec / error line 统计上报包 ----
#pragma pack(push, 1)

class Packet_DBMW_Save_Client_Spec_Statistic : public PacketHeader
{
public:
    Packet_DBMW_Save_Client_Spec_Statistic();
    char m_flag;                  // +0xa（HWspecView.m_flag）
    int m_count;                  // +0xb（HWspecView.m_count）
    STSpecCount m_items[436];     // +0xf（436 * 0xe）
};

class Packet_DBMW_Save_Error_Line_Statistic : public PacketHeader
{
public:
    Packet_DBMW_Save_Error_Line_Statistic();
    int m_count;              // +0xa
    STErrorCount m_items[612];  // +0xe（612 * 0xa）
};

#pragma pack(pop)

#endif // DNF_DB_SERVER_H_
