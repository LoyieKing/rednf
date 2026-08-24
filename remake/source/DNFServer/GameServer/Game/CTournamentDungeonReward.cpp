#include "CTournamentDungeonReward.h"
#include <cstring>

// ORIG ctor 0x8284770：写虚表 0x8c0cbf0；D1 0x82847dc；ResetReward 0x8284820。
// 本 TU 仅提供 CBattle_Field 构造/析构链与 ResetReward 所需符号（ORIG 0x8284820）。
// 其余成员（MakeSelectPacket/_MakeRewardPacket/MakeRewardPacket 等）在头为 G2
// 占位布局时未声明成员，故不在此定义，属后续批次。
CTournamentDungeonReward::CTournamentDungeonReward()
    : m_field04(0), m_rewards()
{
}

CTournamentDungeonReward::~CTournamentDungeonReward() {}

// ORIG 0x8284820：对 2x2 个 TournamentRewardType（+0x8 起）逐个 Clear，
// 最后清零 +0x04。构造函数中的 memset 占位同步移除（成员自动初始化）。
void CTournamentDungeonReward::ResetReward()
{
    for (int c = 0; c < 2; ++c)
        for (int r = 0; r < 2; ++r)
            m_rewards[c][r].Clear();
    m_field04 = 0;
}