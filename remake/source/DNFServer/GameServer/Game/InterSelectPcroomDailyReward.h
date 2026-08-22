#ifndef GAME_INTERSELECT_PCROOM_DAILY_REWARD_H
#define GAME_INTERSELECT_PCROOM_DAILY_REWARD_H

// df_game_r 还原 —— InterSelectPcroomDailyReward::SendPacket（ORIG T 0x81618a6）
// 唯一声明点：InterSelectPcroomDailyReward.cpp 实现，GameWorld.cpp 调用。
// ORIG 为命名空间自由函数（mangled _ZN28InterSelectPcroomDailyReward10SendPacketEP5CUser
// 无 this；命名空间见 MoreTypes.h 同名单 Msg）。
//   objdump（081618a6）: 若 user==0 直接返回；否则
//     PacketGuard ctor; put_header(0,0x22e); put_byte((char)user[0x8ec32]);
//     finalize(true); CUser::Send(user,guard); dtor。
//   0x8ec32 = CUser::m_field8ec32（char）。

class CUser;

namespace InterSelectPcroomDailyReward
{
void SendPacket(CUser* user);
}

#endif  // GAME_INTERSELECT_PCROOM_DAILY_REWARD_H
