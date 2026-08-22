#include "LogManager.h"
// ============================================================================
// df_game_r UserQuest（G2-5 批次）逐函数还原
// 语义依据：docs/class_func_reports/UserQuest.md（汇编为准）+ ORIG 反汇编直读。
// 布局（由 ORIG ctor/dtor 推导，总大小 0x7620）：
//   +0x0000 CUser* m_user
//   +0x0004 WongWork::CQuestClear m_cleared（0x7530）
//   +0x7538 int m_quest[20]、+0x7588 int m_trigger[20]
//   +0x75d8 _Quest_Authen_Data（0x48：characNo/characClass/…/urgentQuest/authen[]）
// 跨类方法一律以 extern "C" asm 标签直调 ORIG 符号，本 TU 只定义 UserQuest
// 自身的方法（构造/析构/23 个报告函数 + set_parent）。
// ============================================================================

#include <cstring>
#include <list>
#include <map>
#include <vector>

// ---- 权威头 ----
#include "CDataManager.h"      // CDataManager / QuestList / ENUM_QUEST_GRADE
#include "CUserCharacInfo.h"   // CUserCharacInfo（CUser 基类）权威声明
#include "QuestClear.h"        // WongWork::CQuestClear / STClearedQuest
#include "InterfacePacketBuf.h"
#include "CTimeGate.h"
#include "CPowerManager.h"
#include "GameTypes.h"         // cMyTrace

// ============================================================================
// 本地最小视图（权威声明见 CUser.h / CInventory.h / CBattle_Field.h；因这些头
// 内置最小 UserQuest 与本 TU 完整 UserQuest 冲突，暂不 include —— 与
// CAchievement.cpp 同款模式；方法定义在权威 TU，mangled 与 ORIG 一致）
// ============================================================================

struct dungeonEventHanaseStruct
{
    int m_type;   // +0x00
    int m_pad4;   // +0x04
    int m_map;    // +0x08
};

enum ENUM_QUEST_EXCEPTION_TYPE
{
    QUEST_EXCEPTION_NONE = 0,
    QUEST_EXCEPTION_1 = 1
};

enum QUEST_CONDITION
{
    QUEST_CONDITION_0 = 0,
    QUEST_CONDITION_1 = 1,
    QUEST_CONDITION_2 = 2,
    QUEST_CONDITION_3 = 3,
    QUEST_CONDITION_4 = 4,
    QUEST_CONDITION_5 = 5
};

namespace WongWork
{

enum ENUM_HACK_TYPE
{
    ENUM_HACK_TYPE_QUEST = 0x191
};

class CHackAnalyzer
{
public:
    void addServerHackCnt(CUser* user, ENUM_HACK_TYPE type, unsigned int count,
                          unsigned int a, unsigned int b);  // _ZN8WongWork13CHackAnalyzer16addServerHackCntEP5CUserNS_13ENUM_HACKTYPEEjjj

    char m_pad[4];   // +0x00
};

}  // namespace WongWork

// ---- CUser 本地视图（继承 CUserCharacInfo.h 权威基类）----
class UserQuest;

class CUser : public CUserCharacInfo
{
public:
    UserQuest* getCurCharacQuestR() const;  // _ZNK5CUser18getCurCharacQuestREv
    bool IsGameMasterMode() const;          // _ZNK5CUser16IsGameMasterModeEv
    WongWork::CHackAnalyzer* getHackAnalyzer();  // _ZN5CUser15getHackAnalyzerEv
    int get_state();                        // _ZN5CUser9get_stateEv
    void* GetParty();                       // _ZN5CUser8GetPartyEv
    char getPowerSide();                    // _ZN5CUser12getPowerSideEv
    bool getGmQuestFlag();                  // _ZN5CUser14getGmQuestFlagEv
};

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

// ---- stSelectQuestParam（0x2c：11 个 int；ctor _ZN18stSelectQuestParamC1EP5CUser
//      定义在 stSelectQuestParam.cpp）----
class stSelectQuestParam
{
public:
    stSelectQuestParam(CUser* user);

    int m_data[11];   // +0x00（+0x24 = m_data[9] 为 select quest idx）
};

// ---- stQuestTriggerState_t（8 字节：3 个 9 位值；方法定义在 QuestTriggerState.cpp）----
class stQuestTriggerState_t
{
public:
    stQuestTriggerState_t();                          // _ZN21stQuestTriggerState_tC1Ev
    stQuestTriggerState_t& operator=(int const& v);   // _ZN21stQuestTriggerState_taSERKi
    void incTirggerState(char v);                     // _ZN21stQuestTriggerState_t15incTirggerStateEc
    void subTriggerState(char v);                     // _ZN21stQuestTriggerState_t15subTriggerStateEc
    int getTrigger();                                 // _ZN21stQuestTriggerState_t10getTriggerEv

    short m_trigger0;  // +0x00
    short m_trigger1;  // +0x02
    short m_trigger2;  // +0x04
};

// ---- _Quest_Authen_Data（权威见 CUser.h，ctor 定义在 CUser.cpp；本视图 0x44
//      覆盖 UserQuest +0x75d8 字段区，ctor 按 ORIG memset 0x48 含尾部两 bool）----
class _Quest_Authen_Data
{
public:
    _Quest_Authen_Data() throw();  // _ZN18_Quest_Authen_DataC1Ev（定义在 CUser.cpp；throw() 抑制 UserQuest ctor 的 EH 清理块，与 ORIG 一致）

    short m_characNo;      // +0x00
    short m_characClass;   // +0x02
    short m_field4;        // +0x04
    char m_pad6[2];        // +0x06
    int m_field8;          // +0x08
    char m_fieldc;         // +0x0c
    char m_padd[3];        // +0x0d
    int m_field10;         // +0x10
    int m_field14;         // +0x14
    int m_urgentQuest;     // +0x18
    char m_authen[0x28];   // +0x1c
};  // 0x44

// ---- SIG_LOAD_QUEST 布局（get_quest/set_quest 推导）----
struct SIG_LOAD_QUEST
{
    unsigned int m_characNo;    // +0x00
    int m_field04;              // +0x04
    char m_pad08[4];            // +0x08
    int m_quest[20];            // +0x0c
    int m_trigger[20];          // +0x5c
    int m_urgentQuest;          // +0xac
    char m_cleared[0x7530];     // +0xb0
    char m_authen[0x28];        // +0x75e0
};

// ---- Quest 布局（仅本 TU 访问到的字段；方法定义在 Quest.cpp，ORIG _ZNK5Quest* 族）----
struct Quest
{
    char exposeQuest() const;                                          // _ZNK5Quest11exposeQuestEv
    char check_possible(const stSelectQuestParam& param) const;        // _ZNK5Quest14check_possibleERK18stSelectQuestParam
    bool isRepeatableQuest() const;                                    // _ZNK5Quest17isRepeatableQuestEv
    int get_init_trigger() const;                                      // _ZNK5Quest16get_init_triggerEv
    int get_appearmap(int mapA, int mapB) const;                       // _ZNK5Quest13get_appearmapEii
    bool check_clear_item(int itemIdx, int dungeonIdx) const;          // _ZNK5Quest16check_clear_itemEii

    char m_pad00[4];                                  // +0x00
    int m_index;                                      // +0x04
    int m_type;                                       // +0x08
    char m_pad0c[0x10];                               // +0x0c
    int m_field1c;                                    // +0x1c
    int m_field20;                                    // +0x20
    int m_levelLimit;                                 // +0x24
    char m_pad28[0x70 - 0x28];                        // +0x28
    std::vector<std::pair<int, int> > m_rewardItems;  // +0x70
    int m_questType;                                  // +0x7c
    int m_questSubType;                               // +0x80
    std::vector<int> m_params;                        // +0x84
    char m_pad90[0x120 - 0x90];                       // +0x90
    char m_field120;                                  // +0x120
    char m_pad121[3];                                 // +0x121
    std::vector<dungeonEventHanaseStruct> m_eventList;   // +0x124
    char m_pad130[0x144 - 0x130];                     // +0x130
    std::vector<ENUM_QUEST_EXCEPTION_TYPE> m_exceptions; // +0x144
};

// ============================================================================
// 跨类调用（全部真实类型调用；asm 桥已删除）
// ============================================================================

bool checkAcceptableQuest(const Quest* quest,
                          const WongWork::CQuestClear& cleared,
                          const stSelectQuestParam& param)
{
    if (quest->m_index == 0x3f8) return true;
    if (!quest->exposeQuest() || !quest->check_possible(param)) return false;
    QuestList* list = G_CDataManager()->m_questList;
    if (!quest->isRepeatableQuest() &&
        !list->check_clear(quest->m_index, cleared)) return false;
    return list->check_ahead_quest(quest->m_index, cleared) &&
           list->check_anti_quest(quest->m_index, cleared);
}

// ============================================================================
// UserQuest（总大小 0x7620，布局见文件头）
// ============================================================================

class UserQuest
{
public:
    UserQuest();
    ~UserQuest();

    void set_parent(CUser* user);

    void reset();
    bool isClearQuest(int questIdx) const;
    void resetClearQuest(int questIdx);
    int set_quest(SIG_LOAD_QUEST const* sig);
    int get_quest(SIG_LOAD_QUEST* sig) const;
    void get_quest_info(char* buf) const;
    void get_mail_quest_info(std::list<int>& questList,
                             stSelectQuestParam const& selectParam) const;
    void set_authen_data(QUEST_CONDITION cond, int v1, int v2);
    int accept_quest(int questIdx, char* buf, int& trigger);
    int giveup_quest(int questIdx);
    int set_trigger(int questIdx, char type, bool bIncrement);
    int finish_quest(int questIdx);
    int item_reward_quest(int mapIdx) const;
    int get_rescue_questmap(int mapIdx) const;
    int check_cond_clear(int questIdx) const;
    int check_quest_subtype_condition(Quest* quest);
    int check_quest_type_condition(Quest* quest);
    int check_quest_condition(int questIdx);
    int IsDoingQuest(int questIdx) const;
    int CheckDungeonEventStoryPause(int dungeonNo, int fieldNo, int mapNo) const;
    void ResetUrgentQuestWaitingList();
    bool hasEpicQuest();
    bool isDoingJobChangeQuest() const;

    CUser* m_user;                    // +0x0000
    WongWork::CQuestClear m_cleared;  // +0x0004（0x7530）
    char m_pad7534[4];                // +0x7534
    int m_quest[20];                  // +0x7538
    int m_trigger[20];                // +0x7588
    _Quest_Authen_Data m_authenData;  // +0x75d8（0x44，ctor 按 ORIG memset 0x48）
    bool m_field761c;                 // +0x761c
    bool m_field761d;                 // +0x761d
};

// ============================================================================
// 构造 / 析构 / set_parent
// ============================================================================

UserQuest::UserQuest()
{
    // m_cleared / m_authenData 子对象 ctor 由编译器自动调用
    // （ORIG：CQuestClear::CQuestClear() + _Quest_Authen_Data::_Quest_Authen_Data()）
}

UserQuest::~UserQuest()
{
}

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

bool UserQuest::isClearQuest(int questIdx) const
{
    return m_cleared.isClearedQuest(questIdx);
}

// ============================================================================
// set_quest / get_quest
// ============================================================================

int UserQuest::set_quest(SIG_LOAD_QUEST const* sig)
{
    for (int i = 0; i <= 19; ++i)
    {
        m_quest[i] = sig->m_quest[i];
        m_trigger[i] = sig->m_trigger[i];
        Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
        if (quest == 0 && m_quest[i] != 0)
        {
            cMyTrace trace("bool UserQuest::set_quest(const SIG_LOAD_QUEST*)", 0x42, 0);
            trace("VERIFY_QUEST : ABNOMAL QUEST INDEX (%d) : CHARAC_NO (%u)",
                  m_quest[i], sig->m_characNo);
            m_quest[i] = 0;
            m_trigger[i] = 0;
        }
    }
    m_cleared.setClearedQuest(
        reinterpret_cast<const WongWork::STClearedQuest*>(sig->m_cleared));
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
    stSelectQuestParam selectParam(m_user);

    InterfacePacketBuf* pb = reinterpret_cast<InterfacePacketBuf*>(buf);
    pb->put_header(0, 0x15);

    std::list<int> questList;
    G_CDataManager()->m_questList->select_quest(questList, selectParam, m_cleared);
    if (selectParam.m_data[9] != -1)
    {
        questList.push_back(selectParam.m_data[9]);
    }

    pb->put_byte(m_user->get_charac_level());
    int index = pb->get_index();
    int count = 0;
    pb->put_short(0);

    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questIdx = *it;
        if (!reinterpret_cast<CTimeGate*>(0x941f7f4)->setBlindTimeGateStateQuest(questIdx))
        {
            ++count;
            pb->put_short(questIdx);
        }
    }

    pb->put_short(index, count);
    pb->finalize(true);
}

void UserQuest::get_mail_quest_info(std::list<int>& questList,
                                    stSelectQuestParam const& selectParam) const
{
    G_CDataManager()->m_questList->select_mail_quest(questList, m_cleared, selectParam);
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
    if (m_user == 0)
    {
        LogManager::logFormat(1, "user_quest.cpp",
                                 "int UserQuest::accept_quest(int, char*, int&)", 0x1e8,
                                 "user is null");
        return 1;
    }
    if (m_user->getCurCharacR() == 0)
    {
        cMyTrace trace("int UserQuest::accept_quest(int, char*, int&)", 0x1ef, 5);
        trace("UserQuest::accept_quest user->m_selected is null");
        return 1;
    }

    Quest* quest = G_CDataManager()->find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(1, "user_quest.cpp",
                                 "int UserQuest::accept_quest(int, char*, int&)", 0x1f8,
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
                m_user, (WongWork::ENUM_HACK_TYPE)0x191, 1, 0, 0);
            return 1;
        }
    }

    std::vector<std::pair<int, int> > rewardItems;
    std::vector<std::pair<int, int> > erasedItems;
    rewardItems = quest->m_rewardItems;

    int erased = m_user->getCurCharacInvenW()->erase_repeated_item(
        rewardItems, erasedItems);
    if (erased > 0)
    {
        for (int i = 0; i < 11; ++i)
        {
            selectParam.m_data[i] = 0;
        }
        const CInventory* invR = m_user->getCurCharacInvenR();
        invR->CalNeedSlot(&selectParam.m_data[0], erasedItems);
        int quick = invR->QuickEmptyCount();
        for (int i = 1; i <= 10; ++i)
        {
            int cnt = invR->InvenEmptycount((Inven_Item::ITEM_TYPE)i);
            if (selectParam.m_data[i] > cnt)
            {
                quick += cnt - selectParam.m_data[i];
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
    int i;
    for (i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            break;
        }
    }
    if (i > 19)
    {
        return 0x13;
    }

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
                    void* maze = reinterpret_cast<CBattle_Field*>((char*)party + 0xb24)->getMaze();
                    if (maze != 0 && *(int*)((char*)maze + 0x50) == questIdx)
                    {
                        return 0x14;
                    }
                }
            }
        }
        if (quest->m_type == 8)
        {
            if (m_user->get_charac_level() > quest->m_levelLimit)
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

// ============================================================================
// set_trigger
// ============================================================================

int UserQuest::set_trigger(int questIdx, char type, bool bIncrement)
{
    int i;
    for (i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            break;
        }
    }
    if (i > 19)
    {
        return -1;
    }

    if (m_user->getGmQuestFlag())
    {
        m_trigger[i] = 0;
    }

    if (type == 1)
    {
        m_trigger[i] = m_trigger[i] + 1;
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

// ============================================================================
// finish_quest
// ============================================================================

int UserQuest::finish_quest(int questIdx)
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            if (m_trigger[i] != 0)
            {
                return 0x16;
            }
            m_quest[i] = 0;
            m_trigger[i] = 0;

            Quest* quest = G_CDataManager()->find_quest(questIdx);
            if (quest != 0 && !quest->isRepeatableQuest())
            {
                m_cleared.setClearedQuest(questIdx);
            }
            if (quest != 0 && quest->m_type == 8)
            {
                m_authenData.m_urgentQuest = -1;
            }
            return 0;
        }
    }
    return 0x13;
}

// ============================================================================
// item_reward_quest / get_rescue_questmap / check_cond_clear
// ============================================================================

int UserQuest::item_reward_quest(int mapIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0 && m_trigger[i] != 0)
        {
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

int UserQuest::get_rescue_questmap(int mapIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] > 0 && m_trigger[i] != 0)
        {
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

int UserQuest::check_cond_clear(int questIdx) const
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

int UserQuest::check_quest_condition(int questIdx)
{
    Quest* quest = G_CDataManager()->m_questList->find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "user_quest.cpp", "bool UserQuest::check_quest_condition(int)", 0x403,
            "G_CDataManager()->m_pQuestList->find_quest(%d) fail", questIdx);
        return 0;
    }
    return check_quest_type_condition(quest);
}

int UserQuest::check_quest_type_condition(Quest* quest)
{
    switch (quest->m_questType)
    {
    case 3:
        if (quest->m_params.at(0) == (int)(short)m_authenData.m_field4)
        {
            return 1;
        }
        return 0;
    case 5:
    {
        PvpResultType* pvp = m_user->getPVPResultR();
        if (quest->m_params.at(0) <= *(int*)((char*)pvp + 0x14))
        {
            return 1;
        }
        return 0;
    }
    case 7:
        if (reinterpret_cast<CPowerManager*>(0x940be50)->GetWinnerSide() ==
            m_user->getPowerSide())
        {
            return 1;
        }
        return 0;
    case 8:
        if (quest->m_params.at(0) <= (int)m_user->GetUserPowerWarPoint())
        {
            return 1;
        }
        return 0;
    default:
        if (quest->m_questType > 0x19)
        {
            return check_quest_subtype_condition(quest);
        }
        return 1;
    }
}

int UserQuest::check_quest_subtype_condition(Quest* quest)
{
    switch (quest->m_questSubType)
    {
    case 0:
    {
        int at0 = quest->m_params.at(0);
        int at1 = quest->m_params.at(1);
        int at2 = quest->m_params.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (m_authenData.m_field8 > at2 * 1000)
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
        int at0 = quest->m_params.at(0);
        int at1 = quest->m_params.at(1);
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
        int at0 = quest->m_params.at(0);
        int at1 = quest->m_params.at(1);
        int at2 = quest->m_params.at(2);
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
        int at0 = quest->m_params.at(0);
        int at1 = quest->m_params.at(1);
        quest->m_params.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        return 0;
    }
    default:
        return 0;
    }
}

// ============================================================================
// IsDoingQuest / CheckDungeonEventStoryPause / isDoingJobChangeQuest
// ============================================================================

int UserQuest::IsDoingQuest(int questIdx) const
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
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == 0)
        {
            continue;
        }
        Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
        if (quest == 0)
        {
            continue;
        }
        for (unsigned int j = 0; j < quest->m_eventList.size(); ++j)
        {
            dungeonEventHanaseStruct& ev = quest->m_eventList[j];
            if (ev.m_type == 0)
            {
                if (ev.m_map == mapNo)
                {
                    return 1;
                }
            }
            else if (ev.m_type == 1)
            {
                if (ev.m_map == fieldNo)
                {
                    return 1;
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
            if (quest != 0 && (quest->m_field1c == 1 || quest->m_field1c == 2))
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
        return;
    }

    Quest* quest = G_CDataManager()->find_quest(m_authenData.m_urgentQuest);
    if (quest == 0)
    {
        m_authenData.m_urgentQuest = -1;
        return;
    }

    int levelLimit = quest->m_levelLimit;
    if (levelLimit > 0x46)
    {
        levelLimit = 0x46;
    }
    if (m_user->get_charac_level() + 1 > levelLimit)
    {
        m_authenData.m_urgentQuest = -1;
    }
}

bool UserQuest::hasEpicQuest()
{
    if (m_user == 0)
    {
        return false;
    }

    std::list<int> questList;
    stSelectQuestParam selectParam(m_user);

    char cleared[0x7531];
    memcpy(cleared, &m_user->getCurCharacQuestR()->m_cleared, 0x7531);

    QuestList* list = G_CDataManager()->m_questList;
    std::multimap<int, int>* npcList = list->getQuestNPCList(ENUM_QUEST_GRADE_NONE);
    list->allowable_questlist_as_npc(
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
            bool bException = false;
            for (unsigned int j = 0; j < quest->m_exceptions.size(); ++j)
            {
                if (quest->m_exceptions[j] == QUEST_EXCEPTION_NONE)
                {
                    bException = true;
                    break;
                }
            }
            if (!bException && selectParam.m_data[0] < quest->m_field20 + 0xc)
            {
                return true;
            }
        }
    }
    return false;
}
