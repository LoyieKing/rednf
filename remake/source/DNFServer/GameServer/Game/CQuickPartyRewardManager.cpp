// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CQuickPartyRewardManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include <cmath>
#include <new>

namespace QuickParty
{

// ---- RewardCardGold（T 0x0826d070）----
// result.m_value00 *= (levels[成员数-2].m_value4 / 100.0f + 1.0f)，截断取整。
// ORIG 反汇编：fnstcw 置 RC=0 + fistpl（截断）；lrintf 为就近偶舍入，语义不符。
#include "CQuickPartyRewardManager/process.inc"
{
    // b28d-20: ORIG 0x0826d070 — IBasicReward 族无 map find，直接
    // m_data.m_basicReward.m_levelDatas（+0x18）；两早退岛（js idx<0 / jge idx>=sz）
    // 由裸 if-return 直产（P223 变体）；size()/operator[] 每次重走
    // m_data.m_basicReward.m_levelDatas 表达式（P183 禁 vector 引用缓存，ORIG 每处
    // `mov this,%edx; mov 0x4(%edx),%edx; add $0x18,%edx`）；比较为**有符号** jge
    //（ORIG 源码是 int sz 而非 size_t 比较）。rate 为具名 float 局部（槽 -0xc，
    // ORIG `mov $0x3f800000,%eax; mov %eax,-0xc` 实证，两语句赋值保槽）。
    int sz = static_cast<int>(m_data.m_basicReward.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;

    float rate = 1.0f;
    rate = rate +
        static_cast<float>(m_data.m_basicReward.m_levelDatas[idx].m_value4) / 100.0f;
    result.m_value00 = static_cast<int>(
        static_cast<float>(result.m_value00) * rate);
}

// ---- LevelData / LevelDataHell ctor（ORIG W 0x08a6ef72 / 0x08a6ef8a：
// LevelData：m_field0=0、m_value4=-1；LevelDataHell：两 int 全置 -1；
// dtor 空）。此处强定义供 EventRewardQuickPartyB2 等 TU 以外部引用解析
// （W 与 T 链接可共存，语义一致）。----
#include "CQuickPartyRewardManager/LevelData.inc"

#include "CQuickPartyRewardManager/LevelDataHell.inc"

// ============================ QuickPartyData ============================

// ORIG 0x826dad4：两 vector<int> C1、BasicReward C1、map C1、三容器 clear、
// +0x1a0[2][2] 全 -1。容器构造由成员默认构造承担；clear 为 ORIG 显式调用序列。
#include "CQuickPartyRewardManager/QuickPartyData.inc"
{
    m_vec00.clear();
    m_dungeonIdxList.clear();
    m_randomRewards.clear();

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            m_minCounts[i][j] = -1;
}

// ORIG 0x826dd52：逆序析构 map → BasicReward → 两 vector<int>（成员自动）。
#include "CQuickPartyRewardManager/dtor_QuickPartyData.inc"

#include "CQuickPartyRewardManager/IBasicReward.inc"
    : IRewardType(data)
{
}

#include "CQuickPartyRewardManager/IRandomwReward.inc"
    : IRewardType(data)
{
}

// ---- HellPartyDifficulty（T 0x0826d116）----
// 命中槽位：result.value00=hell[idx].value4、result.value04=hell[idx].field0（交叉写）。
#include "CQuickPartyRewardManager/process_1.inc"
{
    // b28d-20: ORIG 0x0826d116 — 两早退岛（js / jge）+ 有符号 sz 比较；size 先算
    // 存 -0x10、idx 存 -0xc（ORIG 槽序），两次 operator[] 各自重算 vector 地址（P183）。
    int sz = static_cast<int>(m_data.m_basicReward.m_hellDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    result.m_value00 = m_data.m_basicReward.m_hellDatas[idx].m_value4;
    result.m_value04 = m_data.m_basicReward.m_hellDatas[idx].m_field0;
}

// ---- KillMonsterExp（T 0x0826d198）----
// map[m_type] 命中后按等级档放大 result.m_value00（同就近偶舍入）。
#include "CQuickPartyRewardManager/process_2.inc"
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    // b28d-20: ORIG 0x0826d198 — 与 RewardCardGold 同形（三岛 + 有符号 sz + rate 具名槽）。
    int sz = static_cast<int>(it->second.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    float rate = 1.0f;
    rate = rate +
        static_cast<float>(it->second.m_levelDatas[idx].m_value4) / 100.0f;
    result.m_value00 = static_cast<int>(
        static_cast<float>(result.m_value00) * rate);
}

// ---- EquipItemDropRate（T 0x0826d2aa）----
// 命中后直接写 result.m_rate14 浮点比率（无舍入）。
#include "CQuickPartyRewardManager/process_3.inc"
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    // b28d-20: ORIG 0x0826d2aa — 三早退岛；rate 具名 float 槽（ORIG -0xc
    // `mov $0x3f800000,%eax; mov %eax,-0xc`），尾段 `mov -0xc(%ebp),%edx;
    // mov %edx,0x14(%eax)` 直存 result.m_rate14（非表达式直写，否则 fld1 无槽）。
    int sz = static_cast<int>(it->second.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    float rate = 1.0f;
    rate = rate +
        static_cast<float>(it->second.m_levelDatas[idx].m_value4) / 100.0f;
    result.m_rate14 = rate;
}

// ---- CointLimitCount（T 0x0826d4ac）----
// result.m_value00 -= levels[idx].m_value4；差额经 CParty 计入地下城硬币计数。
#include "CQuickPartyRewardManager/process_4.inc"
{
    std::map<RandomBuffType, LevelDataRandomReward>::const_iterator it =
        m_data.m_randomRewards.find(m_type);
    if (it == m_data.m_randomRewards.end())
        return;

    // b28d-20: ORIG 0x0826d4ac — 三早退岛；sz 存 -0x10、idx 存 -0xc（ORIG 槽序）；
    // `result.m_value00 -= it->second.m_levelDatas[idx].m_value4` 单语句触发 4.1
    // 旧值先入 %esi + idx 入 %ebx 跨 operator->/operator[] 保活（P38/SAVE_EXPR）。
    int sz = static_cast<int>(it->second.m_levelDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    result.m_value00 -= it->second.m_levelDatas[idx].m_value4;
    param.m_party->increase_dungeon_coin_count(result.m_value00);
}

// ---- ReturnAncientDungeonTicket（T 0x0826d598）----
// 仅调 CParty::increase_ancient_dungeon_ticket（不触 result/m_data）。
#include "CQuickPartyRewardManager/process_5.inc"
                                         RewardData_Result& /*result*/)
{
    param.m_party->increase_ancient_dungeon_ticket();
}
// ---- QuestItemDropRate（T 0x0826d398）----
// map[m_type] 命中且 (成员数-2) 为有效档位时，result.m_flag18 按
// flag * (1 + levels[idx].m_value4 / 100.0f) 就近偶舍入放大。
#include "CQuickPartyRewardManager/process_6.inc"
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
#include "CQuickPartyRewardManager/process_7.inc"
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

#include "CQuickPartyRewardManager/RewardCardGold.inc"
    throw() : IBasicReward(data)
{
}

#include "CQuickPartyRewardManager/HellPartyDifficulty.inc"
                                         QuickPartyData& data) throw()
    : IBasicReward(data)
{
}

#include "CQuickPartyRewardManager/KillMonsterExp.inc"
    : IRandomwReward(data)
{
    m_type = type;
}

#include "CQuickPartyRewardManager/EquipItemDropRate.inc"
    : IRandomwReward(data)
{
    m_type = type;
}

#include "CQuickPartyRewardManager/CointLimitCount.inc"
    : IRandomwReward(data)
{
    m_type = type;
}

#include "CQuickPartyRewardManager/ReturnAncientDungeonTicket.inc"
                                                       QuickPartyData& data) throw()
    : IRandomwReward(data)
{
    m_type = type;
}

// ============================ CQuickPartyRewardManager ============================

#include "CQuickPartyRewardManager/CQuickPartyRewardManager.inc"

#include "CQuickPartyRewardManager/dtor_CQuickPartyRewardManager.inc"

// ORIG 0x0826d6b0（报告 QuickParty__CQuickPartyRewardManager/init.md）：
// 导入 Etc/QuickParty.etc 失败 → cMyTrace 报错返回 false；成功则 new 六个奖励
// 对象写入 m_rewards 表（basic[11]=CardGold、basic[12]=HellDifficulty、
// random[0]=KillMonsterExp、random[3]=EquipDropRate、random[9]=CoinLimit、
// random[10]=ReturnTicket），最后 set_rand_buff_total_prob 并返回 true。
// b28d-20: 修前 167 insn（每个 new 后 ~9 条 EH landing pad：operator delete +
// _Unwind_Resume）；按 P216 给 6 个奖励类 ctor 声明/定义加 throw()（见 .h 同批
// 判词）后 109=109 逐条 AE（ORIG 0x0826d6b0 无任何 EH cleanup）。
#include "CQuickPartyRewardManager/init.inc"
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

// ORIG 0x0826d91a：按列求和后写回 m_probs[row][col]——row 声明在外层、循环后泄漏
// 值 11（ORIG 写回段载 -0x8 运行时形 shl3/sub/add，非 0x61 常量折叠，P171 照抄）。
#include "CQuickPartyRewardManager/set_rand_buff_total_prob.inc"
{
    // 声明序=槽序（col=-0x10/total=-0xc/row=-0x8）；row 泄漏值 11 后写回
    int col;
    int total;
    int row;
    for (col = 0; col < 7; ++col)
    {
        total = 0;
        for (row = 0; row < 11; ++row)
            total += m_probs[row][col];
        m_probs[row][col] = total;
    }
}
// ORIG T 0x0826d8a2（报告 getRequireQuickPartyMinCount.md）：
// type∈[0,1]、diff∈[0,3] 越界返回 -1；命中查 m_minCounts[type][diff]
// （寻址 +0x1a0 + type*0x10 + diff*4）。
#include "CQuickPartyRewardManager/getRequireQuickPartyMinCount.inc"
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
#include "CQuickPartyRewardManager/get_random_buff_prob.inc"
                                                   RandomBuffDungeon dungeon)
{
    if ((int)type > 11 || (int)dungeon > 6)
        return 0;
    return m_probs[(int)type][(int)dungeon];
}

}  // namespace QuickParty
