#include "CTournamentDungeonReward.h"
#include <cstring>

// ORIG ctor 0x8284770：写虚表 0x8c0cbf0；D1 0x82847dc；ResetReward 0x8284820。
// 本 TU 仅提供 CBattle_Field 构造/析构链与 ResetReward 所需符号（ORIG 0x8284820）。
// 其余成员（MakeSelectPacket/_MakeRewardPacket/MakeRewardPacket 等）在头为 G2
// 占位布局时未声明成员，故不在此定义，属后续批次。
CTournamentDungeonReward::CTournamentDungeonReward()
{
    memset(m_pad, 0, sizeof(m_pad));
}

CTournamentDungeonReward::~CTournamentDungeonReward() {}

void CTournamentDungeonReward::ResetReward()
{
    memset(m_pad, 0, sizeof(m_pad));
}