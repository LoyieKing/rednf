#include "Inter_OnTimeEventRewardStartOnLogin.h"
#include "CUser.h"
#include "PacketGuard.h"

// data 包缓冲的组件视图（P1：组件视图 → 单指令 disp 寻址，匹配 ORIG
// mov 0x12(%eax) / mov 0xa(%eax)；裸指针加偏移会生成 add 两步）
#pragma pack(push, 1)
struct OnTimeEventRewardStartOnLoginView
{
    char m_pad[0xa];   // +0x00
    int m_characNo;    // +0x0a（回发确认包下发角色号）
    char m_pad2[0x4];  // +0x0e
    int m_rewardNo;    // +0x12（与当前角色号比对）
};
#pragma pack(pop)

int Inter_OnTimeEventRewardStartOnLogin::dispatch_sig(CUser* user, char* data, int len)
{
    int local_data;

    local_data = (int)data;
    if (user == 0)
        return 0;
    if (((OnTimeEventRewardStartOnLoginView*)local_data)->m_rewardNo != user->get_charac_no(-1))
        return 0;
    PacketGuard packet;
    packet.put_header(0, 0x105);
    packet.put_int(((OnTimeEventRewardStartOnLoginView*)local_data)->m_characNo);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}
