// df_game_r 还原 —— XNuclear::CHades（Hades 反外挂客户端成员类，内嵌于 CUser +0x8e954）。
// 参照 docs/class_func_reports/XNuclear__CHades/：
//   Send_Login           @ ORIG 0x084b9200
//   needLogin            @ ORIG 0x084b9718
//   GetIP                @ ORIG 0x084b9774
//   Send_UserStatus      @ ORIG 0x084b97fe
//   Send_ReturnToVillage @ ORIG 0x084b9386（纯空函数）
#include "CUser.h"
#include "CMCAPManager.h"

#include <string.h>

#include "CEnvironment.h"
#include "CHadesServerProxy.h"
#include "DB_InterDeps.h"   // DB_HadesPunish::makeRequest（ORIG T 0x08445dd4）
#include "GlobalData.h"
#include "Packet_Hades_Login.h"
#include "Packet_Hades_StdUpdate.h"
#include "Packet_Hades_GoldTracking.h"
#include "Packet_Hades_UserStatus.h"
#include "Packet_Hades_Logout.h"
#include "Packet_Hades_AddHackType.h"
#include "Packet_Hades_Gold_Plus.h"
#include "Packet_Hades_Gold_Minus.h"

namespace XNuclear
{

// ORIG 0x084b91e8（Init.md）：挂接 user 指针，清零疲劳计数。
void CHades::Init(CUser* user)
{
    m_user = user;
    m_useFatigueCnt = 0;
}

// ORIG 0x084b92ea（Send_Logout.md）。Packet_Hades_Logout（ORIG C1 0x84b996a W，
// 0xfd3/0xe）：+0x0a acc_id；经 proxy SendHadesTcpPacket 发送。
void CHades::Send_Logout()
{
    Packet_Hades_Logout packet;
    packet.m_accId = m_user->get_acc_id();
    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}

// ORIG 0x084b9322（Send_HackType.md）：纯空函数（5 指令）。
void CHades::Send_HackType()
{
}

// ORIG 0x084b9328（AddHackTypeCnt.md）。Packet_Hades_AddHackType（ORIG C1
// 0x84b998e W，0xfd7/0x1a）：+0x0a acc_id、+0x0e (u16)hackType、+0x10 (u16)count、
// +0x12 param3、+0x16 param4；经 proxy 发送。
void CHades::AddHackTypeCnt(unsigned long hackType, unsigned long count,
                            unsigned long param3, unsigned long param4)
{
    needLogin();

    Packet_Hades_AddHackType packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_hackType = (unsigned short)hackType;
    packet.m_count = (unsigned short)count;
    packet.m_field12 = param3;
    packet.m_field16 = param4;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}

// ORIG 0x084b95a8（ChangeSecurity.md）：仅转调 needLogin()，参数未参与逻辑。
void CHades::ChangeSecurity(ENUM_HADES_SECURITY type, unsigned int value)
{
    (void)type;
    (void)value;
    needLogin();
}

// ORIG 0x084b93f4（Send_ChangeSecurity.md）：纯空函数（5 指令）。
void CHades::Send_ChangeSecurity(ENUM_HADES_SECURITY type)
{
    (void)type;
}

// ORIG 0x084b93fa（GoldPlus.md）。Packet_Hades_Gold_Plus（ORIG C1 0x84b99b2 W，
// 0xfda/0x14）：+0x0a acc_id、+0x0e gold、+0x12 (u16)reason；经 proxy 发送。
void CHades::GoldPlus(unsigned int gold, ENUM_GOLD_CONTROL_REASON reason)
{
    needLogin();

    Packet_Hades_Gold_Plus packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_gold = gold;
    packet.m_reason = (unsigned short)reason;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}

// ORIG 0x084b944a（GoldMinus.md）。Packet_Hades_Gold_Minus（ORIG C1 0x84b99d6 W，
// 0xfdb/0x14）：+0x0a acc_id、+0x0e gold、+0x12 (u16)reason；经 proxy 发送。
void CHades::GoldMinus(unsigned int gold, ENUM_GOLD_CONTROL_REASON reason)
{
    needLogin();

    Packet_Hades_Gold_Minus packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_gold = gold;
    packet.m_reason = (unsigned short)reason;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}

void CHades::Send_ReturnToVillage()
{
    // ORIG 0x084b9386：函数体为空（Ghidra: void Send_ReturnToVillage(void){ return; }）
}

// ORIG 0x084b953e（ExpUp.md）：仅转调 needLogin()，参数未参与逻辑。
void CHades::ExpUp(unsigned int flag)
{
    (void)flag;
    needLogin();
}

// ORIG 0x084b9718：缓存(m_connSec) 与 proxy 当前连接秒数不一致则重登并同步 cate=3。
bool CHades::needLogin()
{
    // ORIG 将缓存值保持在 callee-saved 寄存器（push %ebx）并经 setne/test 判定，
    // register 限定符诱导相同分配形态。
    register int cached = m_connSec;
    register bool differ =
        (GlobalData::s_hades_proxy->GetHadesSocketConnSec() != cached);
    if (differ)
    {
        Send_Login();
        StdCateUpdate(3, 0);
        return true;
    }
    return false;
}

// ORIG 0x084b938c。Packet_Hades_StdUpdate（ORIG C1 0x84b9a42 W，0xfd9/0x12）：
//   +0x0a acc_id、+0x0e cate、+0x10 type；经 proxy SendHadesTcpPacket 发送。
void CHades::StdCateUpdate(unsigned short cate, unsigned short type)
{
    needLogin();

    Packet_Hades_StdUpdate packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_stdCate = cate;
    packet.m_fieldC = type;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}

// ORIG 0x084b9200（Send_Login.md）：刷新连接秒缓存，构造 Packet_Hades_Login
// （ORIG C1 0x84b9946 W，0xfd2/0x64）：+0x0a acc_id、+0x5f channel_no(字节截断)、
// +0x0e ip[16]（先零化局部缓冲 → GetIP → strcpy）、+0x1e mac strncpy 0x41、
// +0x60 uid；发送后转调 Send_UserStatus()。
void CHades::Send_Login()
{
    m_connSec = GlobalData::s_hades_proxy->GetHadesSocketConnSec();

    Packet_Hades_Login packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_channel = (unsigned char)G_CEnvironment()->get_channel_no();

    char ip[16] = { 0 };
    GetIP(ip);
    strcpy(packet.m_ip, ip);

    strncpy(packet.m_mac, m_user->m_network.get_mac_addr(), 0x41);
    packet.m_uid = m_user->GetUID();

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
    Send_UserStatus();
}

// ORIG 0x084b9774（GetIP.md）：peer IP 抓取失败（空串）且 CMCAPManager 内
// PC房 IP(+0x28) 非空时，清零输出缓冲后拷贝该 IP。
void CHades::GetIP(char* out)
{
    m_user->m_network.GetPeerIP2(out, 4);
    if (out[0] == '\0')
    {
        if (m_user->m_mcap.m_pcRoomIp[0] != '\0')
        {
            memset(out, 0, 0x10);
            strncpy(out, m_user->m_mcap.m_pcRoomIp, 0x10);
        }
    }
}

// ORIG 0x084b97fe（Send_UserStatus.md）。Packet_Hades_UserStatus（ORIG C1
// 0x84b9a1e W，0xfdd/0x12）：+0x0a acc_id、+0x0e 状态位域——getSecuType 各位
// 逐次读取：&(1|2|0x20) 任一命中→|1；&8→|2；&0x10→|4。
void CHades::Send_UserStatus()
{
    Packet_Hades_UserStatus packet;
    packet.m_accId = m_user->get_acc_id();

    packet.m_status = 0;

    // ORIG（Send_UserStatus.md）：三路逐次调用 getSecuType()，短路 || 汇聚后 |1。
    if ((m_user->getSecuType() & 1) != 0 ||
        (m_user->getSecuType() & 2) != 0 ||
        (m_user->getSecuType() & 0x20) != 0)
        packet.m_status = packet.m_status | 1;

    if ((m_user->getSecuType() & 8) != 0)
        packet.m_status = packet.m_status | 2;

    if ((m_user->getSecuType() & 0x10) != 0)
        packet.m_status = packet.m_status | 4;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}

// ORIG 0x084b9552（UseFatigue.md）：needLogin 后计数 +1；累计 >1 时上报 cate=1 并清零。
void CHades::UseFatigue(unsigned int count)
{
    (void)count;
    needLogin();
    ++m_useFatigueCnt;
    if (m_useFatigueCnt > 1)
    {
        StdCateUpdate(1, 0);
        m_useFatigueCnt = 0;
    }
    else
    {
        return;
    }
}

// ORIG 0x084b949a（GoldTracking.md）：needLogin 后构造 Packet_Hades_GoldTracking
// （ORIG C1 0x84b99fa W，0xfdc/0x29）：+0x0a acc_id、+0x0e money、+0x12 ip[16]
// （显式 memset 0x10，NULL 跳过 strncpy）、+0x22 param4、+0x26 (u16)fatigue、
// +0x28 type；经 proxy 发送。
void CHades::GoldTracking(unsigned char type, unsigned int money, char* ip,
                          unsigned int param4, unsigned int fatigue)
{
    needLogin();

    Packet_Hades_GoldTracking packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_characId = money;
    memset(packet.m_ip, 0, 0x10);
    if (ip != 0)
        strncpy(packet.m_ip, ip, 0x10);
    packet.m_gold = param4;
    packet.m_usedFatigue = (unsigned short)fatigue;
    packet.m_eventType = type;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}

// ORIG 0x084b93ee（Send_ChangeGold.md）：纯空函数（push ebp/mov/pop/ret，5 指令）。
void CHades::Send_ChangeGold()
{
}

// ORIG 0x084b95bc（RunHadesAction.md）：Hades 行动分发。
//  a1==0x40 跳过历史日志；a1&2 → 标记 hack + 交易惩罚 + DB 上报(2)；
//  a1&1 且非 hack 计时中 → 标记 hack + 开计时 + DB 上报(1)；
//  a1&4 → DisConnSig(0x39, true, 0)。
void CHades::RunHadesAction(unsigned int a1, unsigned int a2, unsigned int a3,
                            unsigned int a4, char* a5)
{
    register bool punish;

    if (a1 != 0x40)
        m_user->m_historyLog.HadesHistoryLog(a1, a2, a3, a4);

    if ((a1 & 2) != 0)
    {
        m_user->setHackUserFlag();
        m_user->SetTradePunishType(4);
        DB_HadesPunish::makeRequest(m_user->get_acc_id(), 2, a2, a3, a4, a5);
    }

    punish = (a1 & 1) != 0 && !m_user->isHackUserTimer();
    if (punish)
    {
        m_user->setHackUserFlag();
        m_user->setHackUserTimer();
        DB_HadesPunish::makeRequest(m_user->get_acc_id(), 1, a2, a3, a4, a5);
    }

    if ((a1 & 4) != 0)
        m_user->DisConnSig((DISCONN_SIG)0x39, true, 0);
}

// ---- CHades_GameSvr（ORIG 空标记类型）----
// ORIG C1/C2 @ 0x082a5a68、D1/D2 @ 0x082a5a6e（各 5 字节 = push ebp/mov esp/pop
// ebp/ret 空体）。GameSvr 侧配置占位；无成员、无基类、无虚函数。
// 全局对象 s_hades_（_ZN10GlobalData8s_hades_E @ 0941f728）ctor 由 ORIG
// __static_initialization_and_destruction_0（0x082a3a5a）调用并注册 atexit。
class CHades_GameSvr
{
public:
    CHades_GameSvr();
    ~CHades_GameSvr();
};

CHades_GameSvr::CHades_GameSvr()
{
}

CHades_GameSvr::~CHades_GameSvr()
{
}

}  // namespace XNuclear
// GlobalData 静态成员（namespace 形态，见 GlobalData.h 注释）；完整类型见上。
XNuclear::CHades_GameSvr GlobalData::s_hades_;
