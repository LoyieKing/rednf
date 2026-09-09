#include "LogManager.h"
#include "FunctionGuildMember.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"  // ST_Guild_Mem_Info 完整布局
#include "PacketGuard.h"                                  // PacketGuard 完整类型（put_* 调用）
#include "CMyFileLog.h"
#include "CEnvironment.h"
// df_game_r 还原 —— CGuildServerProxy（公会服务器客户端代理）实现。
// 参照 docs/class_func_reports/CGuildServerProxy.md；单 TU 编译后经
// check_tu_game_orig.sh 与 df_game_r ORIG 对比（AE 口径）。
//
// [b28d-solo-CGuildServerProxy 泳道判词 2026-09-09] 清单 5 件全部 N 封顶，
//   TU 计数 identical+ae=169 / near=1 / diff=5（与基线同值，零劣化）；
//   p167 五符号全 CLEAR（0 MISSING / 0 EXTRA，语义调用点完整）。
//   本轮新范式复验（P309/P311/P316/P226/P227/P211 族）均为负结果，逐件记于函数头。
//
// [b28d-PoolDBLogin 池批·本轮] ≤30 小件 4 件复核（check_tu_routed.sh 现编现测）。
//   TU 三数 基线 identical=30 ae=139 near=1 diff=5 → 现在 identical=30 ae=139
//   near=1 diff=5（**零变差**；两轮 Main-P326 试验各造成 near 1→0 / diff 5→6，
//   均已回滚并 fresh 复测回基线）。修成 0 件，4 件维持封 N（solo_diff 行级读数）：
//    ChangeGuildNameInProxy NEAR ndiff=10（gap=0）：N98 纯槽号互换——end() sret 我方
//      -0xc / ORIG -0x10，pGuild 我方 -0x10 / ORIG -0xc，53=53 逐条同；P326 形实测
//      10→19、ours 53→58（ORIG 比较调点 = 非 const iterator::eq @0x0846ea88，无
//      转换临时槽）⇒ 不适用，读数并列于函数头。
//    SendGameServerInfo DIFF ndiff=6：N75/N107/N18a——ORIG 压栈前 `lea -0x27(%ebp);
//      lea 0xd(%eax),%edx` 两步保活 dst，4.4 恒右起求值且折 `lea+add`；前轮 13 形
//      + P311 元素取址形均不闭合 ⇒ S1 耗尽。
//    ModifyGuildInfoProxy DIFF ndiff=31：N107/N76——逐字段拷贝段 ORIG `lea 0x4(%eax),%edx`
//      物化 dst、src 装 %eax；4.4 恒 dst 留 %eax（+4 折进各 disp，故 62 vs 63 少 1 条）；
//      P326 形实测 31→42（+5 条）已回滚，ORIG 同为非 const eq @0x0846e9a8。
//    SendGuildExpLimit DIFF ndiff=3（gap=-1）：N94——ORIG `add $0x14aa,%edx;
//      mov 0x1c(%eax,%edx,8)` vs 4.4 `mov 0xa56c(%eax,%edx,8)`；0xa56c 非 8 倍数
//      ⇒ 拆常量的数组视角不可构造。
//   本文件 p167 四件全 CLEAR；无 `register ... asm("reg")`；无 .h 改动（R2 无申报项）。
//   超范围发现（交 Main，非本批 ndiff≤30 件）：本 TU 静态初始化器符号名
//   `_GLOBAL__I__ZN12STGuildProxyC2Ev`，ORIG 对应 TU 为
//   `_GLOBAL__I__ZN17CGuildServerProxyC2ESsii`（均 `t` 局部符号，非导出）——
//   4.1/4.4 按「首个动态初始化对象」命名，属跨版本 init-func 命名族。

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

// （static 3 参 int&+ip+port）的调用以 m_tcp 自身地址作 int& 实参（ORIG
// 0846d9be 三槽 this+0x24/c_str()/m_tcpPort 实证）。
// ---- 生命周期 ----

// ---- STGuildProxy（ORIG 弱符号 0x082344de / 0x084ed19a）----

STGuildProxy::STGuildProxy()
{
    m_level = 0;
    m_powerSide = 0;
    m_agitFlag = 0;
    std::memset(&m_guildName, 0, 0x17);   // ORIG：memset(&+0 字段) 先计算地址(加载 this)再装常量(序匹配)
}

STGuildProxy::STGuildProxy(const char* name, unsigned char lev,
                           unsigned char powerSide)
{
    m_level = lev;
    m_powerSide = powerSide;
    std::memcpy(m_guildName, name, 0x16);
}

// ORIG 强符号（0x0846d818 T）：头文件仅声明，此处给出非内联定义。
// [B24D判词 N] 语义正确：string ip/udp/tcp 三参 → m_ip=ip + map C1 + CNetwork C1 +
//   CUdpHandler C1。diff=EH 清理 landing pad 我方多分离一 `jmp` 跳转（4.1 vs 4.4），
//   61/60 纯 lowering 表示差异，语义等价。
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
                              __PRETTY_FUNCTION__, 0x4d4,
                              "[CGuildServerProxy::Init] Can\'t init udp client socket\n");
        return 0;
    }
    return 1;
}

// [b28d-solo-CGuildServerProxy N80: 74/72] P225 while 形已复现 ORIG @0x0846d9c1
//   `cmpl $0x0,-0x1c(%ebp); setne %al; test %al,%al; je`（原 if 形 4.4 恒折
//   `cmpl;je` 少 2 条）。残余=4.1 在 cMyTrace ctor 前把 varargs 右起求值并
//   callee-saved 保活（strerror→ebx、errno→edi、ret→esi ⇒ push edi + sub $0x5c）；
//   4.4 只保活两个调用结果（strerror→ebx、errno→esi），槽变量 ret 恒在使用点
//   `mov -0xc(%ebp),%eax` ⇒ 缺 push edi、帧 -0x10、全槽基差 0x10。P226 语句表达式
//   `({ ret; })`、P227 全表达式临时链均实测零效；P231 裸 register 反使 ret 失槽
//   （ORIG 有槽 -0x1c，违 P0 判读）；b28d 复验追加：varargs 首参改
//   `(*(volatile int*)&ret)` 强制 volatile 读（74→76 劣化，仍缺 push edi）
//   ⇒ N80 封顶。b28d-solo2 复验：ret 声明移至函数头（P212/P309 头部声明律）实测
//   零效（ret 仍 -0xc、帧仍 0x50、缺 push edi）⇒ N80 定案 | ORIG 0x0846d96c
bool CGuildServerProxy::InitTcp()
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (m_tcp.m_sock < 0)
    {
        int ret = m_tcp.CreateConnectionSocket((int&)m_tcp, m_ip.c_str(),
                                               m_tcpPort);
        // [b28d-solo-CGuildServerProxy P225] ORIG @0x0846d9c1 `cmpl $0x0,-0x1c(%ebp);
        // setne %al; test %al,%al; je` —— 4.1 的槽比零物化形，4.4 对 if 恒折
        // `cmpl;je` 直跳；P225 实证 while+break 形可复现（CVillageAttacked/CDropItem）。
        while (ret != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4ea, 0)
                ("CGuildServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x4ef, 9, true, true)
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

bool CGuildServerProxy::OnTcpRecv()
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
                              __PRETTY_FUNCTION__, 0x71f,
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
        cMyTrace(__PRETTY_FUNCTION__, 0x733, 0)
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
        cMyTrace(__PRETTY_FUNCTION__, 0x745, 0)
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
            __PRETTY_FUNCTION__,
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
    // ORIG @846e323：memcpy 源是第二参 guildName（0x10(%ebp)）
    memcpy(pkt.m_guildName, guildName, 0x16);
    memset(&pkt.m_extra[0], 0, 0xd);   // m_extra@+0x25（线格式 0xd 清尾）
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}

// ORIG @846e387：param_1 → m_charNo(+0xe)、param_2 → m_guildKey(+0xa)
// （调用点 Dispatchers.cpp:7786 传 (curNo, guildkey)）。
void CGuildServerProxy::SendGuildNameChange(unsigned int charNo, unsigned int guildKey,
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
        group = (ENUM_SERVER_GROUP)G_CEnvironment()->m_serverEnvir.m_serverGroup;
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

// [b28d-solo-CGuildServerProxy N107/N7: 63/62] 本件已修两处真缺口：
//   ① 删具名 endIt 改内联 end()（ORIG find 落 -0x14、end 落 -0x10；具名 endIt 使
//      4.4 反排成 -0xc/-0x10，与同 TU ChangeGuildNameInProxy 的 ORIG 槽序对齐）；
//   ② 补 pGuild 死槽复现 ORIG @0x0846e9eb/0x0846e9fc 的两次 ptEv（第一次 +0x4 写
//      -0xc 后全程未读，P171 死写入照抄；单语句 `it->second = info;` 只发一次 ptEv）。
//   残余仅逐字段拷贝段：ORIG `lea 0x4(%eax),%edx`（dst 物化 %edx、src 装 %eax），
//   4.4 恒 dst 留 %eax、src 装 %edx 且 +4 折 disp（N107/N18a）。实测 9 形
//   （显式 operator-> / (*it) / *(&it->second) / RHS cast 族 / 具名 q）均不可翻。
//   b28d 复验追加：`*(volatile STGuildProxy*)&it->second`（编译失败：volatile 对象无
//   非 volatile operator=）、`register STGuildProxy* pDst=&it->second; *pDst=info;`
//   （63→65 劣化）均不可用 | ORIG 0x0846e95e
//   b28d-solo2 复验追加：`it->second.operator=(info);` 显式限定调用实测与 `=` 逐条
//   同形（4.4 仍 dst=eax+disp / src=edx），N107/N76 族封顶不变 | ORIG 0x0846e95e
//   b28d-PoolDBLogin 复验（Main P326｜N93 翻案形，一次性）：
//   `it == std::map<unsigned int, STGuildProxy>::const_iterator(m_guildInfoMap.end())`
//   实测 ndiff 31→42、ours 62→67（+5 条 = const_iterator 转换 ctor + 转换后比较调点）
//   ⇒ 本件 ORIG 比较调点是**非 const** `_ZNKSt17_Rb_tree_iteratorISt4pairIKj12STGuildProxyEEeqERKS4_`
//   @0x0846e9a8，仅两槽 -0x14(it)/-0x10(end)，无第三级转换临时 ⇒ P326 形结构性不适用，
//   已回滚（回滚后 fresh 复测 = 基线 63/62 ndiff 31）。
bool CGuildServerProxy::ModifyGuildInfoProxy(unsigned int guildId, STGuildProxy& info)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp", __PRETTY_FUNCTION__, 0x852,
                              "Find fail from guild info proxy(guild_id:%d)", guildId);
        return false;
    }
    else
    {
        // [b28d-solo-CGuildServerProxy P171 死写入照抄] ORIG @0x0846e9eb/0x0846e9fc
        // 对 it->second 连发两次 _Rb_tree_iterator::ptEv：第一次 `add $0x4` 结果
        // 写入 -0xc(%ebp) 后全程未读（死槽），第二次结果 +0x4 作为逐字段拷贝 dst。
        // 该死槽只能由「具名 pGuild 取址 + 另写 it->second」两语句复现（探针
        // /tmp/probe_mgi.cpp f vs f2：单语句只发一次 ptEv）。
        STGuildProxy* pGuild = &it->second;
        it->second = info;
        (void)pGuild;
        return true;
    }
}

// [b28d-solo-CGuildServerProxy N98: 53/53 NEAR] P316 翻案（DIFF→NEAR）：pGuild
//   声明为 `STGuildProxy* volatile` 后，4.4 把 memset/memcpy 的 dst 装载提前到压栈
//   之前（ORIG @0x0846ead7 `mov -0xc(%ebp),%eax` 先于 `movl $0x17,0x8(%esp)`），
//   memcpy src 自然落 %edx（ORIG `mov 0x10(%ebp),%edx` 同形）⇒ 求值序/寄存器全对齐。
//   残余仅槽号互换：4.4 给 volatile pGuild=-0x10、end() 内联临时=-0xc，ORIG 反之
//   （pGuild=-0xc、end 临时=-0x10）。已实测不闭合：①声明移至函数头最前（it 得
//   -0x14 ✓ 但 pGuild/end 仍互换）；②具名 endIt（it/endIt 互换）；③
//   `*(STGuildProxy* volatile*)&pGuild` 读形（+2 条）。均属 N98 槽分配族 ⇒ 封顶
//   | ORIG 0x0846ea3e。b28d-solo2 复验追加：④`end()==it` 操作数互换（P74补②）；
//   ⑤`*&m_guildInfoMap.end()`（P316 sret 折叠抑制）；⑥pGuild 声明移到 it 之后——
//   三形 end() sret 仍恒 -0xc（it -0x14 / pGuild -0x10 或反向），槽号互换不变 ⇒ 定案。
//   ⑦pGuild 移入 else 块内声明（P314 块级旋钮）⇒ pGuild -0x14 / it -0x10 更劣，已回滚。
//   b28d-PoolDBLogin 复验（Main P326｜N93 翻案形，一次性）：同形实测 ndiff 10→19、
//   ours 53→58、NEAR→DIFF（TU 三数 near 1→0 / diff 5→6 **变差**）⇒ 本件 ORIG 比较
//   调点同为**非 const** iterator::eq @0x0846ea88，三级槽 = -0x14(it) / -0x10(end) /
//   -0xc(pGuild 指针，非迭代器临时)，无转换临时可落 ⇒ P326 形不适用，已回滚；
//   N98 槽号互换维持封顶。
void CGuildServerProxy::ChangeGuildNameInProxy(unsigned int guildId, char* name)
{
    STGuildProxy* volatile pGuild;
    // 槽序：find 结果 -0x14（与 ORIG 一致）；end() 临时与 pGuild 互换（N98 残差）。
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp", __PRETTY_FUNCTION__, 0x860,
                              "Find fail from guild info proxy(guild_id:%d)", guildId);
    }
    else
    {
        pGuild = &it->second;
        memset(pGuild, 0, 0x17);
        memcpy(pGuild, name, 0x16);
    }
}

void CGuildServerProxy::ChangeGuildAgitFlagInProxy(unsigned int guildId,
                                                   unsigned char agitFlag)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              __PRETTY_FUNCTION__,
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
                              __PRETTY_FUNCTION__,
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
    pkt.m_charNo = guildKey;
    pkt.m_guildKey = charNo;
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

// [b28d-solo-CGuildServerProxy N94: 43/42] 唯一残差=循环体 scaled-index 大常量拆分：
//   ORIG @0x0846ee96 `mov -0xc(%ebp),%edx; add $0x14aa,%edx; mov 0x1c(%eax,%edx,8),%eax`
//   （C%8=4 拆成 add+disp8），4.4 恒折单条 `mov 0xa56c(%eax,%edx,8),%eax`
//   （N94 定论：4.4 仅当 C%8==0 才拆，源形不可择）⇒ 封顶 | ORIG 0x0846ee44
//   b28d-solo2 复验追加：`*(m_levelUpCost[i] + 1)` 指针形劣化为 shl+add+add+mov 6 条；
//   P311 二维视图需 X*C=0x14aa 且第二维长 ≥0x11，本布局 0xa56c 非 8 倍数不可构造 ⇒ 定案。
void CGuildServerProxy::SendGuildExpLimit()
{
    Packet_Guild_Exp_Limit pkt;
    pkt.m_channel = G_CEnvironment()->get_channel_no();
    pkt.m_guildExpLimit8 = G_CDataManager()->m_guildParameter.m_levelUpCost[8][1];
    pkt.m_guildExpLimit16 = G_CDataManager()->m_guildParameter.m_levelUpCost[16][1];
    for (int i = 0; i < 0x11; i++)
    {
        pkt.m_guildExpLimit[i] = G_CDataManager()->m_guildParameter.m_levelUpCost[i][1];
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
    // [b26L17 判词 N7] 唯一残差：两处虚调用 callee 装载 %eax↔%edx 互换
    //   （ORIG vtable 槽→edx、实参→eax；4.4 恒反）。118/118 助记符全等。
    if (item->GetGuildPowerWarPoint() != 0)   // vtable+0x28（槽 10）
    {
        user->DecreaseGuildPowerWarPoint(powerWarPoint);
        SendIncreaseDecreasePowerWarPoint(user->get_charac_guildkey(),
                                          user->get_charac_no(-1),
                                          -(int)powerWarPoint);
    }
    if (item->GetUserPowerWarPoint() != 0)    // vtable+0x2c（槽 11）
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
    // ORIG @846f0c1：0x10(guildKey)→+0x12、0x14(charNo)→+0xe（字段命名与参数交叉，按 ORIG）
    pkt.m_charNo = guildKey;
    pkt.m_guildKey = charNo;
    pkt.m_boardNo = boardNo;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}
// [b28d-solo-CGuildServerProxy N75/N107/N18a: 36/36] ORIG @0x0846f130 在压 strncpy
//   常量前 `lea -0x27(%ebp),%eax; lea 0xd(%eax),%edx` 两步保活 dst；4.4 恒右起求值
//   （dst 装载落最后）且折 `lea+add`。P226 语句表达式（({ pkt.m_name; })）、具名指针
//   中间量、P231 裸 register（v28 探针：双 lea+顺序对，但改用 %ebx 且 push/pop 破帧）
//   等 13 形实测均不闭合 ⇒ 封顶 | ORIG 0x0846f0f2
//   b28d-solo2 复验追加：`strncpy(&pkt.m_name[0], name, 0x10)`（P311 数组元素取址）
//   与 `pkt.m_name` 逐条同形（4.4 仍右起求值 + add）⇒ P75 跨版本求值序封顶不变。
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

// ---- FunctionGuildMember（ORIG 静态成员，公会成员包构造/日志辅助）----

// ORIG 0x084ca37a T _ZN19FunctionGuildMember26MakePacketGuildMembersInfoER11PacketGuardiP17ST_Guild_Mem_Info
// 报告 docs/class_func_reports/FunctionGuildMember/MakePacketGuildMembersInfo.md：
// put_byte(count) → 每成员：name(0x04)/guildMemDBInfo(0x22) int+len、
// level、job/growthType/char 渠道/性别/黑名单/MG、pad 秒差。
void FunctionGuildMember::MakePacketGuildMembersInfo(PacketGuard& packet,
                                                     int count,
                                                     ST_Guild_Mem_Info* members)
{
    packet.put_byte(count);
    int nameLen = 0;
    int infoLen = 0;
    const long long curSec = GlobalData::s_systemTime_.getCurSec();
    while (count-- != 0)
    {
        nameLen = (int)strlen(members->f.m_name);
        packet.put_int(nameLen);
        packet.put_str(&members->f.m_name[0], nameLen);
        infoLen = (int)strlen(members->f.m_guildMemDBInfo);
        packet.put_int(infoLen);
        packet.put_str(&members->f.m_guildMemDBInfo[0], infoLen);
        packet.put_short((int)members->f.m_level);
        packet.put_byte((int)members->f.m_job);
        packet.put_byte((int)members->f.m_growthType);
        packet.put_byte((int)(unsigned char)members->f.m_channel);
        packet.put_byte((int)members->f.m_sex);
        packet.put_byte((int)members->f.m_blackUser);
        packet.put_byte((int)(unsigned char)members->f.m_grade);
        // ORIG +0x3b（f.m_pad3b，P1 单 disp）：0 或 curSec<secs → 0，否则差
        if (members->f.m_pad3b == 0 ||
            curSec < members->f.m_pad3b)
            packet.put_int(0);
        else
            packet.put_int((int)(curSec - members->f.m_pad3b));
        members = (members + 1);
    }
}

// ORIG 0x084ca55a T _ZN19FunctionGuildMember23LogMakeGuildMembersInfoEjiP17ST_Guild_Mem_Info
// 报告 LogMakeGuildMembersInfo.md：拼成员文本 → ./log/<file>/GuildModi 文件日志。
void FunctionGuildMember::LogMakeGuildMembersInfo(unsigned int guildKey,
                                                  int count,
                                                  ST_Guild_Mem_Info* members)
{
    char buf[0x1000] = { '\0' };   // P4：O0 内联 rep stos，与 ORIG 同形
    sprintf(buf, "gKey(%d) Cnt(%d)", guildKey, count);
    while (count-- != 0)
    {
        sprintf(buf, "%s %s", buf, members->f.m_name);
        sprintf(buf, "%s l(%d)", buf, (int)members->f.m_level);
        sprintf(buf, "%s j(%d)", buf, (int)members->f.m_job);
        sprintf(buf, "%s g(%d)", buf, (int)members->f.m_growthType);
        sprintf(buf, "%s Ch(%d)", buf, (int)(unsigned char)members->f.m_channel);
        sprintf(buf, "%s S(%d)", buf, (int)members->f.m_sex);
        sprintf(buf, "%s B(%d)", buf, (int)members->f.m_blackUser);
        sprintf(buf, "%s MG(%d)", buf, (int)(unsigned char)members->f.m_grade);
        sprintf(buf, "%s\n", buf);
        if (strlen(buf) > 0xfff)
            break;
        members = (members + 1);
    }
    char path[0x100] = { '\0' };   // P4：O0 内联 rep stos，与 ORIG 同形
    sprintf(path, "./log/%s/GuildModi", G_CEnvironment()->get_file_name());
    CMyFileLog log(
        __PRETTY_FUNCTION__,
        0x24ac);
    log(path, "%s", buf);
}
