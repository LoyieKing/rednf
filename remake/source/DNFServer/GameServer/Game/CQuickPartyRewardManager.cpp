// df_game_r 还原 —— QuickParty::CQuickPartyRewardManager 独立 TU。
// 权威符号（ORIG df_game_r nm 实证，地址见各函数）：
//   CQuickPartyRewardManager C1 0x0826d688 T / D1 0x0826d69c T
//   init                    0x0826d6b0 T
//   set_rand_buff_total_prob 0x0826d91a T
//   IRewardType 族 ctor/check W 与六个 process T —— 逐报告转写。
// 类布局与 vtable 链证据见 CQuickPartyRewardManager.h 头注。

#include "CQuickPartyRewardManager.h"

#include "GameTypes.h"   // cMyTrace
#include "CParty.h"      // get_member_count / increase_dungeon_coin_count /
                         // increase_ancient_dungeon_ticket

#include <cmath>         // lrintf（ORIG fnstcw/fldcw 就近偶舍入等价）
#include <new>

namespace QuickParty
{

// ---- LevelData / LevelDataHell ctor（ORIG W 0x08a6ef72 / 0x08a6ef8a：
// LevelData：m_field0=0、m_value4=-1；LevelDataHell：两 int 全置 -1；
// dtor 空）。此处强定义供 EventRewardQuickPartyB2 等 TU 以外部引用解析
// （W 与 T 链接可共存，语义一致）。----
LevelData::LevelData() : m_field0(0), m_value4(-1) {}

LevelDataHell::LevelDataHell() : m_field0(-1), m_value4(-1) {}

// ============================ QuickPartyData ============================

// ORIG 0x826dad4：两 vector<int> C1、BasicReward C1、map C1、三容器 clear、
// +0x1a0[2][2] 全 -1。容器构造由成员默认构造承担；clear 为 ORIG 显式调用序列。
QuickPartyData::QuickPartyData()
{
    m_vec00.clear();
    m_dungeonIdxList.clear();
    m_randomRewards.clear();

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            m_minCounts[i][j] = -1;
}

// ORIG 0x826dd52：逆序析构 map → BasicReward → 两 vector<int>（成员自动）。
QuickPartyData::~QuickPartyData() {}

// ============================ IRewardType 族 ============================

IRewardType::IRewardType(QuickPartyData& data)
    : m_data(data)
{
}

IBasicReward::IBasicReward(QuickPartyData& data)
    : IRewardType(data)
{
}

IRandomwReward::IRandomwReward(QuickPartyData& data)
    : IRewardType(data), m_type(RANDOM_BUFF_INCREASE_EXP)
{
}

// ---- RewardCardGold（T 0x0826d070）----
// result.m_value00 *= (levels[成员数-2].m_value4 / 100.0f + 1.0f)，就近偶舍入。
void RewardCardGold::process(RewardData_Param param, RewardData_Result& result)
{
    const std::vector<LevelData>& levels = m_data.m_basicReward.m_levelDatas;
    int idx = param.m_party->get_member_count() - 2;
    if (idx >= 0 && static_cast<unsigned int>(idx) < levels.size())
    {
        float rate = 1.0f +
            static_cast<float>(levels[idx].m_value4) / 100.0f;
        result.m_value00 =
            static_cast<int>(lrintf(static_cast<float>(result.m_value00) * rate));
    }
}

// ---- HellPartyDifficulty（T 0x0826d116）----
// 命中槽位：result.value00=hell[idx].value4、result.value04=hell[idx].field0（交叉写）。
void HellPartyDifficulty::process(RewardData_Param param, RewardData_Result& result)
{
    const std::vector<LevelDataHell>& hells = m_data.m_basicReward.m_hellDatas;
    int idx = param.m_party->get_member_count() - 2;
    if (idx >= 0 && static_cast<unsigned int>(idx) < hells.size())
    {
        result.m_value00 = hells[idx].m_value4;
        result.m_value04 = hells[idx].m_field0;
    }
}

// ---- KillMonsterExp（T 0x0826d198）----
// map[m_type] 命中后按等级档放大 result.m_value00（同就近偶舍入）。
void KillMonsterExp::process(RewardData_Param param, RewardData_Result& result)
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    const std::vector<LevelData>& levels = it->second.m_levelDatas;
    int idx = param.m_party->get_member_count() - 2;
    if (idx >= 0 && static_cast<unsigned int>(idx) < levels.size())
    {
        float rate = 1.0f +
            static_cast<float>(levels[idx].m_value4) / 100.0f;
        result.m_value00 =
            static_cast<int>(lrintf(static_cast<float>(result.m_value00) * rate));
    }
}

// ---- EquipItemDropRate（T 0x0826d2aa）----
// 命中后直接写 result.m_rate14 浮点比率（无舍入）。
void EquipItemDropRate::process(RewardData_Param param, RewardData_Result& result)
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    const std::vector<LevelData>& levels = it->second.m_levelDatas;
    int idx = param.m_party->get_member_count() - 2;
    if (idx >= 0 && static_cast<unsigned int>(idx) < levels.size())
    {
        result.m_rate14 = 1.0f +
            static_cast<float>(levels[idx].m_value4) / 100.0f;
    }
}

// ---- CointLimitCount（T 0x0826d4ac）----
// result.m_value00 -= levels[idx].m_value4；差额经 CParty 计入地下城硬币计数。
void CointLimitCount::process(RewardData_Param param, RewardData_Result& result)
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    const std::vector<LevelData>& levels = it->second.m_levelDatas;
    int idx = param.m_party->get_member_count() - 2;
    if (idx >= 0 && static_cast<unsigned int>(idx) < levels.size())
    {
        result.m_value00 -= levels[idx].m_value4;
        param.m_party->increase_dungeon_coin_count(result.m_value00);
    }
}

// ---- ReturnAncientDungeonTicket（T 0x0826d598）----
// 仅调 CParty::increase_ancient_dungeon_ticket（不触 result/m_data）。
void ReturnAncientDungeonTicket::process(RewardData_Param param,
                                         RewardData_Result& /*result*/)
{
    param.m_party->increase_ancient_dungeon_ticket();
}
// ---- QuestItemDropRate（T 0x0826d398）----
// map[m_type] 命中且 (成员数-2) 为有效档位时，result.m_flag18 按
// flag * (1 + levels[idx].m_value4 / 100.0f) 就近偶舍入放大。
void QuestItemDropRate::process(RewardData_Param param,
                                RewardData_Result& result)
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    int sz = static_cast<int>(it->second.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    {
        float rate = 1.0f;
        rate = rate +
               static_cast<float>(it->second.m_levelDatas[idx].m_value4) /
                   100.0f;
        result.m_flag18 = static_cast<int>(
            static_cast<float>(result.m_flag18) * rate);
    }
}

// ---- DimensionInoutCount（T 0x0826d5ac）----
// 命中档位后把 levels[idx].m_value4 写入 result.m_value00，
// 并将该值计入 party 的次元进出计数。
void DimensionInoutCount::process(RewardData_Param param,
                                  RewardData_Result& result)
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    int sz = static_cast<int>(it->second.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    {
        result.m_value00 = it->second.m_levelDatas[idx].m_value4;
        param.m_party->increase_dimension_inout_count(result.m_value00);
    }
}


// ---- 具体类 ctor（W，type 仅 RandomwReward 族存储于 +0x08）----

RewardCardGold::RewardCardGold(BasicRewardType /*type*/, QuickPartyData& data)
    : IBasicReward(data)
{
}

HellPartyDifficulty::HellPartyDifficulty(BasicRewardType /*type*/,
                                         QuickPartyData& data)
    : IBasicReward(data)
{
}

KillMonsterExp::KillMonsterExp(RandomBuffType type, QuickPartyData& data)
    : IRandomwReward(data)
{
    m_type = type;
}

EquipItemDropRate::EquipItemDropRate(RandomBuffType type, QuickPartyData& data)
    : IRandomwReward(data)
{
    m_type = type;
}

CointLimitCount::CointLimitCount(RandomBuffType type, QuickPartyData& data)
    : IRandomwReward(data)
{
    m_type = type;
}

ReturnAncientDungeonTicket::ReturnAncientDungeonTicket(RandomBuffType type,
                                                       QuickPartyData& data)
    : IRandomwReward(data)
{
    m_type = type;
}

// ============================ CQuickPartyRewardManager ============================

CQuickPartyRewardManager::CQuickPartyRewardManager() {}   // ORIG 0x826d688：仅基类构造

CQuickPartyRewardManager::~CQuickPartyRewardManager() {}  // ORIG 0x826d69c：仅基类析构

// ORIG 0x0826d6b0（报告 QuickParty__CQuickPartyRewardManager/init.md）：
// 导入 Etc/QuickParty.etc 失败 → cMyTrace 报错返回 false；成功则 new 六个奖励
// 对象写入 m_rewards 表（basic[11]=CardGold、basic[12]=HellDifficulty、
// random[0]=KillMonsterExp、random[3]=EquipDropRate、random[9]=CoinLimit、
// random[10]=ReturnTicket），最后 set_rand_buff_total_prob 并返回 true。
bool CQuickPartyRewardManager::init()
{
    if (!importQuickPartyScript("Etc/QuickParty.etc", this))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x92, 0);
        trace("QUICK_PARTY_LOG : QuickParty.etc FILE LOAD FAIL!!");
        return false;
    }

    m_rewards[0][11] = new RewardCardGold(BASIC_REWARD_CARD_GOLD, *this);          // +0x1ec
    m_rewards[0][12] = new HellPartyDifficulty(BASIC_REWARD_HELL_DIFFICULTY, *this);  // +0x1f0
    m_rewards[1][0]  = new KillMonsterExp(RANDOM_BUFF_INCREASE_EXP, *this);        // +0x1f4
    m_rewards[1][3]  = new EquipItemDropRate(RANDOM_BUFF_INCREASE_DROPPROB, *this);  // +0x200
    m_rewards[1][9]  = new CointLimitCount(RANDOM_BUFF_INCREASE_COINLIMIT, *this);   // +0x218
    m_rewards[1][10] = new ReturnAncientDungeonTicket(RANDOM_BUFF_RETURN_TICKET, *this);  // +0x21c

    set_rand_buff_total_prob();
    return true;
}

// ORIG 0x0826d91a（报告 set_rand_buff_total_prob.md）：对 m_probs 行 0..10 按
// 列求和写入行 11（每列总概率）。
void CQuickPartyRewardManager::set_rand_buff_total_prob()
{
    for (int col = 0; col < 7; ++col)
    {
        int total = 0;
        for (int row = 0; row < 11; ++row)
            total += m_probs[row][col];
        m_probs[11][col] = total;
    }
}
// ORIG T 0x0826d8a2（报告 getRequireQuickPartyMinCount.md）：
// type∈[0,1]、diff∈[0,3] 越界返回 -1；命中查 m_minCounts[type][diff]
// （寻址 +0x1a0 + type*0x10 + diff*4）。
int CQuickPartyRewardManager::getRequireQuickPartyMinCount(int type,
                                                           int dungeonDiff)
{
    if (type < 0 || type > 1)
        return -1;
    if (dungeonDiff < 0 || dungeonDiff > 3)
        return -1;
    return m_minCounts[type][dungeonDiff];
}


// ORIG T 0x0826d860（报告 get_random_buff_prob.md）：type∈[0,11] 且
// dungeon∈[0,6] 时返回 m_probs[type][dungeon]，否则返回 0。
// ORIG 仅上界比较（cmp $0xb/jg、cmp $0x6/jle），负值与原版一致落入查表路径。
int CQuickPartyRewardManager::get_random_buff_prob(RandomBuffType type,
                                                   RandomBuffDungeon dungeon)
{
    if ((int)type > 11 || (int)dungeon > 6)
        return 0;
    return m_probs[(int)type][(int)dungeon];
}

}  // namespace QuickParty
