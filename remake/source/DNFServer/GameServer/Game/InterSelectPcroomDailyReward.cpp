// df_game_r 还原 —— InterSelectPcroomDailyReward::SendPacket（ORIG T 0x81618a6）
//   objdump（081618a6）: 若 user==0 直接返回；否则
//     PacketGuard ctor; put_header(0,0x22e); put_byte((char)user[0x8ec32]);
//     finalize(true); CUser::Send(user,guard); dtor。
//   0x8ec32 = CUser::m_field8ec32（char）。ORIG mangled _ZN28InterSelectPcroomDailyReward10SendPacketEP5CUser。

#include "CInventory.h"   // PacketGuard 完整定义（含 InterfacePacketBuf 基础方法）
#include "CUser.h"
#include "InterSelectPcroomDailyReward.h"

namespace InterSelectPcroomDailyReward
{

void SendPacket(CUser* user)
{
    if (user != 0)
    {
        PacketGuard guard;
        guard.put_header(0, 0x22e);
        guard.put_byte((int)(char)user->m_field8ec32);
        guard.finalize(true);
        user->Send(guard);
    }
}

}  // namespace InterSelectPcroomDailyReward