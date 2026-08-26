// ============================================================================
// LinkClosureA_Data.cpp — CDataManager PvP 组 getter（undef_bizA 收口批 A）
//
// 覆盖符号（唯一 owner = 本 TU；报告缺失 → nm -S 定址 objdump 忠实转写）：
//   CDataManager::GetPvPMode        W 0x0844d4ae（0xe 字节）
//   CDataManager::GetPvPTeamCount   W 0x08568ce0（0xe 字节）
//
// ORIG 汇编均为 push %ebp / mov %esp,%ebp / mov 0x8(%ebp),%eax /
// mov disp(%eax),%eax / leave / ret 六指令形态：直接返回成员字段。
// ORIG disp：GetPvPMode=+0xb44c、GetPvPTeamCount=+0xb450；
// CDataManager.h 布局 m_pvpChannel(+0xb430, pvp_channel_info_t) 起始,
// 两偏移分别对应该结构 +0x1c(m_field1c)/+0x20(m_field20)
// （importPVPChannelList 0x08901f10 扫描序 v8/v9，头注释实证）。
// ============================================================================

#include "CDataManager.h"

int CDataManager::GetPvPMode()
{
    return m_pvpChannel.m_field1c;      // [objdump 0x844d4b4 mov 0xb44c(%eax)]
}

int CDataManager::GetPvPTeamCount()
{
    return m_pvpChannel.m_field20;      // [objdump 0x8568ce6 mov 0xb450(%eax)]
}

// ==== QuestParameterScript 三 getter（ORIG T 0x08a6bd1c/0x08a6bd86/0x08a6bda6）====
// getRewardWeight：map<char,int> find，未命中返 100；exp/gold：vector::at 抛越界。
int QuestParameterScript::getRewardWeight(char level)
{
    std::map<char, int>::const_iterator it = m_map0.find(level);
    if (it != m_map0.end())
        return it->second;
    return 100;
}

int QuestParameterScript::getRewardExp(int idx)
{
    return m_vec24.at(idx);
}

int QuestParameterScript::getRewardGold(int idx)
{
    return m_vec30.at(idx);
}
