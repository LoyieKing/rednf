// df_game_r 还原 —— XNuclear::CHades（Hades 反外挂客户端成员类，内嵌于 CUser +0x8e954）。
// 参照 docs/class_funcReports/XNuclear__CHades/：
//   Send_ReturnToVillage @ ORIG 0x084b9386：纯空函数。
//   UseFatigue           @ ORIG 0x084b9552：needLogin 后累加 +0x1c 计数，>1 时
//                        StdCateUpdate(1,0) 并清零。
//   StdCateUpdate        @ ORIG 0x084b938c：needLogin 后发 Packet_Hades_StdUpdate
//                        （PacketHeader(0xfd9,0x12)，+0x0a acc_id，+0x0e/+0x10 cate/type）。
//   needLogin            @ ORIG 0x84b9718：+0x24 连接秒缓存 != proxy 当前值时重新登录。
#include "CUser.h"

#include <string.h>

#include "CHadesServerProxy.h"
#include "GlobalData.h"

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

// ORIG 0x084b9718：缓存(+0x24) 与 proxy 当前连接秒数不一致则重登并同步 cate=3。
bool CHades::needLogin()
{
    int cached = *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x24);
    int cur = GlobalData::s_hades_proxy->GetHadesSocketConnSec();
    if (cached != cur)
    {
        Send_Login();
        StdCateUpdate(3, 0);
    }
    return cached != cur;
}

// ORIG 0x084b938c。Packet_Hades_StdUpdate（ORIG C1 0x84b9a42 W）：
//   PacketHeader(0xfd9, 0x12)；+0x0a dword = user->get_acc_id()；
//   +0x0e word = cate；+0x10 word = type。经 proxy SendHadesTcpPacket 发送。
void CHades::StdCateUpdate(unsigned short cate, unsigned short type)
{
    needLogin();

    // 本地按 ORIG 布局建模的 Packet_Hades_StdUpdate（0x12 字节头区 + 载荷，
    // PacketHeader 具体布局由 PacketHeader 权威实现提供；此处保持字段偏移一致）。推断：
    // 头部 0xa 字节由 PacketHeader(0xfd9,0x12) 写入。
    struct Packet_Hades_StdUpdate
    {
        char m_header[0x0a];        // +0x00 PacketHeader(0xfd9, 0x12)
        unsigned int m_accId;       // +0x0a
        unsigned short m_cate;      // +0x0e
        unsigned short m_type;      // +0x10
    } packet;

    CUser* user = *reinterpret_cast<CUser**>(reinterpret_cast<char*>(this) + 0x20);
    memset(&packet, 0, sizeof(packet));
    {
        // PacketHeader(0xfd9, 0x12)：与 ORIG 一致地写头部两字段
        // （PacketHeader 布局见 PacketHeader 权威定义：+0x00 id、+0x02 size 等）。
        unsigned short* u16 = reinterpret_cast<unsigned short*>(&packet);
        u16[0] = 0xfd9;
        u16[1] = 0x12;
    }
    packet.m_accId = static_cast<unsigned int>(user->get_acc_id());
    packet.m_cate = cate;
    packet.m_type = type;

    GlobalData::s_hades_proxy->SendHadesTcpPacket(
        reinterpret_cast<char*>(&packet));
}

// ORIG 0x084b9200：完整实现需 Packet_Hades_Login / CNetwork::get_mac_addr /
// Send_UserStatus 等未建模类型。此处按可观测主流程近似：刷新连接秒缓存并标记
// 需要重登（推断——登录包载荷构造待后续批次补齐）。
void CHades::Send_Login()
{
    if (GlobalData::s_hades_proxy != 0)
        *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x24) =
            GlobalData::s_hades_proxy->GetHadesSocketConnSec();
}

// ORIG 0x084b9552：needLogin 后计数 +1；累计 >1 时上报 cate=1 并清零。
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
}

}  // namespace XNuclear
