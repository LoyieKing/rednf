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
#include "GlobalData.h"
#include "Packet_Hades_Login.h"
#include "Packet_Hades_StdUpdate.h"
#include "Packet_Hades_GoldTracking.h"
#include "Packet_Hades_UserStatus.h"

namespace XNuclear
{

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

    unsigned int status = 0;

    // ORIG 逐次调用 getSecuType（不缓存中间值）；guard 经 callee-saved 寄存器
    // 承载三路判定结果（ORIG mov $1/$0 + jmp 汇合形态），register 诱导分配。
    register bool hasSecuGuard;
    if ((m_user->getSecuType() & 1) != 0)
    {
        hasSecuGuard = true;
    }
    else if ((m_user->getSecuType() & 2) != 0)
    {
        hasSecuGuard = true;
    }
    else if ((m_user->getSecuType() & 0x20) != 0)
    {
        hasSecuGuard = true;
    }
    if (hasSecuGuard)
        status = status | 1;

    if ((m_user->getSecuType() & 8) != 0)
        status = status | 2;

    if ((m_user->getSecuType() & 0x10) != 0)
        status = status | 4;

    packet.m_status = status;

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

}  // namespace XNuclear
