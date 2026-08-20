#include "LogManager.h"
// df_game_r 还原 —— CMonitorServerProxy（监视服务器客户端代理）实现。
// 参照 docs/class_func_reports/CMonitorServerProxy.md；单 TU 编译后经
// check_tu_game_orig.sh 与 df_game_r ORIG 对比（AE 口径）。

#include "CMonitorServerProxy.h"

#include <errno.h>
#include <string.h>

#include "Packet_Change_Char_Name.h"
#include "Packet_Exchange_Server_Info.h"
#include "Packet_Game_Server_Regist.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_GTM_OntimeEvent_RequestReward.h"
#include "Packet_Monitor_Call_Guild_Info.h"
#include "Packet_Monitor_Call_Guild_Level_Up.h"
#include "Packet_Monitor_Call_Guild_Members.h"
#include "Packet_Monitor_Char_Changable_Info.h"
#include "Packet_Monitor_Member_Pay_Tax.h"
#include "Packet_Monitor_Notice_Charac_Live_On_Ten_Min.h"
#include "Packet_Monitor_Notify_New_Mail.h"
#include "Packet_Monitor_UDP_HeartBeat.h"
#include "Packet_Monitor_UDP_Login.h"
#include "Packet_Monitor_UDP_Logout.h"
#include "Packet_Monitor_User_Repel_ByCharName.h"
#include "Packet_Notice_Guild_War_End.h"
#include "Packet_Notice_Guild_War_Start.h"
#include "Packet_Request_Charac_Tower_Update_Ranking.h"

#include "DNFFunctionLib.h"

// ORIG DF_MonitorServerProxy.cpp 对 CNetwork<100000,100000>::CreateConnectionSocket
// 的调用同 CGuildServerProxy（2 参使用，原样复刻，见 CGuildServerProxy.cpp 说明）。
namespace
{
// CEnvironment+0x378 视图（同 CGuildServerProxy.cpp）
struct CEnvView
{
    char m_pad[0x378];
    int m_serverGroup;    // +0x378
};

class CMonitorNetView
{
public:
    int CreateConnectionSocket(const char* ip, int port);
};

int CMonitorNetView::CreateConnectionSocket(const char* ip, int port)
{
    int sock = 0;
    return ((CNetwork<100000, 100000>*)this)->CreateConnectionSocket(sock, ip, port);
}
}

// ---- 生命周期 ----

bool CMonitorServerProxy::Init()
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (InitUdp() == false)
    {
        return 0;
    }
    if (InitTcp() == false)
    {
        return 0;
    }
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        GlobalData::gmList->SendGMList_To_UpperServer(0);
    }
    return 1;
}

bool CMonitorServerProxy::InitUdp()
{
    if (m_udp.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_MonitorServerProxy.cpp",
                              "bool CMonitorServerProxy::InitUdp()", 0x485,
                              "[CMonitorServerProxy::Init] Can\'t init udp client socket");
        return 0;
    }
    return 1;
}

bool CMonitorServerProxy::InitTcp()
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (m_tcp.m_sock < 0)
    {
        int ret = ((CMonitorNetView*)&m_tcp)
                      ->CreateConnectionSocket(m_ip.c_str(), m_tcpPort);
        while (ret != 0)
        {
            cMyTrace("bool CMonitorServerProxy::InitTcp()", 0x49b, 0)
                ("CMonitorServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    cMyTrace("bool CMonitorServerProxy::InitTcp()", 0x4a0, 9, true, true)
        ("[!] Connect To Monitor Server ... ");
    return 1;
}

void CMonitorServerProxy::Disconnect()
{
    m_tcp.disconnect();
}

// ---- 收发 ----

void CMonitorServerProxy::SendPacket(char* p, int len)
{
    if (m_udpPort != 0)
    {
        m_udp.SendToServer(p, len, (unsigned short)m_udpPort, m_ip.c_str());
    }
    return;
}

void CMonitorServerProxy::SendTcpPacket(char* p, int len)
{
    if (GetTcpHandler()->get_socket() == -1)
    {
    }
    else
    {
        m_tcp.send_packet(p, len);
    }
    return;
}

int CMonitorServerProxy::OnTcpRecv()
{
    int n = m_tcp.recv_packet();
    if (n > 0)
    {
        if (Parsing(n) == false)
        {
            LogManager::logFormat(1, "DF_MonitorServerProxy.cpp",
                                  "bool CMonitorServerProxy::OnTcpRecv()", 0x685,
                                  "Monitor Recv Parsing Err!!!");
            return 0;
        }
        return 1;
    }
    if (n < 0)
    {
        LogManager::logFormat(1, "DF_MonitorServerProxy.cpp",
                              "bool CMonitorServerProxy::OnTcpRecv()", 0x68e,
                              "Monitor Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
}

int CMonitorServerProxy::OnTcpSend()
{
    if (m_tcp.send_packet() < 0)
    {
        cMyTrace("int CMonitorServerProxy::OnTcpSend()", 0x6a3, 0)
            ("OnSend() Socket[%d] Error(%s)", GetTcpHandler()->get_socket(),
             strerror(errno));
        Disconnect();
        return -1;
    }
    return m_tcp.GetRemainSendSize();
}

bool CMonitorServerProxy::OnTcpError()
{
    if (errno != 0)
    {
        cMyTrace("bool CMonitorServerProxy::OnTcpError()", 0x6b6, 0)
            ("Monitor Server OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}

bool CMonitorServerProxy::Parsing(int len)
{
    return m_tcp.Parsing_ServerPacket(len);
}

// ---- Send*：登录/心跳/信息上报 ----

void CMonitorServerProxy::SendCharLevelGrowType(unsigned int dbid, unsigned int growType,
                                                short guildMemFlag, short level)
{
    Packet_Monitor_Char_Changable_Info pkt;
    pkt.m_dbid = dbid;
    pkt.m_padE = (char)growType;
    pkt.m_guildMemFlag = guildMemFlag;
    pkt.m_growthType = (char)level;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendHeartBeat(int channel)
{
    Packet_Monitor_UDP_HeartBeat pkt;
    pkt.m_channel = (unsigned char)channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendLogin(int channel, unsigned int dbid, int serverNo,
                                    char* ip, unsigned char sex, char* ssn)
{
    Packet_Monitor_UDP_Login pkt;
    pkt.m_channelId = channel;
    pkt.m_dbid = dbid;
    pkt.m_channel = (unsigned char)serverNo;
    if (ip != 0)
    {
        strncpy(pkt.m_ip, ip, 0x10);
    }
    else
    {
        LogManager::logFormat(
            1, "DF_MonitorServerProxy.cpp",
            "void CMonitorServerProxy::SendLogin(int, memberIdentificationNumber_t, int, char*, unsigned char, char*)",
            0x4f3, "CMonitorServerProxy::SendLogin! DB ID(%s)", NumberToString(dbid, 0));
    }
    pkt.m_sex = sex;
    memcpy(pkt.m_ssn, ssn, 6);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendLogout(unsigned int dbid, int channel, char* name,
                                     unsigned int cache0, unsigned int cache4,
                                     unsigned int memberKey, unsigned char foc,
                                     unsigned char cacheFlag)
{
    if (dbid == 0)
    {
    }
    else
    {
        Packet_Monitor_UDP_Logout pkt;
        pkt.m_dbid = dbid;
        pkt.m_channel = (unsigned char)channel;
        pkt.m_cache0 = cache0;
        pkt.m_cache4 = cache4;
        pkt.m_memberKey = memberKey;
        pkt.m_foc = foc;
        pkt.m_cacheFlag = cacheFlag;
        if (name != 0)
        {
            strcpy(pkt.m_name, name);
        }
        SendTcpPacket((char*)&pkt, pkt.packetSize);
    }
    return;
}

void CMonitorServerProxy::SendPacketCallGuildMembers(unsigned int charNo,
                                                     unsigned int guildKey)
{
    Packet_Monitor_Call_Guild_Members pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendGuildLevelup(int uid, unsigned int charNo,
                                           unsigned int guildKey)
{
    Packet_Monitor_Call_Guild_Level_Up pkt;
    pkt.m_uid = uid;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendCallGuildInfo(unsigned int charNo, unsigned int uid,
                                            unsigned char count, unsigned int* guildKeys)
{
    Packet_Monitor_Call_Guild_Info pkt;
    pkt.m_charNo = charNo;
    pkt.m_uid = uid;
    pkt.m_count = count;
    for (int i = 0; i < (int)count; i++)
    {
        pkt.m_guildKeys[i] = guildKeys[i];
    }
    pkt.packetSize = (unsigned short)(count * 4 + 0x13);
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendMemberPayTax(bool payTax, unsigned int charNo,
                                           int money, int fatigue)
{
    if (payTax == false)
    {
        return;
    }
    if (money == 0 && fatigue == 0)
    {
        return;
    }
    Packet_Monitor_Member_Pay_Tax pkt;
    pkt.m_charNo = charNo;
    pkt.m_money = money;
    pkt.m_fatigue = fatigue;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendUserRepel(unsigned int dbid, unsigned int fieldA,
                                        char* name, int len)
{
    Packet_Monitor_User_Repel_ByCharName pkt;
    pkt.m_fieldA = fieldA;
    pkt.m_dbid = dbid;
    memcpy(pkt.m_name, name, len);
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendNotifyNewMail(unsigned int charNo)
{
    Packet_Monitor_Notify_New_Mail pkt;
    pkt.m_charNo = charNo;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendGuildWarEnd(unsigned char channel)
{
    Packet_Notice_Guild_War_End pkt;
    pkt.m_channel = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendGuildWarStart(unsigned char channel)
{
    Packet_Notice_Guild_War_Start pkt;
    pkt.m_channel = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendExchangeServerInfo(int channel, int ip, short port)
{
    Packet_Exchange_Server_Info pkt;
    pkt.m_channel = channel;
    pkt.m_ip = ip;
    pkt.m_port = port;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendNoticeCharLive(unsigned int charNo)
{
    Packet_Monitor_Notice_Charac_Live_On_Ten_Min pkt;
    pkt.m_charNo = charNo;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendRequestUserMid(unsigned int dbid, unsigned int gmId,
                                             char* name, int len)
{
    Packet_GM_Request_Mid pkt;
    pkt.m_gmId = gmId;
    pkt.m_dbid = dbid;
    memcpy(pkt.m_name, name, len);
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendRequestOnTimeEventReward(unsigned int a,
                                                       unsigned int b,
                                                       unsigned int c)
{
    Packet_GTM_OntimeEvent_RequestReward pkt;
    pkt.m_fieldA = a;
    pkt.m_fieldB = b;
    pkt.m_fieldC = c;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::updateTowerRank(unsigned int charNo, unsigned int job,
                                          unsigned int floor, unsigned int score)
{
    Packet_Request_Charac_Tower_Update_Ranking pkt;
    pkt.m_charNo = charNo;
    pkt.m_job = job;
    pkt.m_floor = floor;
    pkt.m_score = score;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendChangeCharName(unsigned int idByChannel,
                                             unsigned int charNo,
                                             unsigned int guildKey, char* name)
{
    Packet_Change_Char_Name pkt;
    pkt.m_idByChannel = idByChannel;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    strcpy(pkt.m_name, name);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CMonitorServerProxy::SendGameServerInfo(char* name, unsigned short port,
                                             unsigned char group, unsigned char channel)
{
    Packet_Game_Server_Regist pkt;
    pkt.m_group = group;
    pkt.m_channel = channel;
    pkt.m_type = 1;
    pkt.m_port = port;
    strncpy(pkt.m_name, name, 0x10);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}
