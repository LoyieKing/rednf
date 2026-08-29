// df_game_r 还原 —— LinkClosureC 批次：CAutoMarketConditionsControl 单用户耐久通知。
// ORIG 依据：报告 CAutoMarketConditionsControl/SendNotiSetDurability_082f85ea.md
//   _ZN28CAutoMarketConditionsControl20SendNotiSetDurabilityEP5CUser T 0x082f85ea
#include "CDataManager.h"   // CAutoMarketConditionsControl 权威类（CDataManager.h 内声明）

#include "CUser.h"          // CUser::Send(PacketGuard&)
#include "PacketGuard.h"

// MakeNotiSetDurability —— ORIG T 0x082f84f8（LinkMiscSecuTimerSolo.cpp 定义）。
void MakeNotiSetDurability(PacketGuard& packet, int value);

// ORIG 0x082f85ea：m_field3e8(+0x3e8)==0 直接返回；否则
// MakeNotiSetDurability(packet, (int)(m_durCurrent(+0x43c double) * 1000.0))
// （常量 1000.0 = _DAT_08c1fc58，.rodata 双精度 1000.0），CUser::Send。
void CAutoMarketConditionsControl::SendNotiSetDurability(CUser* user)
{
    if (m_field3e8 == 0)
        return;

    PacketGuard packet;
    MakeNotiSetDurability(packet,
                          static_cast<int>(m_durCurrent * 1000.0));
    user->Send(packet);
}
