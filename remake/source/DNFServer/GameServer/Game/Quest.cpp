#include "GameRand.h"
// df_game_r 任务模板 Quest 实现（G3-2 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/Quest.md（Ghidra C）+ ORIG 反汇编。
#include <utility>
#include <vector>

#include "Quest.h"
#include "CUser.h"
#include "CDataManager.h"

// ============================================================================
// 其它 TU 提供的符号（ORIG 真实符号）
// ============================================================================

extern "C" char sub_CPowerManager_GetWinnerSide()
    asm("_ZN13CPowerManager13GetWinnerSideEv");

class CDailyScheduleManager
{
public:
    int GetScheduleState(_DailyScheduleType type, unsigned int questIdx) const;
};

namespace GlobalData
{
extern CDailyScheduleManager s_DailyScheduleManager;  // ORIG 0x941f784
}

void makeRewardItemInfo(const std::vector<int>& src,
                        std::vector<RewardItemInfo>& out);
int get_rand_int(int range);

// stQuestTriggerState_t 辅助（ORIG 0x836dce0/0x836dd68：9 位触发值打包）
static void SetTrigger(stQuestTriggerState_t& state, short a, short b, short c)
{
    state.m_trigger0 = (short)((state.m_trigger0 & ~0x1ff) | (a & 0x1ff));
    state.m_trigger1 = (short)((state.m_trigger1 & ~0x1ff) | (b & 0x1ff));
    state.m_trigger2 = (short)((state.m_trigger2 & ~0x1ff) | (c & 0x1ff));
}

static int GetTrigger(const stQuestTriggerState_t& state)
{
    return (state.m_trigger0 & 0x1ff) |
           ((state.m_trigger1 & 0x1ff) << 9) |
           ((state.m_trigger2 & 0x1ff) << 18);
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
            // ORIG 此处日志：Quest Script Error Break !, [grow type] != -1
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

    if (level < m_levelMin)
    {
        return 0;
    }
    if (m_levelMax < level)
    {
        return 0;
    }
    if ((m_questCountLimit != 0) && (m_questCountLimit <= questCount))
    {
        return 0;
    }

    if (m_type == 1)
    {
        if (G_CDataManager()->isThereDailyTrainingQuestList(level, m_index) != 1)
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
    char winner = sub_CPowerManager_GetWinnerSide();
    if (m_powerSide == 0)
    {
        if (winner == side)
        {
            return 1;
        }
    }
    else if (m_powerSide != 1)
    {
        // ORIG 此处日志：Quest Script Error q_index(%d)
        return 0;
    }
    else
    {
        winner = sub_CPowerManager_GetWinnerSide();
        if (winner != side)
        {
            return 1;
        }
    }
    return 0;
}

bool Quest::checkPossibleJob(ENUM_CHARACTERJOB job) const
{
    for (std::vector<stQuestTargetCharacter_t>::const_iterator it =
             m_targetCharacters.begin();
         it != m_targetCharacters.end(); ++it)
    {
        if ((it->m_job == (int)job) || (it->m_job == 0xb))
        {
            return 1;
        }
    }
    return 0;
}

bool Quest::_isNeededCheckGrowType(ENUM_CHARACTERJOB job) const
{
    for (std::vector<stQuestTargetCharacter_t>::const_iterator it =
             m_targetCharacters.begin();
         it != m_targetCharacters.end(); ++it)
    {
        if (((it->m_job == (int)job) || (it->m_job == 0xb)) &&
            (it->m_firstGrow == -1))
        {
            return 0;
        }
    }
    return 1;
}

bool Quest::checkMatchedGrowType(ENUM_CHARACTERJOB job, int firstGrow,
                                 int secondGrow) const
{
    for (std::vector<stQuestTargetCharacter_t>::const_iterator it =
             m_targetCharacters.begin();
         it != m_targetCharacters.end(); ++it)
    {
        if (((it->m_job == (int)job) || (it->m_job == 0xb)) &&
            ((it->m_firstGrow == firstGrow) || (it->m_firstGrow == -1)) &&
            ((it->m_secondGrow == secondGrow) || (it->m_secondGrow == -1)))
        {
            return 1;
        }
    }
    return 0;
}

bool Quest::check_clear_item(int itemIdx, int dungeonIdx) const
{
    if (m_clearItems.size() != 0)
    {
        for (unsigned int i = 0; i < m_clearItems.size(); ++i)
        {
            const DungeonClearItem& item = m_clearItems[i];
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
    if (!out.empty())
    {
        return 0;
    }
    out.reserve(m_killItems.size());
    for (unsigned int i = 0; i < m_killItems.size(); ++i)
    {
        const MonsterKillItem& item = m_killItems[i];
        if (((item.m_dungeonIdx == -1) || (item.m_dungeonIdx == dungeonIdx)) &&
            ((item.m_mapIdx == -1) || (item.m_mapIdx == mapIdx)) &&
            (item.m_enemyType == (int)enemyType) &&
            (item.m_killCount == killCount))
        {
            out.push_back(item);
        }
    }
    return out.empty() ? 0 : 1;
}

char Quest::check_clear_send() const
{
    return m_clearSendFlag;
}

int Quest::get_init_trigger() const
{
    if ((m_triggerType == 2) || (m_triggerType == 6))
    {
        short values[3] = { 0, 0, 0 };
        int idx = 0;
        std::vector<int>::const_iterator it = m_triggerData.begin();
        while (it != m_triggerData.end())
        {
            // ORIG 每次迭代前进 4（type 6 为 5）个 int，取最后一项
            ++it;
            if (it == m_triggerData.end()) break;
            ++it;
            if (it == m_triggerData.end()) break;
            ++it;
            if (it == m_triggerData.end()) break;
            if (m_triggerType == 6)
            {
                ++it;
                if (it == m_triggerData.end()) break;
            }
            ++it;
            if (it == m_triggerData.end()) break;
            values[idx] = (short)*it;
            ++idx;
        }
        stQuestTriggerState_t state;
        SetTrigger(state, values[0], values[1], values[2]);
        return GetTrigger(state);
    }
    if (m_triggerType == 0x19)
    {
        stQuestTriggerState_t state;
        SetTrigger(state, 1, 1, 0);
        return GetTrigger(state);
    }
    if ((m_triggerType == 1) && (m_triggerSubType == 6) &&
        (m_triggerData.size() == 3))
    {
        if (m_triggerData[2] > 0)
        {
            return m_triggerData[2];
        }
    }
    return 1;
}

bool Quest::get_init_achievement_trigger(unsigned short& t0,
                                         unsigned short& t1,
                                         unsigned short& t2) const
{
    if (m_type == 7)
    {
        unsigned short* out[3];
        out[0] = &t0;
        out[1] = &t1;
        out[2] = &t2;
        t0 = 1;
        t1 = 0;
        t2 = 0;
        unsigned int n = m_field90.size();
        if (n > 3)
        {
            n = 3;
        }
        for (unsigned int i = 0; i < n; ++i)
        {
            *out[i] = (unsigned short)m_field90[i];
        }
        return 1;
    }
    return 0;
}

int Quest::get_title_reward() const
{
    if (m_type == 7)
    {
        if (m_titleReward.empty())
        {
            return 0;
        }
        return m_titleReward[0].m_itemIdx;
    }
    return 0;
}

bool Quest::isRepeatableQuest() const
{
    return (m_type == 4) || (m_type == 8);
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
    for (unsigned int i = 0; i < m_clearItems.size(); ++i)
    {
        const DungeonClearItem& item = m_clearItems[i];
        if (item.m_itemIdx != dungeonIdx)
        {
            continue;
        }
        CItem* pItem = G_CDataManager()->find_item(item.m_rewardItem);
        if (!pItem)
        {
            continue;
        }
        if (!pItem->is_stackable())
        {
            if (get_rand_int(100) <= item.m_percent)
            {
                out.push_back(std::make_pair(item.m_rewardItem, 1));
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
                out.push_back(std::make_pair(item.m_rewardItem, count));
            }
        }
    }
    return out;
}

bool Quest::IsOpenScheduleQuest() const
{
    int state = GlobalData::s_DailyScheduleManager.GetScheduleState(
        (_DailyScheduleType)1, (unsigned int)m_index);
    return (state == 1) || (state == 2);
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
    else if ((m_appearMap.m_mapA == mapA) && (m_appearMap.m_mapB == mapB) &&
             (get_rand_int(100) <= m_appearMap.m_percent))
    {
        return m_appearMap.m_appearMap;
    }
    return 0;
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
    m_levelMin = script.m_levelMin;
    m_levelMax = script.m_levelMax;
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
