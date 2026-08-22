#ifndef DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CFAIRPVPSCORE_H
#define DNFPROJECT_SOURCE_DNFSERVER_GAMESERVER_GAME_CFAIRPVPSCORE_H

namespace fair_pvp
{
// CFairPvPScore —— PvP 公平对战分数/训练结算容器（ORIG 符号见 PvP_Room.cpp）
// 当前仅提供占位 getter，保持 ABI：_ZNK8fair_pvp13CFairPvPScore11GetGiveItemEv
// _ZN8fair_pvp13CFairPvPScore19GetRemainBonusCountEv
// _ZN8fair_pvp13CFairPvPScore24GetLastTrainingSealCountEv
// _ZN8fair_pvp13CFairPvPScore29GetLastTrainingSealBonusCountEv
class CFairPvPScore
{
public:
    void* GetGiveItem() const;
    int GetRemainBonusCount();
    int GetLastTrainingSealCount();
    int GetLastTrainingSealBonusCount();

private:
    int m_pad[8];
};
} // namespace fair_pvp

#endif