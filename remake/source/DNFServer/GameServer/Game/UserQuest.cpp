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

// ---- 跨类最小声明 ----
class CUser;
class CUserCharacInfo;
class CInventory;
class CHackAnalyzer;
class CBattle_Field;
class InterfacePacketBuf;
class CTimeGate;
class CPowerManager;
class Quest;
class UserQuest;

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

enum ENUM_QUEST_GRADE
{
    QUEST_GRADE_0 = 0
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

enum eMoneyAddReason
{
    eMoneyAddReason_3 = 3
};

enum eItemAddReason
{
    eItemAddReason_8 = 8
};

class Inven_Item
{
public:
    enum ITEM_TYPE
    {
        ITEM_TYPE_1 = 1,
        ITEM_TYPE_2 = 2,
        ITEM_TYPE_3 = 3,
        ITEM_TYPE_4 = 4,
        ITEM_TYPE_5 = 5,
        ITEM_TYPE_6 = 6,
        ITEM_TYPE_7 = 7,
        ITEM_TYPE_8 = 8,
        ITEM_TYPE_9 = 9,
        ITEM_TYPE_10 = 10
    };
};

namespace WongWork
{

enum ENUM_HACK_TYPE
{
    ENUM_HACK_TYPE_QUEST = 0x191
};

class CQuestClear
{
public:
    unsigned char m_cleared[0x7530];   // +0x00（0x7530）
};

class CHackAnalyzer
{
public:
    char m_pad[4];   // +0x00
};

}  // namespace WongWork

// ---- stSelectQuestParam（0x2c：11 个 int；ctor 在 ORIG）----
class stSelectQuestParam
{
public:
    int m_data[11];   // +0x00（+0x24 = m_data[9] 为 select quest idx）
};

// ---- stQuestTriggerState_t（8 字节：2 short + 1 int）----
class stQuestTriggerState_t
{
public:
    short m_a;   // +0x00
    short m_b;   // +0x02
    int m_c;     // +0x04
};

class cMyTrace
{
public:
    char m_pad[0x10];
};

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

// ---- Quest 布局（仅本 TU 访问到的字段）----
struct Quest
{
    char m_pad00[8];                                  // +0x00
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
// 跨类符号（extern "C" + asm 标签，直调 ORIG 符号）
// ============================================================================

extern "C" void* sub_G_CDataManager() asm("_Z14G_CDataManagerv");
extern "C" Quest* sub_CDataManager_find_quest(void* self, int idx)
    asm("_ZNK12CDataManager10find_questEi");
extern "C" Quest* sub_QuestList_find_quest(void* self, int idx)
    asm("_ZN9QuestList10find_questEi");

#define QUEST_LIST_MGR() (*(void**)((char*)sub_G_CDataManager() + 0x18))

extern "C" void sub_QuestList_select_quest(void* self, std::list<int>& out,
                                           void* selectParam, void const* cleared)
    asm("_ZN9QuestList12select_questERSt4listIiSaIiEER18stSelectQuestParamRKN8WongWork11CQuestClearE");
extern "C" void sub_QuestList_select_mail_quest(void* self, std::list<int>& out,
                                                void const* cleared, void const* selectParam)
    asm("_ZN9QuestList17select_mail_questERSt4listIiSaIiEERKN8WongWork11CQuestClearERK18stSelectQuestParam");
extern "C" std::multimap<int, int> sub_QuestList_getQuestNPCList(void* self, int grade)
    asm("_ZN9QuestList15getQuestNPCListE16ENUM_QUEST_GRADE");
extern "C" void sub_QuestList_allowable_questlist_as_npc(
    void* self, std::list<int>& out, std::multimap<int, int> const& npcList,
    void* selectParam, void const* cleared, bool b)
    asm("_ZN9QuestList26allowable_questlist_as_npcERSt4listIiSaIiEERKSt8multimapIiiSt4lessIiESaISt4pairIKiiEEER18stSelectQuestParamRKN8WongWork11CQuestClearEb");

extern "C" bool sub_checkAcceptableQuest(void const* quest, void const* cleared,
                                         void const* selectParam)
    asm("_Z20checkAcceptableQuestPK5QuestRKN8WongWork11CQuestClearERK18stSelectQuestParam");

extern "C" void sub_CQuestClear_C1(void* self) asm("_ZN8WongWork11CQuestClearC1Ev");
extern "C" void sub_CQuestClear_D1(void* self) asm("_ZN8WongWork11CQuestClearD1Ev");
extern "C" void sub_CQuestClear_clear(void* self) asm("_ZN8WongWork11CQuestClear5clearEv");
extern "C" bool sub_CQuestClear_isClearedQuest(void const* self, unsigned int idx)
    asm("_ZNK8WongWork11CQuestClear14isClearedQuestEj");
extern "C" void sub_CQuestClear_resetClearedQuests(void* self, unsigned int idx)
    asm("_ZN8WongWork11CQuestClear18resetClearedQuestsEj");
extern "C" void sub_CQuestClear_setClearedQuest(void* self, unsigned int idx)
    asm("_ZN8WongWork11CQuestClear15setClearedQuestEj");
extern "C" void sub_CQuestClear_setClearedQuest_arr(void* self, void const* arr)
    asm("_ZN8WongWork11CQuestClear15setClearedQuestEPKNS_14STClearedQuestE");
extern "C" void const* sub_CQuestClear_getClearedQuest(void const* self)
    asm("_ZNK8WongWork11CQuestClear15getClearedQuestEv");

extern "C" void sub_Quest_Authen_Data_C1(void* self) asm("_ZN18_Quest_Authen_DataC1Ev");

extern "C" bool sub_Quest_isRepeatableQuest(void const* self)
    asm("_ZNK5Quest17isRepeatableQuestEv");
extern "C" int sub_Quest_get_init_trigger(void const* self)
    asm("_ZNK5Quest16get_init_triggerEv");
extern "C" int sub_Quest_get_appearmap(void const* self, int a, int b)
    asm("_ZNK5Quest13get_appearmapEii");
extern "C" bool sub_Quest_check_clear_item(void const* self, int a, int b)
    asm("_ZNK5Quest16check_clear_itemEii");

extern "C" void* sub_CUserCharacInfo_getCurCharacR(void const* self)
    asm("_ZNK15CUserCharacInfo13getCurCharacREv");
extern "C" int sub_CUserCharacInfo_get_charac_level(void const* self)
    asm("_ZNK15CUserCharacInfo16get_charac_levelEv");
extern "C" void* sub_CUserCharacInfo_getCurCharacInvenW(void* self)
    asm("_ZN15CUserCharacInfo18getCurCharacInvenWEv");
extern "C" void* sub_CUserCharacInfo_getCurCharacInvenR(void const* self)
    asm("_ZNK15CUserCharacInfo18getCurCharacInvenREv");
extern "C" void* sub_CUserCharacInfo_getPVPResultR(void const* self)
    asm("_ZNK15CUserCharacInfo13getPVPResultREv");
extern "C" short sub_CUserCharacInfo_GetUserPowerWarPoint(void* self)
    asm("_ZN15CUserCharacInfo20GetUserPowerWarPointEv");

extern "C" UserQuest* sub_CUser_getCurCharacQuestR(void const* self)
    asm("_ZNK5CUser18getCurCharacQuestREv");
extern "C" bool sub_CUser_IsGameMasterMode(void const* self)
    asm("_ZNK5CUser16IsGameMasterModeEv");
extern "C" void* sub_CUser_getHackAnalyzer(void* self) asm("_ZN5CUser15getHackAnalyzerEv");
extern "C" int sub_CUser_get_state(void* self) asm("_ZN5CUser9get_stateEv");
extern "C" void* sub_CUser_GetParty(void* self) asm("_ZN5CUser8GetPartyEv");
extern "C" char sub_CUser_getPowerSide(void* self) asm("_ZN5CUser12getPowerSideEv");
extern "C" bool sub_CUser_getGmQuestFlag(void* self) asm("_ZN5CUser14getGmQuestFlagEv");

extern "C" int sub_CInventory_erase_repeated_item(
    void* self, std::vector<std::pair<int, int> > const& src,
    std::vector<std::pair<int, int> >& out)
    asm("_ZN10CInventory19erase_repeated_itemERKSt6vectorISt4pairIiiESaIS2_EERS4_");
extern "C" void sub_CInventory_CalNeedSlot(void const* self, int* slotNeed,
                                           std::vector<std::pair<int, int> > const& items)
    asm("_ZNK10CInventory11CalNeedSlotEPiRKSt6vectorISt4pairIiiESaIS3_EE");
extern "C" int sub_CInventory_QuickEmptyCount(void const* self)
    asm("_ZNK10CInventory15QuickEmptyCountEv");
extern "C" int sub_CInventory_InvenEmptycount(void const* self, Inven_Item::ITEM_TYPE t)
    asm("_ZNK10CInventory15InvenEmptycountEN10Inven_Item9ITEM_TYPEE");
extern "C" int sub_CInventory_insert_event_items(
    void* self, std::vector<std::pair<int, int> > const& items, std::vector<int>& itemIdx,
    eMoneyAddReason moneyReason, eItemAddReason itemReason, bool bFlag,
    char const* s1, char const* s2)
    asm("_ZN10CInventory18insert_event_itemsERKSt6vectorISt4pairIiiESaIS2_EERS0_IiSaIiEE15eMoneyAddReason14eItemAddReasonbPKcSD_");

extern "C" void sub_CHackAnalyzer_addServerHackCnt(void* self, void* user,
                                                   WongWork::ENUM_HACK_TYPE type,
                                                   unsigned int a, unsigned int b,
                                                   unsigned int c)
    asm("_ZN8WongWork13CHackAnalyzer16addServerHackCntEP5CUserNS_13ENUM_HACKTYPEEjjj");

extern "C" void sub_InterfacePacketBuf_put_header(void* self, int a, int b)
    asm("_ZN18InterfacePacketBuf10put_headerEii");
extern "C" void sub_InterfacePacketBuf_put_byte(void* self, int v)
    asm("_ZN18InterfacePacketBuf8put_byteEi");
extern "C" void sub_InterfacePacketBuf_put_short(void* self, int v)
    asm("_ZN18InterfacePacketBuf9put_shortEi");
extern "C" void sub_InterfacePacketBuf_put_short_ref(void* self, int& idx, int v)
    asm("_ZN18InterfacePacketBuf9put_shortERii");
extern "C" void sub_InterfacePacketBuf_put_int(void* self, int v)
    asm("_ZN18InterfacePacketBuf7put_intEi");
extern "C" int sub_InterfacePacketBuf_get_index(void* self)
    asm("_ZN18InterfacePacketBuf9get_indexEv");
extern "C" void sub_InterfacePacketBuf_finalize(void* self, bool b)
    asm("_ZN18InterfacePacketBuf8finalizeEb");

extern "C" bool sub_CTimeGate_setBlindTimeGateStateQuest(void* self, int questIdx)
    asm("_ZN9CTimeGate26setBlindTimeGateStateQuestEi");

extern "C" char sub_CPowerManager_GetWinnerSide(void* self)
    asm("_ZN13CPowerManager13GetWinnerSideEv");

extern "C" void* sub_CBattle_Field_getMaze(void* self) asm("_ZN13CBattle_Field7getMazeEv");

extern "C" void sub_LogManager_logFormat(int level, char const* file, char const* func,
                                         int line, char const* fmt, ...)
    asm("_ZN10LogManager9logFormatEiPKcS1_iS1_z");

extern "C" void sub_cMyTrace_C1(void* self, char const* func, int line, int flag)
    asm("_ZN8cMyTraceC1EPKcii");
extern "C" void sub_cMyTrace_call(void* self, char const* fmt, ...)
    asm("_ZN8cMyTraceclEPKcz");

extern "C" void sub_stSelectQuestParam_C1(void* self, void* user)
    asm("_ZN18stSelectQuestParamC1EP5CUser");

extern "C" void sub_stQuestTriggerState_t_C1(void* self)
    asm("_ZN21stQuestTriggerState_tC1Ev");
extern "C" void sub_stQuestTriggerState_t_op_assign(void* self, int const* v)
    asm("_ZN21stQuestTriggerState_taSERKi");
extern "C" void sub_stQuestTriggerState_t_inc(void* self, char c)
    asm("_ZN21stQuestTriggerState_t15incTirggerStateEc");
extern "C" void sub_stQuestTriggerState_t_sub(void* self, char c)
    asm("_ZN21stQuestTriggerState_t15subTriggerStateEc");
extern "C" int sub_stQuestTriggerState_t_getTrigger(void* self)
    asm("_ZN21stQuestTriggerState_t10getTriggerEv");

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
    short m_characNo;                 // +0x75d8
    short m_characClass;              // +0x75da
    short m_field75dc;                // +0x75dc
    int m_field75e0;                  // +0x75e0
    char m_field75e4;                 // +0x75e4
    int m_field75e8;                  // +0x75e8
    int m_field75ec;                  // +0x75ec
    int m_urgentQuest;                // +0x75f0
    char m_authen[0x28];              // +0x75f4
    bool m_field761c;                 // +0x761c
    bool m_field761d;                 // +0x761d
};

// ============================================================================
// 构造 / 析构 / set_parent
// ============================================================================

UserQuest::UserQuest()
{
    sub_CQuestClear_C1(&m_cleared);
    sub_Quest_Authen_Data_C1(&m_characNo);
}

UserQuest::~UserQuest()
{
    sub_CQuestClear_D1(&m_cleared);
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
    sub_CQuestClear_clear(&m_cleared);
    for (int i = 0; i <= 19; ++i)
    {
        m_quest[i] = 0;
        m_trigger[i] = 0;
    }
    m_urgentQuest = -1;
    memset(m_authen, 0, 0x28);
}

void UserQuest::resetClearQuest(int questIdx)
{
    sub_CQuestClear_resetClearedQuests(&m_cleared, questIdx);
}

bool UserQuest::isClearQuest(int questIdx) const
{
    return sub_CQuestClear_isClearedQuest(&m_cleared, questIdx);
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
        Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), m_quest[i]);
        if (quest == 0 && m_quest[i] != 0)
        {
            cMyTrace trace;
            sub_cMyTrace_C1(&trace, "bool UserQuest::set_quest(const SIG_LOAD_QUEST*)", 0x42, 0);
            sub_cMyTrace_call(&trace,
                              "VERIFY_QUEST : ABNOMAL QUEST INDEX (%d) : CHARAC_NO (%u)",
                              m_quest[i], sig->m_characNo);
            m_quest[i] = 0;
            m_trigger[i] = 0;
        }
    }
    sub_CQuestClear_setClearedQuest_arr(&m_cleared, sig->m_cleared);
    m_urgentQuest = sig->m_urgentQuest;
    memcpy(m_authen, sig->m_authen, 0x28);
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
    memcpy(sig->m_cleared, sub_CQuestClear_getClearedQuest(&m_cleared), 0x7530);
    sig->m_urgentQuest = sub_CUser_getCurCharacQuestR(m_user)->m_urgentQuest;
    memcpy(sig->m_authen, m_authen, 0x28);
    return 1;
}

// ============================================================================
// get_quest_info / get_mail_quest_info
// ============================================================================

void UserQuest::get_quest_info(char* buf) const
{
    stSelectQuestParam selectParam;
    sub_stSelectQuestParam_C1(&selectParam, m_user);

    void* pb = (void*)buf;
    sub_InterfacePacketBuf_put_header(pb, 0, 0x15);

    std::list<int> questList;
    sub_QuestList_select_quest(QUEST_LIST_MGR(), questList, &selectParam, &m_cleared);
    if (selectParam.m_data[9] != -1)
    {
        questList.push_back(selectParam.m_data[9]);
    }

    sub_InterfacePacketBuf_put_byte(pb, sub_CUserCharacInfo_get_charac_level(m_user));
    int index = sub_InterfacePacketBuf_get_index(pb);
    int count = 0;
    sub_InterfacePacketBuf_put_short(pb, 0);

    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questIdx = *it;
        if (!sub_CTimeGate_setBlindTimeGateStateQuest((void*)0x941f7f4, questIdx))
        {
            ++count;
            sub_InterfacePacketBuf_put_short(pb, questIdx);
        }
    }

    sub_InterfacePacketBuf_put_short_ref(pb, index, count);
    sub_InterfacePacketBuf_finalize(pb, true);
}

void UserQuest::get_mail_quest_info(std::list<int>& questList,
                                    stSelectQuestParam const& selectParam) const
{
    sub_QuestList_select_mail_quest(QUEST_LIST_MGR(), questList, &m_cleared, &selectParam);
}

// ============================================================================
// set_authen_data
// ============================================================================

void UserQuest::set_authen_data(QUEST_CONDITION cond, int v1, int v2)
{
    switch (cond)
    {
    case QUEST_CONDITION_0:
        m_characNo = (short)v1;
        m_characClass = (short)v2;
        break;
    case QUEST_CONDITION_1:
        m_field75dc = (short)v1;
        break;
    case QUEST_CONDITION_2:
        m_field75e0 = v1;
        break;
    case QUEST_CONDITION_3:
        m_field75e4 = (char)(v1 != 0);
        break;
    case QUEST_CONDITION_4:
        m_field75e8 = v1;
        break;
    case QUEST_CONDITION_5:
        m_field75ec = v1;
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
        sub_LogManager_logFormat(1, "user_quest.cpp",
                                 "int UserQuest::accept_quest(int, char*, int&)", 0x1e8,
                                 "user is null");
        return 1;
    }
    if (sub_CUserCharacInfo_getCurCharacR(m_user) == 0)
    {
        cMyTrace trace;
        sub_cMyTrace_C1(&trace, "int UserQuest::accept_quest(int, char*, int&)", 0x1ef, 5);
        sub_cMyTrace_call(&trace, "UserQuest::accept_quest user->m_selected is null");
        return 1;
    }

    Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), questIdx);
    if (quest == 0)
    {
        sub_LogManager_logFormat(1, "user_quest.cpp",
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

    if (!sub_CUser_IsGameMasterMode(m_user))
    {
        if (!sub_Quest_isRepeatableQuest(quest) &&
            sub_CQuestClear_isClearedQuest(&m_cleared, questIdx))
        {
            return 0x12;
        }
    }

    stSelectQuestParam selectParam;
    if (!sub_CUser_IsGameMasterMode(m_user))
    {
        sub_stSelectQuestParam_C1(&selectParam, m_user);
        UserQuest* cur = sub_CUser_getCurCharacQuestR(m_user);
        if (!sub_checkAcceptableQuest(quest, &cur->m_cleared, &selectParam))
        {
            sub_CHackAnalyzer_addServerHackCnt(sub_CUser_getHackAnalyzer(m_user), m_user,
                                               WongWork::ENUM_HACK_TYPE_QUEST, 1, 0, 0);
            return 1;
        }
    }

    std::vector<std::pair<int, int> > rewardItems;
    std::vector<std::pair<int, int> > erasedItems;
    rewardItems = quest->m_rewardItems;

    int erased = sub_CInventory_erase_repeated_item(
        sub_CUserCharacInfo_getCurCharacInvenW(m_user), rewardItems, erasedItems);
    if (erased > 0)
    {
        for (int i = 0; i < 11; ++i)
        {
            selectParam.m_data[i] = 0;
        }
        void* invR = sub_CUserCharacInfo_getCurCharacInvenR(m_user);
        sub_CInventory_CalNeedSlot(invR, &selectParam.m_data[0], erasedItems);
        int quick = sub_CInventory_QuickEmptyCount(invR);
        for (int i = 1; i <= 10; ++i)
        {
            int cnt = sub_CInventory_InvenEmptycount(invR, (Inven_Item::ITEM_TYPE)i);
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

    trigger = sub_Quest_get_init_trigger(quest);
    for (int i = 0; i < 20; ++i)
    {
        if (m_quest[i] == 0)
        {
            sub_CQuestClear_resetClearedQuests(&m_cleared, questIdx);
            m_quest[i] = questIdx;
            m_trigger[i] = trigger;

            void* pb = (void*)buf;
            sub_InterfacePacketBuf_put_byte(pb, 1);
            sub_InterfacePacketBuf_put_short(pb, questIdx);
            sub_InterfacePacketBuf_put_int(pb, trigger);

            std::vector<int> itemIdx;
            int insertResult = sub_CInventory_insert_event_items(
                sub_CUserCharacInfo_getCurCharacInvenW(m_user), erasedItems, itemIdx,
                eMoneyAddReason_3, eItemAddReason_8, false, "", "");
            sub_InterfacePacketBuf_put_byte(pb, insertResult);

            for (std::vector<int>::iterator it = itemIdx.begin();
                 it != itemIdx.end(); ++it)
            {
                sub_InterfacePacketBuf_put_short(pb, *it);
                ++it;
                sub_InterfacePacketBuf_put_int(pb, *it);
                ++it;
                sub_InterfacePacketBuf_put_int(pb, *it);
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

    Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), questIdx);
    if (quest != 0)
    {
        if (quest->m_field120 == 1 || quest->m_field120 == 2)
        {
            if (m_trigger[i] == 0)
            {
                return 0x12;
            }
            if (sub_CUser_get_state(m_user) == 5)
            {
                void* party = sub_CUser_GetParty(m_user);
                if (party != 0)
                {
                    void* maze = sub_CBattle_Field_getMaze((char*)party + 0xb24);
                    if (maze != 0 && *(int*)((char*)maze + 0x50) == questIdx)
                    {
                        return 0x14;
                    }
                }
            }
        }
        if (quest->m_type == 8)
        {
            if (sub_CUserCharacInfo_get_charac_level(m_user) > quest->m_levelLimit)
            {
                m_urgentQuest = -1;
            }
            if (hasEpicQuest())
            {
                m_urgentQuest = -1;
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

    if (sub_CUser_getGmQuestFlag(m_user))
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
            sub_stQuestTriggerState_t_C1(&state);
            sub_stQuestTriggerState_t_op_assign(&state, &m_trigger[i]);
            sub_stQuestTriggerState_t_inc(&state, type);
            m_trigger[i] = sub_stQuestTriggerState_t_getTrigger(&state);
        }
        else if (m_trigger[i] > 0)
        {
            stQuestTriggerState_t state;
            sub_stQuestTriggerState_t_C1(&state);
            sub_stQuestTriggerState_t_op_assign(&state, &m_trigger[i]);
            sub_stQuestTriggerState_t_sub(&state, type);
            m_trigger[i] = sub_stQuestTriggerState_t_getTrigger(&state);
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

            Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), questIdx);
            if (quest != 0 && !sub_Quest_isRepeatableQuest(quest))
            {
                sub_CQuestClear_setClearedQuest(&m_cleared, questIdx);
            }
            if (quest != 0 && quest->m_type == 8)
            {
                m_urgentQuest = -1;
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
            Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), m_quest[i]);
            if (quest == 0)
            {
                return 0;
            }
            if (sub_Quest_check_clear_item(quest, mapIdx, (int)m_characClass))
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
            Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), m_quest[i]);
            if (quest == 0)
            {
                return 0;
            }
            int map = sub_Quest_get_appearmap(quest, mapIdx, (int)m_characClass);
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
        if (sub_CUser_getGmQuestFlag(m_user))
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
    Quest* quest = sub_QuestList_find_quest(QUEST_LIST_MGR(), questIdx);
    if (quest == 0)
    {
        sub_LogManager_logFormat(
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
        if (quest->m_params.at(0) == (int)(short)m_field75dc)
        {
            return 1;
        }
        return 0;
    case 5:
    {
        void* pvp = sub_CUserCharacInfo_getPVPResultR(m_user);
        if (quest->m_params.at(0) <= *(int*)((char*)pvp + 0x14))
        {
            return 1;
        }
        return 0;
    }
    case 7:
        if (sub_CPowerManager_GetWinnerSide((void*)0x940be50) == sub_CUser_getPowerSide(m_user))
        {
            return 1;
        }
        return 0;
    case 8:
        if (quest->m_params.at(0) <= (int)sub_CUserCharacInfo_GetUserPowerWarPoint(m_user))
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
        if (at1 != -1 && (int)m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_characNo != at0)
        {
            return 0;
        }
        if (m_field75e0 > at2 * 1000)
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
        if (at1 != -1 && (int)m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_characNo != at0)
        {
            return 0;
        }
        if (m_field75e4 != 0)
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
        if (at1 != -1 && (int)m_characClass < at1)
        {
            return 0;
        }
        if ((int)m_characNo != at0)
        {
            return 0;
        }
        if (at2 == 1 && m_field75e8 != 1)
        {
            return 0;
        }
        if (m_field75e8 < at2)
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
        if (at1 != -1 && (int)m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_characNo != at0)
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
        Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), m_quest[i]);
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
            Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), m_quest[i]);
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
        Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), m_quest[i]);
        if (quest != 0 && quest->m_type == 8)
        {
            return;
        }
    }

    if (hasEpicQuest())
    {
        m_urgentQuest = -1;
        return;
    }

    Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), m_urgentQuest);
    if (quest == 0)
    {
        m_urgentQuest = -1;
        return;
    }

    int levelLimit = quest->m_levelLimit;
    if (levelLimit > 0x46)
    {
        levelLimit = 0x46;
    }
    if (sub_CUserCharacInfo_get_charac_level(m_user) + 1 > levelLimit)
    {
        m_urgentQuest = -1;
    }
}

bool UserQuest::hasEpicQuest()
{
    if (m_user == 0)
    {
        return false;
    }

    std::list<int> questList;
    stSelectQuestParam selectParam;
    sub_stSelectQuestParam_C1(&selectParam, m_user);

    char cleared[0x7531];
    memcpy(cleared, &sub_CUser_getCurCharacQuestR(m_user)->m_cleared, 0x7531);

    std::multimap<int, int> npcList =
        sub_QuestList_getQuestNPCList(QUEST_LIST_MGR(), QUEST_GRADE_0);
    sub_QuestList_allowable_questlist_as_npc(QUEST_LIST_MGR(), questList, npcList,
                                             &selectParam, cleared, false);

    if (!questList.empty())
    {
        for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
        {
            Quest* quest = sub_CDataManager_find_quest(sub_G_CDataManager(), *it);
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
