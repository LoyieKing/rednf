// df_game_r 还原 —— CCommunityServerProxy（独占 OrigCommPx 批）。
// 依据 docs/class_func_reports/CCommunityServerProxy/*.md 与 objdump 转写
// （OnTcpRecv/SendTcpPacket 报告缺失，按 §3.2 nm 定址后 objdump 区间转写）。
#include <errno.h>
#include <string.h>   // memcpy / strerror

#include "GameTypes.h"   // cMyTrace
#include "CCommunityServerProxy.h"
#include "CEnvironment.h"   // G_CEnvironment / get_channel_no
#include "CUser.h"          // CUser::get_acc_id/GetUID/GetServerGroup/getSex
#include "Packet_Community_Login.h"
#include "Packet_Community_Logout.h"
#include "Packet_Request_Add_PvP_Buddy.h"
#include "Packet_Response_Add_PvP_Buddy.h"
#include "Packet_Request_Remove_PvP_Buddy.h"

// ORIG T 0x0846c5c8：基类构造 + string(ip)；m_tcpPort/m_udpPort 在函数体赋值
//（报告汇编：string 构造后先写 +0x30e00 再清 +0x30df8）。
CCommunityServerProxy::CCommunityServerProxy(char* ip, int port)
    : CNetwork<100000, 100000>(), m_ip(ip)
{
    m_tcpPort = port;
    m_udpPort = (ENUM_SOCKET_STATE)0;   // +0x30df8 实为 socket 状态（GDataC6 enum 化），初值 disconnected
}

// ORIG T 0x0846c660：纯转发 InitTcp。
void CCommunityServerProxy::Init()
{
    InitTcp();
}

// ORIG T 0x0846c674：ip/port 有效且 socket 未连时 CreateConnectionSocket；
// 失败 cMyTrace 报错返 0（P15 赋值即条件 + P37 临时链式 trace）。
int CCommunityServerProxy::InitTcp()
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return 0;
    }
    if (m_sock < 0)
    {
        register int ret;
        if ((ret = CreateConnectionSocket(m_sock, m_ip.c_str(), m_tcpPort)) != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbf, 0)
                ("CCommunityServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    return 1;
}

// ORIG T 0x0846c770：连接有效（ip 非空且端口非 0）时 disconnect + 清 udpPort。
void CCommunityServerProxy::Disconnect()
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return;
    }
    else
    {
        disconnect();
        m_udpPort = (ENUM_SOCKET_STATE)0;   // 断开置状态 0
    }
}

// ORIG T 0x0846c7c4：连接态(+0x30df8)==2 时经 CNetwork::send_packet 下发；
// 否则 cMyTrace 报 disconnected（字符串 .rodata 直读转写）。
void CCommunityServerProxy::SendTcpPacket(char* data, int len)
{
    if (m_udpPort != 2)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xd4, 0);
        trace("cannot send packet because Community Server is disconnected.");
        return;
    }
    send_packet(data, len);
}

// ORIG T 0x0846c82c（报告缺失，objdump 转写）：
// recv_packet()>0 → Parsing 失败则 Disconnect 返 0；<0 → logFormat 后 Disconnect 返 0。
bool CCommunityServerProxy::OnTcpRecv()
{
    int recvLen = recv_packet();
    if (recvLen > 0)
    {
        if (!Parsing(recvLen))
        {
            Disconnect();
            return 0;
        }
        return 1;
    }
    if (recvLen < 0)
    {
        LogManager::logFormat(1, "DF_CommunityServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0xed,
                              "Community Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
}

// ORIG T 0x0846c8c4：send_packet()<0 → 报错+Disconnect 返 -1；否则返 m_recvOffset。
int CCommunityServerProxy::OnTcpSend()
{
    if (send_packet() < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x101, 0)
            ("CommunityServer OnSend() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        Disconnect();
        return -1;
    }
    return m_recvOffset;
}

// ORIG T 0x0846c962：errno!=0 → 报错返 0；否则返 1。
bool CCommunityServerProxy::OnTcpError()
{
    if (errno != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x10f, 0)
            ("CommunityServer OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}

// ORIG T 0x0846c9ec：转发 Parsing_ServerPacket（返回 bool，OnTcpRecv 取反检验）。
bool CCommunityServerProxy::Parsing(int recvLen)
{
    return Parsing_ServerPacket(recvLen);
}

// ORIG T 0x0846ca06：Packet_Community_Login 填用户信息 + PvP 好友载荷后下发。
// 字段/调用序逐条对齐报告汇编（m_id←acc_id, server_id←GetServerGroup,
// channel_no←G_CEnvironment()->get_channel_no, m_uid←GetUID,
// charac_no←getCurCharacNo, growType/level/job/sex, name memcpy 0x1d,
// buddyCount←sig[0], buddies memcpy 0x560）。
void CCommunityServerProxy::SendLogin(CUser* user, SIG_LOAD_PVP_BUDDY* sig)
{
    Packet_Community_Login pkt;
    pkt.m_id = user->get_acc_id();
    pkt.gameUserInfo.user_m_id = user->get_acc_id();
    pkt.gameUserInfo.server_id = user->GetServerGroup();
    pkt.gameUserInfo.channel_no = G_CEnvironment()->get_channel_no();
    pkt.gameUserInfo.m_uid = user->GetUID();
    pkt.gameUserInfo.charac_no = user->getCurCharacNo();
    pkt.gameUserInfo.m_growType = user->getCurCharacGrowType();
    pkt.gameUserInfo.m_level = user->get_charac_level();
    pkt.gameUserInfo.m_job = user->get_charac_job();
    pkt.gameUserInfo.m_sex = user->getSex();
    memcpy(pkt.gameUserInfo.m_name, user->getCurCharacName(), 0x1d);
    pkt.buddyCount = sig->m_buddyCount;
    memcpy(pkt.buddies, sig->m_buddies, 0x560);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}

// ORIG T 0x0846cb44：Packet_Community_Logout(m_id=accId, m_characNo) 下发。
void CCommunityServerProxy::SendLogout(unsigned int accId, unsigned int characNo)
{
    Packet_Community_Logout pkt;
    pkt.m_id = accId;
    pkt.m_characNo = characNo;
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}

// ---- StubS3A_deps 批：Send*PvPBuddy（ORIG 逐指令转写）----
// 通用形态：栈上构造对应包（ctor 预填 header+len+清零），填
// +0x0a accId / +0x0e characNo / +0x12 flag，memcpy(name, nameLen) 到 +0x13，
// SendTcpPacket(pkt, *(ushort*)(pkt+2)=packetSize)。

// ORIG T 0x0846cbbc _ZN21CCommunityServerProxy18SendReqAddPvPBuddyEjjcPci
void CCommunityServerProxy::SendReqAddPvPBuddy(
    unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen)
{
    Packet_Request_Add_PvP_Buddy pkt;   // ORIG W ctor 0x0846cf34
    pkt.m_accId = accId;
    pkt.m_characNo = characNo;
    pkt.m_flag = flag;
    memcpy(pkt.m_name, name, nameLen);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}

// ORIG T 0x0846cc22 _ZN21CCommunityServerProxy18SendResAddPvPBuddyEjjcPci
void CCommunityServerProxy::SendResAddPvPBuddy(
    unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen)
{
    Packet_Response_Add_PvP_Buddy pkt;   // ORIG W ctor 0x0846cf90
    pkt.m_accId = accId;
    pkt.m_characNo = characNo;
    pkt.m_flag = flag;
    memcpy(pkt.m_name, name, nameLen);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}

// ORIG T 0x0846cc88 _ZN21CCommunityServerProxy18SendRemovePvPBuddyEjjcPci
void CCommunityServerProxy::SendRemovePvPBuddy(
    unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen)
{
    Packet_Request_Remove_PvP_Buddy pkt;   // ORIG W ctor 0x0846cfec
    pkt.m_accId = accId;
    pkt.m_characNo = characNo;
    pkt.m_flag = flag;
    memcpy(pkt.m_name, name, nameLen);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}
