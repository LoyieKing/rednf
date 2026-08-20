#include "LogManager.h"
// df_game_r 还原 —— CGuildServerProxy（公会服务器客户端代理）实现。
// 参照 docs/class_func_reports/CGuildServerProxy.md；单 TU 编译后经
// check_tu_game_orig.sh 与 df_game_r ORIG 对比（AE 口径）。

#include "CGuildServerProxy.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "CUser.h"
#include "CDataManager.h"

#include "Packet_Add_Guild_Fund.h"
#include "Packet_Change_Char_Name.h"
#include "Packet_Change_Guild_Name.h"
#include "Packet_Game_Server_Regist.h"
#include "Packet_GM_PowerWar_End.h"
#include "Packet_GM_PowerWar_Start.h"
#include "Packet_Guild_Apply_Origial_Power_Side.h"
#include "Packet_Guild_Call_Guild_Invite.h"
#include "Packet_Guild_Change_Power_War_Point.h"
#include "Packet_Guild_Create_Guild_Agit.h"
#include "Packet_Guild_Delete_Guild_Agit.h"
#include "Packet_Guild_Exp_Limit.h"
#include "Packet_Guild_Increase_Decrease_Power_War_Point.h"
#include "Packet_Guild_Power_War_Process_Info.h"
#include "Packet_Guild_Reply_Guild_Invite_From_Invited.h"
#include "Packet_Guild_Request_Guild_Board_Delete.h"
#include "Packet_Guild_Request_Guild_Board_Open.h"
#include "Packet_Guild_Request_Guild_Board_Write.h"
#include "Packet_Guild_Request_Guild_Master_Delegate.h"
#include "Packet_Guild_Request_Guild_Secede.h"
#include "Packet_Guild_Upgrade_Guild_Agit.h"
#include "Packet_Guild_Write_Guild_Member_Memo.h"
#include "Packet_Monitor_Buy_Guild_Skill.h"
#include "Packet_Monitor_Call_Guild_All_Members.h"
#include "Packet_Monitor_Call_Guild_Info.h"
#include "Packet_Monitor_Call_Guild_Level_Up.h"
#include "Packet_Monitor_Call_Guild_Members.h"
#include "Packet_Monitor_Char_Changable_Info.h"
#include "Packet_Monitor_Increase_Guild_Exp.h"
#include "Packet_Monitor_Send_Guild_Mail.h"
#include "Packet_Monitor_UDP_HeartBeat.h"
#include "Packet_Monitor_UDP_Login.h"
#include "Packet_Monitor_UDP_Logout.h"
#include "Packet_Notice_Guild_War_End.h"
#include "Packet_Notice_Guild_War_Start.h"
#include "Packet_Notice_Power_War_CFG.h"
#include "Packet_Refresh_Guild_Info.h"
#include "Packet_Request_Guild_Create.h"
#include "Packet_Request_Join_Power.h"
#include "Packet_Request_Power_War_Start_Info.h"
#include "Packet_Request_Secede_Power.h"
#include "Packet_Web_Notify_Message_To_Guild.h"

#include "DNFFunctionLib.h"

// ORIG DF_GuildServerProxy.cpp 对 CNetwork<100000,100000>::CreateConnectionSocket
// 的调用只传 this+ip+port 三个栈槽（声明为 3 参 int&+ip+port，调用点按 2 参
// 使用——原样复刻）。本视图类以 2 参成员复现调用点形态；定义转发到真实 3 参
// 成员（int& 实参丢弃），保证最终链接可解析。
namespace
{
// CEnvironment+0x378 视图（m_serverEnvir.m_serverGroup，私有成员；ORIG 直访）
struct CEnvView
{
    char m_pad[0x378];
    int m_serverGroup;    // +0x378
};

class CGuildNetView
{
public:
    int CreateConnectionSocket(const char* ip, int port);
};

int CGuildNetView::CreateConnectionSocket(const char* ip, int port)
{
    int sock = 0;
    return ((CNetwork<100000, 100000>*)this)->CreateConnectionSocket(sock, ip, port);
}
}

// ---- 生命周期 ----

// ---- STGuildProxy（ORIG 弱符号 0x082344de / 0x084ed19a）----

STGuildProxy::STGuildProxy()
{
    m_level = 0;
    m_powerSide = 0;
    m_agitFlag = 0;
    std::memset(this, 0, 0x17);
}

STGuildProxy::STGuildProxy(const char* name, unsigned char lev,
                           unsigned char powerSide)
{
    m_level = lev;
    m_powerSide = powerSide;
    std::memcpy(m_guildName, name, 0x16);
}

// ORIG 强符号（0x0846d818 T）：头文件仅声明，此处给出非内联定义。
CGuildServerProxy::CGuildServerProxy(std::string ip, int udpPort, int tcpPort)
    : m_ip(ip), m_udpPort(udpPort), m_guildInfoMap(), m_tcpPort(tcpPort),
      m_tcp(), m_udp()
{
}

bool CGuildServerProxy::Init()
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
    return 1;
}

bool CGuildServerProxy::InitUdp()
{
    if (m_udp.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              "bool CGuildServerProxy::InitUdp()", 0x4d4,
                              "[CGuildServerProxy::Init] Can\'t init udp client socket\n");
        return 0;
    }
    return 1;
}

bool CGuildServerProxy::InitTcp()
{
    int ret;
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (m_tcp.m_sock < 0)
    {
        ret = ((CGuildNetView*)&m_tcp)
                  ->CreateConnectionSocket(m_ip.c_str(), m_tcpPort);
        while (ret != 0)
        {
            cMyTrace("bool CGuildServerProxy::InitTcp()", 0x4ea, 0)
                ("CGuildServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    cMyTrace("bool CGuildServerProxy::InitTcp()", 0x4ef, 9, true, true)
        ("[!] Connect To Guild Server ...");
    return 1;
}

void CGuildServerProxy::Disconnect()
{
    m_tcp.disconnect();
}

// ---- 收发 ----

void CGuildServerProxy::SendPacket(char* p, int len)
{
    if (m_udpPort != 0)
    {
        m_udp.SendToServer(p, len, (unsigned short)m_udpPort, m_ip.c_str());
    }
    return;
}

void CGuildServerProxy::SendTcpPacket(char* p, int len)
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

int CGuildServerProxy::OnTcpRecv()
{
    int n = m_tcp.recv_packet();
    if (n > 0)
    {
        if (Parsing(n) == false)
        {
            return 0;
        }
        return 1;
    }
    if (n < 0)
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              "bool CGuildServerProxy::OnTcpRecv()", 0x71f,
                              "Guild Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
}

int CGuildServerProxy::OnTcpSend()
{
    if (m_tcp.send_packet() < 0)
    {
        cMyTrace("int CGuildServerProxy::OnTcpSend()", 0x733, 0)
            ("OnSend() Socket[%d] Error(%s)", GetTcpHandler()->get_socket(),
             strerror(errno));
        Disconnect();
        return -1;
    }
    return m_tcp.m_recvOffset;
}

bool CGuildServerProxy::OnTcpError()
{
    if (errno != 0)
    {
        cMyTrace("bool CGuildServerProxy::OnTcpError()", 0x745, 0)
            ("GuildServer OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}

bool CGuildServerProxy::Parsing(int len)
{
    return m_tcp.Parsing_ServerPacket(len);
}

// ---- Send*：登录/心跳/信息上报 ----

void CGuildServerProxy::SendCharLevelGrowType(unsigned int dbid, unsigned int growType,
                                              short guildMemFlag, short level)
{
    Packet_Monitor_Char_Changable_Info pkt;
    pkt.m_dbid = dbid;
    pkt.m_padE = (char)growType;
    pkt.m_guildMemFlag = guildMemFlag;
    pkt.m_growthType = (char)level;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendHeartBeat(int channel)
{
    Packet_Monitor_UDP_HeartBeat pkt;
    pkt.m_channel = (unsigned char)channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendLogin(int guildKey, unsigned int dbid, int channel,
                                  char* ip, unsigned char sex, char* ssn)
{
    Packet_Monitor_UDP_Login pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_dbid = dbid;
    pkt.m_channel = (unsigned char)channel;
    if (ip != 0)
    {
        strncpy(pkt.m_ip, ip, 0x10);
    }
    else
    {
        LogManager::logFormat(
            1, "DF_GuildServerProxy.cpp",
            "void CGuildServerProxy::SendLogin(int, memberIdentificationNumber_t, int, char*, unsigned char, char*)",
            0x533, "CGuildServerProxy::SendLogin! DB ID(%s)", NumberToString(dbid, 0));
    }
    pkt.m_sex = sex;
    memcpy(pkt.m_ssn, ssn, 6);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendLogout(unsigned int dbid, int channel, char* name,
                                   unsigned int cache0, unsigned int cache4,
                                   unsigned int memberKey, unsigned char foc)
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
        if (name != 0)
        {
            strcpy(pkt.m_name, name);
        }
        SendTcpPacket((char*)&pkt, pkt.packetSize);
    }
    return;
}

void CGuildServerProxy::SendPacketCallGuildMembers(unsigned int charNo,
                                                   unsigned int guildKey)
{
    Packet_Monitor_Call_Guild_Members pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendPacketCallGuildAllMembers(unsigned int charNo,
                                                      unsigned int guildKey)
{
    Packet_Monitor_Call_Guild_All_Members pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildLevelup(int uid, unsigned int charNo,
                                         unsigned int guildKey, unsigned int fund)
{
    Packet_Monitor_Call_Guild_Level_Up pkt;
    pkt.m_uid = uid;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_fund = fund;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendCallGuildInfo(unsigned int charNo, unsigned int uid,
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

void CGuildServerProxy::SendPowerWarStart(unsigned int mid)
{
    if (GlobalData::s_pGMAccounts_->isGM(mid) == false)
    {
        return;
    }
    Packet_GM_PowerWar_Start pkt;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendPowerWarEnd(unsigned int mid)
{
    if (GlobalData::s_pGMAccounts_->isGM(mid) == false)
    {
        return;
    }
    Packet_GM_PowerWar_End pkt;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendPowerWarProcessInfo(unsigned int channel)
{
    Packet_Guild_Power_War_Process_Info pkt;
    pkt.m_a = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildWarEnd(unsigned char channel)
{
    Packet_Notice_Guild_War_End pkt;
    pkt.m_channel = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildWarStart(unsigned char channel)
{
    Packet_Notice_Guild_War_Start pkt;
    pkt.m_channel = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendPowerWarCfg(unsigned short endKillPoint, int basic,
                                        int first, int decrease, int ranking)
{
    Packet_Notice_Power_War_CFG pkt;
    pkt.m_endKillPoint = endKillPoint;
    pkt.m_basic = basic;
    pkt.m_first = first;
    pkt.m_decrease = decrease;
    pkt.m_ranking = ranking;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendBuyGuildSkill(unsigned int charNo, unsigned int guildKey,
                                          int skillId, int skillLevel, short slot,
                                          unsigned int cost)
{
    Packet_Monitor_Buy_Guild_Skill pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_skillId = skillId;
    pkt.m_skillLevel = skillLevel;
    pkt.m_slot = slot;
    pkt.m_cost = cost;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildMail(unsigned int charNo, unsigned int guildId,
                                      int len, char* msg)
{
    Packet_Monitor_Send_Guild_Mail pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildId = guildId;
    memcpy(pkt.m_msg, msg, len);
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendCallGuildInvite(unsigned int caller, unsigned int guildKey,
                                            char* name)
{
    Packet_Guild_Call_Guild_Invite pkt;
    pkt.m_caller = caller;
    pkt.m_guildKey = guildKey;
    memcpy(pkt.m_name, name, 0x1d);
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendReplyGuildInvite(unsigned int charNo, unsigned char accept)
{
    Packet_Guild_Reply_Guild_Invite_From_Invited pkt;
    pkt.m_charNo = charNo;
    pkt.m_accept = accept;
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendRequestGuildSecede(unsigned int guildKey,
                                               unsigned int requesterCharNo,
                                               int nameLen, char* name)
{
    Packet_Guild_Request_Guild_Secede pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_requesterCharNo = requesterCharNo;
    pkt.m_nameLen = nameLen;
    if (nameLen > 0)
    {
        memcpy(pkt.m_name, name, nameLen);
    }
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendMessageToGuild(unsigned int guildKey, int len, char* msg)
{
    Packet_Web_Notify_Message_To_Guild pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_mode = 2;
    if (len < 0x65)
    {
        memcpy(pkt.m_payload, msg, len);
    }
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendMemoToGuild(unsigned int guildKey, int charNo,
                                        int len, char* memo)
{
    Packet_Guild_Write_Guild_Member_Memo pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    memcpy(pkt.m_memo, memo, len);
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildMasterDelegate(unsigned int guildKey,
                                                unsigned int requesterCharNo,
                                                int len, char* name)
{
    Packet_Guild_Request_Guild_Master_Delegate pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_requesterCharNo = requesterCharNo;
    memcpy(pkt.m_name, name, len);
    SendPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildCreate(unsigned int charNo, char* guildName,
                                        char* masterName)
{
    Packet_Request_Guild_Create pkt;
    pkt.m_charNo = charNo;
    memcpy(pkt.m_guildName, guildName, 0x16);
    memset((char*)&pkt + 0x25, 0, 0xd);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildNameChange(unsigned int guildKey, unsigned int charNo,
                                            char* name)
{
    Packet_Change_Guild_Name pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    strcpy(pkt.m_name, name);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendChangeCharName(unsigned int idByChannel,
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

void CGuildServerProxy::SendReqestPowerWarStartInfo()
{
    Packet_Request_Power_War_Start_Info pkt;
    pkt.m_channel = G_CEnvironment()->get_channel_no();
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendPowerWarPoint(Packet_Guild_Change_Power_War_Point& pkt)
{
    if (GlobalData::s_event_manager->GetRepeatEvent(0x1e)->IsEventing(0))
    {
        SendTcpPacket((char*)&pkt, pkt.packetSize);
    }
}

void CGuildServerProxy::SendJoinPower(unsigned int charNo, unsigned int guildKey,
                                      unsigned char powerSide)
{
    Packet_Request_Join_Power pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_powerSide = powerSide;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendSecedePower(unsigned int charNo, unsigned int guildKey)
{
    Packet_Request_Secede_Power pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

// ---- 公会信息缓存 ----

bool CGuildServerProxy::LoadGuildInfoProxyFromDB(ENUM_SERVER_GROUP group)
{
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    if (group == E_SERVER_GROUP_NONE)
    {
        group = (ENUM_SERVER_GROUP)((CEnvView*)G_CEnvironment())->m_serverGroup;
    }
    db->set_query("seLect guild_id, guild_name, lev , power_side, guild_agit_flag "
                  "from guild_info where expire_flag = 0 and server_id = %d", group);
    ok = db->exec(true);
    if (ok == false)
    {
        return 0;
    }
    int rows = db->get_n_rows();
    for (int i = 0; i < rows; i++)
    {
        ok = db->fetch();
        if (ok == false)
        {
            break;
        }
        unsigned int guildId;
        STGuildProxy info;
        ok = db->get_uint(0, guildId);
        if (ok == false)
        {
            return 0;
        }
        ok = db->get_str(1, info.m_guildName, 0x16);
        if (ok == false)
        {
            return 0;
        }
        ok = db->get_ubyte(2, info.m_level);
        if (ok == false)
        {
            return 0;
        }
        ok = db->get_ubyte(3, info.m_powerSide);
        if (ok == false)
        {
            return 0;
        }
        ok = db->get_ubyte(4, info.m_agitFlag);
        if (ok == false)
        {
            return 0;
        }
        if (m_guildInfoMap.insert(std::make_pair(guildId, info)).second == false)
        {
            return 0;
        }
    }
    return 1;
}

bool CGuildServerProxy::ModifyGuildInfoProxy(unsigned int guildId, STGuildProxy& info)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              "bool CGuildServerProxy::ModifyGuildInfoProxy(unsigned int, STGuildProxy&)",
                              0x852, "Find fail from guild info proxy(guild_id:%d)", guildId);
        return false;
    }
    STGuildProxy* pGuild = &it->second;
    it->second = info;
    return true;
}

void CGuildServerProxy::ChangeGuildNameInProxy(unsigned int guildId, char* name)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              "void CGuildServerProxy::ChangeGuildNameInProxy(unsigned int, char*)",
                              0x860, "Find fail from guild info proxy(guild_id:%d)", guildId);
        return;
    }
    STGuildProxy* pGuild = &it->second;
    memset(pGuild, 0, 0x17);
    memcpy(pGuild, name, 0x16);
}

void CGuildServerProxy::ChangeGuildAgitFlagInProxy(unsigned int guildId,
                                                   unsigned char agitFlag)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              "void CGuildServerProxy::ChangeGuildAgitFlagInProxy(unsigned int, unsigned char)",
                              0x86e, "Find fail from guild info proxy(guild_id:%d)", guildId);
        return;
    }
    STGuildProxy* pGuild = &it->second;
    // ORIG @846ebb4 写入 +0x18（本结构 m_powerSide 槽；DB 装载器把列 3
    // power_side 也读入 +0x18 —— ORIG 两处同址，原样复刻）
    pGuild->m_powerSide = agitFlag;
}

bool CGuildServerProxy::GetGuildInfoProxy(unsigned int guildId, STGuildProxy& out)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              "bool CGuildServerProxy::GetGuildInfoProxy(unsigned int, STGuildProxy&)",
                              0x87d, "Find fail from guild info proxy(guild_id:%d)", guildId);
        return false;
    }
    out = it->second;
    return true;
}

bool CGuildServerProxy::InsertGuildInfoProxy(unsigned int guildId, STGuildProxy& info)
{
    return m_guildInfoMap.insert(std::make_pair(guildId, info)).second;
}

// ---- 公会资金/经验/PowerWar/根据地/公告板 ----

void CGuildServerProxy::SendIncreaseGuildExp(unsigned int charNo, unsigned int guildKey,
                                             unsigned int addExp, char notify)
{
    Packet_Monitor_Increase_Guild_Exp pkt;
    // ORIG @846ece2：param_1 → m_guildKey(+0xe)，param_2 → m_charNo(+0xa)
    pkt.m_guildKey = charNo;
    pkt.m_charNo = guildKey;
    pkt.m_addExp = addExp;
    pkt.m_expType = 1;
    pkt.m_notify = notify;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendIncreaseDecreasePowerWarPoint(unsigned int guildKey,
                                                          unsigned int charNo,
                                                          int point)
{
    Packet_Guild_Increase_Decrease_Power_War_Point pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_charNo = charNo;
    pkt.m_point = point;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendCreateGuildAgit(unsigned int guildKey, unsigned int charNo,
                                            unsigned int powerWarPoint, unsigned int fund)
{
    Packet_Guild_Create_Guild_Agit pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_charNo = charNo;
    pkt.m_powerWarPoint = powerWarPoint;
    pkt.m_fund = fund;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendDeleteGuildAgit(unsigned int guildKey, unsigned int charNo)
{
    Packet_Guild_Delete_Guild_Agit pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_charNo = charNo;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendUpgradeGuildAgit(unsigned int guildKey, unsigned int charNo,
                                             unsigned int powerWarPoint, unsigned int fund)
{
    Packet_Guild_Upgrade_Guild_Agit pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_charNo = charNo;
    pkt.m_powerWarPoint = powerWarPoint;
    pkt.m_fund = fund;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildExpLimit()
{
    Packet_Guild_Exp_Limit pkt;
    pkt.m_channel = G_CEnvironment()->get_channel_no();
    pkt.m_guildExpLimit8 = G_CDataManager()->m_guildParameter.m_levelUpCost[8][0];
    pkt.m_guildExpLimit16 = G_CDataManager()->m_guildParameter.m_levelUpCost[16][0];
    for (int i = 0; i < 0x11; i++)
    {
        pkt.m_guildExpLimit[i] = G_CDataManager()->m_guildParameter.m_levelUpCost[i][0];
    }
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendApplyOriginalPowerSide(unsigned int charNo,
                                                   unsigned int guildKey,
                                                   unsigned char powerSide)
{
    Packet_Guild_Apply_Origial_Power_Side pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_powerSide = powerSide;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::OnBuyItem(CUser* user, CItem const* item,
                                  unsigned int powerWarPoint, unsigned int pp)
{
    if ((*(int (**)(CItem*))(*(int*)item + 0x28))((CItem*)item) != 0)
    {
        user->DecreaseGuildPowerWarPoint(powerWarPoint);
        SendIncreaseDecreasePowerWarPoint(user->get_charac_guildkey(),
                                          user->get_charac_no(-1),
                                          -(int)powerWarPoint);
    }
    if ((*(int (**)(CItem*))(*(int*)item + 0x2c))((CItem*)item) != 0)
    {
        user->DecreasePowerWarPoint((short)pp);
    }
}

void CGuildServerProxy::SendGuildBoardOpen(unsigned int charNo, unsigned int guildKey,
                                           char boardType)
{
    Packet_Guild_Request_Guild_Board_Open pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_boardType = (char)(boardType != 0);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildBoardWrite(char* info, unsigned int boardNo,
                                            unsigned int charNo, int job,
                                            char growType, unsigned int guildKey)
{
    Packet_Guild_Request_Guild_Board_Write pkt;
    pkt.m_boardNo = boardNo;
    pkt.m_charNo = charNo;
    pkt.m_job = (unsigned char)job;
    pkt.m_growType = (unsigned char)growType;
    pkt.m_guildKey = guildKey;
    memcpy(pkt.m_info, info, 0x78);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGuildBoardDelete(unsigned int boardNo,
                                             unsigned int guildKey,
                                             unsigned int charNo)
{
    Packet_Guild_Request_Guild_Board_Delete pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_boardNo = boardNo;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendGameServerInfo(char* name, unsigned short port,
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

void CGuildServerProxy::SendRefreshGuildInfo(unsigned int charNo, unsigned int guildKey)
{
    Packet_Refresh_Guild_Info pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

void CGuildServerProxy::SendAddGuildFund(unsigned int charNo, unsigned int guildKey,
                                         int fund, Packet_Add_Guild_Fund::eFundType type)
{
    Packet_Add_Guild_Fund pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_fund = fund;
    pkt.m_mode = (unsigned char)type;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}
