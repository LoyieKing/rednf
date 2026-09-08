#include "LogManager.h"
// ============================================================================
// df_game_r UserQuest（G2-5 批次）逐函数还原
// 语义依据：docs/class_func_reports/UserQuest.md（汇编为准）+ ORIG 反汇编直读。
// 布局（由 ORIG ctor/dtor 推导，总大小 0x761d pack(1)）：
//   +0x0000 CUser* m_user
//   +0x0004 WongWork::CQuestClear m_cleared（0x7530）
//   +0x7538 int m_quest[20]、+0x7588 int m_trigger[20]
//   +0x75d8 _Quest_Authen_Data（0x44：characNo/characClass/…/urgentQuest/authen[]）
//   +0x761c bool m_field761c（0x761d = CUser.h 的 m_pcRoomAuth 起点）
// 本 TU 定义 UserQuest 全部方法（构造/析构/23 个报告函数 + set_parent），
// 跨类调用均为真实类型 C++ 调用（无 asm 桥）。
// ============================================================================

#include <cstring>
#include <list>
#include <map>
#include <vector>

// ---- 权威头 ----
#include "CDataManager.h"      // CDataManager / QuestList / ENUM_QUEST_GRADE
#include "Quest.h"             // 全局 Quest（权威 0x150 布局；替代本 TU 旧局部视图）
#include "CUserCharacInfo.h"   // CUserCharacInfo（CUser 基类）权威声明
#include "CUser.h"              // 全局权威 CUser / UserQuest（本 TU 实现其方法）
#include "QuestClear.h"        // WongWork::CQuestClear / STClearedQuest
#include "InterfacePacketBuf.h"
#include "CTimeGate.h"
#include "GlobalData.h"      // s_timeGate_（get_quest_info 经 ORIG `mov 0x941f7f4` 读指针）
#include "CPowerManager.h"
#include "GameTypes.h"         // cMyTrace

// ============================================================================
// 本地最小视图（权威声明见 CUser.h / CInventory.h / CBattle_Field.h；因这些头
// 内置最小 UserQuest 与本 TU 完整 UserQuest 冲突，暂不 include —— 与
// CAchievement.cpp 同款模式；方法定义在权威 TU，mangled 与 ORIG 一致）
// ============================================================================




// ---- ORIG 域常量（nm：_ZN21UrgentQuestLevelLimitL…E，r 段）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
namespace UrgentQuestLevelLimit
{
static const int kStartLvl = 27;   // 0x08ae5d64 (0x1b)
static const int kEndLvl = 70;     // 0x08ae5d68 (0x46)
}

// ---- CUser 本地视图（继承 CUserCharacInfo.h 权威基类）----
class CInventory
{
public:
    int erase_repeated_item(const std::vector<std::pair<int, int> >& items,
                            std::vector<std::pair<int, int> >& out);  // 返回 out.size()（ORIG %eax）
    void CalNeedSlot(int* out,
                     const std::vector<std::pair<int, int> >& items) const;
    int QuickEmptyCount() const;
    int InvenEmptycount(Inven_Item::ITEM_TYPE type) const;
    int insert_event_items(const std::vector<std::pair<int, int> >& items,
                           std::vector<int>& outSlots, eMoneyAddReason moneyReason,
                           eItemAddReason itemReason, bool bLog, const char* a,
                           const char* b);
};
class CBattle_Field
{
public:
    void* getMaze();  // _ZN13CBattle_Field7getMazeEv（权威见 CBattle_Field.h，因含 CUser.h 未引入）
};

// getMaze() 返回迷宫对象 +0x50 处的 quest idx 字段视图（giveup_quest ORIG
// 单指令 mov 0x50(%eax) 折叠需要组件视图）
struct MazeQuestView
{
    char m_pad[0x50];  // +0x00
    int m_questIdx;    // +0x50
};

// ---- stQuestTriggerState_t（8 字节：3 个 9 位值；方法定义在 QuestTriggerState.cpp）----


// Quest 类型改用 Quest.h 全局权威类（0x150；本 TU 旧局部视图字段名映射：
//   m_levelLimit→m_levelMax、m_rewardItems→m_rewardSelectList、m_questType→m_triggerType、
//   m_questSubType→m_triggerSubType、m_params→m_triggerData、m_eventList→m_eventDialogs、
//   m_exceptions→m_exceptionList——偏移逐一相同，2026-08-30 统一）。

// ============================================================================
// 跨类调用（全部真实类型调用；asm 桥已删除）
// ============================================================================

bool checkAcceptableQuest(const Quest* quest,
                          const WongWork::CQuestClear& cleared,
                          const stSelectQuestParam& param)
{
    if (quest->m_index == 0x3f8) return true;
    if (!quest->exposeQuest()) return false;
    if (!quest->check_possible(param)) return false;
    if (!quest->isRepeatableQuest())
    {
        if (!G_CDataManager()->m_questList->check_clear(
                quest->m_index, cleared))
        {
            return false;
        }
    }
    if (!G_CDataManager()->m_questList->check_ahead_quest(
            quest->m_index, cleared))
    {
        return false;
    }
    if (!G_CDataManager()->m_questList->check_anti_quest(
            quest->m_index, cleared))
    {
        return false;
    }
    return true;
}

// ============================================================================
// UserQuest（总大小 0x761d pack(1)，布局见文件头；与 CUser.h 同步）
// ============================================================================

#pragma pack(push, 1)
#pragma pack(pop)

// ============================================================================
// 构造 / 析构 / set_parent
// ============================================================================

void UserQuest::set_parent(CUser* user)
{
    m_user = user;
}

// ============================================================================
// reset / resetClearQuest / isClearQuest
// ============================================================================

void UserQuest::reset()
{
    m_user = 0;
    m_cleared.clear();
    for (int i = 0; i <= 19; ++i)
    {
        m_quest[i] = 0;
        m_trigger[i] = 0;
    }
    m_authenData.m_urgentQuest = -1;
    memset(m_authenData.m_authen, 0, 0x28);
}

void UserQuest::resetClearQuest(int questIdx)
{
    m_cleared.resetClearedQuests(questIdx);
}

// [b28c-L9] SIGFIX:UserQuest::isClearQuest —— ORIG 0x086ab920 体为
// `call _ZNK8WongWork11CQuestClear14isClearedQuestEj; leave; ret` 直返无归一化；
// callee 权威型 unsigned char（QuestClear.h:41）⇒ caller 同型化（P9/P174）。
bool UserQuest::isClearQuest(int questIdx) const
{
    return m_cleared.isClearedQuest(questIdx);
}

// ============================================================================
// set_quest / get_quest
// ============================================================================

bool UserQuest::set_quest(SIG_LOAD_QUEST const* sig)   // [b28c-L9] SIGFIX:UserQuest::set_quest (ORIG 串 bool)
{
    for (int i = 0; i <= 19; ++i)
    {
        m_quest[i] = sig->m_quest[i];
        m_trigger[i] = sig->m_trigger[i];
        Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
        if (quest == 0 && m_quest[i] != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x42, 0)
                ("VERIFY_QUEST : ABNOMAL QUEST INDEX (%d) : CHARAC_NO (%u)",
                 m_quest[i], sig->m_characNo);
            m_quest[i] = 0;
            m_trigger[i] = 0;
        }
    }
    m_cleared.setClearedQuest(
        (const WongWork::STClearedQuest*)&sig->m_cleared[0]);
    m_authenData.m_urgentQuest = sig->m_urgentQuest;
    memcpy(m_authenData.m_authen, sig->m_authen, 0x28);
    return 1;
}

int UserQuest::get_quest(SIG_LOAD_QUEST* sig) const
{
    sig->m_field04 = 0;
    for (int i = 0; i <= 19; ++i)
    {
        sig->m_quest[i] = m_quest[i];
        sig->m_trigger[i] = m_trigger[i];
    }
    memcpy(sig->m_cleared, m_cleared.getClearedQuest(), 0x7530);
    sig->m_urgentQuest = m_user->getCurCharacQuestR()->m_authenData.m_urgentQuest;
    memcpy(sig->m_authen, m_authenData.m_authen, 0x28);
    return 1;
}

// ============================================================================
// get_quest_info / get_mail_quest_info
// ============================================================================

void UserQuest::get_quest_info(char* buf) const
{
    // b28d-40: N82 封顶——帧/槽平移（-0x54 vs -0x50 等），语义闭合。ORIG 0x086abba8。
    stSelectQuestParam selectParam(m_user);

    InterfacePacketBuf* pb = reinterpret_cast<InterfacePacketBuf*>(buf);
    pb->put_header(0, 0x15);

    std::list<int> questList;
    G_CDataManager()->m_questList->select_quest(questList, selectParam, m_cleared);
    if (selectParam.m_questCount != -1)
    {
        questList.push_back(selectParam.m_questCount);
    }

    pb->put_byte(m_user->get_charac_level());
    int index = pb->get_index();
    int count = 0;
    pb->put_short(0);

    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questIdx = *it;
        // ORIG get_quest_info 读取全局 s_timeGate_ 指针（mov 0x941f7f4），
        // 而非把指针变量的地址当作对象；改用 GlobalData::s_timeGate_。
        if (!GlobalData::s_timeGate_->setBlindTimeGateStateQuest(questIdx))
        {
            ++count;
            pb->put_short(questIdx);
        }
    }

    pb->put_short(index, count);
    pb->finalize(true);
}

bool UserQuest::get_mail_quest_info(std::list<int>& questList,
                                    stSelectQuestParam const& selectParam) const
{
    return G_CDataManager()->m_questList->select_mail_quest(questList, m_cleared, selectParam);
}
// ============================================================================
// set_authen_data
// ============================================================================

void UserQuest::set_authen_data(QUEST_CONDITION cond, int v1, int v2)
{
    switch (cond)
    {
    case QUEST_CONDITION_0:
        m_authenData.m_characNo = (short)v1;
        m_authenData.m_characClass = (short)v2;
        break;
    case QUEST_CONDITION_1:
        m_authenData.m_field4 = (short)v1;
        break;
    case QUEST_CONDITION_2:
        m_authenData.m_field8 = v1;
        break;
    case QUEST_CONDITION_3:
        m_authenData.m_fieldc = (char)(v1 != 0);
        break;
    case QUEST_CONDITION_4:
        m_authenData.m_field10 = v1;
        break;
    case QUEST_CONDITION_5:
        m_authenData.m_field14 = v1;
        break;
    }
}

// ============================================================================
// accept_quest
// ============================================================================

int UserQuest::accept_quest(int questIdx, char* buf, int& trigger)
{
    // b28d-40: N82 封顶——帧 0xcc vs 0xc0 + 字符串地址重定位/槽平移，语义闭合。
    // ORIG 0x086abf38。
    if (m_user == 0)
    {
        LogManager::logFormat(1, "user_quest.cpp",
                                 __PRETTY_FUNCTION__, 0x1e8,
                                 "user is null");
        return 1;
    }
    if (m_user->getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1ef, 5);
        trace("UserQuest::accept_quest user->m_selected is null");
        return 1;
    }

    Quest* quest = G_CDataManager()->find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(1, "user_quest.cpp",
                                 __PRETTY_FUNCTION__, 0x1f8,
                                 "G_CDataManager()->find_quest(%d)", questIdx);
        return 1;
    }

    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            return 0x12;
        }
    }

    if (!m_user->IsGameMasterMode())
    {
        if (!quest->isRepeatableQuest() &&
            m_cleared.isClearedQuest(questIdx))
        {
            return 0x12;
        }
    }

    stSelectQuestParam selectParam(m_user);
    if (!m_user->IsGameMasterMode())
    {
        UserQuest* cur = m_user->getCurCharacQuestR();
        if (!checkAcceptableQuest(quest, cur->m_cleared, selectParam))
        {
            m_user->getHackAnalyzer()->addServerHackCnt(
                m_user, (WongWork::ENUM_HACKTYPE)0x191, 1, 0, 0);
            return 1;
        }
    }

    std::vector<std::pair<int, int> > rewardItems;
    std::vector<std::pair<int, int> > erasedItems;
    rewardItems = quest->m_rewardSelectList;

    int erased = m_user->getCurCharacInvenW()->erase_repeated_item(
        rewardItems, erasedItems);
    if (erased > 0)
    {
        // Quest.h 权威 stSelectQuestParam 为命名成员（无 m_data 数组）——按连续布局取指针。
        int* pdata = &selectParam.m_level;
        for (int i = 0; i < 11; ++i)
        {
            pdata[i] = 0;
        }
        const CInventory* invR = m_user->getCurCharacInvenR();
        invR->CalNeedSlot(&selectParam.m_level, erasedItems);
        int quick = invR->QuickEmptyCount();
        for (int i = 1; i <= 10; ++i)
        {
            int cnt = invR->InvenEmptycount((Inven_Item::ITEM_TYPE)i);
            if (pdata[i] > cnt)
            {
                quick += cnt - pdata[i];
                if (quick < 0)
                {
                    return 0x11;
                }
            }
        }
    }

    trigger = quest->get_init_trigger();
    for (int i = 0; i < 20; ++i)
    {
        if (m_quest[i] == 0)
        {
            m_cleared.resetClearedQuests(questIdx);
            m_quest[i] = questIdx;
            m_trigger[i] = trigger;

            InterfacePacketBuf* pb = reinterpret_cast<InterfacePacketBuf*>(buf);
            pb->put_byte(1);
            pb->put_short(questIdx);
            pb->put_int(trigger);

            std::vector<int> itemIdx;
            int insertResult = m_user->getCurCharacInvenW()->insert_event_items(
                erasedItems, itemIdx,
                (eMoneyAddReason)3, (eItemAddReason)8, false, "", "");
            pb->put_byte(insertResult);

            for (std::vector<int>::iterator it = itemIdx.begin();
                 it != itemIdx.end(); ++it)
            {
                pb->put_short(*it);
                ++it;
                pb->put_int(*it);
                ++it;
                pb->put_int(*it);
            }
            return 0;
        }
    }
    return 4;
}

// ============================================================================
// giveup_quest
// ============================================================================

int UserQuest::giveup_quest(int questIdx)
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            Quest* quest = G_CDataManager()->find_quest(questIdx);
            if (quest != 0)
            {
                if (quest->m_field120 == 1 || quest->m_field120 == 2)
                {
                    if (m_trigger[i] == 0)
                    {
                        return 0x12;
                    }
                    if (m_user->get_state() == 5)
                    {
                        void* party = m_user->GetParty();
                        if (party != 0)
                        {
                            void* maze = reinterpret_cast<CBattle_Field*>(static_cast<char*>(party) + 0xb24)->getMaze();
                            if (maze != 0 &&
                                reinterpret_cast<MazeQuestView*>(maze)->m_questIdx == questIdx)
                            {
                                return 0x14;
                            }
                        }
                    }
                }
                if (quest->m_type == 8)
                {
                    if (m_user->get_charac_level() > quest->m_levelMax)
                    {
                        m_authenData.m_urgentQuest = -1;
                    }
                    if (hasEpicQuest())
                    {
                        m_authenData.m_urgentQuest = -1;
                    }
                }
            }

            m_quest[i] = 0;
            m_trigger[i] = 0;
            return 0;
        }
    }
    return 0x13;
}

// ============================================================================
// set_trigger
// ============================================================================

int UserQuest::set_trigger(int questIdx, char type, bool bIncrement)
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            if (m_user->getGmQuestFlag())
            {
                m_trigger[i] = 0;
            }

            if (type == 1)
            {
                ++m_trigger[i];   // b25c：++ 形 4.4 单次装载 i（探针 probe_inc s3），ORIG 同构
            }
            else
            {
                if (!check_quest_condition(questIdx))
                {
                    return -1;
                }
                if (bIncrement)
                {
                    stQuestTriggerState_t state;
                    state = m_trigger[i];
                    state.incTirggerState(type);
                    m_trigger[i] = state.getTrigger();
                }
                else if (m_trigger[i] > 0)
                {
                    stQuestTriggerState_t state;
                    state = m_trigger[i];
                    state.subTriggerState(type);
                    m_trigger[i] = state.getTrigger();
                }
            }
            return m_trigger[i];
        }
    }
    return -1;
}

// ============================================================================
// finish_quest
// ============================================================================

int UserQuest::finish_quest(int questIdx)
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            if (m_trigger[i] == 0)
            {
                m_quest[i] = 0;
                m_trigger[i] = 0;

                Quest* quest = G_CDataManager()->find_quest(questIdx);
                if (quest != 0 && !quest->isRepeatableQuest())
                {
                    m_cleared.setClearedQuest(questIdx);
                }
                if (quest->m_type == 8)
                {
                    m_authenData.m_urgentQuest = -1;
                }
                return 0;
            }
            return 0x16;
        }
    }
    return 0x13;
}

// ============================================================================
// item_reward_quest / get_rescue_questmap / check_cond_clear
// ============================================================================

// b28d-40: P223 双守卫分落——外层正向 if 包裹 + 内层 continue 复现
// ORIG 第一守卫直落循环尾、第二守卫落 nop 岛；0x086ac954，AE。
int UserQuest::item_reward_quest(int mapIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            if (m_trigger[i] == 0)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest == 0)
            {
                return 0;
            }
            if (quest->check_clear_item(mapIdx, (int)m_authenData.m_characClass))
            {
                return m_quest[i];
            }
        }
    }
    return 0;
}

// b28d-40: P223 双守卫分落（同 item_reward_quest 形）；0x086aca16，AE。
int UserQuest::get_rescue_questmap(int mapIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] > 0)
        {
            if (m_trigger[i] == 0)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest == 0)
            {
                return 0;
            }
            int map = quest->get_appearmap(mapIdx, (int)m_authenData.m_characClass);
            if (map > 0)
            {
                return map;
            }
        }
    }
    return 0;
}

bool UserQuest::check_cond_clear(int questIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_user->getGmQuestFlag())
        {
            if (m_quest[i] == questIdx)
            {
                return 1;
            }
        }
        else if (m_quest[i] == questIdx && m_trigger[i] == 0)
        {
            return 1;
        }
    }
    return 0;
}

// ============================================================================
// check_quest_condition / check_quest_type_condition /
// check_quest_subtype_condition
// ============================================================================

bool UserQuest::check_quest_condition(int questIdx)
{
    Quest* quest = G_CDataManager()->m_questList->find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "user_quest.cpp", __PRETTY_FUNCTION__, 0x403,
            "G_CDataManager()->m_pQuestList->find_quest(%d) fail", questIdx);
        return 0;
    }
    return check_quest_type_condition(quest);
}

bool UserQuest::check_quest_type_condition(Quest* quest)
{
    switch (quest->m_triggerType)
    {
    case 0:
    case 1:
    case 2:
    case 4:
    case 6:
    case 0xf:
    case 0x19:
        return 1;
    case 5:
        if (quest->m_triggerData.at(0) <=
            m_user->getPVPResultR()->m_pvpGrade)
        {
            return 1;
        }
        return 0;
    case 3:
        if (quest->m_triggerData.at(0) == (int)(short)m_authenData.m_field4)
        {
            return 1;
        }
        return 0;
    case 8:
        if (quest->m_triggerData.at(0) <= (int)(short)m_user->GetUserPowerWarPoint())
        {
            return 1;
        }
        return 0;
    case 7:
        if (GlobalData::s_power_manager->GetWinnerSide() ==
            m_user->getPowerSide())
        {
            return 1;
        }
        return 0;
    case 0x10:
    case 0x13:
        return 1;
    default:
        return check_quest_subtype_condition(quest);
    }
}

bool UserQuest::check_quest_subtype_condition(Quest* quest)
{
    switch (quest->m_triggerSubType)
    {
    case 0:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (at2 * 1000 < m_authenData.m_field8)
        {
            return 0;
        }
        return 1;
    }
    case 1:
    case 2:
    case 3:
    case 6:
        return 1;
    case 4:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (m_authenData.m_fieldc != 0)
        {
            return 0;
        }
        return 1;
    }
    case 5:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass < at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (at2 == 1 && m_authenData.m_field10 != 1)
        {
            return 0;
        }
        if (m_authenData.m_field10 < at2)
        {
            return 0;
        }
        return 1;
    }
    case 7:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (m_authenData.m_field14 < at2)
        {
            return 0;
        }
    }
    default:
        return 0;
    }
}

// ============================================================================
// IsDoingQuest / CheckDungeonEventStoryPause / isDoingJobChangeQuest
// ============================================================================

bool UserQuest::IsDoingQuest(int questIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx && m_trigger[i] != 0)
        {
            return 1;
        }
    }
    return 0;
}

int UserQuest::CheckDungeonEventStoryPause(int dungeonNo, int fieldNo, int mapNo) const
{
    // b28d-40: 两个 continue 改嵌套正向 if 包裹（消 4.4 同层多 continue 的 nop+jmp
    // 中继岛，ORIG 0x086ad079/0x086ad0a7 两守卫直接落外层循环尾 0x086ad159）。
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest != 0)
            {
                for (unsigned int j = 0; j < quest->m_eventDialogs.size(); ++j)
                {
                    switch (quest->m_eventDialogs[j].m_field0)
                    {
                    case 0:
                        if (quest->m_eventDialogs[j].m_field8 == fieldNo)
                        {
                            return 1;
                        }
                        break;
                    case 1:
                        if (quest->m_eventDialogs[j].m_field8 == dungeonNo)
                        {
                            return 1;
                        }
                        break;
                    }
                }
            }
        }
    }
    return 0;
}

bool UserQuest::isDoingJobChangeQuest() const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest != 0 && (quest->m_jobChangeQuest == 1 || quest->m_jobChangeQuest == 2))
            {
                return true;
            }
        }
    }
    return false;
}

// ============================================================================
// ResetUrgentQuestWaitingList / hasEpicQuest
// ============================================================================

void UserQuest::ResetUrgentQuestWaitingList()
{
    // [b28c-L9] fresh 复验 ndiff 11→1：唯一残差 = 4.4 把 quest2!=0 臂尾的
    // 「写 -1 + jmp end」与 else 臂尾的 jmp 合并（共享尾块少一枚 jmp）。
    Quest* quest2;
    for (int i = 0; i <= 19; ++i)
    {
        Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
        if (quest != 0 && quest->m_type == 8)
        {
            return;
        }
    }

    if (hasEpicQuest())
    {
        m_authenData.m_urgentQuest = -1;
    }
    else
    {
        quest2 = G_CDataManager()->find_quest(m_authenData.m_urgentQuest);
        // b25c 翻案：ORIG 4.1 把 quest2==0 的块外置在后（je 落点岛），
        // 且槽序 quest2/-0x18 i/-0x14 quest/-0x10 levelLimit/-0xc。
        // b28d-40：两臂 return 改自然结束（ORIG 0x086ad24e/0x086ad25d 各一条 jmp，
        // return 形多一条 nop+jmp 中继）。
        if (quest2 != 0)
        {
            int levelLimit = quest2->m_levelMax;
            if (levelLimit > UrgentQuestLevelLimit::kEndLvl)
            {
                levelLimit = UrgentQuestLevelLimit::kEndLvl;
            }
            if (m_user->get_charac_level() + 1 > levelLimit)
            {
                m_authenData.m_urgentQuest = -1;
            }
        }
        else
        {
            m_authenData.m_urgentQuest = -1;
        }
    }
}

bool UserQuest::hasEpicQuest()
{
    // b28d-40: N4 封顶——`lea 0x4(%edx)` vs 4.4 `add $0x4,%eax`（add-vs-lea），
    // 语义闭合。ORIG 0x086ad266。
    if (m_user == 0)
    {
        return false;
    }

    std::list<int> questList;
    stSelectQuestParam selectParam(m_user);

    char cleared[0x7531];
    memcpy(cleared, &m_user->getCurCharacQuestR()->m_cleared, 0x7531);

    std::multimap<int, int>* npcList =
        G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_NONE);
    G_CDataManager()->m_questList->allowable_questlist_as_npc(
        questList, *npcList, selectParam,
        *reinterpret_cast<const WongWork::CQuestClear*>(cleared), false);

    if (!questList.empty())
    {
        for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
        {
            Quest* quest = G_CDataManager()->find_quest(*it);
            if (quest == 0)
            {
                continue;
            }
            char bException = 0;
            for (unsigned int j = 0; j < quest->m_exceptionList.size(); ++j)
            {
                if (quest->m_exceptionList[j] == ENUM_QUEST_EXCEPTION_TYPE_0)
                {
                    bException = 1;
                    break;
                }
            }
            if (!bException && selectParam.m_level < quest->m_levelMin + 0xc)
            {
                return true;
            }
        }
    }
    return false;
}
