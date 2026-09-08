#include "GameRand.h"
// df_game_r 任务模板 Quest 实现（G3-2 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/Quest.md（Ghidra C）+ ORIG 反汇编。
// ============================================================================
// b28d-20 lane20 判词汇总（Quest.cpp 独占 TU，ORIG 权威地址见各行）：
//  [1] dungeonEventHanaseStruct::~dungeonEventHanaseStruct（定义在 Quest.h:195，
//      本 TU 无写权，未加行内判词）——ORIG 0x08a61164，52=52 助记符/槽位全等，
//      仅 .o 重定位标签差，TU 级 AE/IDENTICAL。
//  [2] Quest::check_possible 0x08352d86 —— NEAR（N93附 尾部双 return 块序：
//      4.1 Bfalse-ret0 在前，4.4 恒 Btrue-ret1 在前；四形探针全败）。
//  [3] makeRewardItemInfo(pair) 0x083520c8 —— AE/IDENTICAL（it++ 后置 +
//      cMyTrace 全表达式临时链，83=83）。
//  [4] stQuestTriggerState_t::setTrigger 0x0836dce0 —— N18a 封顶（4.1 每字段
//      多发 `mov %edx,%ecx; and $0x1ff,%cx` 冗余截断，4.4 恒折单次 and）。
//  [5] Quest::CheckKillMonster 0x083535d6 —— P223 多岛 goto 形已复现 3 岛；
//      残 1 条 `mov`（N18b/N7 寄存器选择，4.4 恒折 cmp MEM,reg）。
//  [6] Quest::get_appearmap 0x083536f2 —— N7/P197④ 封顶（4.1 setge 物化 +
//      callee-saved 保活；4.4 语句表达式形恒出 jl+mov$1/mov$0 分支物化）。
//  [7] makeRewardItemInfo(RewardItemInfo) 0x08352203 —— AE/IDENTICAL（同上）。
//  [8] Quest::ConvertRewardSelectIndex 0x083537c2 —— NEAR（P226 语句表达式 +
//      真语义修正 return i；残尾部 return 岛 1 条，P223 单源 goto 负例）。
//  [9] Quest::get_init_trigger 0x083530d6 —— NEAR（168=168 助记符全等；残
//      stQuestTriggerState_t 槽基 -0x4a vs ORIG -0x4c，N95/N93 槽对齐差）。
//  [10] Quest::get_clear_itemlist 0x08353404 —— AE/IDENTICAL（声明序 item/out/p
//      + 正向 is_stackable + 具名 randValue + pair 字段直赋）。
//  [11] Quest::set_quest 0x0835233e —— N82 封顶（P167 调用点差集 45=45 归零；
//      两个 trigger 循环按 ORIG 形重写 + trace 补齐；残全为槽号/寄存器分配）。
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
void makeRewardItemInfo(const std::vector<int>& src,
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
void makeRewardItemInfo(const std::vector<int>& src,
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
int get_rand_int(int range);

// stQuestTriggerState_t::setTrigger（ORIG W 0x836dce0：9 位触发值打包）
// b28c-L19 判词 N（4.1 位宽截断 lowering 不可复现）：ORIG 每字段在
// `and $0x1ff,%dx` 后额外发 `mov %edx,%ecx; and $0x1ff,%cx`（对 (a&0x1ff)
// 的寄存器副本再截断一次），4.4 恒折单次 and（探针：原形与显式嵌套
// `(short)((short)(a & 0x1ff) | …)` 两形均 38/41，无源码解）。
// b28d-20 N18a 封顶：本次再试 `(short)(a & 0x1ff) | (m_trigger0 & 0xfe00)`
// 形仍 38/41（ORIG 42），4.4 无源码形态产出该冗余截断。
void stQuestTriggerState_t::setTrigger(short a, short b, short c)
{
    m_trigger0 = (short)((short)(a & 0x1ff) | (m_trigger0 & 0xfe00));
    m_trigger1 = (short)((short)(b & 0x1ff) | (m_trigger1 & 0xfe00));
    m_trigger2 = (short)((short)(c & 0x1ff) | (m_trigger2 & 0xfe00));
}

// ============================================================================
// Quest
// ============================================================================

Quest::Quest()
{
}

Quest::~Quest()
{
}

bool Quest::exposeQuest() const
{
    return m_exposeQuest;
}

// b28d-20 NEAR：ORIG 0x08352d86，163/163 助记符同；残 N93附——尾部
// `if(expertJobLevel<m_field140) return 0; return 1;` 的 4.1 Bfalse-ret0 在前
// 布局，4.4 恒 Btrue-ret1 在前（显式 else/逆置/三目/正向包裹四形探针全败）。
bool Quest::check_possible(const stSelectQuestParam& param) const
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
        // X1 形（XOR_FAMILY.tsv，probe /tmp/xg2.cpp 形 B）：callee 返 int，
        // `!= 1` 在 4.4 产 `cmp $0x1,%eax; setne %al; test`，ORIG 是
        // `call; xor $0x1,%eax; test %al,%al; je`。
        register unsigned char daily asm("eax") =
            (unsigned char)G_CDataManager()->isThereDailyTrainingQuestList(level, m_index);
        daily ^= 1;
        if (daily)
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
            return 1;
        }
        return 0;
    }
    return 1;
}

// SIGFIX:Quest::check_power_side —— ORIG __PRETTY_FUNCTION__ 串 "bool Quest::check_power_side(char)"
// 定返回 bool（返回类型不入 mangled；P174）。
bool Quest::check_power_side(char side)
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

bool Quest::checkPossibleJob(ENUM_CHARACTERJOB job) const
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

bool Quest::_isNeededCheckGrowType(ENUM_CHARACTERJOB job) const
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

bool Quest::checkMatchedGrowType(ENUM_CHARACTERJOB job, int firstGrow,
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

bool Quest::check_clear_item(int itemIdx, int dungeonIdx) const
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

// b28d-20：ORIG 0x083535d6。P223 多岛复现成功（3 个 nop+jmp 岛 + 尾 nop，
// 与 ORIG 0x83536a9/ac/af 逐址对应）；残 1 条 `mov`（N18b：4.4 对
// `enemyType != item.m_enemyType` 恒载双寄存器，ORIG 单段 eax+mem cmp）。
bool Quest::CheckKillMonster(int mapIdx, int dungeonIdx, int killCount,
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
        if (item.m_enemyType != enemyType)
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

char Quest::check_clear_send() const
{
    return m_clearSendFlag;
}

// b28d-20 NEAR：ORIG 0x083530d6。`*it++` 后置形 + `{0}` 两写清零 + 拆嵌套
// if 后 168=168 助记符全等；残 stQuestTriggerState_t 槽基 -0x4a vs ORIG
// -0x4c（N95/N93：4.1 按 4 对齐该 8 字节对象，4.4 按 short 自然对齐 2）。
int Quest::get_init_trigger() const
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
        stQuestTriggerState_t state;
        state.setTrigger(values[0], values[1], values[2]);
        return state.getTrigger();
    }
    if (m_triggerType == 0x19)
    {
        stQuestTriggerState_t state;
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

bool Quest::get_init_achievement_trigger(unsigned short& t0,
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

int Quest::get_title_reward() const
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

bool Quest::isRepeatableQuest() const
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

// b28d-20 N7/P197④ 封顶：ORIG 0x083536f2。语句表达式形已复现 m_percent
// 升格 %ebx 跨 get_rand_int 保活；但 4.1 的 `setge %al; test` 物化在 4.4
// 恒退化为 `jl + mov$1/jmp/mov$0` 分支物化（while 形 55、裸形 50、语句
// 表达式 53，ORIG 47，无源码形态同时满足保活+setge）。
int Quest::get_appearmap(int mapA, int mapB) const
{
    // ORIG：m_percent 在 mapA 判定后、get_rand_int 前物化 %ebx 跨 call，
    // 尾 setge。register 局部声明前置即被 4.4 提前物化（+2 条、edx/eax
    // 交换，N7 族），分支内声明不可行（&& 短路），残差 N 封顶。
    if (m_appearMap.m_mapB == -1)
    {
        if ((m_appearMap.m_mapA == mapA) &&
            (({ m_appearMap.m_percent; }) >= get_rand_int(100)))
        {
            return m_appearMap.m_appearMap;
        }
    }
    else if ((m_appearMap.m_mapB == mapA) &&
             (m_appearMap.m_mapB == mapB) &&
             (m_appearMap.m_percent >= get_rand_int(100)))
    {
        return m_appearMap.m_appearMap;
    }
    return 0;
}


// b28d-20 NEAR：ORIG 0x083537c2。原嵌套 if+goto 产生 3 处 setcc 落栈物化；
// 改为单一短路表达式 + P226 语句表达式后 88/87，仅残尾部 `return i` 的
// 4.1 共享 nop 岛（P223 单源 goto 尾岛负例）。**真语义修正**：ORIG 循环
// 正常结束返回 i（== size()）而非 0xffffffff（早退才返回 -1），已按 ORIG 照抄。
unsigned int Quest::ConvertRewardSelectIndex(const CUser* user, int index)
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
               ((m_selectReward[i].m_job == user->get_charac_job()) &&
                ((m_selectReward[i].m_growType == -1) ||
                 (m_selectReward[i].m_growType ==
                  (int)user->getCurCharFirstGrowType()))); }))
        {
            if (index == count)
            {
                return i;
            }
            ++count;
        }
    }
    return i;
}

// b28d-20 AE：ORIG 0x08353404。声明序 item/out/p + pItem/randValue 循环外
// 具名（ORIG -0x2c/-0x28 清 0 实证）+ 正向 `if (is_stackable())`（消 xor $1）
// + pair 字段直赋（消 make_pair 临时）；134/134 槽位对齐。
std::vector<std::pair<int, int> > Quest::get_clear_itemlist(
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

bool Quest::IsOpenScheduleQuest() const
{
    int state = 0;
    int result = GlobalData::s_DailyScheduleManager->GetScheduleState(
        (_DailyScheduleType)1, (unsigned int)m_index);
    return (result == 1) || (result == 2);
}


// b28d-20 N82 封顶：ORIG 0x0835233e。appearMap 改整体赋值；两个 trigger
// 循环按 ORIG 形重写（循环1 `p.first=*it; it++` + 奇尾 cMyTrace 补齐；
// 循环2 `*it++` + `i<n && it!=end` 条件）；m_appearMap 结构拷贝、trace 补齐后
// p167 调用点差集 45=45 归零（语义完成）。残差全为槽号/寄存器分配（451/453）。
void Quest::set_quest(QuestScript script)
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
        std::pair<int, int> p;
        std::vector<int>::iterator it = m_triggerData.begin();
        while (it != m_triggerData.end())
        {
            p.first = *it;
            it++;
            if (it == m_triggerData.end())
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x109d, 5);
                trace("[%s][%d]_(it == intDatas_.end())",
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
        unsigned int n = (unsigned int)(m_triggerData.size() >> 1);
        std::vector<int>::iterator it = m_triggerData.begin();
        for (unsigned int i = 0; (i < n) && (it != m_triggerData.end()); ++i)
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
void emitDungeonClearItemWeakSymbols()
{
    DungeonClearItem item;   // C1/C2（调 clear）
    item.clear();            // clear
    item.~DungeonClearItem();  // D1/D2
}
// QuestList::getQuestList（CDataManager.h 类内内联，ORIG W 0x08374c7a：
// 8 字节 = mov this,%eax; ret）。Quest.cpp 现有代码不 odr-use 它
// （重建访问 QuestList 均经 G_CDataManager()->m_questList 成员直访）→
// 弱符号不发射；此处真实使用强制发射（不改语义、不造符号）。
void emitQuestListGetWeakSymbol()
{
    QuestList* ql = G_CDataManager()->m_questList;
    QuestList* p = ql->getQuestList();
    (void)p;
}
}
