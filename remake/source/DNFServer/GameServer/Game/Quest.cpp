// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Quest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r 任务模板 Quest 实现（G3-2 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/Quest.md（Ghidra C）+ ORIG 反汇编。
// ============================================================================
// b28d-solo-Quest 判词汇总（Quest.cpp 独占 TU，ORIG 权威地址见各行；
// 上一轮 b28d-20 lane20 判词已按本轮实测翻案/更新）：
//  [1] dungeonEventHanaseStruct::~dungeonEventHanaseStruct（定义在 Quest.h:195）
//      ORIG 0x08a61164 —— TU 口径 AE（52=52 助记符/槽位全等，仅 .o 重定位标签差）。
//  [2] Quest::check_possible 0x08352d86 —— AE（本轮：内层 if 加 else{return 0;}
//      后尾部四岛序 ret0,ret0,ret1,ret1 与 ORIG 全等；N93附 判词作废）。
//  [3] makeRewardItemInfo(pair) 0x083520c8 —— AE/IDENTICAL。
//  [4] stQuestTriggerState_t::setTrigger 0x0836dce0 —— N18a 封顶（4.1 每字段多发
//      `mov %edx,%ecx; and $0x1ff,%cx` 冗余截断，4.4 恒折单次 and；本轮再试
//      |= 形 32/41、无 cast 形 38/41、原形 38/41 ⇒ 无源码解）。
//  [5] Quest::CheckKillMonster 0x083535d6 —— N18b 封顶（enum 形参比较：ORIG
//      `mov -0x2c(%ebp),%eax; cmp 0x18(%ebp),%eax` mem 直比，4.4 恒载寄存器
//      reg,reg；本轮 18 形探针（(int)/(enum) cast/交换/取址视图）全败）。
//  [6] Quest::get_appearmap 0x083536f2 —— AE（本轮：while(percent>=rand){return}
//      形复现 setge 物化 + %ebx 跨 call 保活；N7/P197④ 判词作废）。
//  [7] makeRewardItemInfo(RewardItemInfo) 0x08352203 —— AE/IDENTICAL。
//  [8] Quest::ConvertRewardSelectIndex 0x083537c2 —— AE（本轮：goto 尾标签复现
//      `je` 直达 return 岛 + RewardItemInfoJobView enum 视图复现 m_job eax→ebx
//      中转；P223 单源 goto 负例判词作废）。
//  [9] Quest::get_init_trigger 0x083530d6 —— AE（本轮：state 局部 aligned(4)
//      复现 4 对齐槽 -0x4c；168=168）。
//  [10] Quest::get_clear_itemlist 0x08353404 —— AE/IDENTICAL。
//  [11] Quest::set_quest 0x0835233e —— AE（本轮：QuestScript::m_field108/
//      m_field130/m_field22d bool 化 + m_powerSide uchar 化 + 循环1 it 前置声明
//      + 循环2 i/n int + trace 全表达式临时链；449=449 ndiff=0；N82 判词作废）。
// ============================================================================
#include <utility>
#include <vector>

#include "Quest.h"
#include "CUser.h"
#include "CDataManager.h"
#include "GlobalData.h"
#include "LogManager.h"

// ============================================================================
// 其它 TU 提供的符号（ORIG 真实符号）
// ============================================================================


// 全局对象声明统一走权威头 GlobalData.h（s_DailyScheduleManager 为指针，
// ORIG 0x8a5... 调用点 `mov <A>,%eax; mov %eax,(%esp)` 装载指针值实证）。

// ORIG 0x8352203：src 按相邻元素配对填充 RewardItemInfo{field0,field4}；
// 奇数元素时记录错误并返回
// b28d-20 AE：it++ 后置 + cMyTrace 全表达式临时链（83=83，槽位对齐；
// 残差仅 .o 重定位立即数）。
#include "Quest/makeRewardItemInfo.inc"
                        std::vector<RewardItemInfo>& out)
{
    RewardItemInfo item;
    std::vector<int>::const_iterator it = src.begin();
    while (it != src.end())
    {
        item.m_itemIdx = *it;
        it++;
        if (it == src.end())
        {
            // ORIG 0x83522ce：cMyTrace 记录 "it == data.rewardSelectItems_.end()"
            cMyTrace(__PRETTY_FUNCTION__, 0x104d, 5)(
                "[%s][%d]_it == data.rewardSelectItems_.end()",
                __PRETTY_FUNCTION__, 0x104d);
            return;
        }
        item.m_count = *it;
        it++;
        out.push_back(item);
    }
}
// ORIG 0x83520c8：src 按相邻元素配对填充 std::pair<int,int>{first,second}；
// 奇数元素时 cMyTrace 记录 "[%s][%d]_it == data.rewardSelectItems_.end()"
// （0x1031 行，PRETTY __PRETTY_FUNCTION__）。
// 与 RewardItemInfo 版同 TU 相邻（0x83520c8/0x8352203），ORIG T 强符号，无调用点（导出）。
// b28d-20 AE：同上形（it++ + 全表达式临时链），83=83 槽位对齐。
#include "Quest/makeRewardItemInfo_1.inc"
                        std::vector<std::pair<int, int> >& out)
{
    std::pair<int, int> p;
    std::vector<int>::const_iterator it = src.begin();
    while (it != src.end())
    {
        p.first = *it;
        it++;
        if (it == src.end())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1031, 5)(
                "[%s][%d]_it == data.rewardSelectItems_.end()",
                __PRETTY_FUNCTION__, 0x1031);
            return;
        }
        p.second = *it;
        it++;
        out.push_back(p);
    }
}
#include "Quest/get_rand_int.inc"

// stQuestTriggerState_t::setTrigger（ORIG W 0x836dce0：9 位触发值打包）
// [E04 2026-09-10] 旧 b28d-solo-Quest 判词「N18a 封顶：4.4 无源码形态产出该冗余
// 截断」作废 —— 真因是字段为「16 位容器 9 位位域」（QuestTriggerState.h 位域建模）。
// 位域直赋 `m_triggerN = x;` 在 4.4.7 下逐条复现 ORIG：值截断 `mov %eax,%edx;
// and $0x1ff,%dx` → 读容器 `movzwl 旧; and $0xfe00,%dx` → or → `mov %dx,容器`；
// ORIG 的 `mov %edx,%ecx; and $0x1ff,%cx` 冗余截断由位域赋值自身产生。
// 探针 /tmp/qts/b.cpp setTrigger 41=41 逐条一致（改前 41/62，gap +21）。
#include "Quest/setTrigger.inc"
{
    m_trigger0 = a;
    m_trigger1 = b;
    m_trigger2 = c;
}

// ============================================================================
// Quest
// ============================================================================

#include "Quest/Quest.inc"
{
}

#include "Quest/dtor_Quest.inc"
{
}

#include "Quest/exposeQuest.inc"
{
    return m_exposeQuest;
}

// b28d-solo-Quest AE：ORIG 0x08352d86，162=162。尾部块序翻正形=内层 if 只保留
// 早返 `if(expertJobLevel<m_field140) return 0;`，把内层否定路径改为
// `else { return 0; }` 再 fall-through `return 1;` ⇒ 四岛序 ret0,ret0,ret1,ret1
// 与 ORIG 全等（b28d-20 N93附「四形全败」判词作废）。
#include "Quest/check_possible.inc"
{
    int job = param.m_job;
    int firstGrow = param.m_firstGrow;
    int secondGrow = param.m_secondGrow;  // ORIG 局部（checkMatchedGrowType 用 -1）
    int level = param.m_level;
    int questCount = param.m_pvpGrade;  // ORIG param+0x10
    int expertJobType = param.m_expertJobType;     // param+0x1c
    int expertJobLevel = param.m_expertJobLevel;   // param+0x20

    if (!checkPossibleJob((ENUM_CHARACTERJOB)job))
    {
        return 0;
    }
    if (_isNeededCheckGrowType((ENUM_CHARACTERJOB)job))
    {
        if (m_jobChangeQuest == 1)
        {
            // ORIG 0x8352e13：日志后返回 0
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__,
                0x1193, "Quest Script Error Break !, [grow type] != -1 and [job change quest] = 1");
            return 0;
        }
        if (m_jobChangeQuest == 2)
        {
            if (!checkMatchedGrowType((ENUM_CHARACTERJOB)job, firstGrow, -1))
            {
                return 0;
            }
        }
        else if ((m_jobChangeQuest != 10) && (m_jobChangeQuest != 0x14))
        {
            if (!checkMatchedGrowType((ENUM_CHARACTERJOB)job, firstGrow, -1))
            {
                return 0;
            }
        }
    }

    // ORIG `mov 0x20;cmp;jle <pass>`：levelMin <= level 跳下一检查。
    // 返回 0 条件 = levelMin > level ⇒ level < levelMin。与 Ghidra
    // `if (iVar2 < this+0x20)` 一致。
    if (level < m_levelMin)
    {
        return 0;
    }
    // ORIG `mov 0x24;cmp;jge <pass>`：levelMax >= level 跳下一检查。
    // 返回 0 条件 = levelMax < level ⇒ level > levelMax（严格大于，不含等号）。
    // Ghidra `else if (this+0x24 < iVar2)` 一致。
    if (level > m_levelMax)
    {
        return 0;
    }
    // ORIG `mov 0x28;test;je <inner>; cmp;jle <inner>`：countLimit==0 或
    // countLimit<=questCount 进内部检查；返回 0 条件 = countLimit>0 且
    // countLimit > questCount。Ghidra `else if (this+0x28==0 || this+0x28<=iVar3)`
    // 一致（原写法 `<=` 与 ORIG 反向，语义差一位，已反）。
    if ((m_questCountLimit != 0) && (m_questCountLimit > questCount))
    {
        return 0;
    }

    if (m_type == 1)
    {
        // b28d-solo-Quest [P0/N96 清绑 + P168a 档①]：ORIG 0x08352f3b `call
        // isThereDailyTrainingQuestList; xor $0x1,%eax; test %al,%al; je`
        // ⇒ callee 返回类型 bool 化（S37 已同步 CDataManager.h:3468/.cpp:690）
        // + 裸形 `!call()` 精确复现。此前 `register asm("eax")` 绑定已删除
        // （P0 全域禁绑）；int callee 的 `&1`/`(uchar)^1`/`!call()` 三形探针
        // 分别多 and / 产 cmp+setne / 产 testl，均非 ORIG 形。
        if (!G_CDataManager()->isThereDailyTrainingQuestList(level, m_index))
        {
            return 0;
        }
    }
    // ORIG tail：`mov 13c;test;je <ret1>; mov 140;cmp -1;je <ret1>;`
    // 两个 == 判定共享同一个 return 1 尾块（0x223）。前置 `if(13c!=0 && 140!=-1)`
    // 逆置结构让 4.4 对两条件同发 je→共享块（probe p4 形态）。
    if ((m_field13c != 0) && (m_field140 != -1))
    {
        if ((m_field13c == -1) || (m_field13c == expertJobType))
        {
            if (expertJobLevel < m_field140)
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
        // b28d-solo-Quest [N93附 复核]：ORIG 0x8352dxx 尾块序 ret0,ret0,ret1,ret1
        // ⇒ 两 ret0 合并到相邻发射（内层早返 + else 块），ret1 归 fall-through。
        return 1;
    }
    return 1;
}

// SIGFIX:Quest::check_power_side —— ORIG __PRETTY_FUNCTION__ 串 "bool Quest::check_power_side(char)"
// 定返回 bool（返回类型不入 mangled；P174）。
#include "Quest/check_power_side.inc"
{
    if (m_powerSide == 0xff)
    {
        return 1;
    }
    char winner = GlobalData::s_power_manager->GetWinnerSide();
    if (m_powerSide == 0)
    {
        if (winner == side)
        {
            return 1;
        }
    }
    else
    {
        if (m_powerSide == 1)
        {
            char winner2 = GlobalData::s_power_manager->GetWinnerSide();
            if (winner2 != side)
            {
                return 1;
            }
        }
        else
        {
            LogManager::logFormat(1, "data_manager.cpp", __PRETTY_FUNCTION__,
                                  0x1161, "Quest Script Error q_index(%d)", m_index);
            return 0;
        }
    }
    return 0;
}

#include "Quest/checkPossibleJob.inc"
{
    std::vector<stQuestTargetCharacter_t>::const_iterator it =
        m_targetCharacters.begin();
    std::vector<stQuestTargetCharacter_t>::const_iterator end =
        m_targetCharacters.end();
    while (it != end)
    {
        if ((it->m_job == job) || (it->m_job == 0xb))
        {
            return 1;
        }
        ++it;
    }
    return 0;
}

#include "Quest/_isNeededCheckGrowType.inc"
{
    std::vector<stQuestTargetCharacter_t>::const_iterator it =
        m_targetCharacters.begin();
    std::vector<stQuestTargetCharacter_t>::const_iterator end =
        m_targetCharacters.end();
    while (it != end)
    {
        if (((it->m_job == job) || (it->m_job == 0xb)) &&
            (it->m_firstGrow == -1))
        {
            return 0;
        }
        ++it;
    }
    return 1;
}

#include "Quest/checkMatchedGrowType.inc"
                                 int secondGrow) const
{
    std::vector<stQuestTargetCharacter_t>::const_iterator it =
        m_targetCharacters.begin();
    std::vector<stQuestTargetCharacter_t>::const_iterator end =
        m_targetCharacters.end();
    while (it != end)
    {
        if (((it->m_job == job) || (it->m_job == 0xb)) &&
            ((it->m_firstGrow == firstGrow) || (it->m_firstGrow == -1)) &&
            ((it->m_secondGrow == secondGrow) || (it->m_secondGrow == -1)))
        {
            return 1;
        }
        ++it;
    }
    return 0;
}

#include "Quest/check_clear_item.inc"
{
    if (m_clearItems.size() != 0)
    {
        DungeonClearItem item;
        for (int i = 0; i < (int)m_clearItems.size(); ++i)
        {
            item = m_clearItems.at(i);
            if (item.m_dungeonIdx == -1)
            {
                if (item.m_itemIdx == itemIdx)
                {
                    return 1;
                }
            }
            else if ((item.m_itemIdx == itemIdx) &&
                     (item.m_dungeonIdx == dungeonIdx))
            {
                return 1;
            }
        }
    }
    return 0;
}

// b28d-solo-Quest 判词 N18b 封顶：ORIG 0x083535d6。P223 多岛复现成功（3 个
// nop+jmp 岛 + 尾 nop，与 ORIG 0x83536a9/ac/af 逐址对应）。残 1 条 `mov`：
// ORIG `mov -0x2c(%ebp),%eax; cmp 0x18(%ebp),%eax`（mem 直比），4.4 对 enum
// 形参比较恒载寄存器 `mov -0x2c,%edx; mov 0x18(%ebp),%eax; cmp %eax,%edx`。
// 本轮 18 形探针（无 cast/(int)/(ENUM) cast/交换/取址视图/一元+ 等，4.4.7 -m32
// -O0）全败；同函数 int 形参（mapIdx/dungeonIdx/killCount）比较已 mem 直比 ⇒
// 差异根因=ENUM_QUEST_ENEMY_TYPE 全非负 ⇒ 4.4 底层 unsigned、比较走寄存器。
// [F2e-Singles2 C05② 复核 2026-09-10] 4 处比较对象全为**形参**（0xc/0x10/0x14/0x18
//   (%ebp)，含 ENUM_QUEST_ENEMY_TYPE）⇒ C05②「两个具名局部比较」命中形**不成立**
//   （边界表 ❌ 形参子型），维持 N18b（18 形探针已穷举）。our 94 vs ORIG 93（gap −1）。
#include "Quest/CheckKillMonster.inc"
                             ENUM_QUEST_ENEMY_TYPE enemyType,
                             std::vector<MonsterKillItem>& out) const
{
    // ORIG 0x83535dc：m_killItems 为空（无击杀要求）直接失败
    if (m_killItems.empty())
    {
        return 0;
    }
    out.reserve(m_killItems.size());
    for (int i = 0; i < (int)m_killItems.size(); ++i)
    {
        MonsterKillItem item = m_killItems[i];
        // ORIG 0x83535d6：4.1 if-join 岛形——前 3 个 && 条件失败各落一个
        // nop 中继岛（0x83536a9/ac/af），第 4 个失败直落尾块（0x83536b0）。
        // 4.4 复现形 = 负条件 goto 到物理连续尾标签串（P223 多岛细则）。
        if ((item.m_dungeonIdx != -1) && (item.m_dungeonIdx != dungeonIdx))
        {
            goto next_item1;
        }
        if ((item.m_mapIdx != -1) && (item.m_mapIdx != mapIdx))
        {
            goto next_item2;
        }
        // b28d-solo-Quest [P209]：enum 全非负 ⇒ 4.4 底层 unsigned、比较恒载寄存器；
        // ORIG 0x835367f `mov -0x2c(%ebp),%eax; cmp 0x18(%ebp),%eax`（mem 直比，有符号）
        // ⇒ 显式转 int 走 signed 比较。
        if (item.m_enemyType != (int)enemyType)
        {
            goto next_item3;
        }
        if (item.m_killCount == killCount)
        {
            out.push_back(item);
        }
    next_item1:;
    next_item2:;
    next_item3:;
    }
    if (out.size() == 0)
    {
        return 0;
    }
    return 1;
}

#include "Quest/check_clear_send.inc"
{
    return m_clearSendFlag;
}

// b28d-solo-Quest AE：ORIG 0x083530d6。`*it++` 后置形 + `{0}` 两写清零 + 拆嵌套
// if + 两处 state 局部 `__attribute__((aligned(4)))`（复现 4.1 对 8 字节聚合的
// 4 对齐槽 -0x4c/-0x54；4.4 默认按 short 自然对齐 2 落 -0x4a/-0x52）⇒ 168=168。
#include "Quest/get_init_trigger.inc"
{
    if ((m_triggerType == 2) || (m_triggerType == 6))
    {
        std::vector<int>::const_iterator it = m_triggerData.begin();
        std::vector<int>::const_iterator end = m_triggerData.end();
        short values[3] = { 0 };
        int idx = 0;
        while (it != end)
        {
            // ORIG 0x8353145：每次迭代前进 4（type 6 为 5）个 int，取最后一项；
            // ORIG 形为 `*it++`（后置递增的临时副本再解引用，sret 槽 -0x34 等）。
            int v1 = *it++;
            int v2 = *it++;
            int v3 = *it++;
            if (m_triggerType == 6)
            {
                *it++;
            }
            int v4 = *it++;
            values[idx] = (short)v4;
            idx++;
        }
        // b28d-solo-Quest：ORIG state 槽基 -0x4c（4 对齐；4.1 对 8 字节聚合按 4 对齐），
        // 4.4 按 short 自然对齐落 -0x4a。局部 aligned(4) 只影响该对象（不动类型布局）。
        stQuestTriggerState_t state __attribute__((aligned(4)));
        state.setTrigger(values[0], values[1], values[2]);
        return state.getTrigger();
    }
    if (m_triggerType == 0x19)
    {
        stQuestTriggerState_t state __attribute__((aligned(4)));
        state.setTrigger(1, 1, 0);
        return state.getTrigger();
    }
    if ((m_triggerType == 1) && (m_triggerSubType == 6))
    {
        // ORIG 0x8353309：`call size; cmp $3; sete %al; test %al,%al; je`
        // ⇒ 拆嵌套 if 让末条件单独物化（P225 推广）。
        if (m_triggerData.size() == 3)
        {
            int v = m_triggerData.at(2);
            if (v > 0)
            {
                return v;
            }
        }
    }
    return 1;
}

#include "Quest/get_init_achievement_trigger.inc"
                                         unsigned short& t1,
                                         unsigned short& t2) const
{
    if (m_type != 7)
    {
        return 0;
    }
    unsigned short* out[3] = { 0, 0, 0 };
    out[0] = &t0;
    out[1] = &t1;
    out[2] = &t2;
    *out[0] = 1;
    *out[1] = 0;
    *out[2] = 0;
    for (unsigned int i = 0;
         i < (m_field90.size() < 3 ? m_field90.size() : 3); ++i)
    {
        *out[i] = (unsigned short)m_field90[i];
    }
    return 1;
}

#include "Quest/get_title_reward.inc"
{
    if (m_type != 7)
    {
        return 0;
    }
    if (m_titleReward.size() == 0)
    {
        return 0;
    }
    return m_titleReward[0].m_itemIdx;
}

#include "Quest/isRepeatableQuest.inc"
{
    switch (m_type)
    {
    case 4:
    case 8:
        return 1;
    default:
        return 0;
    }
}

// b28d-solo-Quest AE：ORIG 0x083536f2。`while (m_percent >= get_rand_int(100))
// { return m_appearMap; }` 形同时复现 4.1 的 `cmp %eax,%ebx; setge %al;
// test %al,%al; je` 条件物化与 m_percent 升格 %ebx 跨 call 保活（两处分支各一）
// ⇒ 47=47（b28d-20 N7/P197④「无源码形态」判词作废；P225/P226 边界）。
#include "Quest/get_appearmap.inc"
{
    // b28d-solo-Quest [P225/P226 边界]：while(cond){return X;} 形复现 ORIG
    // 0x08353729 的 `cmp %eax,%ebx; setge %al; test %al,%al; je`（%ebx 跨
    // get_rand_int 保活）。
    if (m_appearMap.m_mapB == -1)
    {
        if (m_appearMap.m_mapA == mapA)
        {
            while (m_appearMap.m_percent >= get_rand_int(100))
            {
                return m_appearMap.m_appearMap;
            }
        }
    }
    else if ((m_appearMap.m_mapB == mapA) &&
             (m_appearMap.m_mapB == mapB))
    {
        while (m_appearMap.m_percent >= get_rand_int(100))
        {
            return m_appearMap.m_appearMap;
        }
    }
    return 0;
}


// b28d-solo-Quest AE：ORIG 0x083537c2。
// ① [P223 正例] `if (index == count) goto return_index;` + 函数尾
//    `return_index: return i;` 复现 ORIG `cmp -0xc(%ebp),%eax; je +0x123` 直达
//    尾 return 岛（b28d-20「P223 单源 goto 尾岛负例」判词作废）。
// ② [P254/P235] ORIG 0x08353844 `mov 0x8(%eax),%eax; mov %eax,%ebx`（enum 字段
//    保留 eax→ebx 中转）⇒ 同布局 enum 视图读 m_job（RewardItemInfo 四 int）。
struct RewardItemInfoJobView
{
    int m_itemIdx;   // +0x00
    int m_count;     // +0x04
    ENUM_CHARACTERJOB m_job;  // +0x08
    int m_growType;  // +0x0c
};

// 单一短路表达式 + P226 语句表达式承载条件。**真语义修正**：ORIG 循环
// 正常结束返回 i（== size()）而非 0xffffffff（早退才返回 -1），已按 ORIG 照抄。
#include "Quest/ConvertRewardSelectIndex.inc"
{
    if (!user)
    {
        return 0xffffffff;
    }
    if ((index == -1) || (index == 0xffff))
    {
        return 0xffffffff;
    }
    int i = 0;
    int count = 0;
    for (i = 0; i < (int)m_selectReward.size(); ++i)
    {
        if (({ (m_selectReward[i].m_job == 0xb) ||
               ((((const RewardItemInfoJobView*)&m_selectReward[i])->m_job ==
                 (ENUM_CHARACTERJOB)user->get_charac_job()) &&
                ((m_selectReward[i].m_growType == -1) ||
                 (m_selectReward[i].m_growType ==
                  (int)user->getCurCharFirstGrowType()))); }))
        {
            // b28d-solo-Quest [P47/P220]：ORIG 0x083538b8 `cmp -0xc(%ebp),%eax;
            // je +0x123`（je 直达函数尾 return 岛）⇒ 试 goto 尾标签形。
            if (index == count)
            {
                goto return_index;
            }
            ++count;
        }
    }
return_index:
    return i;
}

// b28d-20 AE：ORIG 0x08353404。声明序 item/out/p + pItem/randValue 循环外
// 具名（ORIG -0x2c/-0x28 清 0 实证）+ 正向 `if (is_stackable())`（消 xor $1）
// + pair 字段直赋（消 make_pair 临时）；134/134 槽位对齐。
#include "Quest/get_clear_itemlist.inc"
    int dungeonIdx) const
{
    DungeonClearItem item;
    std::vector<std::pair<int, int> > out;
    std::pair<int, int> p;
    CItem* pItem = 0;
    int randValue = 0;
    for (int i = 0; i < (int)m_clearItems.size(); ++i)
    {
        item = m_clearItems.at(i);
        if (item.m_itemIdx != dungeonIdx)
        {
            continue;
        }
        pItem = G_CDataManager()->find_item(item.m_rewardItem);
        if (!pItem)
        {
            // ORIG 0x83534e3：find_item 失败记录日志
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x1279,
                "G_CDataManager()->find_item(%d)", item.m_rewardItem);
            continue;
        }
        if (pItem->is_stackable())
        {
            int count = 0;
            for (int k = 0; k < item.m_count; ++k)
            {
                randValue = get_rand_int(100);
                if (randValue <= item.m_percent)
                {
                    ++count;
                }
            }
            if (count > 0)
            {
                p.first = item.m_rewardItem;
                p.second = count;
                out.push_back(p);
            }
        }
        else
        {
            randValue = get_rand_int(100);
            if (randValue <= item.m_percent)
            {
                p.first = item.m_rewardItem;
                p.second = 1;
                out.push_back(p);
            }
        }
    }
    return out;
}

#include "Quest/IsOpenScheduleQuest.inc"
{
    int state = 0;
    int result = GlobalData::s_DailyScheduleManager->GetScheduleState(
        (_DailyScheduleType)1, (unsigned int)m_index);
    return (result == 1) || (result == 2);
}


// b28d-solo-Quest AE：ORIG 0x0835233e，449=449 ndiff=0（p167 调用点差集 45=45）。
// 本轮翻正项：① QuestScript::m_field108/m_field130/m_field22d bool 化 +
// m_powerSide unsigned char 化（P300/档②、P265/档③，消 3 处 setne 与 1 处
// eax 中转）；② 循环1 iterator 先于 pair 声明（P28/4.4 先声明占浅槽）；③ 循环2
// n/i 改 int（P2/P209，消 jae→jge）；④ 块内 cMyTrace 改全表达式临时链（P221，
// 复现 trace 浅槽 -0x30）⇒ 上一轮 N82 封顶判词作废。
#include "Quest/set_quest.inc"
{
    m_name = script.m_name;
    m_index = script.m_index;
    m_type = script.m_questType;
    m_npcIdx = script.m_npcIdx;
    m_targetCharacters = script.m_targetCharacters;
    m_jobChangeQuest = script.m_jobChangeQuest;
    m_questCountLimit = script.m_questCountLimit;
    m_levelMin = script.m_levelMin[0];
    m_levelMax = script.m_levelMin[1];
    m_intDataList = script.m_intDataList;
    m_antiQuestList = script.m_antiQuestList;
    m_clearSendFlag = script.m_field130;
    m_clearItems = script.m_clearItems;
    m_killItems = script.m_killItems;
    m_appearMap = script.m_appearMap;
    m_rewardSelectList = script.m_rewardSelectList;
    m_triggerType = script.m_triggerType;
    m_triggerSubType = script.m_triggerSubType;
    m_triggerData = script.m_triggerData;
    m_field90 = script.m_field1b0;
    m_postalReward = script.m_postalReward;
    m_field101 = script.m_field22e;
    m_field130 = script.m_field108;

    if (m_triggerType == 0)
    {
        // ORIG 0x835256c：pair p(-0x48) 具名 + `p.first = *it; it++;` 形；
        // 奇数尾部走 cMyTrace（0x8352602 "(it == intDatas_.end())"）后跳出。
        // b28d-solo-Quest：ORIG 槽 it=-0x40（浅）/pair=-0x48（深）+ 发射序 begin→pair C1
        // ⇒ 4.4「先声明占浅槽」下 iterator 必须声明在 pair 之前（P28）。
        std::vector<int>::iterator it = m_triggerData.begin();
        std::pair<int, int> p;
        while (it != m_triggerData.end())
        {
            p.first = *it;
            it++;
            if (it == m_triggerData.end())
            {
                // b28d-solo-Quest [P221]：ORIG trace 槽 -0x30 浅于 it(-0x40)/pair(-0x48)
                // ⇒ 全表达式临时链（构造即调用）复现块内临时槽方向。
                cMyTrace(__PRETTY_FUNCTION__, 0x109d, 5)(
                    "[%s][%d]_(it == intDatas_.end())",
                    __PRETTY_FUNCTION__, 0x109d);
                break;
            }
            p.second = *it;
            it++;
            m_triggerDataPairs.push_back(p);
        }
    }
    else if (m_triggerType == 0x19)
    {
        m_triggerDataPairs.clear();
        // ORIG 0x83526bf：sibling 分支 pair p(-0x50)；ORIG 形 `*it++` 后置
        // 递增（临时槽 -0x18/-0x14）且循环条件 = `i < n && it != end`。
        std::pair<int, int> p;
        // b28d-solo-Quest：ORIG 尾测 `cmp -0x10(%ebp),%eax; jge`（有符号）⇒ i/n 为 int
        // （unsigned 形在 4.4 恒 jae，P2/P209）。
        int n = (int)(m_triggerData.size() >> 1);
        std::vector<int>::iterator it = m_triggerData.begin();
        for (int i = 0; (i < n) && (it != m_triggerData.end()); ++i)
        {
            p.first = *it++;
            p.second = *it++;
            m_triggerDataPairs.push_back(p);
        }
    }

    m_fieldDC.clear();
    m_fieldE8 = 0;
    m_rewardItems.clear();
    m_fieldB8 = script.m_rewardType;
    m_fieldC0 = script.m_field1e4;
    m_fieldBC = script.m_field228;
    if ((m_fieldB8 == 0) || (m_fieldB8 == 0x19))
    {
        m_titleReward = script.m_titleReward;
        m_selectReward = script.m_selectReward;
        m_fieldDC = script.m_field20c;
        m_fieldE8 = script.m_field218;
        makeRewardItemInfo(script.m_rewardData, m_rewardItems);
    }
    m_fieldF8 = script.m_field128;
    m_fieldFC = script.m_field12c;
    m_exposeQuest = script.m_field22d;
    m_field102 = script.m_field23d;
    m_field104 = script.m_field240;
    m_mailFlag = script.m_mailQuestFlag;
    m_mailTitle = script.m_mailTitle;
    m_mailContents = script.m_mailContents;
    m_mailItems = script.m_mailItems;
    m_field120 = script.m_field270;
    m_eventDialogs = script.m_eventDialogs;
    m_powerSide = script.m_powerSide;
    m_field134 = 0;
    m_field134 = script.m_fieldD8;
    m_field138 = script.m_field280;
    m_field13a = script.m_field282;
    m_field13c = script.m_field2a8;
    m_field140 = script.m_field2ac;
    m_exceptionList = script.m_exceptionList;
}
// ============================================================================
// 仅 ORIG 弱符号发射（DungeonClearItem，Quest.h 内联真实定义）：
//   C1/C2 0x0836de02（体=call clear）／clear 0x0836ddc2（itemIdx=0,
//   dungeonIdx=-1, rewardItem=0, count=0, percent=0, field14=-1）／
//   D1/D2 0x08a6866c（空体）。Quest.cpp 现有代码只经 vector::at/operator[]
//   拷贝或引用访问元素，未 odr-use 默认构造/clear/析构 → 弱符号不发射，
//   此处真实使用强制发射（不改语义、不造符号）。
namespace
{
#include "Quest/emitDungeonClearItemWeakSymbols.inc"
{
    DungeonClearItem item;   // C1/C2（调 clear）
    item.clear();            // clear
    item.~DungeonClearItem();  // D1/D2
}
// QuestList::getQuestList（CDataManager.h 类内内联，ORIG W 0x08374c7a：
// 8 字节 = mov this,%eax; ret）。Quest.cpp 现有代码不 odr-use 它
// （重建访问 QuestList 均经 G_CDataManager()->m_questList 成员直访）→
// 弱符号不发射；此处真实使用强制发射（不改语义、不造符号）。
#include "Quest/emitQuestListGetWeakSymbol.inc"
{
    QuestList* ql = G_CDataManager()->m_questList;
    QuestList* p = ql->getQuestList();
    (void)p;
}
}
