// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMonitorServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN19CMonitorServerProxyC2ESsii`（ORIG 0x84715a6）；ctor 已是首强符号。
#include <iostream>

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

#include "CMonitorServerProxy/CMonitorServerProxy.inc"
    : m_ip(ip), m_udpPort(udpPort), m_tcpPort(tcpPort), m_tcp(), m_udp()
{
}

#include "CMonitorServerProxy/Init.inc"
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

#include "CMonitorServerProxy/InitUdp.inc"
{
    if (m_udp.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_MonitorServerProxy.cpp", __PRETTY_FUNCTION__,
                              0x485,
                              "[CMonitorServerProxy::Init] Can\'t init udp client socket");
        return 0;
    }
    return 1;
}

// [b28d-27] DIFF N80：ORIG 0x08470ad6；ORIG 在 cMyTrace ctor call 之前右起求
// varargs 并跨 call 保活 callee-saved（%edi/%esi），4.4 恒 ctor 后求（具名/临时/
// register 三探针不可翻，N80 实证 0x0847196d）⇒ 封顶。p167 CLEAR（9/9）。
#include "CMonitorServerProxy/InitTcp.inc"
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (m_tcp.m_sock < 0)
    {
        int ret = m_tcp.CreateConnectionSocket(m_tcp.m_sock, m_ip.c_str(),
                                               m_tcpPort);
        while (ret != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x49b, 0)
                ("CMonitorServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x4a0, 9, true, true)
        ("[!] Connect To Monitor Server ... ");
    return 1;
}

#include "CMonitorServerProxy/Disconnect.inc"
{
    m_tcp.disconnect();
}

// ---- 收发 ----

#include "CMonitorServerProxy/SendPacket.inc"
{
    if (m_udpPort != 0)
    {
        m_udp.SendToServer(p, len, (unsigned short)m_udpPort, m_ip.c_str());
    }
    return;
}

#include "CMonitorServerProxy/SendTcpPacket.inc"
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

#include "CMonitorServerProxy/OnTcpRecv.inc"
{
    int n = m_tcp.recv_packet();
    if (n > 0)
    {
        if (Parsing(n) == false)
        {
            LogManager::logFormat(1, "DF_MonitorServerProxy.cpp", __PRETTY_FUNCTION__,
                                  0x685, "Monitor Recv Parsing Err!!!");
            return 0;
        }
        return 1;
    }
    if (n < 0)
    {
        LogManager::logFormat(1, "DF_MonitorServerProxy.cpp", __PRETTY_FUNCTION__,
                              0x68e, "Monitor Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
}

#include "CMonitorServerProxy/OnTcpSend.inc"
{
    if (m_tcp.send_packet() < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6a3, 0)
            ("OnSend() Socket[%d] Error(%s)", GetTcpHandler()->get_socket(),
             strerror(errno));
        Disconnect();
        return -1;
    }
    return m_tcp.GetRemainSendSize();
}

#include "CMonitorServerProxy/OnTcpError.inc"
{
    if (errno != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6b6, 0)
            ("Monitor Server OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}

#include "CMonitorServerProxy/Parsing.inc"
{
    return m_tcp.Parsing_ServerPacket(len);
}

// ---- Send*：登录/心跳/信息上报 ----

#include "CMonitorServerProxy/SendCharLevelGrowType.inc"
                                                short guildMemFlag, short level)
{
    Packet_Monitor_Char_Changable_Info pkt;
    pkt.m_dbid = dbid;
    pkt.m_padE = (char)growType;
    pkt.m_guildMemFlag = guildMemFlag;
    pkt.m_growthType = (char)level;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendHeartBeat.inc"
{
    Packet_Monitor_UDP_HeartBeat pkt;
    pkt.m_channel = (unsigned char)channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendLogin.inc"
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
            __PRETTY_FUNCTION__,
            0x4f3, "CMonitorServerProxy::SendLogin! DB ID(%s)", NumberToString(dbid, 0));
    }
    pkt.m_sex = sex;
    memcpy(pkt.m_ssn, ssn, 6);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendLogout.inc"
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

#include "CMonitorServerProxy/SendPacketCallGuildMembers.inc"
                                                     unsigned int guildKey)
{
    Packet_Monitor_Call_Guild_Members pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendGuildLevelup.inc"
                                           unsigned int guildKey)
{
    Packet_Monitor_Call_Guild_Level_Up pkt;
    pkt.m_uid = uid;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendCallGuildInfo.inc"
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

#include "CMonitorServerProxy/SendMemberPayTax.inc"
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

#include "CMonitorServerProxy/SendUserRepel.inc"
                                        char* name, int len)
{
    Packet_Monitor_User_Repel_ByCharName pkt;
    pkt.m_fieldA = fieldA;
    pkt.m_dbid = dbid;
    memcpy(pkt.m_name, name, len);
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendNotifyNewMail.inc"
{
    Packet_Monitor_Notify_New_Mail pkt;
    pkt.m_charNo = charNo;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendGuildWarEnd.inc"
{
    Packet_Notice_Guild_War_End pkt;
    pkt.m_channel = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendGuildWarStart.inc"
{
    Packet_Notice_Guild_War_Start pkt;
    pkt.m_channel = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendExchangeServerInfo.inc"
{
    Packet_Exchange_Server_Info pkt;
    pkt.m_channel = channel;
    pkt.m_ip = ip;
    pkt.m_port = port;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendNoticeCharLive.inc"
{
    Packet_Monitor_Notice_Charac_Live_On_Ten_Min pkt;
    pkt.m_charNo = charNo;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendRequestUserMid.inc"
                                             char* name, int len)
{
    Packet_GM_Request_Mid pkt;
    pkt.m_gmId = gmId;
    pkt.m_dbid = dbid;
    memcpy(pkt.m_name, name, len);
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendRequestOnTimeEventReward.inc"
                                                       unsigned int b,
                                                       unsigned int c)
{
    Packet_GTM_OntimeEvent_RequestReward pkt;
    pkt.m_fieldA = a;
    pkt.m_fieldB = b;
    pkt.m_fieldC = c;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/updateTowerRank.inc"
                                          unsigned int floor, unsigned int score)
{
    Packet_Request_Charac_Tower_Update_Ranking pkt;
    pkt.m_charNo = charNo;
    pkt.m_job = job;
    pkt.m_floor = floor;
    pkt.m_score = score;
    SendPacket((char*)&pkt, pkt.packetSize);
}

#include "CMonitorServerProxy/SendChangeCharName.inc"
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

// [b28d-27] DIFF N18a/P75：ORIG 0x084714ec；ORIG 目标地址两段 lea
// （`lea -0x27(%ebp),%eax; lea 0xd(%eax),%edx`）并提前装载，4.4 恒折 `lea+add`
// 且延迟到压参（register asm 两向实测更差）⇒ 封顶。p167 CLEAR（3/3）。
#include "CMonitorServerProxy/SendGameServerInfo.inc"
                                             unsigned char group, unsigned char channel)
{
    Packet_Game_Server_Regist pkt;
    pkt.m_group = group;
    pkt.m_channel = channel;
    pkt.m_type = 1;
    pkt.m_port = port;
    // 残差（N18a 族，probe 实证）：ORIG 4.1 `lea -0x27(%ebp),%eax;
    // lea 0xd(%eax),%edx` 两段 lea 直落 %edx + name 经 %eax；4.4 恒折
    // `lea+add` 且实参装载寄存器互换。register asm(edx) 绑定 ndiff 6→7、
    // asm(eax) 6→8，均比直写差 ⇒ 保持源形，记 N。
    strncpy(pkt.m_name, name, 0x10);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}
