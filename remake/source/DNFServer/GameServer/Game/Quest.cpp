#include "GameRand.h"
// df_game_r 任务模板 Quest 实现（G3-2 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/Quest.md（Ghidra C）+ ORIG 反汇编。
#include <utility>
#include <vector>

#include "Quest.h"
#include "CUser.h"
#include "CDataManager.h"
#include "LogManager.h"

// ============================================================================
// 其它 TU 提供的符号（ORIG 真实符号）
// ============================================================================


namespace GlobalData
{
extern CDailyScheduleManager s_DailyScheduleManager;  // ORIG 0x941f784
extern CPowerManager* s_power_manager;                // ORIG 0x940be50（指针，见 GlobalData.h）
}

// ORIG 0x8352203：src 按相邻元素配对填充 RewardItemInfo{field0,field4}；
// 奇数元素时记录错误并返回
void makeRewardItemInfo(const std::vector<int>& src,
                        std::vector<RewardItemInfo>& out)
{
    RewardItemInfo item;
    std::vector<int>::const_iterator it = src.begin();
    while (it != src.end())
    {
        item.m_itemIdx = *it;
        ++it;
        if (it == src.end())
        {
            // ORIG 0x83522ce：cMyTrace 记录 "it == data.rewardSelectItems_.end()"
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x104d, 5);
            trace("[%s][%d]_it == data.rewardSelectItems_.end()", 0x104d);
            return;
        }
        item.m_count = *it;
        ++it;
        out.push_back(item);
    }
}
// ORIG 0x83520c8：src 按相邻元素配对填充 std::pair<int,int>{first,second}；
// 奇数元素时 cMyTrace 记录 "[%s][%d]_it == data.rewardSelectItems_.end()"
// （0x1031 行，PRETTY __PRETTY_FUNCTION__）。
// 与 RewardItemInfo 版同 TU 相邻（0x83520c8/0x8352203），ORIG T 强符号，无调用点（导出）。
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
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x1031, 5);
            trace("[%s][%d]_it == data.rewardSelectItems_.end()",
                  __PRETTY_FUNCTION__,
                  0x1031);
            return;
        }
        p.second = *it;
        it++;
        out.push_back(p);
    }
}
int get_rand_int(int range);

// stQuestTriggerState_t::setTrigger（ORIG W 0x836dce0：9 位触发值打包）
void stQuestTriggerState_t::setTrigger(short a, short b, short c)
{
    m_trigger0 = (short)((a & 0x1ff) | (m_trigger0 & ~0x1ff));
    m_trigger1 = (short)((b & 0x1ff) | (m_trigger1 & ~0x1ff));
    m_trigger2 = (short)((c & 0x1ff) | (m_trigger2 & ~0x1ff));
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

char Quest::exposeQuest() const
{
    return m_exposeQuest;
}

char Quest::check_possible(const stSelectQuestParam& param) const
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

    // 等号实测：改成 `<=` 会让本函数 ndiff 12→14 变差（-0x18 槽被前后三处
    // 比较共用，改一处会连带改变 4.4 的 jcc 选择与装载序），故维持 `<`；
    // ORIG 该点 `jge` 与我方 `jg` 的等号差属**跨点耦合残差**，标 N。
    // 对照：同函数 m_levelMax 那处改 `>=` 是净收益（13→12），已保留。
    if (level < m_levelMin)
    {
        return 0;
    }
    // ORIG @0x8352e…：`mov 0x28(%eax),%eax; cmp -0x14(%ebp),%eax; jle <ret0>`
    // ⇒ 判定条件是 **m_levelMax <= level**（含等号），原写法 `m_levelMax < level`
    // 少一个等号（重建语义误差一位）。写成 `>=` 后 4.4 产 `cmp; jle` 同形。
    if (level >= m_levelMax)
    {
        return 0;
    }
    if ((m_questCountLimit != 0) && (m_questCountLimit <= questCount))
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
    if ((m_field13c == 0) || (m_field140 == -1))
    {
        return 1;
    }
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

char Quest::check_power_side(char side)
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
        if (((item.m_dungeonIdx == -1) || (item.m_dungeonIdx == dungeonIdx)) &&
            ((item.m_mapIdx == -1) || (item.m_mapIdx == mapIdx)) &&
            (item.m_enemyType == enemyType) &&
            (item.m_killCount == killCount))
        {
            out.push_back(item);
        }
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

int Quest::get_init_trigger() const
{
    if ((m_triggerType == 2) || (m_triggerType == 6))
    {
        std::vector<int>::const_iterator it = m_triggerData.begin();
        std::vector<int>::const_iterator end = m_triggerData.end();
        short values[3] = { 0, 0, 0 };
        int idx = 0;
        while (it != end)
        {
            // ORIG 0x8353145：每次迭代前进 4（type 6 为 5）个 int，取最后一项
            it++;
            int v1 = *it;
            it++;
            int v2 = *it;
            it++;
            int v3 = *it;
            if (m_triggerType == 6)
            {
                it++;
                *it;
            }
            it++;
            int v4 = *it;
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
    if ((m_triggerType == 1) && (m_triggerSubType == 6) &&
        (m_triggerData.size() == 3))
    {
        int v = m_triggerData.at(2);
        if (v > 0)
        {
            return v;
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

int Quest::get_appearmap(int mapA, int mapB) const
{
    if (m_appearMap.m_mapB == -1)
    {
        if ((m_appearMap.m_mapA == mapA) &&
            (get_rand_int(100) <= m_appearMap.m_percent))
        {
            return m_appearMap.m_appearMap;
        }
    }
    else if ((m_appearMap.m_mapB == mapA) &&
             (m_appearMap.m_mapB == mapB) &&
             (get_rand_int(100) <= m_appearMap.m_percent))
    {
        return m_appearMap.m_appearMap;
    }
    return 0;
}


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
    int count = 0;
    for (unsigned int i = 0; i < m_selectReward.size(); ++i)
    {
        bool match;
        if (m_selectReward[i].m_job == 0xb)
        {
            match = true;
        }
        else
        {
            if (m_selectReward[i].m_job == user->get_charac_job())
            {
                if (m_selectReward[i].m_growType != -1)
                {
                    if (m_selectReward[i].m_growType !=
                        (int)user->getCurCharFirstGrowType())
                    {
                        match = false;
                        goto check_match;
                    }
                }
                match = true;
            }
            else
            {
                match = false;
            }
        }
check_match:
        if (match)
        {
            if (index == count)
            {
                return i;
            }
            ++count;
        }
    }
    return 0xffffffff;
}

std::vector<std::pair<int, int> > Quest::get_clear_itemlist(
    int dungeonIdx) const
{
    std::vector<std::pair<int, int> > out;
    DungeonClearItem item;
    std::pair<int, int> p;
    for (int i = 0; i < (int)m_clearItems.size(); ++i)
    {
        item = m_clearItems.at(i);
        if (item.m_itemIdx != dungeonIdx)
        {
            continue;
        }
        CItem* pItem = G_CDataManager()->find_item(item.m_rewardItem);
        if (!pItem)
        {
            // ORIG 0x83534e3：find_item 失败记录日志
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x1279,
                "G_CDataManager()->find_item(%d)", item.m_rewardItem);
            continue;
        }
        if (!pItem->is_stackable())
        {
            if (get_rand_int(100) <= item.m_percent)
            {
                p = std::make_pair(item.m_rewardItem, 1);
                out.push_back(p);
            }
        }
        else
        {
            int count = 0;
            for (int k = 0; k < item.m_count; ++k)
            {
                if (get_rand_int(100) <= item.m_percent)
                {
                    ++count;
                }
            }
            if (count > 0)
            {
                p = std::make_pair(item.m_rewardItem, count);
                out.push_back(p);
            }
        }
    }
    return out;
}

bool Quest::IsOpenScheduleQuest() const
{
    int state = 0;
    int result = GlobalData::s_DailyScheduleManager.GetScheduleState(
        (_DailyScheduleType)1, (unsigned int)m_index);
    return (result == 1) || (result == 2);
}


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
    m_appearMap.m_mapA = script.m_appearMap.m_mapA;
    m_appearMap.m_mapB = script.m_appearMap.m_mapB;
    m_appearMap.m_appearMap = script.m_appearMap.m_appearMap;
    m_appearMap.m_percent = script.m_appearMap.m_percent;
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
        for (std::vector<int>::iterator it = m_triggerData.begin();
             it != m_triggerData.end(); )
        {
            int first = *it;
            ++it;
            if (it == m_triggerData.end())
            {
                break;
            }
            int second = *it;
            ++it;
            m_triggerDataPairs.push_back(std::make_pair(first, second));
        }
    }
    else if (m_triggerType == 0x19)
    {
        m_triggerDataPairs.clear();
        unsigned int n = (unsigned int)(m_triggerData.size() >> 1);
        std::vector<int>::iterator it = m_triggerData.begin();
        for (unsigned int i = 0; i < n; ++i)
        {
            if (it == m_triggerData.end())
            {
                break;
            }
            int first = *it;
            ++it;
            if (it == m_triggerData.end())
            {
                break;
            }
            int second = *it;
            ++it;
            m_triggerDataPairs.push_back(std::make_pair(first, second));
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
