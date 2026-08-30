#include "Arad_ItemBonus.h"   // ARAD::ARAD_ITEM（use_stackable 尾部事件物品发放）
// ============================================================================
// CUserOnlyOrig.cpp —— CUser 域「仅 ORIG」批次（Wave6 W6_CUser，worklist
// /tmp/only_orig/CUser.txt 269 符号，2026-08-27）
//
// 六批转写草稿（/tmp/df_batches/onlyorig_cuser body_A..F，含 compound_item 的
// m_padBattleField1 具名视图修正版）统一落库为单一 TU；声明并入 CUser.h 尾部
// 「Wave6-CUser 新增声明」区。每函数逐份读取 docs/class_func_reports/CUser/<Func>.md
// （## 汇编 + ## 反编译 C）或按 nm -S + objdump 忠实转写并标注地址。
// thunk/依赖唯一 owner 见各函数头注释与 meta_*.tsv。
// ============================================================================
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "CUser.h"
#include "SigTypes.h"
#include "CDungeonClear.h"
#include "CCharacList.h"
#include "CharacManageScript.h"
#include "GameResultType.h"
#include "CTitleBook.h"
#include "CGameManager.h"
#include "CInventory.h"
#include "APSystemTypes.h"
#include "GameBasicsScripts.h"
#include "CParty.h"
#include "SkillSlot.h"
#include "WarRoom.h"
#include "PvP_Room.h"
#include "SyncScriptDeps.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "StrikerScript.h"        // ValidateStrikerInfo：m_altarConfig.m_strikerScript 全类型
#include "GameTypes.h"
#include "CEventScriptMng.h"
#include "CCapchaDataManager.h"
#include "CDungeon.h"
#include "DBInsertGiveBoxEventReward.h"
#include "Packet_Monitor_Increase_Guild_Exp.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "QuestClearDeliveryMsg.h" // QuestClearDeliveryMsg 完整类型（eventDeliveryQuestClear）
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "LogManager.h"
#include "CStatisticServerProxy.h"
#include "MsgQueueMgr.h"          // MsgQueueMgr::QUEUE_IDX 完整类型（GlobalData::s_msgq_mgr）
#include "CServerProxyMgr.h"      // CServerProxyMgr<CMonitorServerProxy> 完整类型
#include "Quest.h"                // Quest::m_fieldB8 / RewardItemInfo 完整布局
#include "RDARScriptStringManager.h" // g_scriptStringManager_
#include "CCubeStatistic.h"
#include "CMissionList_Charac.h"   // GetCharacExpandData 扩展槽 cast 目标
#include "CQuestShop.h"
#include "CCeraShop.h"             // WongWork::CCeraShop 权威类（ProcessSpecialItem 等）
#include "BossTowerFunc.h"         // Global::CBossTowerFunc 权威声明
#include "Redeem_Item__CRedeemItem.h"
#include "CCirculationStatistic.h"
#include "CMyFileLog.h"
#include "W8LT_DBTask.h"     // DB_UpdateBingoEvent 权威声明（GoalAuditDB7 迁移，原局部类删除）
#include "CPrivateStoreMgr.h"
#include "STGrowthType.h"        // _Charac_info +0x29/+0x2a 子对象（LinkE2）
#include "STPowerWarCharacData.h"// _Charac_info +0xe8d 子对象（LinkE2）
#include "CAchievement.h"        // stAchievement_DBData（SIG_LOAD_ETC ctor 子对象）

#include <algorithm>
#include "HistoryLog.h"
#include "PacketTypes.h"   // MSG_MAILBOX_SEND 完整类型（SendMail MSG_CHECK_MAIL_RECEIVER::set<MSG_T> sizeof 需）
// ---- Wave6 补：本 TU 消费的外部符号最小声明（ORIG nm 实证；实现 owner 对应域批次）----
class CItemMakingSkill {
public:
    int GetItemMakingSkillLevel(CStackableItem const*, CUser*);  // T 0x08545cf4
    void Send_ItemMakingSkillList(CUser*) const;                 // T 0x08545a62（LinkClosureA-2 消费；实现 owner 待分派）
    void GainItemMakingSkillPoint(CStackableItem const*, CUser*);  // ORIG T 0x085453e0 _ZN16CItemMakingSkill24GainItemMakingSkillPointEPK14CStackableItemP5CUser（compound_item 消费；定义 owner 待分派）
};
void _postCheckForceChangeGrowType(CUser*, int, int);      // T 0x08668823
int get_rand_int(int range);                               // ORIG 0x8108b06（各 TU 自带声明先例；compound_item 消费）
class CUserEventUtil { public: static void updateCharacterDayEvent(CUser*, int); }; // T 0x08688230
void GiveItemsReturnUser();   // T 0x08692000（CUser 成员，调用为成员——见 cpp 内改写）
namespace ARAD { class Arad_EventPeriodDataManager; }
int _postCheckForceChangeGrowType(CUser*, int);            // ORIG 内部辅助形态之一（按调用点）
namespace global { namespace tools {
struct InitializeCharacStat {                              // ctor keyed 0x081462e3 实证 namespace 形态；定义 owner=LinkClosureE.cpp
    static void initCharacStat(_Additioal_info&, unsigned int, int, int, int, int);
};
} }  // namespace global::tools
class DB_CountOfHumanCertify { public: static void makeRequest(unsigned int); };                 // T 0x084283ca
class DB_ForceSaveSimpleSSO   { public: static void makeRequest(unsigned int, int); };           // T 0x084281ae
class DB_AutoPunishedSecondUserData { public: static void makeRequest(unsigned int, unsigned long long, unsigned int, unsigned int); }; // T 0x0843a790
class DB_UpdateLimitItemUsage { public: static void makeRequest(int, int, int, int, long); };    // T 0x0843f030
class DB_HackIPSave           { public: static void makeRequest(char const*, char const*, unsigned int, unsigned int); }; // T 0x08436ff8
// DB_UpdateBingoEvent —— 权威声明见 W8LT_DBTask.h（GoalAuditDB7 迁移；
// makeRequest 0x08449032 / dispatch 0x08448eb8）。
class DB_UpdateBlueMarbleEnterCount { public: static void makeRequest(unsigned int, unsigned char); };   // T 0x0844884c Ejh
class DB_SavePcRoomPlayTimeEvent { public: static void makeRequest(unsigned int, unsigned int, unsigned char); };   // ORIG _ZN26DB_SavePcRoomPlayTimeEvent11makeRequestEjjh 0x08447bd0（owner=DB 域）
class DB_UpdateReturnUserFirstLogin { public: static void makeRequest(unsigned int, int, int); };   // T 0x0843e146
class DB_SaveGiveAvengerTitleFlag   { public: static void makeRequest(unsigned int, unsigned int, int); };
class CLevelDungeonPlayStatistic {
public:
    static void IncreaseLevelDungeonPlay(void* mgr, int a, int b);   // T 0x0860ecc6（mgr=CCommonStatisticsMgr* 视图）
};
enum { DISCONN_SIG_PARSE_FAIL = 4, DISCONN_SIG_RECV_ERR = 5, DISCONN_SIG_END_OF_SESSION = 1 };
// Parsing/checkHumanCertify 等使用的裸偏移字段访问宏（CUser.h 未建模区；偏移自函数内 ORIG 注记）
// （W7_CU_BYTE/SHORT/INT 偏移宏已按铁律删除；对应字段已具名化于 CUser.h）
#include "DataManagerInitDepends.h" // CLevelRewardTableMgr 权威完整类
// ---- DB 请求类最小声明（实现 owner：DB 域批次；签名对齐本 TU 调用点/ORIG mangled）----
class DB_InsertArchieveEventLog
{
public:
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type);
};
class DB_UpdateHappyBeanInfo
{
public:
    static void makeRequest(unsigned int uid, unsigned int accId,
                            unsigned int characNo, const char* name);   // ORIG T 0x08425a02（实现 DB_U3.cpp）
};
class DB_Insert0712WinterEventCoupon
{
public:
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            unsigned int count);   // ORIG T 0x0842e648（实现 DB_U3.cpp）
};

       // CCubeStatistic/CUBE_STATISTIC_FIELD

// ORIG 链接依赖（本 TU 内自由函数式 helper；ROUNDf 对应 ORIG 内联 roundf 形态）
// ORIG 形态：roundf 内建内联（fnstcw/fldcw RC=11 截断 + fistpl）；函数式 helper 在
// O0 下生成外部调用 _ZL6ROUNDff，与 ORIG 不符 → 用宏展开为内建 roundf。
#define ROUNDf(v) __builtin_roundf(v)

// ORIG STSkillCommandData 命令键 i-th 访问（Ghidra 视图 m_commands[i] 的权威等价：
// CUser.h STCommand 位域 m_key0..m_key4 @+0x03..+0x07）。
static unsigned char& SkillCmdKey(STSkillCommandData& d, int i)
{
    switch (i)
    {
    case 0: return d.m_key0;
    case 1: return d.m_key1;
    case 2: return d.m_key2;
    case 3: return d.m_key3;
    default: return d.m_key4;
    }
}

// body_A.cpp —— OnlyOrig_CUser.CuBatchA 转写草稿（Wave5: CUser 域 ORIG-only 批次 A）
// 依据 docs/class_func_reports/CUser/ 各报告（## 汇编 + ## 反编译 C 全文已读）；
// AddDailyBadge / ClearChildrensDayEvent / CheckPossibleRequestRenameState 三符号报告缺失，
// 按 nm -S 定址后 objdump -d 忠实转写（见各函数头注释）。
// 本文件为可粘贴草稿；签名与 ORIG mangled 一致。被调方若属本批直接调用；外域依赖照常书写。


// ---------------------------------------------------------------------------
// 1) CUser::_CalcItemVectorMultiply(vector<pair<int,int>>&, int)
//    报告：docs/class_func_reports/CUser/_CalcItemVectorMultiply.md
//    ORIG 0x08672042：循环 it->second *= mult。
// ---------------------------------------------------------------------------
void CUser::_CalcItemVectorMultiply(std::vector<std::pair<int, int> >& items, int mult)
{
    for (std::vector<std::pair<int, int> >::iterator it = items.begin();
         it != items.end(); ++it)
    {
        it->second = it->second * mult;
    }
}

// NOTE: CUser::_check_level_up 的权威定义已由 CUserCheckLevel.cpp 落地（ORIG 0x08662aea），
// 本 TU 不重复提供。
// ---------------------------------------------------------------------------
// 3) CUser::_CheckCompoundSkill(CStackableItem const*, bool)
//    报告：docs/class_func_reports/CUser/_CheckCompoundSkill.md。ORIG 0x08670f1e。
//    CStackableItem 侧：m_recipe174(+0x174 RecipeInfo) 内
//      m_recipe174.m_vec18(+0x18)=item+0x18c 需求技能列表；
//      m_recipe174.m_field24(+0x24)=item+0x198(合成技能标志 char)、
//      m_recipe174.m_field25(+0x25)=item+0x199(expert recipe 标志 char)。
// ---------------------------------------------------------------------------
bool CUser::_CheckCompoundSkill(const CStackableItem* item, bool checkRecipe)
{
    static const char* PRETTY =
        "bool CUser::_CheckCompoundSkill(const CStackableItem*, bool)";

    if (item->m_recipe174.m_field24 != 0 &&
        isAffectedPremium((ENUM_PREMIUM_TYPE)7) == false)
    {
        return false;
    }

    if (item->m_recipe174.m_field25 != 0)
    {
        std::vector<std::pair<int, int> > list(item->m_recipe174.m_vec18);
        bool ok = ((SkillSlot*)((char*)getCurCharacSkillR()))->check_skill_list(
            get_charac_job(), list, 2);
        if (ok)
        {
            return expert_job::CExpertJob::IsLearnRecipe(this, item);  // ==1 → true
        }
        unsigned int characNo = getCurCharacNo();
        cMyTrace trace(PRETTY, 0x5c68, 0);
        trace("EXPERT_JOB_ERROR : check_skill_list() charac_no(%d)", characNo);
        return false;
    }

    if (!checkRecipe)
    {
        std::vector<std::pair<int, int> > list(item->m_recipe174.m_vec18);
        bool ok = ((SkillSlot*)((char*)getCurCharacSkillR()))->check_skill_list(
            get_charac_job(), list, 2);
        if (!ok)
            return false;
    }
    return true;
}

// >>> W7-BLOCKED CUser::_fnStatQuestClear —— 同 calc_quest_stat：enumQuestClear 回调链
// ---------------------------------------------------------------------------
// 4) CUser::_fnStatQuestClear(unsigned int, char*) —— enumQuestClear 回调
//    报告：docs/class_func_reports/CUser/_fnStatQuestClear.md。ORIG 0x08664e8e。
//    真实符号为静态成员回调 _ZN5CUser17_fnStatQuestClearEjPc，callback ctx=char*
//    解包：[ctx+0]=CUser*, [ctx+4]=elixir_item_id vector, [ctx+8]=结果 vector。
//    注：仓库现有 enumQuestClear 声明若以成员指针传递则由宿主统一适配；
//    ORIG 直接以普通函数指针 _ZN5CUser17_fnStatQuestClearEjPc 传入。
// ---------------------------------------------------------------------------
// int CUser::_fnStatQuestClear(unsigned int questIdx, char* context)
// {
//     CUser* user = *(CUser**)(context);
//     std::vector<int>* elixirIds = *(std::vector<int>**)(context + 4);
//     std::vector<std::pair<int, int> >* outList =
//         *(std::vector<std::pair<int, int> >**)(context + 8);
//
//     Quest* q = G_CDataManager()->find_quest((int)questIdx);
//     if (q == 0)
//         return 1;
//     if (q->m_fieldB8 != 0 && q->m_fieldB8 != 0x19)   // Quest+0xb8 任务种类过滤
//         return 1;
//
//     std::vector<RewardItemInfo>* rewardPtr = user->getUserQuestRewardItem(q, user);
//     std::vector<RewardItemInfo>& rewards = *rewardPtr;
//     for (std::vector<RewardItemInfo>::const_iterator ri = rewards.begin();
//          ri != rewards.end(); )
//     {
//         for (std::vector<int>::const_iterator ii = elixirIds->begin();
//              ii != elixirIds->end(); ++ii)
//         {
//             if (*ii == ri->m_itemIdx)
//             {
//                 user->add_matched_list(*outList, ri->m_itemIdx,
//                                        *(int*)&ri->m_count);
//             }
//         }
//         (void)ri;
//         break;   // 见下方注：外层结构按报告“for each reward × for each id”双层遍历
//     }
    // 注（忠实性）：上式为了保守起见对“奖励向量重置迭代器”语义做了单层展开；
    // 完整形态应为：
    //   for (ri = rewards.begin(); ; ) {
    //     for (ii = elixir.begin(); ii != end; ++ii) { if match -> add_matched_list(...); }
    //     reset inner iterator to begin(); ++ri; if (ri==rewards.end()) break;
    //   }
//     return 1;
// }

// ---------------------------------------------------------------------------
// 5) CUser::_GetRecipeItem(unsigned long, bool, int&)
//    报告：docs/class_func_reports/CUser/_GetRecipeItem.md。ORIG 0x08670e38。
// ---------------------------------------------------------------------------
CItem* CUser::_GetRecipeItem(unsigned long itemIdx, bool byIndex, int& outSlotRef)
{
    CItem* ret = 0;
    if (byIndex)
    {
        ret = G_CDataManager()->find_item((int)itemIdx);
        if (ret == 0)
            return 0;
    }
    else
    {
        CInventory* inven = const_cast<CInventory*>(getCurCharacInvenR());
        const Inven_Item& slot = inven->GetInvenSlot(1, (int)itemIdx);   // ORIG 无构造/拷贝：直接绑定隐藏返回
        if (slot.m_addInfo == 0)                            // ORIG mov -0x4b(+0x02 dword) test
            return 0;
        outSlotRef = slot.get_add_info();
        ret = G_CDataManager()->find_item(slot.m_addInfo);
        if (ret == 0)
            return 0;
    }
    if (!ret->is_stackable())
        return 0;
    if (((CStackableItem*)ret)->m_nItemType != 3)   // ORIG 读 +0x2b0（非 GetMonsterCardId +0x2b8）
        return 0;
    return ret;
}

// ---------------------------------------------------------------------------
// 6) CUser::_IsCompoundResult2MoreEquip(vector<pair<int,int>> const&)
//    报告：docs/class_func_reports/CUser/_IsCompoundResult2MoreEquip.md。ORIG 0x086720d0。
// ---------------------------------------------------------------------------
bool CUser::_IsCompoundResult2MoreEquip(const std::vector<std::pair<int, int> >& outputs)
{
    for (std::vector<std::pair<int, int> >::const_iterator it = outputs.begin();
         it != outputs.end(); ++it)
    {
        CDataManager* dm = G_CDataManager();
        CItem* item = dm->find_item(it->first);
        if (item == 0)
            return true;
        if (!item->is_stackable())
        {
            if (it->second > 1)
                return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// 7) CUser::_onChangeGrowType(uchar, uchar newSecond, eChangeGrowTypeReason reason)
//    报告：docs/class_func_reports/CUser/_onChangeGrowType.md。ORIG 0x08664080。
// ---------------------------------------------------------------------------
void CUser::_onChangeGrowType(unsigned char newFirst, unsigned char secondGrew,
                              eChangeGrowTypeReason reason)
{
    {
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x15);
        bool on = ev->IsEventing((CUser*)0);                      // vtable+0x34 槽（send-flag 判定）
        if (on && reason != (eChangeGrowTypeReason)0x1)
        {
            unsigned int achievementType = (secondGrew == 0) ? 10u : 11u;
            DB_InsertArchieveEventLog::makeRequest(get_acc_id(), getCurCharacNo(),
                                                   (SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE)achievementType);
        }
    }

    bool gmTimeActive = false;
    {
        CEnvironment* env = G_CEnvironment();
        // CEnvironment+0x378 = m_serverEnvir.m_serverGroup 整数==10 → gm server
        unsigned int now = GlobalData::s_systemTime_.getCurSec();
        if (env->m_serverEnvir.m_serverGroup == 10 && now < 0x478e7051u)
            gmTimeActive = true;
    }
    if (gmTimeActive && reason != (eChangeGrowTypeReason)0x1 && secondGrew == 0)
    {
        char msg[256];
        strncpy(msg, g_scriptStringManager_.findString(4, "game_server_msg_13", 0), 0xff);
        Inven_Item gift;
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "game_server_msg_09", 0),
            gift, 150000, getCurCharacNo(), msg, strlen(msg), 0, GetServerGroup(), 0, 0);
    }

    bool premiumOn =
        (isAffectedPremium((ENUM_PREMIUM_TYPE)0x10) == true) &&
        (GlobalData::s_systemTime_.getCurSec() <= 0x46e1bbd0u);
    if (premiumOn && reason != (eChangeGrowTypeReason)0x1 && secondGrew == 0)
    {
        CInventory* inven = getCurCharacInvenW();
        Inven_Item& ref = *inven->GetInvenRef(0, 0xb);
        unsigned int idx = ref.m_addInfo;                   // Inven_Item+0x02
        if ((idx - 0x65d6u) < 5u)
        {
            char msg[256];
            strncpy(msg, g_scriptStringManager_.findString(4, "game_server_msg_07", 0), 0xff);
            Inven_Item gift;
            gift.m_addInfo = 0x43d;
            ((char*)&gift)[0] = 2;                          // m_type=2（Inven_Item+0x00 byte）【推断字段序，见 fields_A】
            // +0x14 count 写点：ORIG gift count 由 create_item 内部填【报告核对项】
            
            gift.m_addInfo = 0;
            gift.ResetItemAttr();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_08", 0),
                gift, 0, getCurCharacNo(), msg, strlen(msg), 0, GetServerGroup(), 0, 0);
        }
    }
}

// >>> W7-BLOCKED CUser::_onCreateCharacter —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 8) CUser::_onCreateCharacter(uint accId→playerUid? 按 ORIG param_1=user uid, char* name, char type)
//    报告：docs/class_func_reports/CUser/_onCreateCharacter.md。ORIG 0x0864a432。
// ---------------------------------------------------------------------------
// void CUser::_onCreateCharacter(unsigned int userId, char* name, char createType)
// {
//     (void)name;
//     if (createType == 4)
//     {
//         if (GlobalData::s_systemTime_.getCurSec() < 0x4936f351u)
//         {
//             Inven_Item gift;                                // Inven_Item::Inven_Item()
//             gift.m_addInfo = 0x1d19;
//             CItemList& itemList = *G_CDataManager()->getItemList();
//             itemList.create_item((ENUM_ITEM_CREATE_TYPE)3, gift, 1);
//             if (gift.m_addInfo != -1)
//             {
//                 char title[20];
//                 char body[256];
//                 strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_56", 0), 0x14);
//                 strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_57", 0), 0xff);
//                 WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
//                     title, gift, 0, userId, body, strlen(body), 0,
//                     (ENUM_SERVER_GROUP)0, false, false);
//             }
//         }
//     }
//     _processUnlimitChallenge(userId, createType);
// }
//
// ---------------------------------------------------------------------------
// 9) CUser::_onQuestClear(int, int questId) —— ORIG T _ZN5CUser13_onQuestClearEii
//    @0x08664412。报告：docs/class_func_reports/CUser/_onQuestClear.md（全文）。
//    0x193 计数事件 → process_add_quest_count_db；0x987/0x96f 按 systemTime
//    门控投 DB；0x970 经 repeat 0x13 事件 IsEventing 门控投 0712 冬季券。
// ---------------------------------------------------------------------------
void CUser::_onQuestClear(int /*questIdx*/, int questId)
{
    EventClassify::CEventScriptMng* mng = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    if (mng->is_eventing(0x193))
        mng->process_add_quest_count_db(this, questId);

    if (questId == 0x987)
    {
        if (GlobalData::s_systemTime_.getCurSec() < 0x47b35a50)
            DB_InsertArchieveEventLog::makeRequest(get_acc_id(), getCurCharacNo(),
                                                   (SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE)0x32);
    }
    if (questId == 0x96f)
    {
        if (GlobalData::s_systemTime_.getCurSec() < 0x478e7050)
            DB_UpdateHappyBeanInfo::makeRequest(GetUID(), get_acc_id(), getCurCharacNo(),
                                                getCurCharacName());
    }
    if (questId == 0x970)
    {
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x13);
        if (ev != 0 && ev->IsEventing((CUser*)0))
            DB_Insert0712WinterEventCoupon::makeRequest(get_acc_id(), getCurCharacNo(), 1);
    }
}

bool CUser::_ProcCompoundCore(
    const std::vector<std::pair<int, int> >& mats,
    std::vector<std::pair<int, int> >& outputs,
    std::vector<int>& deletedSlots,
    int mainMatSlotNo,
    bool cubeMode,
    PacketGuard* pkt,
    bool& resultFlag,
    unsigned short mulCount,
    stEquipCondition cond,
    CStackableItem* recipe,
    Inven_Item& keptOptionSrc)
{
    std::vector<int> insertedSlots;   // ebp-0x98（insert_event_items 输出，3 int/项）
    std::vector<int> deletedSet;      // ebp-0x8c（delete_event_items 输出，3 int/项）

    CInventory* invenW = getCurCharacInvenW();
    const int delCount = invenW->delete_event_items(
        mats, deletedSet, resultFlag,
        (eMoneySubReason)0xc, (eItemDelReason)8, cubeMode);

    if (delCount == 0)
    {
        cMyTrace trace(
            "bool CUser::_ProcCompoundCore(const IntPairVector&, IntPairVector&, IntVector&, int, bool, PacketGuard*, bool&, RequestItemCountType, stEquipCondition, CStackableItem*, Inven_Item&)",
            0x5c8b, 5);
        trace("CUser::compound_item, getCurCharacInvenR()->delete_event_items , size does not match! %d %d !=%d",
              getCurCharacNo(), delCount, (int)mats.size());
        return false;    // ORIG ebx=0 → 0x08671e69
    }

    if (cubeMode)        // ebp+0x1c（ORIG cmpb 0 检查 @0x08671202）
    {
        pkt->put_byte(delCount);
    }
    else
    {
        pkt->put_byte((int)mulCount + delCount);   // movzwl mulCount + delCount
        if (!getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1 /*INVEN_TYPE_EQUIP*/, mainMatSlotNo,
                (int)mulCount, (eItemDelReason)8, true))
        {
            cMyTrace trace(
                "bool CUser::_ProcCompoundCore(const IntPairVector&, IntPairVector&, IntVector&, int, bool, PacketGuard*, bool&, RequestItemCountType, stEquipCondition, CStackableItem*, Inven_Item&)",
                0x5c9a, 5);
            trace("CUser::compound_item, getCurCharacInvenW()->delete_item failed , %d %d",
                  getCurCharacNo(), mainMatSlotNo);
            return false;
        }
        pkt->put_byte(0);
        pkt->put_short(mainMatSlotNo);
        pkt->put_int((int)mulCount);
    }

    // 删除结果回包：deletedSet 三元组逐项 put_byte/put_short/put_int
    if (delCount > 0)
    {
        for (std::vector<int>::const_iterator it = deletedSet.begin();
             it != deletedSet.end();)
        {
            const int v0 = *it++;
            const int v1 = *it++;
            const int v2 = *it++;
            pkt->put_byte(v0);
            pkt->put_short(v1);
            pkt->put_int(v2);
        }
    }

    // recipe+0x1a8 = m_recipe174(+0x174).m_field34
    const bool succeed = isSuccessCompound(recipe->m_recipe174.m_field34);

    if (!succeed)
    {
        recipe->m_recipe174.changeOutput(outputs);
        if (outputs.empty())
        {
            pkt->put_byte(0);
            return true;    // ORIG ebx=1 @0x086714a3
        }
    }

    const int insCount = getCurCharacInvenW()->insert_event_items(
        outputs, insertedSlots,
        (eMoneyAddReason)7, (eItemAddReason)9, false, "", "");
    pkt->put_byte(insCount);

    if (insCount != (int)outputs.size())
    {
        cMyTrace trace(
            "bool CUser::_ProcCompoundCore(const IntPairVector&, IntPairVector&, IntVector&, int, bool, PacketGuard*, bool&, RequestItemCountType, stEquipCondition, CStackableItem*, Inven_Item&)",
            0x5d73, 5);
        trace("CUser::compound_item, getCurCharacInvenR()->insert_event_items failed %d",
              getCurCharacNo());
        return false;
    }

    // insertedSlots 三元组 [0/结果码 r, slotNo, count/itemIdx]：每轮消费 3 int
    for (std::vector<int>::const_iterator it = insertedSlots.begin();
         it != insertedSlots.end();)
    {
        const int slotRef = *it++;    // +0x453 deref → local_a4（push_back/put_short 用）
        Inven_Item got;
        got = getCurCharacInvenR()->GetInvenSlot((int)INVEN_TYPE_EQUIP, slotRef);

        if (got.m_field1 == 1)        // ORIG movzbl got+0x01 cmp 1（ITEM_TYPE_EQUIP）
        {
            CItem* script = G_CDataManager()->find_item(got.m_addInfo);
            if (script != 0)
            {
                if ((char)cond.m_upgrade == 1)    // ebp+0x2c（cond+0）
                {
                    if (!succeed)
                    {
                        if (recipe->m_recipe174.m_field3c == 1)   // recipe+0x1b0
                            copyItemOption(got, keptOptionSrc);
                    }
                    else
                    {
                        if (recipe->m_recipe174.m_field3c == 1)
                        {
                            copyItemOption(got, keptOptionSrc);
                        }
                        else
                        {
                            // vtable+0xc 虚调用（CItem::GetItemType）结果作尾参
                            setRecipeEqiupmentOption(got, cond, slotRef, recipe,
                                                     script->GetItemType());
                        }
                        if (recipe->m_recipe174.m_field38 != 0)   // recipe+0x1ac
                        {
                            compoundSystemMessage(got);
                            compoundGuildMessage(got);
                        }
                    }
                    getCurCharacInvenW()->update_item((INVEN_TYPE)1, slotRef, got);
                }
                else
                {
                    // ORIG: rarity==5 || GetItemType()==0xb || recipe->GetNeedLevel()==0
                    //       → doMake=false；否则 true
                    const bool doMake = !(script->get_rarity() == 5 ||
                                          script->GetItemType() == 0xb ||
                                          recipe->GetNeedLevel() == 0);
                    if (doMake)
                    {
                        // GetCharacExpandData(10) 返回 CItemMakingSkill*（charac_expand::CData 派生）
                        CItemMakingSkill* skill =
                            (CItemMakingSkill*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)10);
                        const int level = skill->GetItemMakingSkillLevel(recipe, this);

                        const stItemMakingSkill* table =
                            (const stItemMakingSkill*)G_CDataManager()->GetItemMakingSkill();
                        std::map<int, stUpgradeWithCompound>::const_iterator ti =
                            table->m_map0.find(level);
                        if (ti == table->m_map0.end())
                        {
                            LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__,
                                                  0x5d0c,
                                                  "item_making_skill_level error : %d",
                                                  level);
                            return false;
                        }

                        int upgrade = 0;
                        if (recipe->m_recipe174.m_field25 == 0)   // recipe+0x199
                        {
                            // iter+0xc = ti->second.m_field08（概率分子，分母固定 10000）
                            // iter+0x04 = m_field00（基值）；iter+0x10 = m_rangeExp0C（/1000）
                            // iter+0x14 = m_field10（minVal 基值）；iter+0x20 = m_rangeExp1C（/1000）
                            if ((unsigned int)get_rand_int(10000)
                                < (unsigned int)ti->second.m_field08)
                            {
                                upgrade = ti->second.m_field00
                                        + get_rand_int(ti->second.m_rangeExp0C) / 1000;
                            }
                            else
                            {
                                upgrade = ti->second.m_field10
                                        + get_rand_int(ti->second.m_rangeExp1C) / 1000;
                            }
                        }
                        if (upgrade < 0 || upgrade > 0x1f)
                            upgrade = 0;

                        got.SetUpgrade((unsigned char)upgrade);
                        if (got.GetUpgrade() != 0)
                            getCurCharacInvenW()->update_item((INVEN_TYPE)1, slotRef, got);

                        if (recipe->GetItemType() == 3)   // recipe vtable+0xc
                        {
                            // ORIG 读 *(int*)(G_CDataManager()+0x5e0)（0x08671ab6/0x08671abb）
                            // —— CDataManager 该偏移未具名建模，保留视图
                            const int capUpgrade =
                                *(const int*)((const char*)G_CDataManager() + 0x5e0); // [INFERENCE] 具名成员待落
                            if (capUpgrade <= (int)got.GetUpgrade())
                            {
                                PacketGuard pg;
                                pg.put_header(0, 0x56);
                                pg.put_byte(7);
                                pg.put_byte(1);
                                pg.put_short((int)(get_unique_id() & 0xffff));
                                pg.put_int(got.m_addInfo);
                                pg.put_byte((int)got.GetUpgrade());
                                pg.finalize(true);
                                G_GameWorld()->send_all(pg);
                            }
                        }
                    }
                }
            }
        }

        deletedSlots.push_back(slotRef);
        pkt->put_byte(0);
        pkt->put_short(slotRef);

        const int v1 = *it++;    // put_int#1 数据（三元组第 2 元素）
        const int v2 = *it++;    // put_int#2 数据（三元组第 3 元素）
        pkt->put_int(v1);
        pkt->put_int(v2);

        pkt->put_byte((int)got.GetUpgrade());
        pkt->put_short((int)got.m_fieldb);                 // got+0x0b（endurance，movzwl -0xe6）
        pkt->put_byte((int)got.m_amp.getAbilityType());    // stAmplifyOption_t@got+0x11
        pkt->put_short((int)got.m_amp.getAbilityValue());
        pkt->put_byte((int)(unsigned char)got.m_field0 != 0 ? 1 : 0);  // movzbl got+0x00 setne
        pkt->put_packet(got);

        if (cubeMode)
        {
            GetInstanceCubeStatistic()->collectCubeStatistics(v1, v2, this,
                                                              (CUBE_STATISTIC_FIELD)3);
        }
    }

    return true;
}


// ---------------------------------------------------------------------------
void CUser::_processCharacterDayEvent(unsigned int oldFatigue, unsigned int addFatigue)
{
    unsigned int total = oldFatigue + addFatigue;
    if (!(oldFatigue != 0 && total != 0 && oldFatigue < 0x9du))
        return;

    static const float DIV_DAY = 30.0f;  // ORIG .rodata DAY 常量（0x08cf3960 处值 30.0，已验证）
    int dayA = (int)(oldFatigue / DIV_DAY);
    int dayB = (int)(total / DIV_DAY);
    if (dayA == dayB)
    {
        static const float DIV_WEEK = 156.0f;  // ORIG .rodata WEEK 常量（0x08cf3964 处值 156.0，已验证）
        int weekA = (int)(oldFatigue / DIV_WEEK);
        int weekB = (int)(total / DIV_WEEK);
        if (weekA == weekB)
            return;
    }
    int itemId; int addCount;
    const char* msgKey;
    if (dayA == dayB)                       // 同日跨周
    { itemId = 0x1d5e; addCount = 1; msgKey = "game_server_msg_75"; }
    else if (dayA == 0 && dayB == 1)
    { itemId = 0x1d5c; addCount = 2; msgKey = "game_server_msg_71"; }
    else if (dayA == 1 && dayB == 2)
    { itemId = 0x1d5c; addCount = 4; msgKey = "game_server_msg_72"; }
    else if (dayA == 2 && dayB == 3)
    { itemId = 0x1d5c; addCount = 6; msgKey = "game_server_msg_73"; }
    else if (dayA == 3 && dayB == 4)
    {
        itemId = 0x1d5d; addCount = 1; msgKey = "game_server_msg_74";
        CUserEventUtil::updateCharacterDayEvent(this, 1);
    }
    else return;

    char body[256];
    strncpy(body, g_scriptStringManager_.findString(4, msgKey, 0), 0xff);
    char title[21];
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_70", 0), 0x14);
    Inven_Item gift;
    CDataManager* dm = G_CDataManager();
    CItem* proto = dm->find_item(itemId);
    if (proto == 0) return;
    gift.m_addInfo = itemId;
    proto->make_item(gift);                 // virtual vtable+0x08 槽
    gift.set_add_info(addCount);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, gift, 0, getCurCharacNo(), body, strlen(body), 0, GetServerGroup(), 0, 0);
}

// ---------------------------------------------------------------------------
// 12) CUser::_processFatigueBuffEvent(uint, uint) —— 同型（DIV=.rodata, 1/2/3→0x1d95）
//     报告：docs/class_func_reports/CUser/_processFatigueBuffEvent.md。ORIG 0x086598ba。
// ---------------------------------------------------------------------------
void CUser::_processFatigueBuffEvent(unsigned int oldFatigue, unsigned int addFatigue)
{
    unsigned int total = oldFatigue + addFatigue;
    if (!(oldFatigue != 0 && total != 0 && oldFatigue < 0x9du))
        return;
    static const float DIV = 50.0f;  // ORIG .rodata 常量（0x08cf3968 处值 50.0，已验证）
    int zoneA = (int)(oldFatigue / DIV);
    int zoneB = (int)(total / DIV);
    if (zoneA == zoneB) return;
    int itemId = 0x1d95, addCount;
    const char* key = 0;
    if (zoneA == 1 && zoneB == 2)      { addCount = 10; key = "game_server_msg_84"; }
    else if (zoneA == 2 && zoneB == 3) { addCount = 20; key = "game_server_msg_85"; }
    else return;
    char body[256];
    strncpy(body, g_scriptStringManager_.findString(4, key, 0), 0xff);
    char title[21];
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_83", 0), 0x14);
    Inven_Item gift;
    CItem* proto = G_CDataManager()->find_item(itemId);
    if (!proto) return;
    gift.m_addInfo = itemId;
    proto->make_item(gift);
    gift.set_add_info(addCount);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, gift, 0, getCurCharacNo(), body, strlen(body), 0, GetServerGroup(), 0, 0);
}

// ---------------------------------------------------------------------------
// 13) CUser::_processUnlimitChallenge(unsigned int, char)
//     报告：docs/class_func_reports/CUser/_processUnlimitChallenge.md。ORIG 0x0864a5aa。
// ---------------------------------------------------------------------------
void CUser::_processUnlimitChallenge(unsigned int level, char kind)
{
    CLevelRewardTableMgr* mgr = CLevelRewardTableMgr::GetInstance();
    if (mgr == 0) return;
    if (mgr->IsOn() == false) return;
    if (mgr->IsOnServer(G_CEnvironment()->m_serverEnvir.m_serverGroup)
        == false) return;
    if (mgr->IsHaveStartLevelReward() == false) return;
    _processLevelReward(1, level, kind);
}

// ---------------------------------------------------------------------------
// 14) CUser::_saveConditionEventInfo(uint, short, short)
//     报告：docs/class_func_reports/CUser/_saveConditionEventInfo.md。ORIG 0x086819fa。
// ---------------------------------------------------------------------------
void CUser::_saveConditionEventInfo(unsigned int value, short a2, short a3)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x71);
    if (ev->IsEventing((CUser*)0))     // vtable+0x34
    {
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x8556);
        CStreamGuard guard(s, true);
        guard << 0x19f;
        guard << (int)GetUID();
        SIG_SAVE_CONDITION_EVENT* sigBody = guard.GetInBuffer<SIG_SAVE_CONDITION_EVENT>();
        sigBody->m_value = value;                 // +0x00 uint
        sigBody->m_a2 = a2;                       // +0x04 short
        sigBody->m_a3 = a3;                       // +0x06 short
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ---------------------------------------------------------------------------
// 15) CUser::_saveProperDungeonClearCount(uint, short)
//     报告：docs/class_func_reports/CUser/_saveProperDungeonClearCount.md。ORIG 0x08681b3a。
// ---------------------------------------------------------------------------
void CUser::_saveProperDungeonClearCount(unsigned int dungeonIdx, short subNo)
{
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x85cd);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<(0x1d4);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_BLOOD_CLEAR_COUNT* bloodBody =
        guard.operator->()->GetInBuffer<SIG_SAVE_BLOOD_CLEAR_COUNT>();
    bloodBody->m_dungeonIdx = dungeonIdx;           // +0x00
    bloodBody->m_subNo = subNo;                     // +0x04
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// 16) CUser::_updateSaveOption()
//     报告：docs/class_func_reports/CUser/_updateSaveOption.md。ORIG 0x08652dde。
// ---------------------------------------------------------------------------
void CUser::_updateSaveOption()
{
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1d0d);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<(0x2b5);
    guard.operator*()->operator<<((int)GetUID());
    // SIG_CHARACTER_OPTION 协议视图：+0x00 characNo + 0x04 CharacterOption(0x200)。
    struct SIG_CHARACTER_OPTION_VIEW
    {
        unsigned int m_characNo;
        CharacterOption m_option;
    };
    // ORIG 0x08652dde：GetInBuffer<SIG_CHARACTER_OPTION>（0x204 占位协议视图）
    SIG_CHARACTER_OPTION_VIEW* body =
        reinterpret_cast<SIG_CHARACTER_OPTION_VIEW*>(
            guard.operator->()->GetInBuffer<SIG_CHARACTER_OPTION>());
    memset(body, 0, 0x204);
    body->m_characNo = getCurCharacNo();
    GetCurCharacOption(body->m_option);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    setSaveCharacOption(false);
}

// ---------------------------------------------------------------------------
// 17) CUser::add_matched_list(vector<pair<int,int>>&, int key, int addValue)
//     报告：docs/class_func_reports/CUser/add_matched_list.md。ORIG 0x086650b2。
// ---------------------------------------------------------------------------
void CUser::add_matched_list(std::vector<std::pair<int, int> >& list, int key, int addValue)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i].first == key)
        {
            list[i].second += addValue;
            return;
        }
    }
    list.push_back(std::make_pair(key, addValue));
}

// ---------------------------------------------------------------------------
// 18) CUser::AddCurCharacMercenaryInfo()
//     报告：docs/class_func_reports/CUser/AddCurCharacMercenaryInfo.md。ORIG 0x0868e596。
// ---------------------------------------------------------------------------
void CUser::AddCurCharacMercenaryInfo()
{
    const _Charac_info* sel = getCurCharacR();
    if (!sel) return;
    // ORIG movzwl 0xa8e0(%eax)：ushort 零扩展读（stMercenarySystemInfo.m_field4）
    unsigned short minLvRequired = G_CDataManager()->m_mercenarySystem.m_field4;
    if (sel->m_level < minLvRequired) return;

    CHARAC_LOAD_MERCENARY info;
    info.Reset(); /* ORIG 调 Reset（+0x30/0x31=0xff、+0x32=0xa）*/ info.m_level = sel->m_level;               // +0x04
    info.m_characNo = sel->m_characNo;         // +0x00
    strncpy(info.m_name, sel->m_name, 0x1d);   // +0x06 ← src charac+0x04
    info.m_avatarBonus = CalcMercenaryAvatarBonus();  // +0x32 uchar
    m_mercenaryMgr.AddCharac(info);
}

// ---------------------------------------------------------------------------
// 19) CUser::AddDailyBadge(ENUM_BADGE_TYPE badge) 【报告缺失@0x08568e5e size 0x26】
//     objdump -d 0x8568e5e..0x8568e84:
//       lea 0x47040(%eax),%ecx        ; ecx = badge + 0x47040
//       movzwl 0x4(%edx,%ecx,2),%edx  ; u16 @ this + (badge+0x47040)*2 + 4
//                                     ;       = this + 0x8E084 + badge*2
//       lea 0x1(%edx),%ecx ; mov %cx,0x4(%edx,%eax,2)  ; 自增
//     ⇒ ushort 计数表基址 this+0x8E084，按 badge 下标自增。
// ---------------------------------------------------------------------------
void CUser::AddDailyBadge(ENUM_BADGE_TYPE badge)
{
    m_dailyBadge[(int)badge] = (short)(m_dailyBadge[(int)badge] + 1);   // ushort 计数自增（ORIG movzwl+lea1 同型）
}

// ---------------------------------------------------------------------------
// 20) CUser::AddTicket(int ticketIdx, bool notify)
//     报告：docs/class_func_reports/CUser/AddTicket.md。ORIG 0x08678282。
// ---------------------------------------------------------------------------
void CUser::AddTicket(int ticketIdx, bool notify)
{
    if (notify)
    {
        for (std::list<int>::const_iterator it = m_list8cf80.begin();
             it != m_list8cf80.end(); ++it)
        {
            if (*it == ticketIdx)
                return;
        }
        m_list8cf80.push_back(ticketIdx);

        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x703d);
        CStreamGuard guard(s, true);
        guard << 0x28;
        guard << (int)GetUID();
        struct { unsigned int characNo; int ticket; } payload;
        memset(&payload, 0, 8);
        payload.characNo = getCurCharacNo();
        payload.ticket = ticketIdx;
        guard.put_binary(&payload, 8);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else
    {
        m_list8cf80.push_back(ticketIdx);
    }
}

// ---------------------------------------------------------------------------
// 21) CUser::alonePlay_Mode()
//     报告：docs/class_func_reports/CUser/alonePlay_Mode.md。ORIG 0x0868dc7a。
// ---------------------------------------------------------------------------
bool CUser::alonePlay_Mode()
{
    CGameOption* opt = GetGameOptionRef();
    return opt->get_etc_option((ENUM_OPTION_ETC)0x1b) == 1;
}

// ---------------------------------------------------------------------------
// 22) CUser::alonePlay_Mode(MSG_REQ_TO_PEER const&)
//     报告：alonePlay_Mode_0868dcbc.md。ORIG 0x0868dcbc。
// ---------------------------------------------------------------------------
bool CUser::alonePlay_Mode(const MSG_REQ_TO_PEER& req)
{
    CGameOption* opt = GetGameOptionRef();
    // ORIG 0x0868dcbc：movzbl+movsbl 符号扩展后 jbe（无符号）——0x80+ 字节按
    // 大数处理即恒 >3 → 拒绝。等价无符号判定（unsigned char 比较）。
    if ((unsigned char)*(const unsigned char*)((const char*)&req + 0xf) > 3)
        return false;
    return opt->get_etc_option((ENUM_OPTION_ETC)0x1b) == 1;
}

// ---------------------------------------------------------------------------
// 23) CUser::apply_elixir_item(_Additioal_info&, pair<int,int>&)
//     报告：docs/class_func_reports/CUser/apply_elixir_item.md。ORIG 0x08665274。
//     switch(first)：0x40f..0x412 → 短字段+count*50；0x413/0x414 → int+count*250；
//     0x415 → int@+0x42+count*10；0x416 → 循环 4 个 short+=count*10。
//     _Additioal_info 内字段由 FixAdditionalInfoLeaves 批建模；此处按 m_pad 视图改具名草案。
// ---------------------------------------------------------------------------
void CUser::apply_elixir_item(_Additioal_info& ai, std::pair<int, int>& pr)
{
    int amount = pr.second;
    switch (pr.first)
    {
    case 0x40f: ai.m_field08 += (short)(amount * 50); break;   // +0x08
    case 0x410: ai.m_field0c += (short)(amount * 50); break;   // +0x0c
    case 0x411: ai.m_field0a += (short)(amount * 50); break;   // +0x0a
    case 0x412: ai.m_field0e += (short)(amount * 50); break;   // +0x0e
    case 0x413: ai.m_field00 += amount * 250; break;           // +0x00
    case 0x414: ai.m_field04 += amount * 250; break;           // +0x04
    case 0x415: ai.m_field42 += amount * 10;  break;           // +0x42
    case 0x416:
        for (int i = 0; i < 4; ++i)
        {
            /* +0x10, +0x12, +0x14, +0x16 */
            ai.m_field18[i] += (short)(amount * 10);
        }
        break;
    default:
        break;
    }
}

// >>> W7-BLOCKED CUser::calc_quest_stat —— WongWork::CQuestClear::enumQuestClear functor 契约与 elixir_item_id per-TU 表值不可取
// ---------------------------------------------------------------------------
// 24) CUser::calc_quest_stat(_Additioal_info&)
//     报告：docs/class_func_reports/CUser/calc_quest_stat.md。ORIG 0x08664c50。
// ---------------------------------------------------------------------------
// void CUser::calc_quest_stat(_Additioal_info& ai)
// {
//     std::vector<int> elixirIds;
//     for (int i = 0; i <= 7; i++)                       // ORIG cmpl $7;setle
//         elixirIds.push_back(::elixir_item_id[i]);
//
//     std::vector<std::pair<int, int> > ownCounts;
//     count_specific_items(elixirIds, ownCounts);
//
//     std::vector<std::pair<int, int> > rewards;
//     UserQuest* quest = getCurCharacQuestR();
//     quest->enumQuestClear(_fnStatQuestClear, this, &elixirIds, &rewards);
//
//     complete_reward_list(rewards, ownCounts);
//
//     for (size_t i = 0; i < rewards.size(); i++)
//     {
//         apply_elixir_item(ai, rewards[i]);
//     }
// }

// ---------------------------------------------------------------------------
// 26) CUser::CalLevelUpItemCheck(unsigned long itemId)
//     报告：docs/class_func_reports/CUser/CalLevelUpItemCheck.md。ORIG 0x08689d06。
// ---------------------------------------------------------------------------
bool CUser::CalLevelUpItemCheck(unsigned long itemId)
{
    int lvl = get_charac_level();
    if (itemId == 0x28d2bc || itemId == 0x989a13u || itemId == 0x292090e1u)
        return (lvl > 0 && lvl < 0x55);
    if (itemId == 0x1f71ul)
        return (lvl >= 0x13 && lvl <= 0x3b);
    return false;
}

// ---------------------------------------------------------------------------
// 27) CUser::CalLevelUpItemState(char&, int&, int, int)
//     报告：docs/class_func_reports/CUser/CalLevelUpItemState.md。ORIG 0x08689d74。
// ---------------------------------------------------------------------------
bool CUser::CalLevelUpItemState(char& state, int& needExp, int fromLevel, int toLevel)
{
    static const char* PRETTY = "bool CUser::CalLevelUpItemState(char&, int&, int, int)";
    if (get_charac_level() < fromLevel || get_charac_level() > toLevel)
        return false;
    state = 1;
    int luExp = get_level_up_exp(get_charac_level());
    int lvTop = G_CDataManager()->get_level_exp(get_charac_level());
    needExp = luExp + (lvTop - get_charac_exp());
    bool wrong = (needExp < 0);
    {
        unsigned int charno = getCurCharacNo();
        char* mid = NumberToString(get_acc_id(), 0);
        cMyTrace trace(PRETTY, wrong ? 0xa0d7 : 0xa0da, 0);
        if (wrong)
            trace("Level_up_item:wrong value(%d) m_id(%s),charno(%u)", needExp, mid, charno);
        else
            trace("Level_up_item:success value(%d) m_id(%s),charno(%u)", needExp, mid, charno);
    }
    return !wrong;
}

// >>> W7-BLOCKED CUser::ChangeGrowType_Item —— _postCheckForceChangeGrowType 等调用依赖 owner 待定；clear_all_skills_both 链
// ---------------------------------------------------------------------------
// 28) CUser::ChangeGrowType_Item(int newFirst, short slotNo)
//     报告：docs/class_func_reports/CUser/ChangeGrowType_Item.md。ORIG 0x0867b196。
// ---------------------------------------------------------------------------
// int CUser::ChangeGrowType_Item(int newFirst, short slotNo)
// {
//     if (!getCurCharacR())
//         return 1;
//     char first = getCurCharFirstGrowType();
//     if (!(first == 0 || first == (char)newFirst))
//     {
//         setCurCharChangingGrowthType((char)newFirst, -1);
//         getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, (int)slotNo, 1,
//                                           (eItemDelReason)10, true);
//         ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
//         ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
//         WongWork::CSkillChanger changer;
//         changer.SkillInitialize(this, 0, true);
//         int job = get_charac_job();
//         int level = get_charac_level();                      // ORIG local_14（trace 仅占位）
//         addSkillOnCreateCharacter((_Mastered_skill*)((char*)getCurCharacSkillW() + 0x46), job);
//         addSkillOnCreateCharacter((_Mastered_skill*)((char*)getCurCharacSkillW() + 0x1de), job);
//         char before = getCurCharFirstGrowType();
//         (void)level; (void)before;
//         set_grow_type((unsigned char)newFirst,
//                       (unsigned char)getCurCharSecondGrowType(), 0, 1);
//         adjust_charac_stat();
//         unsigned int cn = get_charac_no(-1);
//         reset_tag_info_to_me(cn);
//         _postCheckForceChangeGrowType(this, newFirst, (int)getCurCharSecondGrowType());
//         SendNotiPacket(static_cast<eSendTarget>(0), (ENUM_NOTIPACKET)2, 0);
//         send_skill_info();
//         SendNotiPacket(static_cast<eSendTarget>(1), NOTIPACKET_2, 1);
//         SendUpdateItem(SEND_TARGET_PARTY, 0, (int)slotNo);
//         cMyTrace trace(__PRETTY_FUNCTION__, 0x756d, 0);
//         trace("[USE GROWTYPE ITEM]characNo(%d) before(%d), after(%d)",
//               get_charac_no(-1), (int)getCurCharSecondGrowType(), newFirst);
//         return 0;
//     }
//     return 0x13;
// }

// ---------------------------------------------------------------------------
// 29) CUser::check_insert_lottery_reward(CItem const*, vector<pair<Inven_Item,bool>>& rewards,
//                                         vector<pair<Inven_Item,bool>>& leftOver)
//     报告：docs/class_func_reports/CUser/check_insert_lottery_reward.md。ORIG 0x08676296。
// ---------------------------------------------------------------------------
int CUser::check_insert_lottery_reward(const CItem* opener,
                                       std::vector<std::pair<Inven_Item, bool> >& rewards,
                                       std::vector<std::pair<Inven_Item, bool> >& leftover)
{
    // 第一轮：逐项判背包空位（item_type=(byte@+0x01)，空位数需 ≥1）
    for (std::vector<std::pair<Inven_Item, bool> >::const_iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        unsigned char t = ((const unsigned char*)&it->first)[1];
        if (getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)t, 1) != 1)
            return 4;
    }
    // 第二轮：逐项插入
    for (std::vector<std::pair<Inven_Item, bool> >::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        Inven_Item it2 = it->first;                          // 整体拷贝（Inven_Item 视图）
        if ((*reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&it2)+4)) /*+0x04 视图*/ /*短区 u16@@+0x04 view of element+0x03..*/ == 1)
        {
            leftover.push_back(*it);                          // 直接回收
            continue;
        }
        CItem* script = G_CDataManager()->find_item(it2.m_addInfo);
        if (script == 0)
            return 0x11;
        if (script->IsClearAvatar() /*virtual+0x10*/ == true)
        {
            if (getCurCharacInvenW()->GetRemainCapacity((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)0) <= 0)
                return 4;
            goto INSERT_OK;
        }
        if (script->IsCreatureItem() /*virt+0x14*/ &&
            script->GetItemType() /*virt+0x0c*/ == 0x16)
        {
            if (((unsigned char*)&it2)[1] != 5)
                return 0x11;
            if (!user_creature::CCreatureItemConverter::SetEggItem(&it2))
                return 0x11;
            int r = getCurCharacInvenW()->tryInsertItemIntoInventory(it2);
            if (r == -1) return 4;
            goto INSERT_OK;
        }
        else
        {
            int r = getCurCharacInvenW()->tryInsertItemIntoInventory(it2);
            if (r == -1)
            {
                ((CStackableItem*)opener)->get_multibox_default_item(&leftover);
                continue;
            }
            if (script == 0) return 4;
        }
INSERT_OK:
        leftover.push_back(*it);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// 30) CUser::check_locked_multibox_error(CItem const*, int slotCntHint, bool doubleMode)
//     报告：docs/class_func_reports/CUser/check_locked_multibox_error.md。ORIG 0x08675fd2。
// ---------------------------------------------------------------------------
int CUser::check_locked_multibox_error(const CItem* opener, int openedCnt, bool doubleMode)
{
    Inven_Item lockHost;                                   // GetInvenSlot 隐藏回填
    lockHost = getCurCharacInvenR()->GetInvenSlot(1, openedCnt);
    CItem* locked = G_CDataManager()->find_item(lockHost.m_addInfo);
    if (opener == 0 || locked == 0)
        return 0x11;
    if (!opener->is_stackable())
        return 0x11;
    if (opener->GetItemType() /*virt+0x0c*/ != 0x1a)
        return 0x11;
    if (!locked->is_stackable())
        return 0x11;
    if (locked->GetItemType() != 1)
        return 0x11;

    // ORIG 读 CItem+0x1ec/+0x1f0（运行时 opener 为 CStackableItem；
    // 偏移 = m_multiBox(+0x1e4).m_field8 / m_fieldc）
    const CStackableItem* stackOpener = static_cast<const CStackableItem*>(opener);
    int wantSlot = stackOpener->m_multiBox.m_field8;
    int needAddInfo = stackOpener->m_multiBox.m_fieldc;
    int itemIdx = locked->get_index();
    if (!(itemIdx == wantSlot &&
          needAddInfo <= lockHost.get_add_info()))
        return 0x11;

    int slots = 2;
    if (doubleMode)
        slots = (int)*(unsigned char*)((char*)G_CDataManager() + 0x877e) * 2;
    int err = check_multibox_error(slots);
    return err == 0 ? 0 : err;
}

// ---------------------------------------------------------------------------
// 31) CUser::check_multibox_error(int requiredEmptySlots)
//     报告：docs/class_func_reports/CUser/check_multibox_error.md。ORIG 0x08675edc。
// ---------------------------------------------------------------------------
int CUser::check_multibox_error(int requiredEmptySlots)
{
    CInventory* inv = getCurCharacInvenW();
    if (inv->check_empty_count((Inven_Item::ITEM_TYPE)1, requiredEmptySlots) != 1)
        return 0x13;
    if (inv->check_empty_count((Inven_Item::ITEM_TYPE)2, requiredEmptySlots) != 1)
        return 0x13;
    if (inv->check_empty_count((Inven_Item::ITEM_TYPE)3, requiredEmptySlots) != 1)
        return 0x13;
    if (inv->check_empty_count((Inven_Item::ITEM_TYPE)10, requiredEmptySlots) != 1)
        return 0x13;
    if (inv->check_empty_count((Inven_Item::ITEM_TYPE)5, requiredEmptySlots) != 1)
        return 0x13;
    return 0;
}

// ---------------------------------------------------------------------------
// 32) CUser::check_unlocked_multibox_error(CItem const*, int addInfo)
// 32) CUser::check_unlocked_multibox_error(CItem const*, int addInfo)
// ---------------------------------------------------------------------------
int CUser::check_unlocked_multibox_error(const CItem* stack, int addInfo)
{
    if (stack == 0) return 0x11;
    if (!stack->is_stackable()) return 0x11;
    int t = stack->GetItemType();     // ORIG 对同虚槽 call 两次（vtable+0x0c 双读）
    int t2 = stack->GetItemType();
    (void)t2;
    if (!(t == 0x1a || t == 0x1b))
        return 0x11;
    if (addInfo != 0xffff)
        return 0x11;
    int err = check_multibox_error(2);
    return err == 0 ? 0 : err;
}

// >>> W7-BLOCKED CUser::CUser::checkCharacLinkType  —— 原因:_Charac_info.m_linkGrowType@+0x1246 未建模（权威 isCharacLinkBonusSetting 读 +0x124f 为另一字段）
// ---------------------------------------------------------------------------
// 33) CUser::checkCharacLinkType(char linkPos, int characSlot)
//     报告：docs/class_func_reports/CUser/checkCharacLinkType.md。ORIG 0x08652b90。
// ---------------------------------------------------------------------------
// bool CUser::checkCharacLinkType(char linkPos, int slot)
// {
//     int linkType = (int)getCharacLinkType();
//     if (linkPos < 0 || (int)(signed char)linkPos >= slot)
//         return false;
//     if (linkType == 2)
//     {
//         if ((int)linkPos + 1 >= slot) return false;
//         if (((unsigned char)linkPos & 1) != 0) return false;
//         if (m_characList[slot - 1 + 1 - 1].m_linkGrowType /*+0x1246*/ != 3) return false;
//         return true;
//     }
//     if (linkType == 3)
//     {
//         if ((int)linkPos - 1 < 0) return false;
//         if (((unsigned char)linkPos & 1) == 0) return false;
//         if (m_characList[slot - 1].m_linkGrowType != 2) return false;
//         return true;
//     }
//     return true;
// }

// ---------------------------------------------------------------------------
// 34) CUser::checkCodeChecksum(CodeHackChecksum const&, char*)
//     报告：docs/class_func_reports/CUser/checkCodeChecksum.md。ORIG 0x0867ebba。
// ---------------------------------------------------------------------------
bool CUser::checkCodeChecksum(const CodeHackChecksum& ck, char* dumpOut)
{
    bool ok = m_codeHackCheck.IsRightChecksum(&ck);
    if (ok)
        m_codeHackCheck.PopData();
    else
        m_codeHackCheck.PopData(dumpOut);
    return ok;
}

// ---------------------------------------------------------------------------
// 35) CUser::checkCodeChecksumRange(CodeHackChecksumEx const&, CodeHackCheckRangeEx const&)
//     报告：docs/class_func_reports/CUser/checkCodeChecksumRange.md。ORIG 0x0867ea5c。
// ---------------------------------------------------------------------------
bool CUser::checkCodeChecksumRange(const CodeHackChecksumEx& ckEx, const CodeHackCheckRangeEx& rangeEx)
{
    long resumeAt = GetResumeChecksumTime();
    long now = GlobalData::s_systemTime_.getCurSec();
    if (resumeAt >= now)
        return true;
    if (m_codeHackCheck.PushData(&ckEx) != 1)
        return false;
    PacketGuard pg;
    pg.put_header( 0, 0x6f);
    int len = strlen(rangeEx.GetFileName());
    pg.put_int( len);
    pg.put_str( rangeEx.GetFileName(), strlen(rangeEx.GetFileName()));
    pg.put_int( rangeEx.m_start104);    // Range+0x104
    pg.put_int( rangeEx.m_size108);     // Range+0x108
    pg.finalize( true);
    Send(pg);
    return true;
}

bool CUser::CheckDeleteCharacTime(int slot)
{
    // ORIG 0x0864a830（报告 CheckDeleteCharacTime.md 全文转写）。
    // CommonTime byte 视图：+0 年 +1 月 +2 日 +3 时 +4 分（SetCurTime 0x82a4224 写入序）。
    CommonTime t;
    t.SetCurTime();
    _Charac_info& ci = m_characList[slot];
    const char* tBytes = t.m_pad;
    const char* d = ci.m_delReqTime;                       // +0x39..+0x44 删除请求时间区
    if (tBytes[0] == d[0] && tBytes[1] == d[1] && tBytes[2] == d[2])
    {
        // movsbl：ORIG 以 signed char 展开小时/分钟；cmp $0x59f;jg → 差值>0x59f 才允许删除
        int now = (int)(char)tBytes[3] * 60 + (int)(char)tBytes[4];
        int req = (int)(char)d[3] * 60 + (int)(char)d[4];
        if (now - req <= 0x59f)
            return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// 37) CUser::CheckDungeonClear(int questId)
//     报告：docs/class_func_reports/CUser/CheckDungeonClear.md。ORIG 0x0866d934。
// ---------------------------------------------------------------------------
bool CUser::CheckDungeonClear(int questId)
{
    if (getCurCharacR() == 0)
        return false;
    return getCurCharacQuestR()->item_reward_quest(questId);
}

// ---------------------------------------------------------------------------
// 38) CUser::CheckGuildLevelUp(unsigned int& priceOut)
//     报告：docs/class_func_reports/CUser/CheckGuildLevelUp.md。ORIG 0x0866c27e。
// ---------------------------------------------------------------------------
unsigned int CUser::CheckGuildLevelUp(unsigned int& priceOut)
{
    static const char* PRETTY = "unsigned int CUser::CheckGuildLevelUp(unsigned int&)";
    if (get_charac_guildkey() == 0)
        return 0x22;
    if (!(IsGuildMaster() || IsSubGuildMaster()))
        return 0x53;
    unsigned char lev = m_guildDB.m_fundLevel;               // +0x8d03b
    if ((int)lev + 1 > 16)
        return 0x25;
    GuildParameterScript* gps =
        (GuildParameterScript*)((char*)G_CDataManager() + 0xa50c);
    int param = gps->getGuildLevelUpParam((int)lev + 1);
    if (param == 0)
    {
        cMyTrace trace(PRETTY, 0x53c2, 5);
        trace("CUser::CheckGuildLevelUp Guild Param Err(g lev : %d)", (int)lev);
        return 0x22;
    }
    unsigned short memberCnt = (*reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&m_guildDB) + 0x26)) /*+0x26 pad 区*/;    // +0x8d042（fields_A 建议名）
    unsigned int price = gps->getLevelupPrice((int)lev + 1, (int)memberCnt);
    if (m_guildDB.m_fundMoney < price)                       // +0x8d0c0
        return 0x16;
    priceOut = price;
    unsigned int needFund = *(unsigned int*)(param + 4);
    STGuildDBInfoOnly* db = (STGuildDBInfoOnly*)GetGuildDBInfo();
    if (db->m_fundExp < needFund)     // ORIG +0x29 字段
        return 0x54;
    return 0;
}

// >>> W7-BLOCKED CUser::checkHumanCertify —— m_capchaCertifyAns/m_cleanPadVerifyFlag 偏移未建模
// ---------------------------------------------------------------------------
// 39) CUser::checkHumanCertify(int len, unsigned short* answer)
//     报告：docs/class_func_reports/CUser/checkHumanCertify.md。ORIG 0x0867f4c8。
//     人证码段：m_capchaCertifyAns[5](ushort×5)@+0x8e0ec、失败计数 m_humanCertifyErrorCnt@+0x8e0fc、
//     清屏验证标志 char@+0x8e3ed（fields_A 建议 m_cleanPadVerifyFlag）。
// ---------------------------------------------------------------------------
// bool CUser::checkHumanCertify(int len, unsigned short* answer)
// {
//     static const char* PRETTY = "bool CUser::checkHumanCertify(int, short unsigned int*)";
//     unsigned short o5 = m_capchaCertifyAns[4], o4 = m_capchaCertifyAns[3],
//                    o3 = m_capchaCertifyAns[2], o2 = m_capchaCertifyAns[1],
//                    o1 = m_capchaCertifyAns[0];
//     unsigned short a1 = answer[0], a2 = answer[1], a3 = answer[2], a4 = answer[3],
//                    a5 = answer[4];
//     char* mid = NumberToString(get_acc_id(), 0);
//     cMyTrace trace(PRETTY, 0x7e07, 0);
//     trace("checkHumanCertify m_id:%s, orig:%x%x%x%x%x, ans:%x%x%x%x%x",
//           mid, o1, o2, o3, o4, o5, a1, a2, a3, a4, a5);
//     DB_CountOfHumanCertify::makeRequest(get_acc_id());
//     if (memcmp(answer, m_capchaCertifyAns, len) == 0)
//     {
//         if (m_humanCertifyErrorCnt != 0)
//             DB_ForceSaveSimpleSSO::makeRequest(get_acc_id(), 0);
//         m_humanCertifyErrorCnt = 0;
//         setHumanCertified(true);
//         return true;
//     }
//     m_humanCertifyErrorCnt++;
//     bool forceFail = (isCleanPadVerifyLimit() && m_cleanPadVerifyFlag != 0);
//     if (forceFail)
//     {
//         DB_ForceSaveSimpleSSO::makeRequest(get_acc_id(), m_humanCertifyErrorCnt);
//         SendCmdErrorPacket((ENUM_CMDPACKET)0x91, 8);
//     }
//     setHumanCertified(true);
//     return false;
// }

// ---------------------------------------------------------------------------
// 40) CUser::checkInformNoticeFlag(uint bitIdx, uint byteIdx)
//     报告：docs/class_func_reports/CUser/checkInformNoticeFlag.md。ORIG 0x0868f9e8。
//     位表 @+0x8ebb1（uchar[16]，bit 由低到高）。返回 1 = 该位未置。
// ---------------------------------------------------------------------------
bool CUser::checkInformNoticeFlag(unsigned int bitIdx, unsigned int byteIdx)
{
    if (bitIdx > 7) return false;
    if (byteIdx > 15) return false;
    return !((m_informNoticeFlags[byteIdx] >> bitIdx) & 1);
}

bool CUser::CheckItemLockSlotSpace(int spaceType, int spaceIdx) const
{
    // ORIG 0x086469f0（报告 CheckItemLockSlotSpace.md 全文转写）。
    // 链：getCurCharacR → GetCurCharacInventoryRef(00680c8a) → m_amp.GetLock →
    //     GetCharacExpandDataR(2)=item_lock::CItemLock → CheckItemLock(lock 编号)。
    if (getCurCharacR() == 0)
        return false;
    const Inven_Item* slot = GetCurCharacInventoryRef((unsigned short)spaceType,
                                                      (unsigned short)spaceIdx);
    if (slot == 0)
        return false;
    unsigned char lockNo = slot->m_amp.GetLock();
    if (lockNo == 0)
        return false;
    item_lock::CItemLock* lock =
        (item_lock::CItemLock*)GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2);
    return lock->CheckItemLock(lockNo);
}

// ---------------------------------------------------------------------------
// 42) CUser::CheckLimitMiniumAge(int ageLimit, int& uniqueLow16)
//     报告：docs/class_func_reports/CUser/CheckLimitMiniumAge.md。ORIG 0x0864fc3e。
// ---------------------------------------------------------------------------
bool CUser::CheckLimitMiniumAge(int ageLimit, int& outUnique)
{
    int age = GetAge();
    if ((unsigned int)(age - 1) < (unsigned int)ageLimit)
    {
        outUnique = (int)(get_unique_id() & 0xffff);
        return false;
    }
    if (age - 1 == ageLimit)
    {
        if (!IsOverBirthDay())
        {
            outUnique = (int)(get_unique_id() & 0xffff);
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// 43) CUser::CheckPossibleRequestRenameState(int slot) const
//     【报告缺失@0x0822ff62 size 0x60】objdump 全文转写。
//     slot 越界(<0 或 >=m_characList.size()) → false；
//     否则返回 m_characList[slot].m_viewState(+0x14c6)==0xff。
// ---------------------------------------------------------------------------
bool CUser::CheckPossibleRequestRenameState(int slot) const
{
    if (slot < 0)
        return false;
    if ((unsigned int)m_characList.size() <= (unsigned int)slot)
        return false;
    return m_characList[slot].m_viewState == 0xff;
}

// ---------------------------------------------------------------------------
// 44) CUser::checkSelectCharac(int slot)
//     报告：docs/class_func_reports/CUser/checkSelectCharac.md。ORIG 0x0864aaf6。
// ---------------------------------------------------------------------------
int CUser::checkSelectCharac(int slot)
{
    static const char* PRETTY = "bool CUser::checkSelectCharac(int)";
    if (getCurCharacR() != 0)
    {
        cMyTrace trace(PRETTY, 0xea4, 5);
        trace("CUser::SelectCharac m_selected is not null");
        return false;
    }
    if (slot < 0 || (int)m_characList.size() <= slot)
        return false;
    if (G_GameWorld()->IsPVPChannel() == false)
        return true;
    short lvl = m_characList[slot].m_level;
    if (lvl >= (short)G_CDataManager()->GetMinLevelPvPChannel()
        && lvl > (short)G_CDataManager()->GetMaxLevelPvPChannel())
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// 45) CUser::CheckTicket(int ticketIdx)
//     报告：docs/class_func_reports/CUser/CheckTicket.md。ORIG 0x086785f4。
// ---------------------------------------------------------------------------
bool CUser::CheckTicket(int ticketIdx)
{
    if (m_clientSpec.m_gmUserFlag != 0)
        return true;
    if (G_GameWorld()->IsPVPChannel())
        return true;
    for (std::list<int>::const_iterator it = m_list8cf80.begin();
         it != m_list8cf80.end(); ++it)
    {
        if (*it == ticketIdx)
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// 46) CUser::ClearChildrensDayEvent() 【报告缺失@0x0832815a size 0x63】objdump 全文转写。
//     标志 @+0x8e098 清零后，将 +0x23824/4 与 +0x23830/4 两表（每项 stride 4，
//     固定内偏移 0xc / 0x4 处的 int）置 -1，i = 0..9（cmpl $9; setle 循环界）。
// ---------------------------------------------------------------------------
void CUser::ClearChildrensDayEvent()
{
    if (m_childrensDayEventState == 0)
        return;
    m_childrensDayEventState = 0;
    for (int i = 0; i <= 9; i++)
    {
        m_childrensDaySlots[i] = -1;
        m_childrensDayPeriods[i] = -1;
    }
}

// ---------------------------------------------------------------------------
// 47) CUser::ClearMission(short missionId)
//     报告：docs/class_func_reports/CUser/ClearMission.md。ORIG 0x08662606。
// ---------------------------------------------------------------------------
void CUser::ClearMission(short missionId)
{
    if (isGMUser())
    {
        CMissionList_Charac* ml = (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        ml->ClearMission(*this, missionId);
    }
}

// ---------------------------------------------------------------------------
// 48) CUser::ClearMissionKind(short kind)
//     报告：docs/class_func_reports/CUser/ClearMissionKind.md。ORIG 0x086626a0。
// ---------------------------------------------------------------------------
void CUser::ClearMissionKind(short kind)
{
    CMissionList_Charac* ml = (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    ml->ClearMissionKind(*this, kind);
}

// ---------------------------------------------------------------------------
// 49) CUser::ClearSkills()
//     报告：docs/class_func_reports/CUser/ClearSkills.md。ORIG 0x0867b4c4。
//     条件（xor $1 后 jne 双取反 ⇒「必须同时不成立」）：
//       flag(+0x711d8)==0 且 getCurCharacR()!=0 → 返回 false；
//       否则 SkillSlotW.debugCommandResetSkill() 并返回 true。
// ---------------------------------------------------------------------------
bool CUser::ClearSkills()
{
    bool block = (m_clientSpec.m_gmUserFlag != 0) && (getCurCharacR() != 0);   // Client_Spec+0xbfc → CUser+0x711d8
    if (!block)
    {
        ((SkillSlot*)getCurCharacSkillW())->debugCommandResetSkill();
        return true;
    }
    return false;
}

void CUser::collectPcRoomPlayTime()
{
    // 报告：docs/class_func_reports/CUser/collectPcRoomPlayTime.md。ORIG 0x0868f7ba。
    int now = GlobalData::s_systemTime_.getCurSec();
    m_totalPcRoomPlayTime += now - (int)m_pcRoomPlayStartTime;   // +0x8eba0 / +0x8eb9c
    m_pcRoomPlayStartTime = (unsigned int)now;
}
//
//
// ============================================================================
// body_B.cpp —— 批次B（OnlyOrig_CUser.CuBatchB）50 符号可粘贴函数体草稿
// 证据：docs/class_func_reports/CUser/*.md 汇编+反编译 C 双段全文；
//       报告缺失符号以 nm -S 定址 + objdump 反汇编取证（meta 中标注）。
// 规则：source/toolchain/game_verify_guide.md §3/§4；asm_translation_patterns P1-P63。
// ============================================================================
//
// >>> W7-BLOCKED CUser::Competition_Mercenary_Charac —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 1. CUser::Competition_Mercenary_Charac(unsigned int, char, char)  T 0x0868e6c0
// 报告：CUser/Competition_Mercenary_Charac.md
// void CUser::Competition_Mercenary_Charac(unsigned int slot, char modeA, char modeB)
// {
    // ORIG: UserMercenaryInfoMgr::CompetitionCharac(this+0x6ef6c, this, slot, modeA, modeB)
//     m_mercenaryMgr.CompetitionCharac(this, slot, modeA, modeB);
// }
//
// >>> W7-BLOCKED CUser::complete_reward_list —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 2. CUser::complete_reward_list(vector<pair<int,int>>&, vector<pair<int,int>>&) T 0x08664dce
// 报告：CUser/complete_reward_list.md。双重 while 循环，first 相同则扣 second。
// void CUser::complete_reward_list(std::vector<std::pair<int,int> >& needList,
//                                  std::vector<std::pair<int,int> >& gotList)
// {
//     unsigned int i = 0;
//     while (i < needList.size()) {
//         unsigned int j = 0;
//         while (j < gotList.size()) {
//             if (needList[i].first == gotList[j].first) {
//                 needList[i].second = needList[i].second - gotList[j].second;
//             }
//             j = j + 1;
//         }
//         i = i + 1;
//     }
// }
//
// >>> W7-BLOCKED CUser::compound_item —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 3. CUser::compound_item(int, bool, unsigned short, int) T 0x0867218c
// 报告：CUser/compound_item.md（1397 行全文已读）。合成主链。
// int CUser::compound_item(int itemIdx, bool useRecipe2ndCheck, unsigned short count, int subCmd)
// {
//     static const float kHundred = 100.0f;   // ORIG _DAT_08cf3954 = 100.0f（flds 实证）
//
//     int recipeKeyOut = 0;                                        // -0x7c（_GetRecipeItem 出参）
//     CStackableItem* recipe =
//         (CStackableItem*)_GetRecipeItem((unsigned long)itemIdx, useRecipe2ndCheck, recipeKeyOut); /*head 返回 CItem* */
//     if (recipe == NULL) {
//         return 0x11;
//     }
//
//     if (is_recipe_items_avatar_type(*(const std::vector<std::pair<int,int> >*)((char*)recipe + 0x174))) {
//         return process_recipe_avatar_items(itemIdx, useRecipe2ndCheck, count, subCmd, recipe);
//     }
//
//     if (!_CheckCompoundSkill(recipe, useRecipe2ndCheck)) {      // xor $1/test → !result
//         return 0x16;
//     }
//
//     int blankSlots[11];                                         // -0x120 char[0x2c]={0} P4
//     memset(blankSlots, 0, sizeof(blankSlots));
//     int needSlots[11];                                          // -0x14c 清零
//     memset(needSlots, 0, sizeof(needSlots));
//
//     CInventory* inven = const_cast<CInventory*>(getCurCharacInvenR());
//     int freeQuick = inven->QuickEmptyCount();                   // -0x54
//
//     int keepSlots[11];                                          // -0x178 清零
//     memset(keepSlots, 0, sizeof(keepSlots));
//
//     std::vector<std::pair<int,int> > delList(*(std::vector<std::pair<int,int> >*)((char*)recipe + 0x174));   // -0x88 ← +0x174
//     std::vector<std::pair<int,int> > resultList(                    // -0x94
//         *(std::vector<std::pair<int,int> >*)((char*)recipe + 0x180));
//     if (!delList.empty()) {                                     // ORIG empty()==0 → insert 自身复制
//         delList.insert(delList.end(), delList.begin(), delList.end());
//     }
//     std::vector<std::pair<int,int> > delListCopy(delList);      // -0xa0
//
//     if (subCmd == 0xf2) {
//         CalculateExpertJobMaterials(delListCopy);
//     }
//
//     std::vector<std::pair<int,int> > resultMulList(             // -0xa0 ← +0x180 材料表
//         *(std::vector<std::pair<int,int> >*)((char*)recipe + 0x180));
//     _CalcItemVectorMultiply(delListCopy, (int)count);
//     _CalcItemVectorMultiply(resultMulList, (int)count);
//
//     if (_IsCompoundResult2MoreEquip(resultMulList)) {           // Ghidra 参数误写 this；汇编 -0xa0
//         return 0x17;
//     }
//
//     if (!inven->CheckNeedItemList(resultMulList)) {
//         return 0x15;
//     }
//
//     stEquipCondition condObj;                                   // -0xa8 (-0xa8/-0xa4 两 byte 字段)
//     Inven_Item condItem;                                        // -0x1b5
//     int checkRet = inven->CheckEquipmentItemCondition(resultMulList, condObj, condItem);
//     if (checkRet != 0) {
//         return checkRet;
//     }
//
//     inven->CalBlankSlot(blankSlots, resultMulList);
//     if ((!useRecipe2ndCheck) && (recipeKeyOut == 1)) {
//         blankSlots[2] = blankSlots[2] + 1;                      // +0x118 → blankSlots+8？ORIG -0x118 对应 blankSlots[2]
//     }
//     inven->CalNeedSlot(needSlots, delList);
//
//     freeQuick = freeQuick + blankSlots[0];
//     for (int si = 1; si <= 10; ++si) {
//         if ((keepSlots[si] + blankSlots[si]) < needSlots[si]) {
//             freeQuick = freeQuick + blankSlots[si] + (keepSlots[si] - needSlots[si]);
//             if (freeQuick < 0) {
//                 return 4;
//             }
//         }
//     }
//
//     CInventory* invW = const_cast<CInventory*>(getCurCharacInvenR());                     // -0x4c
//     STItemScript::SEXPERTJOB::stExpertCompoundResultVariation variation;  // ctor T 0x850d0e8
//     int jobExp = GetCurCharacExpertJobExp((int)jobExp /*GetCurCharacExpertJobExp(exp)*/,
//                                           0);                    // 占位——见下方注释
    // 注：ORIG 序列 = GetCurExpertJobLevel(GetCurCharacExpertJobExp(user))，
    // 上行仅示意；实现期以下列真实两调用为准：
//     int expVal = GetCurCharacExpertJobType();                    // GetCurCharacExpertJobExp 相邻字段访问
//     (void)expVal;
//
//     bool limitStackHit = false;                                  // local_49(-0x45)
//     CInventory shadowInv;                                        // -0x9ac
//     InventoryMemory shadowMem;                                   // -0xcaeb(局部大块)
//     shadowInv.SetInventoryMemory(&shadowMem);
//     shadowInv.setCopy(*getCurCharacInvenR());
//
//     bool consumedAll = true;                                     // esi=1 判定见后
//     if (shadowInv.tryDeleteEventItems(delListCopy)) {            // 失败返真→错误码
//         return 0x11;
//     }
//     if (!useRecipe2ndCheck) {
        // delete base/primary item from shadow inventory
//         if (!shadowInv.delete_item((INVEN_TYPE)1, itemIdx, count, eItemDelReason(8), false)) {
//             return 0x11;
//         }
//     }
//     else {
//         if (!shadowInv.delete_item((INVEN_TYPE)1, itemIdx, (int)(unsigned short)0 /*slot 由 ORIG 参1 = ebp-4 路径*/ ,
//                                    eItemDelReason(8), false)) {
//             return 0x11;
//         }
//     }
//
//     bool hasSuccessChance = (((volatile float&)variation.m_fieldAc) > 0.0f) || true;   // 见下
    // ORIG：flds -0xac(%ebp)（variation 第一 float）与 0 比较；<=0 时跳 resultMult 合成段。
//     if (variation.firstBonusRate() <= 0.0f) {
//         if (shadowInv.tryInsertEventItems(resultMulList)) {
//             consumedAll = true;                                   // esi=1
//         }
//         else {
            // ebx=4, error path
//             consumedAll = false;
            // 注意：此处 ORIG 直接返回 4 的路径在 ~CInventory(shadowInv) 之后统一收口
//         }
//     }
//     else {
//         std::vector<std::pair<int,int> > scaled(resultMulList);   // -0xd8
//         std::vector<std::pair<int,int> >::iterator it;
//         for (it = scaled.begin(); it != scaled.end(); ++it) {     // P50 直构
//             CItem* defIt = G_CDataManager()->find_item(it->first);
//             if (defIt != NULL) {
//                 if (defIt->is_stackable()) {
//                     if (((CStackableItem*)defIt)->getStackableLimit() == 1)
//                         limitStackHit = true;
//                 } else {
//                     limitStackHit = true;
//                 }
//             }
//             if (!limitStackHit) {
//                 it->second = (int)((float)variation.totalCount * (kHundred / kHundred)) *
//                              it->second / (int)kHundred;    // 见下精确式
                // 精确还原（ORIG 0x86727a7..0x86727f9）：
                // eax = pair.second(pair.second 原值)；roundUp = (int)((float)varFloat / 100.f 截断)；
                // eax = roundUp * 原second → 存回 pair.second
                // —— 即：pair.second = (int)(variation.ratioFloat) * it->second
//             }
//         }
//         if (!shadowInv.tryInsertEventItems(scaled)) {
//             if (limitStackHit) { /* ebx=4 */ consumedAll=false; }
//             else               { /* ebx=100 错误码 */ consumedAll=false; }
//         } else {
//             consumedAll = true;
//         }
//     }
//
    // 此处代码被截断保护——完整版由集成者按报告续接：
    // PacketGuard pkt; put_header(pkt,1,subCmd); put_byte(pkt,1);
    // if (recipe->field199byte) {...成功循环 IsSuccess(...) 统计 expertJob 成功数...}
    // vector<int> updatedSlots; clear;
    // if (!_ProcCompoundCore(delList,resultMulList,updatedSlots,itemIdx,
    //     useRecipe2ndCheck,pkt,bSendEquip,(unsigned short)count,condObj,condItem?…))
    //     …失败 return 0x11;
    // finalize/send/SendUpdateItem/send_equip(0x16)/_onCompoundItem/
    // GainItemMakingSkillPoint/WriteComposition；
    // 各出口前依栈逆序析构 local_a4/local_98/local_88。
//     return compound_item_tail(itemIdx, useRecipe2ndCheck, count, subCmd,
//                               recipe, delList, delListCopy, resultMulList, resultMulList,
//                               condItem);
// }
// 【推断标注】上行 compound_item 尾段聚合签名为转写辅助，落地时按上注直接内联展开，
// 不要新增 helper（Ghidra 无此符号）。0x867218c..0x867306d 全区间指令序已在报告中取证。

// ---------------------------------------------------------------------------
// 4. CUser::compoundGuildMessage(Inven_Item const&) T 0x0869304e
void CUser::compoundGuildMessage(Inven_Item const& item)
{
    char buf[0x78];
    memset(buf, 0, sizeof(buf));                                // rep stos 0x1e*4
    int key = item.getKey();
    CItem* def = G_CDataManager()->find_item(key);
    if (def != NULL) {
        strcpy(buf, getCurCharacName());
        sprintf(buf, "%s님이 +%d %s 조합에 성공 하였습니다.",
                getCurCharacName(), (unsigned)item.GetUpgrade() & 0xff, def->GetItemName());
        unsigned int gkey = get_charac_guildkey();
        unsigned int cno = getCurCharacNo();
        unsigned int sg = GetServerGroup();
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)sg);
        proxy->SendGuildBoardWrite(buf, 0u, cno, -1, (char)-1, gkey);
    }
}

// ---------------------------------------------------------------------------
// 5. CUser::compoundSystemMessage(Inven_Item const&) T 0x08692f62
void CUser::compoundSystemMessage(Inven_Item const& item)
{
    PacketGuard pkt;
    pkt.put_header( 0, 0x56);
    pkt.put_byte( 0xc);
    pkt.put_short( (unsigned)get_unique_id() & 0xffff);
    pkt.put_int( item.getKey());
    pkt.put_byte( (unsigned)item.GetUpgrade() & 0xff);
    pkt.finalize( true);
    G_GameWorld()->send_all(pkt);
}

// >>> W7-BLOCKED CUser::compute_level_penalty —— CDataManager +0x8ce0/+0x8ce4 属 QuestParameterScript(+0x18/+0x1c)内部未具名
// ---------------------------------------------------------------------------
// 6. CUser::compute_level_penalty(int) T 0x0866e3a8
// int CUser::compute_level_penalty(int levelDelta)
// {
//     if ((levelDelta < 7) || (0xb < levelDelta)) {
//         if (levelDelta < 0xc) {
//             return 100;
//         }
//         return G_CDataManager()->m_field8ce4;
//     }
//     return G_CDataManager()->m_field8ce0;
// }

// >>> W7-BLOCKED CUser::compute_quest_basic_reward_exp —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 7. CUser::compute_quest_basic_reward_exp(Quest*) T 0x0866e3e2
// int CUser::compute_quest_basic_reward_exp(Quest* quest)
// {
//     static const float kHundredF = 100.0f;                      // _DAT_08cf3954
//     if (quest->m_type == 4) {
//         return 0;
//     }
//     int levelDelta = get_charac_level() - quest->m_levelMin;    // +0x20
//     int penalty = 100;
//     if (quest->m_field130 != 0) {                               // movzbl/xor1/test-je 语义
//         penalty = compute_level_penalty(levelDelta);
//     }
//     QuestParameterScript& qp = *(QuestParameterScript*)((char*)G_CDataManager() + 0x8cc8);
//     int weight = qp.getRewardWeight((char)quest->m_field102);
//     int expBase = 0;
//     if ((quest->m_field101 == 0) && (quest->m_field130 == 0)) {
//         expBase = qp.getRewardExp(quest->m_levelMin);
//     }
//     else {
//         expBase = qp.getRewardExp(get_charac_level());
//     }
//     expBase = (((expBase * weight) / 100) * penalty) / 100;     // imul 0x51eb851f 除法组
//     int premiumBonus = 0;
//     if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x4f)) {
//         int rate = getAddExpRateFromPremium((ENUM_PREMIUM_TYPE)0x4f);
//         premiumBonus = setPremiumBonusExp((unsigned)expBase, rate);
//     }
//     STExpReward st;
//
//     get_growth_power_exp_reward(st);
//     unsigned char powByte = ((const unsigned char*)&st)[3];     // -0x25 读 +3 字节
//     short powShort = (short)(unsigned)powByte;                  // filds 有符号16位读
//     float ratio = (float)powShort / kHundredF;                  // flds/fdivrp
//     int seriaBuf = (int)((float)expBase * ratio);               // fnstcw chop → 截断转换
//     set_charac_seria_buf_bonus_exp(seriaBuf);
//     return expBase + premiumBonus + seriaBuf;
// }

// 8. CUser::compute_quest_basic_reward_gold：唯一 owner 为
//    LinkClosureA_User.cpp（W7_BuildHarmonize 去重；该版 m_field130==1
//    分支与 ORIG xor1/test 语义精确一致）。
// ---------------------------------------------------------------------------
// 9. CUser::copyItemOption(Inven_Item&, Inven_Item&) T 0x08671eb2
void CUser::copyItemOption(Inven_Item& dst, Inven_Item& src)
{
    dst.SetUpgrade(src.GetUpgrade());
    dst.set_add_info(src.get_add_info());
    dst.m_upgradeSep.m_field = (dst.m_upgradeSep.m_field & 0xe0) | (src.m_upgradeSep.m_field & 0x1f);
    dst.m_amp = src.m_amp;                              // +0x11 int
    dst.m_fieldd = src.m_fieldd;                  // +0xd int
    const stAmplifyOption_t& amp = dst.m_amp;                       // stAmplifyOption_t 视图 @+0x11
    LogHistory("Compound+,%u,%d,%d,%d,%d,%d,%d,%d,",
               *(unsigned int*)&dst.m_addInfo,               // +2 uint
               (unsigned)dst.GetUpgrade() & 0xff,
               dst.get_add_info(),
               (unsigned)(dst.m_upgradeSep.m_field & 0x1f),
               dst.m_fieldd,
               (unsigned)amp.getAbilityType() & 0xff,
               (unsigned)amp.getAbilityValue() & 0xffff,
               (unsigned)amp.GetLock() & 0xff);
}

// ---------------------------------------------------------------------------
// 10. CUser::count_specific_items(vector<int>&, vector<pair<int,int>>&) T 0x0866514a
void CUser::count_specific_items(std::vector<int>& ids, std::vector<std::pair<int,int> >& out)
{
    CInventory* inv = const_cast<CInventory*>(getCurCharacInvenR());
    CCargo* cargo = const_cast<CCargo*>(getCurCharacCargoR());
    int total = 0;
    Inven_Item probe;
    // Inven_Item::Inven_Item 0x80cb854
    unsigned int i = 0;
    while (i < ids.size()) {
        total = 0;
        if (inv->GetInvenData(ids[i], probe) != -1)
            total += probe.m_addInfo2;                       // probe+7 int（fields 建议）
        if (cargo->GetCargoData(ids[i], probe) != -1)
            total += probe.m_addInfo2;
        if (total > 0) {
            out.push_back(std::make_pair(ids[i], total));       // make_pair<int&,int&> 实参序实证 first=ids[i]
        }
        ++i;
    }
}

// >>> W7-BLOCKED CUser::CreateCharac —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 11. CUser::CreateCharac(SIG_CREATE_CHARAC*) T 0x0864a186（CUser.h 已有声明）
// int CUser::CreateCharac(SIG_CREATE_CHARAC* sig)
// {
//     _Charac_info info;                                          // 局部 0x14e9 字节, 0x14fc 帧
//     memset(&info, 0, sizeof(info)); /*推断:ORIG Reset 无符号，局部清零*/ info.m_characNo = *(unsigned int*)((char*)sig + 0x5348);
//     strncpy(info.m_name, (const char*)sig + 4, 0x1e);
//     *(unsigned char*)((char*)&info + 0x22) = 1;      // -0x14c7 +0x22
//     *(unsigned char*)((char*)&info + 0x23) = 2;      // -0x14c6 +0x23
//     *(unsigned char*)((char*)&info + 0x24) = 0;      // -0x14c5 +0x24
//     *(unsigned char*)((char*)&info + 0x25) = 0;      // -0x14c4 +0x25
//     info.m_field26 = (char)sig->m_slot;                      // -0x14c3 +0x26 = msg+0x22
//     memcpy((char*)&info + 0x88,
           // -0x1461 +0x88
//            ((char*)G_CDataManager()->m_characList /*+0x14*/ + (signed char)sig->m_slot * 0x7dc,
//            0x52);
//     *(short*)((char*)&info + 0x27) = 1;                         // movw $1,-0x14c2
//     *(unsigned int*)((char*)&info + 0x2b) = 0;                  // movl $0,-0x14be
//     *(unsigned char*)((char*)&info + 0x29) = 0;                 // movb $0,-0x14c0
//     *(unsigned char*)((char*)&info + 0x75) = 100;               // movb $100,-0x1474
//     CommonTime ct;
//     ct.SetCurTime();                                            // 0x82a4224 写入 info+0x39
//     memcpy(((char*)&info) + 0x39, &ct, sizeof(CommonTime));
//     *(short*)((char*)&info + 0x33) = 0;                         // movw $0,-0x14b6
//     *(unsigned short*)((char*)&info + 0x35) = DEFAULT_MAX_FATIGUE; // mov &DEFAULT_MAX_FATIGUE,%ax
//     *(short*)((char*)&info + 0x37) = 0;                         // movw $0,-0x14b2
//     *(short*)((char*)&info + 0x4f) = 0;                         // movw $0,-0x149a
//     *(unsigned char*)((char*)&info + 0x83) = 0;                 // movb $0,-0x1466
//     *(unsigned int*)((char*)&info + 0x84) = 0;                  // movl $0,-0x1465
//     *(unsigned int*)((char*)&info + 0xdf4) = 0;                 // movl $0,-0x6f5
//     *(unsigned int*)((char*)&info + 0xdf8) = 0;                 // movl $0,-0x6f1
//     *(unsigned char*)((char*)&info + 0xe8f) = 0;                // movb $0,-0x65a
//     *(unsigned char*)((char*)&info + 0x1239) = 0xff;            // movb $0xff,-0x2b0
//     *(unsigned char*)((char*)&info + 0x1240) =
//         (sig->m_eventCharacFlag != 0) ? 1 : 0;                          // setne
//     m_inventoryMemory.reset();     
//     CInventory tmpInv;                                          // aCStack_1413
//     CCargo tmpCargo;                                            // aCStack_743
//     tmpInv.SetInventoryMemory(&m_inventoryMemory);              // 两次均用 user+0x79794
//     tmpCargo.SetInventoryMemory(&m_inventoryMemory);
//     m_characList.push_back(info);
//     m_historyLog.CharacAdd(m_characList.size() - 1);
//     m_characterView.enableSaveCharacView();
//     _onCreateCharacter(*(unsigned int*)((char*)sig + 0x5348), (char*)sig + 4,
//                        (signed char)sig->m_slot);
//     return 0;
// }
// 【推断标注】_Charac_info 内部字节布局字段（m_pad22 等）以现仓 _Charac_info.h 具名成员为准逐一对位；
// CreateCharac 结构体内偏移全部取自 ORIG disp（-0x14e9 基址换算），无臆测。
//
// >>> W7-BLOCKED CUser::customizingSkillCommand —— 命令表 getter 未建模
// ---------------------------------------------------------------------------
// 12. CUser::customizingSkillCommand(STSkillCommandData&) T 0x08654f74
// int CUser::customizingSkillCommand(STSkillCommandData& data)
// {
//     std::map<int, STSkillCommandData>* cmds = getSkillCommand();
//     if (cmds == NULL) {
//         return 0;
//     }
//     int i = 0;
//     while ((signed char)data.m_count > i) {
//         if (SkillCmdKey(data, i) == 8) {
//             SkillCmdKey(data, i) = 7;
//         }
//         i++;
//     }
//     std::map<int, STSkillCommandData>::iterator it = cmds->find((int)data.m_command);
//     if (it != cmds->end()) {
//         it->second.m_command = data.m_command;
//         it->second.m_count = data.m_count;
//         int j = 0;
//         while ((signed char)data.m_count > j) {
//             SkillCmdKey(it->second, j) = SkillCmdKey(data, j);
//             j++;
//         }
//     }
//     else {
//         cmds->insert(
//             std::pair<int, STSkillCommandData>((int)data.m_command, data));
//     }
//     enableSaveSkill();
//     return 1;
// }
//
// >>> W7-BLOCKED CUser::decide_growth_power_reward_system —— STGrowthPowerData.breakAwaySectionByte 与 CDataManager::growthPower(sret)未建模；get_growth_power_exp_reward ORIG 无参
// ---------------------------------------------------------------------------
// 13. CUser::decide_growth_power_reward_system() T 0x0868d780
// void CUser::decide_growth_power_reward_system()
// {
//     char rewardFlag = 0;                     // -0xe
//     char seriaFlag = 0;                      // -0xd
//     CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
//     if (!ev->IsAvailableSlot34(0)) {         // vtable+0x34 槽直调返回 byte（P5/Ghidra）
//         get_charac_level();
//         STGrowthPowerData gpd;
//         G_CDataManager()->m_stGrowthPower8710.get_break_away_section_data(&gpd);
//         unsigned char cur = get_growth_power_reward();
//         if (cur < gpd.breakAwaySectionByte) {
//             PacketGuard pkt;
//             InterfacePacketBuf::clear(pkt);
//             pkt.put_header( 0, 0x178);
//             pkt.put_byte( get_charac_level());
//             pkt.put_byte( 1);
//             pkt.put_byte( 1);
//             pkt.finalize( true);
//             Send(pkt);
//             set_growth_power_exp_reward_right(true);
//         }
//         set_growth_power_exp_reward(get_charac_level());
//         return;
//     }
//     if (ev->IsAvailableSlot34(0)) {
//         int lv = get_charac_level();
//         stSeriaBlessingScript& sb = G_CDataManager()->m_seriaBlessing5094;
//         if (sb.is_possible_lv(lv)) {
//             seriaFlag = 1;
//         }
//     }
//     if (seriaFlag) {
//         set_growth_power_exp_reward_right(true);
//     }
//     get_charac_level();
//     STGrowthPowerData gpd2;
//     G_CDataManager()->m_stGrowthPower8710.get_break_away_section_data(&gpd2);
//     unsigned char cur2 = get_growth_power_reward();
//     if (cur2 < gpd2.breakAwaySectionByte) {
//         rewardFlag = 1;
//     }
//     if ((rewardFlag != 0) || (seriaFlag != 0)) {
//         PacketGuard pkt;
//         try {
//             InterfacePacketBuf::clear(pkt);
//             pkt.put_header( 0, 0x178);
//             pkt.put_byte( get_charac_level());
//             pkt.put_byte( (rewardFlag != 0) ? 1 : 0);
//             pkt.put_byte( (seriaFlag != 0) ? 1 : 0);
//             pkt.finalize( true);
//             Send(pkt);
//         } catch (...) { }
//     }
//     set_growth_power_exp_reward(get_charac_level());
// }
// 【推断】vtable+0x34 槽名与 CDataManager +0x8710/+0x5094 字段名待权威头落位时校准。

// ---------------------------------------------------------------------------
// 14. CUser::decideCharacLinkMessage() T 0x08652a58
void CUser::decideCharacLinkMessage()
{
    char flag;
    if ((flag = isCharacLinkMessageFlag()) == 0) {
        for (unsigned int i = 0;
             i < m_characList.size(); ++i) {
            _Charac_info* p = &m_characList[i];
            short lvl = (short)p->m_level;                 // movzwl/movswl +0x27
            bool doSend;
            if ((lvl < 0x3c) ||
                (flag = isCharacLinkMessageFlag(), flag == 1)) {   // P48 条件臂重复调用
                doSend = false;
            }
            else {
                doSend = true;
            }
            if (doSend) {
                sendCharacLinkMessage();
                saveCharacLinkMessageFlag(true);
                return;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// 15. CUser::decideLinkCharacSlotIndexes(char, int) T 0x08652b06
void CUser::decideLinkCharacSlotIndexes(char idx, int maxIdx)
{
    char linkType = getCharacLinkType();
    if ((idx < maxIdx) && (-1 < idx)) {
        if (linkType == 2) {
            setLinkCharacSlotIndex(idx + 1);
        }
        else if (linkType == 3) {
            setLinkCharacSlotIndex(idx - 1);
        }
        else {
            setLinkCharacSlotIndex(-1);
        }
    }
}

// >>> W7-BLOCKED CUser::defaultSkillCommand —— 命令表 getter 未建模
// ---------------------------------------------------------------------------
// 16. CUser::defaultSkillCommand() T 0x086550ee
// bool CUser::defaultSkillCommand()
// {
//     std::map<int, STSkillCommandData>* cmds = getSkillCommand();
//     bool isEmpty;
//     if ((cmds == NULL) || cmds->empty()) {
//         isEmpty = true;
//     }
//     else {
//         isEmpty = false;
//     }
//     if (!isEmpty) {
//         cmds->clear();
//         enableSaveSkill();
//     }
//     return !isEmpty;
// }

int CUser::DeleteCharac(unsigned int characNo)
{
    // ORIG 0x0864a63a（报告 DeleteCharac.md + objdump 逐指令核对）。
    char erased = 0;
    std::vector<_Charac_info>::iterator delIt = m_characList.begin();
    int i = 0;
    while ((int)m_characList.size() > i)
    {
        _Charac_info& ci = m_characList[i];
        if (ci.m_characNo == (int)characNo)
        {
            HistoryLog::WriteDeleteCh(reinterpret_cast<FILE*>(m_field796f8),
                                      ci.m_name, (int)ci.m_field26,
                                      (int)(short)ci.m_level, (int)ci.m_field29);
            m_historyLog.CharacDel(i);
            erased = 1;
        }
        if (ci.m_field14b8 == (int)characNo)                   // +0x14b8 striker link 槽
            ci.m_field14b8 = 0;
        if (erased != 1)
            ++delIt;
        ++i;
    }
    if (erased != 0)
    {
        m_characList.erase(delIt);
        CalcurateManageLevel();
    }
    m_characterView.enableSaveCharacView();
    return 0;
}

// ---------------------------------------------------------------------------
// 18. CUser::DeleteCurrentCharacRentalItemInfo(int) T 0x08677050
void CUser::DeleteCurrentCharacRentalItemInfo(int key)
{
    const _Charac_info* cur = getCurCharacR();
    if ((cur != NULL) && (cur->m_characNo != 0)) {
        unsigned int accId = cur->m_characNo;
        int findKey = key;
        typedef std::map<unsigned int,
                std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> > >
            RentalOuterMap;                                     // m_rentalInfo (+0x6ef78)
        RentalOuterMap& outer = *(RentalOuterMap*)&m_rentalInfo;
        RentalOuterMap::iterator it = outer.find(accId);
        if (it != outer.end()) {
            std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> >::iterator it2 =
                it->second.find(findKey);
            if (it2 != it->second.end()) {
                it->second.erase(it2);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// 19. CUser::deleteToBlackList(unsigned int) T 0x0867f040
int CUser::deleteToBlackList(unsigned int id)
{
    if (!m_vec8cef4.empty()) {
        // ORIG 0x0867f040：begin() 拷贝初始化 iterator，无默认构造（无 C1 符号）
        std::vector<unsigned int>::iterator it = m_vec8cef4.begin();
        for (; it != m_vec8cef4.end(); ++it) {
            if (*it == id) {
                m_vec8cef4.erase(it);
                return 1;
            }
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// 20. CUser::DelTradePunishType(int) T 0x086898c2
void CUser::DelTradePunishType(int mask)
{
    printf("DelTradePunishType : (%d) // Prev TradePunishType:  %d\n", mask, m_postChecksumData.m_tradePunishType);
    m_postChecksumData.m_tradePunishType = m_postChecksumData.m_tradePunishType & ~mask;
    printf("After TradePunishType:  %d\n", m_postChecksumData.m_tradePunishType);
}

// ---------------------------------------------------------------------------
// 21. CUser::discard_mouse_register() T 0x08680bd2
void CUser::discard_mouse_register()
{
    Sanicova::CPad* pad = getPad();
    char ver = pad->getVersionInfo();
    if (ver == 1) {
        PacketGuard pkt;
        pkt.put_header( 0, 0xab);
        pkt.put_byte( 1);
        pkt.finalize( true);
        Send(pkt);
    }
}

// ---------------------------------------------------------------------------
// 22. CUser::DoItemLock(int, int) T 0x08646d4a
void CUser::DoItemLock(int paramA, int paramB)
{
    item_lock::CItemLock* lock =
        (item_lock::CItemLock*)GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(2));
    lock->DoItemLock(this, paramA, paramB);
}

// 23. CUser::DoItemUnlock(int, int) T 0x08646d82
void CUser::DoItemUnlock(int paramA, int paramB)
{
    item_lock::CItemLock* lock =
        (item_lock::CItemLock*)GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(2));
    lock->DoItemUnlock(this, paramA, paramB);
}

// 24. CUser::DoItemUnlockCancel(int, int) T 0x08646dba
void CUser::DoItemUnlockCancel(int paramA, int paramB)
{
    item_lock::CItemLock* lock =
        (item_lock::CItemLock*)GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(2));
    lock->DoItemUnlockCancel(this, paramA, paramB);
}

// >>> W7-BLOCKED CUser::EnableCharacInfo —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 25. CUser::EnableCharacInfo(CACHE_SECTION) W 0x081253f4【报告缺失·objdump 0x81253f4】
// ORIG: movb $0x1, this+0x8d0fd+section
// void CUser::EnableCharacInfo(CACHE_SECTION section)
// {
//     m_pad8d0fd[(int)section] = 1;
// }
//
// 26. CUser::eventDeliveryQuestClear(int,int,vector<pair<int,int>>&,int,int,int) T 0x086645a4
// QuestClearDeliveryMsg 已建模（QuestClearDeliveryMsg.h；ctor ORIG W 0x08697914）。
QuestClearDeliveryMsg::QuestClearDeliveryMsg()
{
    m_questParamA = 0;       // +0x10
    m_paramD = 0;            // +0x14
    m_paramE = 0;            // +0x18
    m_paramF = 0;            // +0x1c
    m_itemsPtr = 0;          // +0x20
}

void CUser::eventDeliveryQuestClear(int a, int b,
                                    std::vector<std::pair<int,int> >& items,
                                    int d, int e, int f)
{
    QuestClearDeliveryMsg msg;                                  // C2 0x8697914
    int sg = GetServerGroupToPvP();
    int cno = getCurCharacNo();
    unsigned int accId = get_acc_id();
    msg.setBaseData(1, accId, cno, sg);                         // ORIG 0x86978ec
    msg.m_questParamA = a;                                      // +0x10  ← ebp+0xc
    msg.m_itemsPtr = &items;                                    // +0x20  ← ebp+0x14
    msg.m_paramD = d;                                           // +0x14  ← ebp+0x18
    msg.m_paramE = e;                                           // +0x18  ← ebp+0x1c
    msg.m_paramF = f;                                           // +0x1C  ← ebp+0x20
    GlobalData::s_event_manager->eventDeliveryNotify(msg.m_type, msg);
}
//
// >>> W7-BLOCKED CUser::findSkillCommandData —— CSkillList m_keyCommands124 hash_map 与 CSkill 完整类型未建模
// ---------------------------------------------------------------------------
// 27. CUser::findSkillCommandData(int, STSkillCommandData&) T 0x08654b94
// bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out)
// {
//     std::map<int, STSkillCommandData>* cmds = getSkillCommand();
//     if (cmds == NULL) {
//         return false;
//     }
//     std::map<int, STSkillCommandData>::iterator it = cmds->find(skillId);
//     if (it != cmds->end()) {
//         out.m_command = it->second.m_command;                   // movzwl +4
//         out.m_count = it->second.m_count;           // movzbl +6
//         int i = 0;
//         while ((signed char)out.m_count > i) {
//             SkillCmdKey(out, i) = SkillCmdKey(it->second, i);
//             if (SkillCmdKey(out, i) == 7) {
//                 SkillCmdKey(out, i) = 8;
//             }
//             i++;
//         }
//         return true;
//     }
    // miss → 技能表 fallback
//     int job = get_charac_job();
//     CSkillList* skillList = G_CDataManager()->m_skillList10->getSkillList(job);   // dm+0x10 CSkillList*
//     if (skillList == NULL) {
//         return false;
//     }
//     __gnu_cxx::hash_map<int, CSkill*>& keyMap = skillList->m_keyCommands124;     // +0x124
//     __gnu_cxx::hash_map<int, CSkill*>::iterator hit = keyMap.find(skillId);
//     if (hit == keyMap.end()) {
//         cMyTrace trace(__PRETTY_FUNCTION__,
//                        0x2113, 5);
//         trace("SKILL_COMMAND_LOG : KEY INDEX DUPLICATE!!");
//         return false;
//     }
//     CSkill* sk = hit->second;                                   // operator-> → +4
//     int skillKey = hit->first;                                  // operator-> → *it(+0)
//     int cnt = (int)sk->m_keyCommandUnits124.size();             // vector<STKeyCommandUnit>@+0x124
//     if ((cnt < 6) && (0 < cnt)) {
//         out.m_command = (short)skillKey;                        // local_18
//         out.m_count = (char)cnt;
//         for (unsigned int u = 0; u < (unsigned)cnt; ++u) {
//             SkillCmdKey(out, u) = (unsigned char)sk->m_keyCommandUnits124[u].values[0];
//             if (SkillCmdKey(out, u) == 7) {
//                 SkillCmdKey(out, u) = 8;
//             }
//         }
//         return true;
//     }
//     if (cnt != 0) {
//         cMyTrace trace(__PRETTY_FUNCTION__,
//                        0x2102, 5);
//         trace("SKILL_COMMAND_LOG : ABNOMAL COMMAND CNT!! (%d) (%d)", skillKey, cnt);
//     }
//     return false;
// }

// >>> W7-BLOCKED CUser::findSkillCommandData —— STSkillCommandData map 视图未建模
// ---------------------------------------------------------------------------
// 28. CUser::findSkillCommandData(int, STSkillCommandData&, bool) W/T 0x08654e6a
// 【报告缺失·objdump 全区间反汇编】纯 map 查找版：尾参 bool 未使用（溢出槽不回读）。
// int CUser::findSkillCommandData(int skillId, STSkillCommandData& out, bool flag)
// {
//     (void)flag;                                                 // -0x1c 仅存储
//     std::map<int, STSkillCommandData>* cmds = getSkillCommand();
//     if (cmds == NULL) {
//         return 0;
//     }
//     std::map<int, STSkillCommandData>::iterator it = cmds->find(skillId);
//     if (it == cmds->end()) {
//         return 0;
//     }
//     out.m_command = it->second.m_command;
//     out.m_count = it->second.m_count;
//     int i = 0;
//     while ((signed char)out.m_count > i) {
//         SkillCmdKey(out, i) = SkillCmdKey(it->second, i);
//         if (SkillCmdKey(out, i) == 7) {
//             SkillCmdKey(out, i) = 8;
//         }
//         i++;
//     }
//     return 1;
// }

// >>> W7-BLOCKED CUser::gain_exp_sp —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 29. CUser::gain_exp_sp(int,int&,int&,eExpAddReason,int,bool) T 0x0866a3fe
// bool CUser::gain_exp_sp(int exp, int& spRef, int& sfpRef,
//                         ::eExpAddReason reason, int subReason, bool chargeFlag)
// {
//     if ((*reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x1a8)) == 2) {                  // env+0x1a8
//         if (0x31 < get_charac_level()) {
//             exp = 0;
//         }
//     }
//     if (exp < 0) {
//         exp = 0;
//     }
//     if ((reason == eExpAddReason(1)) || (reason == eExpAddReason(2))) {
//         W7_CU_INT(0x8e068) = W7_CU_INT(0x8e068) + exp;                      // +0x8e068
//     }
//     W7_CU_INT(0x8e05c) = W7_CU_INT(0x8e05c) + exp;                          // +0x8e05c
//     incPlayExpAdd(exp);
//
//     char leveled = 0;                                           // -0x29
//     if (get_charac_level() <= 0x54) {                           // setle
//         leveled = (char)_check_level_up(exp, spRef, sfpRef, reason, subReason);
//         SkillSlot* sk = (SkillSlot*)getCurCharacSkillR();
//         int beforeSp = sk->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0);
//         gain_sp(spRef);
//         sk = (SkillSlot*)getCurCharacSkillR();                              // P48 二次独立调用
//         int beforeSfp = sk->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2);
//         gain_sfp(sfpRef);
//         history_log_sp(beforeSp, spRef, eSPAddReason(0));
//         history_log_sfp(beforeSfp, sfpRef, eSPAddReason(0));
//     }
//
//     PacketGuard pkt;
//     if (leveled == 0) {
//         return (chargeFlag != 0);
//     }
//     CalcurateUserMaxLevel();
//     int maxEquipLvl = GetCurCharacMaxEquipLevel();
//     bool atMax;
//     if (maxEquipLvl == 0) {
//         atMax = false;
//     }
//     else {
//         atMax = (get_charac_level() == GetCurCharacMaxEquipLevel());
//     }
//     if (atMax) {
//         SetCurCharacMaxEquipLevel(0);
//     }
//     getCurCharacQuestW()->ResetUrgentQuestWaitingList();
//     char questInfo[0x40];                                       // local buf
//     getCurCharacQuestR()->get_quest_info(questInfo);
//     m_characterView.enableSaveCharacView();
//     Send(pkt);
//     if (get_charac_level() == 0x55) {
//         resetCharacFatigueGrownUpBuff();
//     }
//     _UpdateUserInfo4Guild();
//     {
//         char growType = getCurCharacGrowType();                 // movsbl
//         short lv2 = (short)get_charac_level();                  // movswl
//         unsigned int cno = get_charac_no(-1);
//         unsigned int accId = get_acc_id();
//         unsigned int sg = GetServerGroup();
//         CGuildServerProxy* gp = CServerProxyMgr<CGuildServerProxy>::GetServerProxy(
//             GlobalData::s_guild_proxy_mgr, sg);
//         gp->SendCharLevelGrowType(accId, cno, lv2, growType);
//     }
//     {
//         char growType2 = getCurCharacGrowType();
//         short lv3 = (short)get_charac_level();
//         unsigned int cno2 = get_charac_no(-1);
//         unsigned int accId2 = get_acc_id();
//         unsigned int sg2 = GetServerGroup();
//         CMonitorServerProxy* mp = CServerProxyMgr<CMonitorServerProxy>::GetServerProxy(
//             GlobalData::s_monitor_proxy_mgr, sg2);
//         mp->SendCharLevelGrowType(accId2, cno2, lv3, growType2);
//     }
//     if (!VerifyPresentAvengerTitle()) {                         // xor1/test → !res
//         unsigned int curNo = getCurCharacNo();
//         char* idStr = NumberToString(get_acc_id(), 0);
//         cMyTrace trace(__PRETTY_FUNCTION__,
//                        0x5046, 0);
//         trace("CUser::VerifyPresentAvengerTitle() m_id(%s), charac_no(%u)", idStr, curNo);
//     }
//     AddCurCharacMercenaryInfo();
//
    // ---- fatigue/noti 区块（按 ORIG 0x86730? 汇编分支序）----
//     if ((m_chState == 5) || (m_chState == 8) || (m_chState == 7) ||
//         (m_chState == 10) || (m_chState == 12)) {
//         if (chargeFlag == 0) {                                   // +0x43a: xor1/test je
//             SendNotiPacket(eSendTarget(1), ENUM_NOTIPACKET(0x25), 0);
//             set_charac_fatigue_buf_bonus_exp(0);
//         }
//         else {
//             if (leveled != 0) {                                  // +0x479
//                 SendNotiPacket(eSendTarget(1), ENUM_NOTIPACKET(0x25), 0);
//             }
//         }
//         if (leveled != 0) {
//             decide_growth_power_reward_system();
//         }
//     }
//     if (leveled != 0) {
//         int playCnt = GetCurCharacDungeonPlayCount();
//         int lvlNow = get_charac_level();
//         CLevelDungeonPlayStatistic::IncreaseLevelDungeonPlay(GlobalData::s_statisticMgr,
//                                                              lvlNow, playCnt);
//         ResetCurCharacDungeonPlayCount();
//         int jobExp = GetCurCharacExpertJobExp();
//         expert_job::CExpertJob::IncreaseExpertJobExp(this, jobExp);
//         ReCalcChattingEmoticon();
//         SendChattingEmoticon();
//     }
//     if (leveled != 0) {
//         int groupIdx0 = 0x55;                                    // -0x20
//         int groupIdx1 = 0x50;                                    // -0x1c
//         (void)groupIdx0; (void)groupIdx1;                        // ORIG 帧冗余保留
//         if (get_charac_level() == 0x55) {
//             APSystem::CUserProc::ClearActionAndSendtoUser(this,
//                 APSystem::_ActionGroupIndex(0x21), 0, ENUM_PACKETCLASS(0));
//         }
//     }
//     return (leveled != 0);
// }
//
// >>> W7-BLOCKED CUser::gainMoneyFromHackUser —— CAutoPunishRuleHackTypeMgr 静态方法形态/DB hook 符号链未核实
// ---------------------------------------------------------------------------
// 30. CUser::gainMoneyFromHackUser(uint,ull,uint,ull) T 0x0867fbba
// void CUser::gainMoneyFromHackUser(unsigned int who,
//                                   unsigned long long gold,
//                                   unsigned int cnt,
//                                   unsigned long long extraGold)
// {
//     if (isLoadingHackGold()) {                                   // cVar1=='\1'
//         incTotalHackUserTradeGold(gold + extraGold);             // add/adc 64 位加法
//         incTotalHackUserTradeCnt(cnt);
//         unsigned int tradeCnt = getTotalHackUserTradeCnt();
//         unsigned long long tradeGold = getTotalHackUserTradeGold();
//         if (WongWork::CAutoPunishRuleHackTypeMgr_checkTotalTradeRuleOver(
//                 GlobalData::g_autoPunishRuleHackTypeMgr, tradeCnt, tradeGold)) {
//             SetTradePunishType(8);
//         }
//         bool punish = (isTradePunishType(8) != 0);
        // DB_InsertAutoPunishSecondUser::makeRequest ORIG _ZN29...Ejjyb 0x0842e34e
        // 栈形态(this,[p1],[p2.lo],[p2.hi],[bool]) 与 mangled(j,j,y,b) 差一参数位——
        // 调用点按 ORIG 目标签名书写，待 compare 校准第二 j 实参来源（疑似 p1 重传）。
//         DB_InsertAutoPunishSecondUser_makeRequest_hook(get_acc_id(), who, gold, punish);
        // 【勿直接启用上一行】宿主落地时应替换为:
        // DB_InsertAutoPunishSecondUser req; req.makeRequest(get_acc_id(), <j?>, gold, punish);
        // 其中 ORIG esp 栈序实测= [accId, gold(lo/hi), punish]，第二 j 需比对 ORIG dispatch 用法再定。
//     }
//     else {
//         setLoadHackGold(true);
//         unsigned int uid = GetUID();
//         unsigned long long totGold = gold + 0ULL;                // 并入下述请求
//         (void)totGold;
//         unsigned int accId = get_acc_id();
//         unsigned long long merged = (unsigned long long)(unsigned int)uid << 32;  // CONCAT44 占位
//         (void)merged;
        // DB_AutoPunishedSecondUserData::makeRequest(uint,ull,uint,uint) ORIG 0x0843a790
        // ORIG 栈序: obj,[gold.lo],[gold.hi],[uid ebx],[p3]。按 mangled Ejyjj 书写：
//         DB_AutoPunishedSecondUserData_makeRequest_hook(accId, gold, uid, who /*实际为 p3?*/,
//                                                        (unsigned)extraGold);
        // 【勿直接启用上一行】宿主落地时应替换为真实静态入口或对象方法——
        // ORIG 该调用以 ecx=get_acc_id() 作 this，示意一致：
        // DB_AutoPunishedSecondUserData dbq; dbq.makeRequest(uid, (ull)accId, who, hi(extraGold));
//     }
// }
// 备注：本函数两个 DB 请求的「对象来源=普通 thiscall 对象」与 ORIG ecx=get_acc_id()/GetUID()
// 返回值充当首参的 ABI 形态有出入——compare_game_full 定位后以 mangled 主线修正书写即可。

// ---------------------------------------------------------------------------
// 31. CUser::get_ingame_event_history(int, pair<int,int>&) T 0x0868d50c【报告缺失·无参版报告≠本符号】
void CUser::get_ingame_event_history(int key, std::pair<int,int>& out)
{
    std::map<int, std::pair<int,int> >& hist = m_map8eb68;   // +0x8eb68 具名成员
    std::map<int, std::pair<int,int> >::iterator it = hist.find(key);
    if (it != hist.end()) {
        out.second = it->second.second;                         // +0x8 先写（edx 槽）
        out.first = it->second.first;                           // +0x4 后写（eax 槽）
    }
}
// 无参版 Ev @0x868d4f0（报告在案）：return (this + 0x8eb68) 即上述 map 引用 getter。

// >>> W7-BLOCKED CUser::get_pvp_exp_of_week —— STPvpResult/getPVPResultRef 视图与 CServerProxyMgr 调用形态未建模
// ---------------------------------------------------------------------------
// 32. CUser::get_pvp_exp_of_week() W 0x08230100【报告缺失·objdump】
// int CUser::get_pvp_exp_of_week()
// {
//     STPvpResult* r = getPVPResultRef();                          // CUserCharacInfo::getPVPResultRefR
//     if (r->m_weekEndExp10 == -1) {
//         return r->m_weekStartExp8;
//     }
//     return r->m_weekStartExp8 - getPVPResultRef()->m_weekEndExp10;   // P48 双调
// }
//
// 33. CUser::get_skill_count() T 0x0866af86
// int CUser::get_skill_count()
// {
//     return ((SkillSlot*)getCurCharacSkillR())->get_skill_count();
// }
//
// 34. CUser::get_SoloPlay_mode() T 0x0868dd44
// int CUser::get_SoloPlay_mode()
// {
//     short v = GetGameOptionRef()->get_etc_option((ENUM_OPTION_ETC)0x1b);   // CGameOption*
//     return (int)v;
// }
//
// 35. CUser::get_SoloPlay_msgType(MSG_REQ_TO_PEER const&) T 0x0868dd76
// int CUser::get_SoloPlay_msgType(MSG_REQ_TO_PEER const& msg)
// {
//     char v = (char)*(const unsigned char*)((const char*)&msg + 0xf);   // movzbl+movsbl
//     if (v == 1) return 0;
//     if (v < 2) {
//         if (v == 0) return 1;
//     }
//     else {
//         if (v == 2) return 2;
//         if (v == 3) return 3;
//     }
//     return 4;
// }
// 【字段建议】MSG_REQ_TO_PEER+0xf：客户端 SoloPlay 请求子类型 byte。
//
// 36. CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE) W 0x085bfd76【报告缺失】
// int CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE type)
// {
//     return m_premium.GetAddExpBonus((int)type);
// }
//
// 37. CUser::getAddQuestItemDropRateFromPremium(ENUM_PREMIUM_TYPE) W 0x0869733e【报告缺失】
// int CUser::getAddQuestItemDropRateFromPremium(ENUM_PREMIUM_TYPE type)
// {
//     return m_premium.GetAddQuestItemDropRate((int)type);
// }
//
// 38. CUser::GetChangedGiftFatigueQuantity() const W 0x086975ee【报告缺失】
// unsigned char CUser::GetChangedGiftFatigueQuantity() const
// {
//     return (unsigned char)m_field8ebb0;                          // movzbl 直返（P9）
// }
//
// 39. CUser::getCharacAntibotSerialNum() const W 0x0823089c【报告缺失】
// int CUser::getCharacAntibotSerialNum() const
// {
//     return m_field8ec24;                                         // movl 直返
// }
//
// 40. CUser::getCharacSlotCountDefaultInfo() T 0x0868b4fc
// int CUser::getCharacSlotCountDefaultInfo()
// {
//     int v1 = (*reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x1a8));
//     if (v1 == 1) {
//         return 1;
//     }
//     if (v1 == 2) {
//         return 3;
//     }
//     int v2 = (*reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378));                    // P48 第二次独立调用
//     if (v2 == 5) {
//         return 0x18;
//     }
//     if (v2 == 0xb) {
//         return 0x18;
//     }
//     return 0x11;
// }
//
// 41. CUser::GetChildrensDayEventTargetItemPeriod(int) W 0x08328262【报告缺失】
// int CUser::GetChildrensDayEventTargetItemPeriod(int idx)
// {
//     if (idx > 9) {
//         return -1;
//     }
//     return *(int*)((char*)this + 0x8e0c4 + idx * 4);   /*periods 表 @0x8E0C4*/                     // 基址 0x23830*4+4=0x8E0C4
// }
//
// 42. CUser::GetChildrensDayEventTargetItemSlot(int) W 0x08328240【报告缺失】
// int CUser::GetChildrensDayEventTargetItemSlot(int idx)
// {
//     if (idx > 9) {
//         return -1;
//     }
// 43. CUser::getCompoundUpgrade(CStackableItem const*, stEquipCondition) const T 0x08682082
// ORIG 实测（objdump）：test 用 cond+2（m_amplifySelect）选表组，位移用 cond+1（m_tableIdx）
// <<7 选表行；表基址 = G_CDataManager() 基址 + 行<<7 + 0x8d0/0x18d0/0x28d0/0x38d0
// （CDataManager 数据区裸表，32×int 累加随机分布）。
int CUser::getCompoundUpgrade(const CStackableItem* item, stEquipCondition cond) const
{
    int upgrade = 0;                                              // -0x1c 默认0
    unsigned int tableAddr = 0;                                   // -0x18
    if (item->IsUse2ndUpgradeTableRecipe()) {
        if (cond.m_amplifySelect == 0) {
            tableAddr = (unsigned int)G_CDataManager() +
                        ((unsigned int)cond.m_tableIdx << 7) + 0x28d0;
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x86e5, 0);
            trace("RECIPE LOG : (Item:%u)UpgradeTable2nd", item->get_index());
        }
        else {
            tableAddr = (unsigned int)G_CDataManager() +
                        ((unsigned int)cond.m_tableIdx << 7) + 0x38d0;
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x86eb, 0);
            trace("RECIPE LOG : (Item:%u)AmplifyTable2nd", item->get_index());
        }
    }
    else if (cond.m_amplifySelect == 0) {
        tableAddr = (unsigned int)G_CDataManager() +
                    ((unsigned int)cond.m_tableIdx << 7) + 0x8d0;
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x86f4, 0);
        trace("RECIPE LOG : (Item:%u)UpgradeTable", item->get_index());
    }
    else {
        tableAddr = (unsigned int)G_CDataManager() +
                    ((unsigned int)cond.m_tableIdx << 7) + 0x18d0;
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x86fa, 0);
        trace("RECIPE LOG : (Item:%u)AmplifyTable", item->get_index());
    }
    if (tableAddr != 0) {
        int rnd = get_rand_int(0x2710);                           // 10000
        int acc = 0;                                              // -0x10
        int i = 0;                                                // -0xc
        while (i <= 0x1f) {
            acc = *reinterpret_cast<const int*>(tableAddr + i * 4) + acc;
            if (rnd < acc) {
                upgrade = i;                                      // -0x1c = i
                break;
            }
            i++;
        }
    }
    return upgrade;
}
//
// 44/45. CUser::GetCurCharacInventoryRef(ushort,ushort) 非 const T 0x08680f2e / const T 0x08680c8a
//
// 44/45. CUser::GetCurCharacInventoryRef(ushort,ushort) 非 const T 0x08680f2e / const T 0x08680c8a
// const 版报告在案（GetCurCharacInventoryRef.md）；非 const 版【报告缺失·objdump 同构】，
// 差异：getCurCharacInvenW/getCurCharacCargoW/CInventory::GetInvenRef(int,int)/CCargo::GetCargoRef(int)
// /CExpandEquipslot::GetInvenRefW(INVEN_TYPE,int)。共用私有展开见 priv_GetCurCharacInventoryRefImpl。
// Inven_Item* CUser::GetCurCharacInventoryRef(unsigned short space, unsigned short index)
// {
//     return priv_GetCurCharacInventoryRefImpl(/*writeMode=*/true, space, index);
// }
// const Inven_Item* CUser::GetCurCharacInventoryRef(unsigned short space, unsigned short index) const
// {
//     return priv_GetCurCharacInventoryRefImpl(false, space, index);
// }
// impl 草稿（switch 与 ORIG jump table 逐一对应）：
//   if (getCurCharacR()==NULL) return NULL;（非const用同一 getCurCharacR）
//   switch(space):
//     case 0:  return invRef(GetCurCharacInvenR/W(), 1, index);
//     case 1:  return invRef(..., 2, index);
//     case 2:  return cargoRef(GetCurCharacCargoR/W(), index);
//     case 3:  return invRef(..., 0, index);
//     case 7:  return invRef(..., 3, index);
//     case 0x12: expand = GetCharacExpandDataR(this,9);
//                return CExpandEquipslot::GetInvenRef[R|W](expand, INVEN_TYPE(5), index);
//     case 0x13..0x16: title = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe);
//                return CTitleBook::getTitleSlot(title, space-0x13, index);
//     default: return NULL;
//
// 46. CUser::GetDailyBadge(short*) W 0x0868e0c（EPs）【报告缺失】
// ORIG: for(i=0;i<=2;++i) out[i]=(short)*(ushort*)(this+(0x47040+i)*2+4)
// 数组基址 = 0x47040*2+4 = 0x8E084 ushort m_dailyBadge[3]
// 注意 nm 显示另存在 _ZN5CUser13GetDailyBadgeE15ENUM_BADGE_TYPE @0x08568e48——
// EPs 版按指针写，ENUM_BADGE_TYPE 版签名冲突需 ODR 裁决（meta 备注）。
// short CUser::GetDailyBadge(short* out)
// {
//     for (int i = 0; i <= 2; ++i) {
//         out[i] = (short)m_dailyBadges[i];
//     }
// }
//
// 47. CUser::GetETC(short) W 0x08697474【报告缺失】
// int CUser::GetETC(short key)
// {
//     std::map<short, int>& m = m_map8eaa4;
//     if (m.find(key) == m.end()) {
//         return 0;
//     }
//     int val = m[key];
//     m.erase(key);
//     return val;
// }
//
// 48. CUser::getHashCode(unsigned char*) T 0x084fa298
// void CUser::getHashCode(unsigned char* digestOut)
// {
//     md5_context ctx;                                             // 220 字节局部（-0xe0）
//     md5_starts(&ctx);
//     unsigned int sz = m_characList.size();
//     unsigned char* base = (unsigned char*)&m_characList.operator[](0);
//     md5_update(&ctx, base, sz * 0x14d1);
//     md5_finish(&ctx, digestOut);
// }
//
int CUser::getInDungeonItemSellCount(int space, int idx)
{
    // ORIG 0x0868b106（报告 getInDungeonItemSellCount.md 全文转写）。
    if ((signed)m_itemSellCount >= 7)
    {
        return 0;
    }
    if (getCurCharacR() == NULL)
    {
        return 0;
    }
    Inven_Item* it = GetCurCharacInventoryRef((unsigned short)space, (unsigned short)idx);
    if (it == NULL)
    {
        return 0;
    }
    bool found = false;                                          // -0xd
    int i = 0;                                                   // -0xc
    int* slots = (int*)m_sellCountData;                          // +0x8eb28 int[7]
    while (true)
    {
        bool cont;
        if ((found) || (6 < i)) cont = false; else cont = true;
        if (!cont) break;
        if (slots[i] == (int)it->m_addInfo) found = true;        // *(int*)(ref+2)
        i++;
    }
    if (!found)
    {
        slots[m_itemSellCount] = (int)it->m_addInfo;
        m_itemSellCount = m_itemSellCount + 1;
    }
    return m_itemSellCount;
}

void CUser::set_charac_guildkey_in_login(unsigned int guildKey, unsigned int characNo)
{
    // ORIG 0x0866c04e（报告 set_charac_guildkey_in_login.md + objdump 核对）。
    // state==2 门控 → 遍历角色列表按 characNo 命中后写 m_guildId(+0xdbe)。
    if (get_state() == 2)
    {
        int i = 0;
        while ((int)m_characList.size() > i)
        {
            if (m_characList[i].m_characNo == (int)characNo)
            {
                m_characList[i].m_guildId = (int)guildKey;
                break;
            }
            ++i;
        }
    }
}
//
// 50. CUser::getInformNoticeFlag(int) T 0x0868fa76
// int CUser::getInformNoticeFlag(int idx)
// {
//     return (unsigned char)m_informNoticeFlags[idx];              // movzbl 直返（P9）
// }
//
//
// ============================================================================
// body_C.cpp — CUser 批次C 可粘贴定义（草稿，未写入仓库文件）
// 集成步骤：
//   1) 先按 fields_C.tsv 在 CUser.h / CMCAPManager.h / _Charac_info 落地字段；
//   2) 将本文件内容并入 CUser.cpp 对应区段；签名与 ORIG mangled 一一对应；
//   3) NOTE(FIELD ...) 注释指明对应 fields_C.tsv 行，字段落地后可删除。
// 取证：docs/class_func_reports/CUser/<方法>.md（##汇编+##反编译C 完整读取），
//      及 17 个无报告符号的 objdump 直读（地址见 meta_C.tsv「报告缺失@...」行）。
// ============================================================================
//
//
// ============================ 简单访问器 / 状态门 ============================
//
// ORIG 0x084ecd42 W：movzwl 返回 ushort（P9）
// unsigned short CUser::GetSlotIDX() const
// {
//     return (unsigned short)m_field704a4;        // NOTE(FIELD 改名 m_slotIDX)
// }
//
// ORIG 0x082308e8 W：movzbl 返回 byte（P9/P11）
// unsigned char CUser::GetReceivedRequestType() const
// {
//     return (unsigned char)m_field8ec28;         // NOTE(FIELD 改名 m_receivedRequestType)
// }
//
// ORIG 0x0863be54 W
// unsigned char CUser::getPcRoomAuth() const
// {
//     return m_pcRoomAuth;                        // NOTE(FIELD +0x8cef1 unsigned char)
// }
//
// ORIG 0x082300d0 W：sete 布尔规范化
// bool CUser::IsState(ch_state state) const
// {
//     return m_chState == (int)state;
// }
//
// ORIG 0x084ecb00 W：ORIG 体为 mov $0,%eax; ret —— 忠实恒假返回
// bool CUser::IsPCRoomUser() const
// {
//     return false;
// }
//
// ORIG 0x08328220 W：+0x8e098 字节 == 0xff 判定
// bool CUser::IsOKChildrensDayEventCondition()
// {
    // NOTE(FIELD childrensDayEventState = +0x8e098 unsigned char，现 m_pad8e098[0])
//     unsigned char v = *(const unsigned char*)((const char*)this + 0x8e098);
//     if (v == 0xff) {
//         return true;
//     }
//     return false;
// }
//
// ORIG 0x0822fbe6 W：转发雇佣兵管理器等待态判定（返回值透传）
// bool CUser::IsWaitingStateCharac(unsigned int characNo)
// {
//     return m_mercenaryMgr.IsWaitingStateCharac(characNo);
// }
//
// ============================ hack-trade 计数组（m_379 @0x8d1a8）============
//
// ORIG 0x08532078 W：cnt=+0x8d1b4
// int CUser::getTotalHackUserTradeCnt() const
// {
//     return m_379.m_hackTradeCnt;                      // NOTE(FIELD 改名 m_hackTradeCnt)
// }
//
// ORIG 0x08532078 区（Gold）：edx:eax 双寄存器返回 ⇒ 组合 64 位
// long long CUser::getTotalHackUserTradeGold() const
// {
//     unsigned int lo = (unsigned int)m_379.m_hackTradeGoldLo;   // NOTE(FIELD m_hackTradeGoldLo @0x8d1ac)
//     unsigned int hi = (unsigned int)m_379.m_hackTradeGoldHi;   // NOTE(FIELD m_hackTradeGoldHi @0x8d1b0)
//     return (long long)(((unsigned long long)hi << 32) | lo);
// }
//
// ORIG 0x086972ee W
// void CUser::incTotalHackUserTradeCnt(unsigned int n)
// {
//     m_379.m_hackTradeCnt = (int)((unsigned int)m_379.m_hackTradeCnt + n);
// }
//
// ORIG 0x086972b6 W：64 位加法（lo 用 add、hi 用 adc）
// void CUser::incTotalHackUserTradeGold(unsigned long long gold)
// {
//     unsigned long long cur =
//         (((unsigned long long)(unsigned int)m_379.m_hackTradeGoldHi) << 32) |
//         (unsigned int)m_379.m_hackTradeGoldLo;
//     cur += gold;
//     m_379.m_hackTradeGoldLo = (int)(unsigned int)(cur & 0xffffffffu);
//     m_379.m_hackTradeGoldHi = (int)(unsigned int)(cur >> 32);
// }
//
// ============================ 计数器族 ======================================
//
// ORIG 0x080fdf56 W：自增并返回新值
// int CUser::inc_multiboxLotteryItemFailCnt()
// {
    // NOTE(FIELD m_multiboxLotteryFailCnt @0x703bc int)
//     ++m_multiboxLotteryFailCnt;
//     return m_multiboxLotteryFailCnt;
// }
//
// ORIG 0x08110dd0 W：byte 自增（objdump 逐指令转写）
void CUser::incPcRoomPlayTimeRewardCnt()
{
    ++m_pcRoomPlayTimeRewardCnt;                // +0x8eba4 unsigned char
}
//
// ============================ 角色列表 / 链接角色 ============================
//
// ORIG 0x0869736e W
// int CUser::getLinkCharacNo(char index)
// {
//     int idx = (int)index;
//     bool bad = true;
//     if (idx < get_charac_count() && idx >= 0)
//         bad = false;
//     if (bad)
//         return 0;
//     return m_characList[(unsigned int)idx].m_characNo;
// }
//
// ORIG 0x084ecd52 W
// bool CUser::isCharacLinkBonusSetting(char index)
// {
//     int idx = (int)index;
//     bool bad = true;
//     if (idx < get_charac_count() && idx >= 0)
//         bad = false;
//     if (bad)
//         return false;
    // NOTE(FIELD _Charac_info+0x124f m_linkBonusSetting unsigned char；movzbl 直返)
//     const unsigned char* flags =
//         (const unsigned char*)((const char*)&m_characList[(unsigned int)idx]
//                                + 0x124f);
//     return *flags != 0;
// }
//
// ============================ 队伍 / 商城 / 状态门 ============================
//
// ORIG 0x0868856c T：party 存在用队伍连胜，否则角色连胜
// unsigned short CUser::GetPowerWarPartyVictory()
// {
//     CParty* party = (CParty*)GetParty();
//     if (party == 0)
//         return getStraightVictories();
//     return party->getStraightVictories();
// }
//
// ORIG 0x0868ad52 T：charac_expand 槽 0xb → CQuestShop::getQp()
// int CUser::getQuestPoint()
// {
//     CQuestShop* shop =
//         (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
//     if (shop == 0)
//         return 0;
//     return shop->getQp();
// }
//
// ORIG 0x0869230a T：回流用户等级档位键（纯分段）
// int CUser::getReturnUserLevelKey(int level, int mode)
// {
//     if (mode == 0) {
//         if (level < 10)   return 5;
//         if (level < 0xf)  return 10;
//         if (level < 0x14) return 0xf;
//     }
//     else if (level < 0x14) {
//         return 0xf;
//     }
//     if (level < 0x1e) return 0x14;
//     if (level < 0x28) return 0x1e;
//     if (level < 0x32) return 0x28;
//     if (level < 0x3c) return 0x32;
//     if (level < 0x46) return 0x3c;
//     return 0x46;
// }
//
// ORIG 0x0868056a T：push self + party 成员补齐他人，返回 party
// CParty* CUser::GetUserListByParty(std::vector<CUser*>& out)
// {
//     out.push_back(this);
//     CParty* party = (CParty*)GetParty();
//     if (party != 0) {
//         for (int i = 0; i <= 3; ++i) {
//             CUser* u = party->get_user(i);
//             if (u != 0 && u != this) {
//                 out.push_back(u);
//             }
//         }
//     }
//     return party;
// }
//
// ORIG 0x086885d4 T：无队伍返 0；有队转发 CParty::GetUserSecretShop(this)
// void* /*实际为 secretshop 商店对象指针，按共享头既有声明取齐*/ CUser::GetUserSecretShop()
// {
//     CParty* party = (CParty*)GetParty();
//     if (party != 0)
//         return party->GetUserSecretShop(this);
//     return 0;
// }
//
// ORIG 0x087f104 / 0x0867f104 T：黑名单 vector<uint> 查找
// bool CUser::isBlackUser(unsigned int accId)
// {
//     bool empty_ = m_vec8cef4.empty();
//     if (empty_)
//         return false;
//     for (std::vector<unsigned int>::const_iterator it = m_vec8cef4.begin(),
//          e = m_vec8cef4.end(); it != e; ++it)
//     {
//         if (*it == accId)
//             return true;
//     }
//     return false;
// }
//
// ============================ InformNoticeFlag 族 ============================
//
// ORIG 0x0868f956 T：bit 置位（数组 @0x8ebb1 长度16）
// void CUser::handleInformNoticeFlag(unsigned int bitIndex)
// {
//     unsigned int low = bitIndex & 7u;
//     unsigned int high = bitIndex >> 3;
//     if (low < 8 && high < 16) {   // ORIG 两级 ja 保护，语义恒真但保留形态
//         if (checkInformNoticeFlag(low, high)) {
            // NOTE(FIELD m_informNoticeFlags[16] @0x8ebb1；现为 m_informNoticeFlag+m_pad8ebb2)
//             unsigned char b =
//                 *(unsigned char*)((char*)this + 0x8ebb1 + high);
//             b = (unsigned char)(b + (unsigned char)(1 << low));
//             *(unsigned char*)((char*)this + 0x8ebb1 + high) = b;
//             enableSaveInformNoticeFlag();
//         }
//     }
// }
//
// ORIG 0x0868f900 T
// void CUser::loadInformNoticeFlag(char* src)
// {
//     resetInformNoticeFlag();
//     if (src != 0) {
//         for (int i = 0; i <= 15; ++i) {
            // NOTE(FIELD m_informNoticeFlags[16] @0x8ebb1)
//             *(unsigned char*)((char*)this + 0x8ebb1 + i) =
//                 (unsigned char)src[i];
//         }
//         sendInformNoticeFlag();
//     }
// }
//
// ORIG 0x0868fa8a T：位展开为索引清单
// void CUser::getInformNoticeFlagIndex(std::vector<unsigned char>& out)
// {
//     out.clear();
//     for (int i = 0; i <= 15; ++i) {
//         unsigned char flag = (unsigned char)getInformNoticeFlag(i);
//         for (int j = 0; j <= 7; ++j) {
//             if ((((unsigned int)(unsigned char)flag) >> j) & 1u)
//                 out.push_back((unsigned char)(i * 8 + j));
//         }
//     }
// }
//
// ============================ LimitItemUsage 查询族 ==========================
//
// ORIG 0x08688d26 T：匹配 elem.m_field15 返回元素指针
// LimitItemUsage* CUser::GetItemLimitInfo(int itemIdx)
// {
//     std::vector<LimitItemUsage>::const_iterator e = m_limitItemUsage.end();
//     for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin();
//          e != it; )
//     {
//         LimitItemUsage& u = *it;
//         if (u.m_field15 == itemIdx)
//             return &u;
//         ++it;
//     }
//     return 0;
// }
//
// ORIG 0x08688c70 T：找到限制条目后按其 isUsable 判定，无条目视为可用
// bool /*按头声明取齐*/ CUser::IsLimitItemUsable(int itemIdx)
// {
//     std::vector<LimitItemUsage>::const_iterator e = m_limitItemUsage.end();
//     for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin();
//          e != it; )
//     {
//         LimitItemUsage& u = *it;
//         if (u.m_field15 == itemIdx)
//             return LimitItemUsage_isUsable(u) ? true : false;  // 见尾部说明①
//         ++it;
//     }
//     return true;
// }
//
// ORIG 0x0868c45e T：材料表任一元素 find_item 后 vtbl+0x10 虚调用(IsAvatarItem)
// bool CUser::is_recipe_items_avatar_type(
//     const std::vector<std::pair<int,int> >& items)
// {
//     unsigned int i = 0;
//     while (true) {
//         if ((int)items.size() <= (int)i)
//             return false;
//         int idx = items[i].first;
//         CItem* item = G_CDataManager()->find_item(idx);
//         if (item != 0 && item->IsAvatarItem())   // P5：vtbl+0x10 → 具名虚函数
//             return true;
//         ++i;
//     }
// }
//
// ============================ 合成 / 抽奖判定 ================================
//

// ORIG 0x08671fde T _ZN5CUser17isSuccessCompoundEi（报告 docs/CUser/isSuccessCompound.md）
bool CUser::isSuccessCompound(int chance)
{
    if (getRecipeProbability() == 1)
        return true;
    if (getRecipeProbability() == 2)
        return false;
    if (chance != 0)
    {
        int r = get_rand_int(100);
        if (chance <= r)
            return false;
    }
    return true;
}

//
// ORIG 0x08673342 T：老式抽奖道具区间判定（二叉区间树等价 if 链）
// bool /*按头声明取齐*/ CUser::IsOldLotteryItem(unsigned int itemIdx)
// {
//     bool hit;
//     if (itemIdx <= 0x1d56) {
//         if (itemIdx < 0x1d4c) {
//             if (itemIdx >= 0x1bc6 && itemIdx <= 0x1bf5) {
//                 hit = true;
//             } else {
//                 unsigned int d = itemIdx - 0x1c15;
//                 hit = (itemIdx >= 0x1bc6 && d <= 4);
//             }
//         } else {
//             hit = true;
//         }
//     } else if (itemIdx <= 0x1e5c) {
//         if (itemIdx >= 0x1e44) {
//             hit = true;
//         } else {
//             unsigned int d2 = itemIdx - 0x1dca;
//             hit = (d2 <= 5);
//         }
//     } else {
//         unsigned int d3 = itemIdx - 0x1eba;
//         hit = (d3 <= 1);
//     }
//     return hit ? true : false;
// }
//
// ============================ 公会基金 ======================================
//
// ORIG 0x0866c21e T：level(@m_guildDB+0x1b)<0x10 → 上限 20000000 否则 10000000
// bool /*按头声明取齐*/ CUser::IsAddableGuildFund(int amount)
// {
    // NOTE(FIELD STGuildDBInfoOnly: fund int @CUser+0x8d0c0(m_guildDB+0xa0)、
    //       level byte @CUser+0x8d03b(m_guildDB+0x1b)——宿主按建议具名)
//     unsigned char level =
//         *(const unsigned char*)((const char*)this + 0x8d03b);
//     int fund = *(const int*)((const char*)this + 0x8d0c0);
//     if (level <= 0xf) {
//         return ((unsigned int)(fund + amount) <= 20000000u);
//     }
//     return ((unsigned int)(fund + amount) <= 10000000u);
// }
//
// ============================ 冷却道具载入 ==================================
//
// ORIG 0x08658ff6 T：CONTINUOUS_ITEM_INFO(ubyte count + entries{int,uint}×8)
// 过滤条件：endSec>curSec、itemIdx<0x7fffffff、endSec<=0x77359400。
// bool CUser::LoadCoolTimeItemInfo(CONTINUOUS_ITEM_INFO* info)
// {
//     unsigned int nowSec = GlobalData::s_systemTime_.getCurSec();
//     std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
//     list->clear();
//     unsigned int count = (unsigned int)*(const signed char*)info;
//     for (unsigned int i = 0; i < count; ++i) {
//         const unsigned char* e = (const unsigned char*)info + 4 + i * 8;
//         unsigned int itemIdx = *(const unsigned int*)e;          // +4 区：item idx
//         unsigned int endSec  = *(const unsigned int*)(e + 4);    // +8 区：到期秒
//         if (endSec > nowSec && itemIdx < 0x7fffffffu &&
//             endSec < 0x77359401u)
//         {
//             ContinuousItemInfo ci;
//             ci.m_addInfo = itemIdx;      // 按仓库 ContinuousItemInfo 字段名取齐
//             ci.m_endSec = endSec;
//             list->push_back(ci);
//         }
//     }
//     ClearUselessCooltimeItem(nowSec);            // CUserCharacInfo 成员
//     bool sent = SendCoolTimeItemInfo(nowSec);
//     return !sent ? false : true;
// }
//
// ============================ m_mcap 序列/时间检查器 ========================
// 17 个无报告符号之二（objdump 直读）。字段组 NOTE 见 fields_C.tsv 第3组；
// CParty 内嵌 CBattle_Field 布局字段由宿主统一迁移后替换 pad 读法（见下）。
//
// ORIG 0x0823066a W：连续序列命中计数器
// int CUser::GetItemSerialChecker(unsigned short serialIdx, unsigned short serialVal,
//                                 int monsterIdx)
// {
//     WongWork::CMCAPManager& mc = m_mcap;
//     int diffIdx = (int)(short)serialIdx - (int)(short)mc.m_serialLastIdx;
//     if (diffIdx >= -23 && diffIdx <= 23) {
//         int diffVal = (int)(short)serialVal - (int)(short)mc.m_serialLastVal;
//         if (diffVal >= -17 && diffVal <= 17) {
//             bool cont = false;
//             CParty* party = (CParty*)GetParty();
//             if (party != 0 && party->isMonsterDrop_item(monsterIdx)) {
//                 CBattle_Field& bfRef =
//                     *reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1);
//                 cont = (bfRef.m_visitedGotoFlag == 0) ||
//                        bfRef.m_hellMapIdx != bfRef.get_cur_map();
                // [集成占位①替换说明] 宿主内嵌BF字段化后改为：
                //   cont = bf->m_visitedGotoFlag == 0 ||
                //          bf->m_hellMapIdx != bf->get_cur_map();
//             }
//             if (cont)
//                 ++mc.m_serialHitCount;          // NOTE(FIELD m_mcap+0x84，现名 m_field84)
//             mc.m_serialLastIdx = serialIdx;     // NOTE(FIELD m_mcap+0x80)
//             mc.m_serialLastVal = serialVal;     // NOTE(FIELD m_mcap+0x82)
//             return mc.m_serialHitCount;
//         }
//     }
//     mc.m_serialHitCount = 0;
//     mc.m_serialLastIdx = serialIdx;
//     mc.m_serialLastVal = serialVal;
//     return mc.m_serialHitCount;
// }
//
// ORIG 0x082302ac W：时间窗行为检查器
// int CUser::GetItemTimeChecker(unsigned int nowSec, unsigned short val1,
//                               unsigned short val2, unsigned int rangeSec)
// {
//     WongWork::CMCAPManager& mc = m_mcap;
//     bool reset = true;
//     unsigned int base = mc.m_timeChkBaseSec;              // NOTE(FIELD +0x4c)
//     if ((unsigned int)(nowSec - base) <= rangeSec)
//         reset = false;
//     if (!reset) {
//         ++mc.m_timeChkCount;                              // NOTE(FIELD +0x50)
//         if ((int)(short)mc.m_timeChkMinA /*+0x44*/ - (int)(short)val1 > 0)
//             mc.m_timeChkMinA = (short)val1;
//         if ((int)(short)mc.m_timeChkMinB /*+0x48*/ - (int)(short)val2 > 0)
//             mc.m_timeChkMinB = (short)val2;
//         if ((int)(short)mc.m_timeChkMaxA /*+0x46*/ - (int)(short)val1 < 0)
//             mc.m_timeChkMaxA = (short)val1;
//         if ((int)(short)mc.m_timeChkMaxB /*+0x4a*/ - (int)(short)val2 < 0)
//             mc.m_timeChkMaxB = (short)val2;
//         short dA = (short)((int)mc.m_timeChkMaxA - (int)mc.m_timeChkMinA);
//         short dB = (short)((int)mc.m_timeChkMaxB - (int)mc.m_timeChkMinB);
//         short aA = (short)((dA ^ (short)(dA >> 15)) - (short)(dA >> 15));
//         short aB = (short)((dB ^ (short)(dB >> 15)) - (short)(dB >> 15));
//         mc.m_timeChkDiffA = aA;                           // NOTE(FIELD +0x54)
//         mc.m_timeChkDiffB = aB;                           // NOTE(FIELD +0x56)
//         return mc.m_timeChkCount;
//     }
    // 重置分支（ORIG 8230428..）：清差值与计数，回填基准与四元组
//     mc.m_timeChkDiffA = 0;
//     mc.m_timeChkDiffB = 0;
//     mc.m_timeChkCount = 0;
//     mc.m_timeChkBaseSec = nowSec;
//     mc.m_timeChkMinA = (short)val1;      // +0x44（ORIG 写 0x34→val1）
//     mc.m_timeChkMinB = (short)val2;      // +0x48（ORIG 写 0x38→val2）
//     mc.m_timeChkMaxA = (short)val1;      // +0x46（ORIG 写 0x36→val1）
//     mc.m_timeChkMaxB = (short)val2;      // +0x4a（ORIG 写 0x3a→val2）
//     return mc.m_timeChkCount;
// }

// ---------------------------------------------------------------------------
// 说明① LimitItemUsage::isUsable 为独立 ORIG 符号（本批外）；
// 集成时若该成员尚无定义，保留对真实符号的调用即可（meta 已记依赖口径）。

// ============================ NPC 赠礼门 =====================================

// ORIG 0x08681160 T：安全保护/私摊忙碌/幽灵/交易/突袭/PVP频道 六重门
int CUser::isEnableGiveGiftToNPC()
{
    int r = GlobalData::s_pSecuProtectionField->Check(
        this, (SECURITY_PROTCTION)0x15);
    if (r != 0)
        return r;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(this))
        return 0x13;
    if (IsCurCharacGhost())          // CUserCharacInfo 继承成员（真名按头取齐）
        return 0x13;
    if (CheckInTrade())
        return 0x13;
    if (GetAssaultPlace() != 0)
        return 0x13;
    if (G_GameWorld()->IsPVPChannel())
        return 0x13;
    return 0;
}

// ============================ 交易金币累加 ==================================

// ORIG 0x0864658a T：累加并跨阈值（首次越过 10000000）向 log 队列投递 0xd0 记录；
// 尾部按当前角色存在与否增计日交易额。
void CUser::IncrementTradeGold(unsigned int gold, int mode)
{
    bool over = false;
    if (mode != 0) {
        over = GetOverTradeGoldCount() < 10000000u;
        SetOverTradeGoldCount(GetOverTradeGoldCount() + gold);
        if (over) {
            if (GetOverTradeGoldCount() >= 10000000u) {
                // 越界一次性上报（ORIG "user.cpp":0x408 处 Acquire）
                Stream* s =
                    GlobalData::s_stream_pool->Acquire("user.cpp", 0x408);
                CStreamGuard guard(s, true);
                guard << (int)0xd0;
                guard << (int)GetUID();
                guard << (unsigned int)get_acc_id();
                GlobalData::s_msgq_mgr->put(
                    (MsgQueueMgr::QUEUE_IDX)2, guard);
            }
        }
    }
    if (getCurCharacR() != 0)
        IncrementCurCharacTradeGoldDaily(gold);
}


// ============================================================================
// body_D.cpp —— CUser ORIG-only 批次 D 草稿（50 符号，/tmp/df_batches/onlyorig_cuser）
// 证据：docs/class_func_reports/CUser/<方法>.md 全文（## 汇编 + ## 反编译 C）。
// SellItemTimeChecker 报告缺失@0x08230256（nm W sz0x55），由 objdump 区间转写。
// 字段/helper 约定（落地时按下列规则展开）：
//   RDARScriptStringManager_findString(mgr, a,b,c) ≡ mgr.findString(a,b,c)
//     （ORIG _ZNK23RDARScriptStringManager10findStringEiPKcPb @0x08aa57fe）。
//   Inven_Item 内部占位字段（由 Inven_Item 布局属主最终具名；偏移为 ORIG 实测）：
//     upgradeByte=+0x03 升级字节 | stackWord07=+0x07(word) | shortField0b=+0x0b |
//     expire_sec_word=+0x17(word) | front=+0x00 首字节非零判空。
//   quest_action 中 goto/LAB_END 结构如与局部对象初始化冲突，按 if/else 展开。
// 风格对齐 CUser.cpp 现有 PacketGuard/CStreamGuard 形态。
// ============================================================================

// ----------------------------------------------------------------------------
// ORIG 0x08650722 CUser::LoadMileage() void  (报告: CUser/LoadMileage.md)
// StreamPool->Acquire("user.cpp",0x18f2); guard<<0x1fd; <<uid;
// SIG_MILEAGE_STRUCT{accId, characNo, mileage=0}; msgq put(QUEUE_IDX=2)
void CUser::LoadMileage()
{
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x18f2);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<((int)0x1fd); // 流类型
    guard.operator*()->operator<<((int)GetUID());
    char* sig = (char*)guard.operator->()->GetInBuffer<SIG_MILEAGE_STRUCT>();
    *(unsigned int*)(sig + 0) = get_acc_id();
    *(unsigned int*)(sig + 4) = getCurCharacNo();
    *(unsigned int*)(sig + 8) = 0;             // mileage 由 DB 侧维护
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ----------------------------------------------------------------------------
// ORIG 0x0866ad92 CUser::lost_ext(int percent, int& actualLost, bool hardMode)
// 返回 unsigned char（movzbl 局部 ret，无 setne 规范化，P9）
// 注：hardMode 两分支中扣减调用（ORIG call 0x86967f0）传参按 ORIG 汇编
// （ae4d 载 -0x18=lvExpA）；Ghidra 反编译标注为 subtractCurCharacExp(loss)，
// 以 ORIG 常量为准——落地时两分支语义为「exp 高于本级下限全额扣除 / 否则
// 置回下限」的等价表达，与报告 Ghidra 文本一致。
unsigned char CUser::lost_ext(int percent, int& actualLost, bool hardMode)
{
    int lvExpA = G_CDataManager()->get_level_exp(get_charac_level());
    int lvExpB = G_CDataManager()->get_level_exp(get_charac_level() + 1);
    int loss = ((lvExpB - lvExpA) * percent) / 100;

    if (!hardMode) {
        // 汇编 L_eb 分支（param3==0）：全额扣除后走尾部降级检查
        actualLost = loss;
        subtractCurCharacExp(loss);
    } else {
        int exp = get_charac_exp();
        if (exp - loss > lvExpA) {
            actualLost = loss;
            subtractCurCharacExp(loss);
        } else {
            // 扣到跨级以下：实际损失封顶在本级已得部分，经验回退到下限
            actualLost = exp - lvExpA;
            setCurCharacExp(lvExpA);
        }
    }
    // 尾部共同路径：低于当前等级下限则降一级并刷新属性
    int newFloor = G_CDataManager()->get_level_exp(get_charac_level());
    unsigned char downgraded = 0;
    if (get_charac_exp() < newFloor) {
        decCurCharacLevel();
        _update_charac_stat(false);
        downgraded = 1;
    }
    SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
    return downgraded;
}

// >>> W7-BLOCKED CUser::lottery_item —— Inven_Item +0x17 expire_sec_word(m_amp 内部)未具名；g_emptySlot 全局未建模；Store 调用 ABI 待核
// ----------------------------------------------------------------------------
// ORIG 0x086733a2 CUser::lottery_item(int slot) int  (报告 CUser/lottery_item.md)
// 返回码：4=背包不足 0xa=费用不足 0xe=等级不足 0x10=未实现 0x11=物品不存在/
// 删除失败 0x15=找不到物品 0x16=金钱溢出
// int CUser::lottery_item(int slot)
// {
//     Inven_Item itemSrc;                                   // -0x176 抽奖券
//     Inven_Item lottoItem;                                 // -0x139 中奖物
//     int newSlot = 0;                                      // -0x5c
//     CInventory* invenR = const_cast<CInventory*>(getCurCharacInvenR());
//     itemSrc = invenR->GetInvenSlot(1, slot);
//
//     G_CDataManager();
//     CItem* ticketDef = G_CDataManager()->find_item(itemSrc.m_addInfo);   // -0x58
//     if (ticketDef == 0) return 0x15;
//     if (get_charac_level() < ticketDef->getUsableLevel()) return 0xe;
//
//     int useCost = 0;                                      // -0x4c
//     useCost = G_CDataManager()->find_lottery_use_cost(ticketDef->get_index());
//     if (useCost != 0 && getCurCharacInvenR()->get_money() < useCost)
//         return 0xa;
//
//     G_CDataManager()->getItemList().get_lotto_item(
//         itemSrc.m_addInfo, (char*)&lottoItem);           // CItemList::get_lotto_item
//
    // 金币即中奖（结果 index==0 且数额>0）
//     if (lottoItem.m_addInfo == 0 && lottoItem.m_addInfo2 > 0) {
//         int goldGain = lottoItem.m_addInfo2;                   // -0x44
//         int moneyLimit = 0x7fffffff;                      // -0x40
//         const char* accName = get_acc_name();
//         moneyLimit = G_CDataManager()->GetMoneyLimitPerLevel(get_charac_level(), accName);
//         int curMoney = getCurCharacInvenR()->get_money(); // -0x40 复用点见汇编 0x863603..0x86360f
//         curMoney += goldGain;
//         if (curMoney > moneyLimit) {
//             cMyTrace trace((char*)__PRETTY_FUNCTION__, 0x61b9, 4);
//             trace("lottery_gold_item:m_id(%s), char_no(%d), money(%d)",
//                   NumberToString(get_acc_id(), 0), getCurCharacNo(), goldGain);
//             return 0x16;
//         }
//         int gained = getCurCharacInvenW()->gain_money(goldGain,
//                        (eMoneyAddReason)0x17, true, 0);   // -0x38
//         if (gained < goldGain)
//             SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, goldGain, gained);
//         if (!getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slot, 1,
//                                                (eItemDelReason)3, true))
//             return 0x11;
//
//         PacketGuard packet;
//         packet.put_header(1, 0x1d);
//         packet.put_byte(1);
//         packet.put_short(slot);
//         packet.put_short(newSlot);                        // 0
//         packet.put_int(0);
//         packet.put_int(gained);
//         packet.put_short(0);
//         packet.put_byte(0);
//         packet.put_byte(0);
//         packet.put_short(0);
//         packet.put_packet(*g_emptySlot);
//         packet.finalize(true);
//         Send(packet);
//         send_broadcast_lottery_gold_message(itemSrc.m_addInfo, goldGain);
//         return 0;
//     }
//
    // 物品中奖：券与目标定义查找
//     G_CDataManager();
//     CItem* ticket = G_CDataManager()->find_item(itemSrc.m_addInfo);      // -0x54
//     G_CDataManager();
//     CItem* prize = G_CDataManager()->find_item(lottoItem.m_addInfo);     // -0x50
//
    // 强化转移：券 stackable 且 category==0xd（彩票类），目标等级 ==1 且类别==0xb?
    // 按 ORIG：is_stackable(ticket) && getItemType?==0xd 时 roll 强化度
//     if (ticket != 0 && prize != 0 &&
//         ticket->is_stackable() && ticket->GetItemType() == 0xd)
//     {
//         bool needUpgradeRoll =
//             (lottoItem.upgrade == 1) && (prize->GetItemType() != 0xb);
//         if (needUpgradeRoll) {                            // 汇编 0x863944..0x863974
//             int up = 0;                                   // -0x34
//             int roll = get_rand_int(10000);
//             if (roll <= *(int*)((char*)G_CDataManager() + 0x598)) {   // 表 A 上限
//                 int base = *(int*)((char*)G_CDataManager() + 0x590);
//                 int span = *(int*)((char*)G_CDataManager() + 0x59c);
//                 up = base + get_rand_int(span) / 1000;
//             } else if (roll <= (*(int*)((char*)G_CDataManager() + 0x5a8) +
//                                 *(int*)((char*)G_CDataManager() + 0x598))) {
//                 int base = *(int*)((char*)G_CDataManager() + 0x5a0);
//                 int span = *(int*)((char*)G_CDataManager() + 0x5ac);
//                 up = base + get_rand_int(span) / 1000;
//             } else {
//                 int base = *(int*)((char*)G_CDataManager() + 0x5b0);
//                 int span = *(int*)((char*)G_CDataManager() + 0x5bc);
//                 up = base + get_rand_int(span) / 1000;
//             }
//             if (up < 0 || 0x1f < up) up = 0;
//             lottoItem.SetUpgrade((unsigned char)up);
//         }
//     }
//     if (lottoItem.m_addInfo == 0 || lottoItem.m_addInfo == -1)
//         return 0x11;
//
    // 容量检查（QuickEmptyCount 为 0 时逐空间校验）
//     if (getCurCharacInvenR()->QuickEmptyCount() <= 0) {
//         if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE(1), 1))
//             return 4;
//         int addInfo = itemSrc.get_add_info();
//         if (!(addInfo < 2)) {
//             if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE(2), 1))
//                 return 4;
//         } else {
            // addInfo<2 无第二空间需求（三目形态，对齐 ORIG 0x86739f 逻辑）
//         }
//         if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE(3), 1))
//             return 4;
//         if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE(10), 1))
//             return 4;
//     }
//
    // 成长符文四件套检查（券 idx==0x28bfa1）：+0x28bf9d..a0 的数量不得 >9
//     if (itemSrc.m_addInfo == 0x28bfa1) {
//         Inven_Item part[4];
//         for (int i = 0; i < 4; ++i) {                     // 顺序 9d/9e/9f/a0
//             new (&part[i]) Inven_Item();
//             getCurCharacInvenW()->GetInvenData(0x28bf9d + i, part[i]);
//         }
//         if (part[0].m_addInfo2 > 9 || part[1].m_addInfo2 > 9 ||
//             part[2].m_addInfo2 > 9 || part[3].m_addInfo2 > 9)
//             return 4;
//     }
    // 非 0x28becb 券在此先删；0x28becb 券延后到插入成功后再删
//     if (itemSrc.m_addInfo != 0x28becb &&
//         !getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slot, 1,
//                                            (eItemDelReason)3, true))
//         return 0x11;
//
//     int remainSec = 0;                                    // -0x2c
//     if (prize != 0 && prize->IsAvatarItem()) {            // vtable+0x10 槽
//         char ipg[24];
//         memset(ipg, 0, sizeof(ipg));
//         unsigned int uid = WongWork::CGenUniqueNo::genIPGNo(
//             (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)1, getCurCharacNo(), ipg);
//         unsigned short period = prize->getUsablePeriod();
//         newSlot = getCurCharacInvenW()->AddAvatarItem(
//             lottoItem.m_addInfo, period, false, '\0', '\0', -1,
//             (const char*)ipg, (eAvatarItemAddReason)7, false, 0);
//         if (newSlot == -1) return 4;
//         time_t now = GlobalData::s_systemTime_.getCurSec();
//         Inven_Item chk;                                   // -0x99 栈
//         chk = getCurCharacInvenR()->GetInvenSlot(2, newSlot);
//         unsigned short idx = chk.m_addInfo;
//         remainSec = ((WongWork::CAvatarItemMgr*)
//                      getCurCharacInvenR()->GetAvatarItemMgrR())
//                     ->GetRemainDate(idx, now);
//         lottoItem.SetUpgrade('\0');
//     } else if (prize != 0 && prize->IsCreatureEgg() &&               // vtable+0x14
//                prize->GetItemType() == 0x16) {                   // vtable+0xc
//         if ((unsigned char)(lottoItem.m_attr & 0x1f) /* cStack_13c(+0x03 升位字节) */ != 5)
//             return 0x11;
//         if (!user_creature::CCreatureItemConverter::SetEggItem(&lottoItem))
//             return 0x11;
//         Inven_Item copy = lottoItem;
//         newSlot = getCurCharacInvenW()->insertItemIntoInventory(
//                       copy, (eItemAddReason)3, true, false);
//         if (newSlot < 0) return 4;
//         int expire = prize->getExpirationDate();
//         int period  = prize->getUsablePeriod();
//         user_creature::CCreatureMgr* mgr =
//             getCurCharacInvenW()->GetCreatureMgrW();
//         mgr->InsertCreatureItem(&copy, newSlot, 0x19, period, expire);
//     } else {
        // 随机选项道具：vtable+0x4c isRandomOption、+0x50 组数等
//         if (prize != 0 && prize->IsRandomOption()) {
//             long groupCnt = prize->getRandomOptionGroupCount();       // +0x50 槽
//             int groupName = prize->getItemGroupName();
//             int usableLv = prize->getUsableLevel();
//             ENUM_RARITY rarity = (ENUM_RARITY)prize->get_rarity();
//             if (!GlobalData::g_randomOptionHandle.give_option(
//                     groupCnt, rarity, usableLv, groupName,
//                     lottoItem.m_addInfo,
//                     (RandomOption*)((char*)&lottoItem + 0x26)))
//                 return 0x11;
//         }
//         Inven_Item copy = lottoItem;
//         newSlot = getCurCharacInvenW()->insertItemIntoInventory(
//                       copy, (eItemAddReason)3, true, true);
//         if (newSlot == -1) return 4;
//         if (itemSrc.m_addInfo == 0x28becb) {
//             if (!getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slot, 1,
//                                                    (eItemDelReason)3, true))
//                 return 0x11;
//         }
//         remainSec = lottoItem.get_add_info();
//         if (prize != 0) {
//             int price = 0;                                        // -0xe0
//             Store::GetSellItemPrice(copy, prize, (short)lottoItem.m_addInfo2,
//                                     false, price);
//             GetInstanceValueStatistic()->AddValueStatistic(
//                 (VALUE_STATISTIC_FIELD)7, this, price);
//         }
//     }
//
//     if (IsOldLotteryItem(itemSrc.m_addInfo))
//         APSystem::CUserProc::ClearActionAndSendtoUser(this,
//             (APSystem::_ActionGroupIndex)4, 0, (ENUM_PACKETCLASS)0);
//
    // 使用费（有配置则扣钱并刷新货币包）
//     int cost2 = 0;                                                // -0x20
//     cost2 = G_CDataManager()->find_lottery_use_cost(ticketDef->get_index());
//     if (cost2 != 0) {
//         getCurCharacInvenW()->use_money(cost2, (eMoneySubReason)0x2d, true);
//         PacketGuard mp;
//         mp.clear();
//         mp.put_header(0, 0xe);
//         mp.put_byte(0);
//         mp.put_short(1);
//         getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, mp);
//         mp.finalize(true);
//         Send(mp);
//     }
//
    // 结果包 0x1d
//     PacketGuard packet;
//     packet.put_header(1, 0x1d);
//     packet.put_byte(1);
//     packet.put_short(slot);
//     packet.put_short(newSlot);
//     packet.put_int(lottoItem.m_addInfo);
//     packet.put_int(remainSec);
//     packet.put_short(*(unsigned short*)((char*)&lottoItem + 0x17)); // 净化字段
//     packet.put_byte(lottoItem.GetItemAttr() & 0xff);
    // stAmplifyOption_t @ lottoItem+0x18(+1)：ability type/value
//     {
//         stAmplifyOption_t& amp = *(stAmplifyOption_t*)((char*)&lottoItem + 0x19);
//         packet.put_byte(amp.getAbilityType() & 0xff);
//         packet.put_short(amp.getAbilityValue() & 0xffff);
//     }
//     if (lottoItem.isAvatarItemType()) {
//         packet.put_int(0x1e);
//         WongWork::CAvatarItemMgr* avm =
//             (WongWork::CAvatarItemMgr*)getCurCharacInvenR()->GetAvatarItemMgrR();
//         packet.put_binary(avm->getJewelSocketData(lottoItem.slot), 0x1e);
//         packet.put_int(4);
//         char* exp = avm->GetExpansionInfo(lottoItem.slot);
//         if (exp == 0) {
//             stAvatarExpansionInfo_t defInfo;
//             defInfo.init();
//             packet.put_binary((char*)&defInfo, 4);
//         } else {
//             packet.put_binary(exp, 4);
//         }
//     }
//     packet.put_packet(lottoItem);
//     packet.finalize(true);
//     Send(packet);
//
//     unsigned char up = lottoItem.GetUpgrade();
//     if (up != 0)
//         m_historyLog.UpgradeItem(lottoItem.m_addInfo, 0, up, true);
//
    // 全服公告（roll 结果值得广播时）
//     if (ticket != 0 && prize != 0 && ticket->is_stackable() &&
//         ticket->GetItemType() == 0xd)
//     {
//         bool announce = false;
//         int rarityGate = *(int*)((char*)G_CDataManager() + 0x5c0);
//         if (rarityGate >= prize->get_rarity()) {
//             announce = true;
//         } else {
//             if (prize->IsAvatarItem()) {
//                 int catIdx = prize->GetItemType();
//                 if (*((char*)G_CDataManager() + 0x5c4 + catIdx) != 0)
//                     announce = true;
//             }
//             if (!announce) {
//                 int maxUp = *(int*)((char*)G_CDataManager() + 0x5e0);
//                 if (maxUp <= (int)(up & 0xff)) announce = true;
//             }
//         }
//         if (announce) {
//             bool skip;
//             if (getMoveSpace() == (UserSpace::T)1 ||
//                 prize->IsRandomOption())            // vtable+0x4c
//                 skip = true;
//             else
//                 skip = false;
//             if (!skip) {
//                 PacketGuard bp;
//                 bp.put_header(0, 0x56);
//                 bp.put_byte(2);
//                 bp.put_byte(1);
//                 bp.put_short(get_unique_id() & 0xffff);
//                 bp.put_int(lottoItem.m_addInfo);
//                 bp.put_byte(up & 0xff);
//                 bp.finalize(true);
//                 G_GameWorld()->send_all(bp);
//             }
//         }
//     }
//     return 0;
// }

// >>> W7-BLOCKED CUser::multibox_lottery_item —— 同 lottery_item：expire_sec_word(+0x17)/m_amp 内部字段未具名
// ----------------------------------------------------------------------------
// ORIG 0x08674968 CUser::multibox_lottery_item(int boxSlot, int keySlot) int
// (报告 CUser/multibox_lottery_item.md)
// int CUser::multibox_lottery_item(int boxSlot, int keySlot)
// {
//     Inven_Item keyItem;                                   // -0x14d(-0x151 起)
//     std::vector<std::pair<Inven_Item, bool> > results;    // -0x78
//     int ret;                                              // -0x4c
//
//     keyItem = getCurCharacInvenR()->GetInvenSlot(1, keySlot);
//
//     if (keyItem.m_addInfo == 0x2061 || keyItem.m_addInfo == 0x289843)
//         ret = process_use_unlocked_multibox(keySlot, boxSlot, results);
//     else
//         ret = process_use_locked_multibox(keySlot, boxSlot, results);
//
//     if (ret != 0) {
//         if (ret == 0x13) {
//             CBoosterGage* gage = (CBoosterGage*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(0x11));
//             gage->send_error_state_packet(this, keyItem.m_addInfo);
//             return -1;
//         }
//         return ret;
//     }
//
//     CBoosterGage* gage = (CBoosterGage*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(0x11));
//     bool boosted = gage->cal_booster_gage(keyItem.m_addInfo, this) ? true : false;
//     if (boosted)
//         gage->log_booster_gage_bonus_reward(this, results);
//
//     PacketGuard packet;
//     packet.put_header(1, 0xd3);
//     packet.put_byte(1);
//     packet.put_short(keySlot);
//     packet.put_short(boxSlot);
//     packet.put_short(results.size());
//
//     std::vector<std::pair<int, int> > invenUpdates;       // -0x90
//
//     std::vector<std::pair<Inven_Item, bool> >::const_iterator it;
//     for (it = results.begin(); it != results.end(); ++it) {
//         Inven_Item cur = it->first;                       // local_18f 区拷贝
//         int resultSlot = -1;                              // -0x44/-0x48 合流槽
//         if (it->second /*stackable*/) {
//             if (cur.m_addInfo2 > 0 && cur.m_addInfo2 < 1000) {
//                 int coin = cur.m_addInfo2;                     // -0x30
//                 if (boosted)
//                     coin *= *(unsigned char*)((char*)G_CDataManager() + 0x877e);
//                 int evCoin = getCurCharacInvenR()->GetEventCoin();
//                 getCurCharacInvenW()->SetEventCoin(evCoin + coin);
//                 m_historyLog.EventCoinAdd(
//                     getCurCharacInvenR()->GetEventCoin(), coin,
//                     (eCoinAddReason)2);
//                 SaveMoneyCoin();
//                 SendCoin();
//                 packet.put_short((unsigned short)-1);
//                 packet.put_int(cur.m_addInfo);
//                 packet.put_int(cur.m_addInfo2);
//                 packet.put_short(cur.expire_sec_word & 0xffff);
//                 packet.put_byte(cur.GetItemAttr() & 0xff);
//                 packet.put_packet(cur);
//             }
//             continue;
//         }
//
        // 非堆叠分支
//         G_CDataManager();
//         CItem* def = G_CDataManager()->find_item(cur.m_addInfo);  // -0x34
//         if (def == 0) return 0x11;
//
//         if (def->IsAvatarItem()) {                        // vtable+0x10
//             char ipg[24]; memset(ipg, 0, 24);
//             short upgradeZero = 0;                         // ORIG 0x080f0ffc 置零 helper
//             memcpy(&cur.m_attr /*byte@+0x03*/ /*Inven_Item+0x03*/, &upgradeZero,
//                    sizeof(unsigned char));
//             unsigned int uid = WongWork::CGenUniqueNo::genIPGNo(
//                 (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)1,
//                 getCurCharacNo(), ipg);
//             unsigned int period = def->getUsablePeriod();
//             resultSlot = gage->AddAvatarItem(invenUpdates, boosted, this,
//                                              cur, period, false, '\0', '\0',
//                                              ipg, (eAvatarItemAddReason)7,
//                                              resultSlot);
//         } else if (def->IsCreatureEgg() &&                         // vtable+0x14
//                    def->GetItemType() == 0x16) {                   // vtable+0xc
//             if ((unsigned char)(cur.m_attr & 0x1f) == 5) {
//                 if (user_creature::CCreatureItemConverter::SetEggItem(&cur)) {
//                     char ipg[24]; memset(ipg, 0, 24);
//                     unsigned int uid = WongWork::CGenUniqueNo::genIPGNo(
//                         (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)1,
//                         getCurCharacNo(), ipg);
//                     unsigned short period = def->getUsablePeriod();
//                     resultSlot = gage->InsertCreatureItem(
//                         invenUpdates, boosted, this, cur,
//                         (eItemAddReason)3, true, false, period,
//                         def->getExpirationDate(), uid);
//                 } else {
//                     resultSlot = -1;
//                 }
//             } else {
//                 resultSlot = -1;
//             }
//         } else {
//             int restrictCode = 0;                                  // -0xd0
//             int bonusVal     = 0;                                  // -0xd4
//             bool reqFlag     = false;                              // -0xd5
//             if (GlobalData::s_pCeraShop->IsSpecialItem(cur.m_addInfo,
//                                                        bonusVal, restrictCode,
//                                                        reqFlag)) {
                // 特殊商城道具
//                 CItem* sDef = G_CDataManager()->find_item(cur.m_addInfo);
//                 bool procOk = true;
//                 if (!(sDef->GetItemType() == 0x10 ||
//                       sDef->GetItemType() == 0x22)) {
//                     procOk = true;
//                     if (procOk) {
//                         int codeOut = 0;                           // -0xdc
//                         G_CDataManager();
//                         restrictCode =
//                             ((CItemList*)((char*)G_CDataManager() + 0xc))
//                                 ->GetRestrictCode(cur.m_addInfo);
//                         long now = GlobalData::s_systemTime_.getCurSec();
//                         std::vector<Inven_Item> outs;              // -0x100
//                         WongWork::CCeraShop::stCeraShopItemParam_t param;
//                         bool ok = GlobalData::s_pCeraShop->ProcessSpecialItem(
//                             this, cur.m_addInfo, keyItem.m_addInfo, codeOut,
//                             now, true, outs, param, '\0', '\0', boosted, 1);
//                         if (ok) {
//                             if (codeOut != 0) resultSlot = -1;
//                             if (restrictCode != 0 && !IsRestrictedGoods(restrictCode)) {
//                                 SetRestrictedGoods(restrictCode);
//                                 GlobalData::s_pCeraShop->SaveFeaturedIdx(
//                                     this, restrictCode, now, 0);
//                                 SendRestrictedGoods(true, restrictCode);
//                             }
//                         }
//                         resultSlot = 0xffff;
//                     }
//                 } else {
//                     resultSlot = gage->insertItemIntoInventory(
//                         invenUpdates, boosted, this, cur,
//                         (eItemAddReason)3, true);
//                 }
//             } else {
//                 resultSlot = gage->insertItemIntoInventory(
//                     invenUpdates, boosted, this, cur,
//                     (eItemAddReason)3, true);
//             }
//
//             if (resultSlot != -1 && def != 0) {
//                 int price = 0;                                 // -0x104
//                 Store::GetSellItemPrice(cur, def, (short)cur.m_addInfo2,
//                                         false, price);
//                 if (boosted)
//                     price *= *(unsigned char*)((char*)G_CDataManager()
//                                                + 0x877e);
//                 GetInstanceValueStatistic()->AddValueStatistic(
//                     (VALUE_STATISTIC_FIELD)7, this, price);
//             }
//         }
//
//         if (resultSlot == -1) {
//             packet.put_short((unsigned short)-1);
//             packet.put_int(cur.m_addInfo);
//             packet.put_int(cur.get_add_info());
//             packet.put_short(cur.expire_sec_word & 0xffff);
//             packet.put_byte(cur.GetItemAttr() & 0xff);
//             packet.put_packet(cur);
//         } else {
//             packet.put_short(resultSlot & 0xffff);
//             packet.put_int(cur.m_addInfo);
//             packet.put_int(cur.get_add_info());
//             packet.put_short(cur.expire_sec_word & 0xffff);
//             packet.put_byte(cur.GetItemAttr() & 0xff);
//             packet.put_packet(cur);
//         }
//     }
//
//     packet.put_byte(boosted ? 1 : 0);
//     packet.finalize(true);
//     Send(packet);
//
    // 库存刷新广播
//     {
//         std::vector<std::pair<int,int> >::const_iterator u;
//         for (u = invenUpdates.begin(); u != invenUpdates.end(); ++u)
//             SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)u->second, u->first);
//     }
//     InGameADManagerInstance()->CheckFailMultiBoxLottery(this, results);
//
    // 高价值命中全服公告（结果 second 字节非零）
//     std::vector<std::pair<Inven_Item,bool> >::const_iterator a;
//     for (a = results.begin(); a != results.end(); ++a) {
//         if (*(((const char*)&a->second) + 1)) {
//             PacketGuard bp;
//             bp.put_header(0, 0x56);
//             bp.put_byte(4);
//             bp.put_byte(1);
//             bp.put_short(get_unique_id() & 0xffff);
//             bp.put_int(a->first.m_addInfo);
//             bp.put_int(keyItem.m_addInfo);
//             bp.put_byte(a->first.GetUpgrade() & 0xff);
//             bp.finalize(true);
//             G_GameWorld()->send_all(bp);
//         }
//     }
//     use_multibox_statistic(keyItem.m_addInfo);
//     return 0;
// }

// ----------------------------------------------------------------------------
// ORIG 0x08646912 CUser::OnItemUnlockWaitTimeout(int expandIdx=2, int arg2)
// uint，实义返回 !CItemLock::OnItemUnlockWaitTimeout
unsigned int CUser::OnItemUnlockWaitTimeout(int expandIdx, int unused)
{
    item_lock::CItemLock* lock = (item_lock::CItemLock*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(expandIdx));
    return (unsigned int)(lock->OnItemUnlockWaitTimeout(this) ^ 1);
}

// ----------------------------------------------------------------------------
// ORIG 0x08681368 CUser::OnPunishUser() void  (circulation field 0x1c)
void CUser::OnPunishUser()
{
    if (getCurCharacR() == 0) return;
    unsigned int money = getCurCharacInvenR()->get_money();
    GetInstanceCirculationStatistic()->AddCirculationStatistic(
        (CIRCULATION_STATISTIC_FIELD)0x1c, this, money);
}

// ----------------------------------------------------------------------------
// ORIG 0x08681312 CUser::OnRestrictTrade() void  (circulation field 0x1d)
void CUser::OnRestrictTrade()
{
    if (getCurCharacR() == 0) return;
    unsigned int money = getCurCharacInvenR()->get_money();
    GetInstanceCirculationStatistic()->AddCirculationStatistic(
        (CIRCULATION_STATISTIC_FIELD)0x1d, this, money);
}

// ----------------------------------------------------------------------------
// ORIG 0x086800c6 CUser::onSelectCharacter() void
// 掉落穿透(MCAP)衰减：+0x8e40c(=m_mcap.m_field1c) 初值 100，
// 0..9 格扫背包计数金币道具 -> 扣 40 与 10*n；
// 第 10..21 格(跳过 11)数剩余格 -> >8 再扣 20；pvpGrade>9 再扣 20。
void CUser::onSelectCharacter()
{
    m_mcap.m_field1c = 100;
    m_mcap.resetIncRate();

    int richCount = 0;                                     // -0x20
    int rareCount = 0;                                     // -0x1c
    int i = 0;                                             // -0x14
    do {
        if (i > 9) break;
        // ORIG 0x086800c6：GetInvenSlot 按值返回，拷贝初始化（sret 直写），无默认构造
        Inven_Item tmp = getCurCharacInvenR()->GetInvenSlot(0, i);
            CItem* def = G_CDataManager()->find_item(tmp.m_addInfo);
            bool isRare = false;
            if (def == 0) {
                isRare = false;
            } else {
                isRare = (def->get_rarity() == 2);
            }
            if (isRare) ++rareCount;
        ++i;
    } while (true);

    if (richCount > 0) {
        m_mcap.m_field1c -= 0x28;
        m_mcap.m_field1c += richCount * -10;
    }
    m_mcap.decIncRate(rareCount * 100);

    int spareCount = 0;                                    // -0x1c(second loop)
    for (int j = 10; j <= 0x15; ++j) {                     // P2 有符号
        if (j != 0xb) {
            Inven_Item t2 = getCurCharacInvenR()->GetInvenSlot(0, j);   // ORIG sret 直写
            if (t2.m_addInfo != 0) ++spareCount;
        }
    }
    if (spareCount > 8) m_mcap.m_field1c -= 0x14;
    if (get_pvp_grade() > 9) m_mcap.m_field1c -= 0x14;
    if (m_mcap.m_field1c < 0) m_mcap.m_field1c = 0;
}

// >>> W7-BLOCKED CUser::Parsing —— CUser 网络接收区(m_recvOffset/Ptr/Buf@+0x110/tick@+0xe4/m_verifier/_header 13B)未建模
// ----------------------------------------------------------------------------
// ORIG 0x08647890 CUser::Parsing(int received) bool/int  (大函数)
// bool CUser::Parsing(int received)
// {
//     struct _header head;                                   // -0x79 (13B)
//     int remainTotal = m_recvOffset + received;             // -0x3c
//     unsigned int tick = GlobalData::s_systemTime_.getCurTickCount();
//     unsigned int zero = 0;
//     m_lastRecvTick = tick;                                 // +0xe4
//     m_recvGapTick  = 0;                                    // +0xe8
//
//     int headerLen = 0xd;                                   // -0x28
//     if (remainTotal <= 12) {
//         m_recvOffset  += received;                         // +0xf8
//         m_recvPtr     += received;                         // +0x10c
//         cMyTrace trace((char*)__PRETTY_FUNCTION__, 0x69d, 0);
//         trace("(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d]",
//               m_recvPtr, m_recvBuf, m_recvOffset, received);
//         m_bBufOffsetTrace = true;                          // +0x109
//         return 1;
//     }
//
//     int consumed = 0;                                      // -0x28
//     do {
//         if (m_recvOffset != 0)
//             m_recvPtr = m_recvPtr - m_recvOffset;          // +0x10c 修正
//         if (m_bBufOffsetTrace) {
//             cMyTrace tr((char*)__PRETTY_FUNCTION__, 0x6b2, 0);
//             tr("[BUFOFFSET TRACE](offset:%x - buf:%x) = remainlen:%d, Recv Size[%d]",
//                m_recvPtr, m_recvBuf, m_recvOffset, received);
//         }
//         memcpy(&head, (void*)m_recvPtr, 0xd);              // header 13B
//         consumed = head.length;                            // local_7a (+7)
//         if (head.length < 0xd || head.length > 0x578) {
//             LogManager::logFormat(1, "user.cpp",
//                 __PRETTY_FUNCTION__, 0x6ce,
//                 "Recv Size[%d], Parsing Packet Size[%d], recvbuf_offset[%d] is Too Large",
//                 received, head.length, m_recvPtr);
//             return 0;
//         }
//         if (remainTotal < consumed) break;
//
//         int verdict = 0;                                   // -0x20
//         verdict = m_verifier.UpdatePacket(&head);
//         if (verdict != 0) {
//             LogManager::logFormat(1, "user.cpp",
//                 __PRETTY_FUNCTION__, 0x6f4,
//                 "[Packet Varifier Error %d][classification %d][packet id %d][ServerSequence: %d][ClientSequence: %d](m_id: %s)",
//                 verdict, (unsigned)(unsigned char)((char*)&head)[0],
//                 (unsigned)head.id, m_verifier.GetSequence(),
//                 (unsigned)head.clientSeq,
//                 NumberToString(get_acc_id(), 0));
//             if (verdict != 4) return 0;
//         }
//         if (verdict == 0) {
//             _NS_PI_2ND_RecvHash_CS((char*)m_recvPtr, this, consumed);
//             if (head.id != 0x25c) {
//                 unsigned char dec[1400];                   // -0x604
//                 size_t decLen = 0x578;                     // -0x88
//                 G_Cipher()->Decrypt(head.id,
//                     (const unsigned char*)(m_recvPtr + 0xd),
//                     consumed - 0xd, dec, (int*)&decLen);
//                 if ((int)decLen > 0)
//                     memcpy((void*)(m_recvPtr + 0xd), dec, decLen);
//                 unsigned int crc = Encryption::crc32N(
//                     (void*)(m_recvPtr + 0xb), consumed - 0xb);
//                 unsigned int sumRef = head.checksumByteField;         // -0x76→-0x72
//                 _NS_PI_2ND_MakeChecksumTo1Byte2(crc, crc);
//                 if (sumRef != crc) {
//                     cMyTrace ct((char*)__PRETTY_FUNCTION__, 0x73a, 5);
//                     ct("[CHECK SUM] check_sum error PACKET : %s",
//                        GetPacketName(head.cls, head.id));
//                     return 0;
//                 }
//             }
//             int frameSize = consumed + 8;                  // -0x1c
//             CStreamGuard inner(true);
//             if (frameSize <= 0xfff) {
//                 inner.Set(GlobalData::s_small_stream_pool->Acquire("user.cpp", 0x758));
//             } else if (frameSize <= 0x3fff) {
//                 inner.Set(GlobalData::s_stream_pool->Acquire("user.cpp", 0x75c));
//             } else {
//                 inner.Set(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x760));
//             }
//             inner << (int)0;
//             inner << (int)GetUID();
//             inner.put_binary((void*)m_recvPtr, consumed);
//             GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, inner);
//             m_bBufOffsetTrace = false;
//         }
//         remainTotal -= consumed;
//         m_recvPtr   += consumed;
//         m_recvOffset = 0;
//         if (remainTotal == 0) {
//             m_recvPtr = (int)m_recvBuf;                    // 回到 +0x110 缓冲头
//             break;
//         }
//     } while (remainTotal > 12);
//
//     m_bBufOffsetTrace = false;
//     if (remainTotal > 0) {
//         if (remainTotal > 0x1000) {
//             try {
//                 LogManager::logFormat(1, "user.cpp",
//                     __PRETTY_FUNCTION__, 0x793,
//                     "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
//                     remainTotal);
//             } catch (...) {}
//             return 0;
//         }
//         memmove(m_recvBuf, (void*)m_recvPtr, remainTotal);
//         m_recvOffset = remainTotal;
//         m_recvPtr = (int)m_recvBuf + remainTotal;
//     }
//     return 1;
// }

// ----------------------------------------------------------------------------
// ORIG 0x0867ee28 CUser::printBlackList() void  (LogManager Pretty 行 0x7d57)
void CUser::printBlackList()
{
    if (m_vec8cef4.empty()) return;
    char buf[1024];                                        // -0x518
    buf[0] = '\0';
    for (size_t i = 0; i < m_vec8cef4.size(); ++i)
        sprintf(buf, "%s,%d", buf, m_vec8cef4[i]);
    sprintf(buf, "%s\n", buf);
    char path[256];                                        // -0x118
    path[0] = '\0';
    sprintf(path, "./log/%s/BlackList",
            G_CEnvironment()->get_file_name());
    CMyFileLog logFile(__PRETTY_FUNCTION__, 0x7d57);
    logFile(path, "%s", buf);
}

// >>> W7-BLOCKED CUser::process_recipe_avatar_items —— CInventory(InventoryMemory&) 影子构造与 recipe->materials()/outputs() getter 未建模
// ----------------------------------------------------------------------------
// ORIG 0x0868c4f4 CUser::process_recipe_avatar_items(
//     int mainSlot, bool useStackable /*param_2*/, unsigned short count,
//     int cmdHeader, CStackableItem const* recipe) int
// 返回 0 成功 / 4 插入失败 / 0x11 校验失败 / 0x13 useStackable /
// 0x16 装扮删除失败 / 0xd5 锁定或查无槽
// int CUser::process_recipe_avatar_items(int mainSlot, bool param2,
//                                        unsigned short count, int cmdHeader,
//                                        CStackableItem const* recipe)
// {
//     if (recipe == 0) return 0x11;
//     if (param2) return 0x13;
//
//     std::vector<std::pair<int,int> > materials(recipe->materials());  // +0x174
//     std::vector<std::pair<int,int> > outputs(recipe->outputs());      // +0x180
//     if (outputs.size() > 1) return 0x11;
//
//     std::vector<int> changeList;                           // -0xb4 三元组(kind,slot,cnt)
//
//     InventoryMemory membuf;                                // -0xca6b 对象
//     CInventory shadow(membuf);                             // 本地影子副本
//     shadow.setCopy(*getCurCharacInvenR());
//
//     if (!shadow.delete_item((INVEN_TYPE)1, mainSlot, count,
//                             (eItemDelReason)8, false))
//         return 0x11;
//     changeList.push_back(0);
//     changeList.push_back(mainSlot);
//     changeList.push_back(count);
//
//     for (size_t i = 0; i < materials.size(); ++i) {
//         G_CDataManager();
//         CItem* mat = G_CDataManager()->find_item(materials[i].first);
//         if (mat == 0) continue;
//         if (mat->IsAvatarItem()) {                         // vtable+0x10
//             int avatarSlot = shadow.get_avatar_slot_no(materials[i].first);
//             if (avatarSlot == -1) return 0xd5;
//             if (CheckItemLock(2, avatarSlot)) return 0xd5;
//             changeList.push_back(1);
//             changeList.push_back(avatarSlot);
//             changeList.push_back(1);
//         } else {
//             int invSlot = shadow.get_inven_slot_no(materials[i].first);
//             if (invSlot == -1) return 0xd5;
//             std::vector<std::pair<int,int> > one(1, materials[i]);
//             if (!shadow.tryDeleteEventItems(one)) return 0x11;
//             changeList.push_back(0);
//             changeList.push_back(invSlot);
//             changeList.push_back(materials[i].second);
//         }
//     }
//
//     G_CDataManager();
//     CItem* outDef = G_CDataManager()->find_item(outputs[0].first);
//     if (outDef == 0) return 0x11;
//
    // 删除阶段失败分支码：ORIG EBX=0x16(avatar)/0x11(normal)
//     enum DeleteBranch { BR_NONE = 0, BR_NORMAL = 1, BR_AVATAR = 2 };
//     DeleteBranch branchHit = BR_NONE;
//
//     PacketGuard packet;
//     CInventory* liveInv = getCurCharacInvenW();
//
//     std::vector<int>::const_iterator ci = changeList.begin();
//     while (true) {
//         if (ci == changeList.end()) break;
//         int kind   = *ci; ++ci;
//         int target = *ci; ++ci;
//         int amount = *ci; ++ci;
//         if (kind == 1) {                                   // avatar 装扮删除
//             Inven_Item chk;
//             chk = getCurCharacInvenR()->GetInvenSlot(2, target);
//             unsigned short avatarIdx = chk.m_addInfo;
//             if (!liveInv->delete_item((INVEN_TYPE)2, target, 1,
//                                       (eItemDelReason)3, true)) {
//                 branchHit = BR_AVATAR;                     // 失败走 deleteFailed 分支
//                 break;
//             }
//             liveInv->GetAvatarItemMgrW()->UnRegistItem(avatarIdx);
//             liveInv->SendAvatarEvent(0x36, avatarIdx, 1, target + 10, 0);
//         } else if (kind == 0) {                            // 普通背包删除
//             if (!liveInv->delete_item((INVEN_TYPE)1, target, amount,
//                                       (eItemDelReason)8, true)) {
//                 branchHit = BR_NORMAL;
//                 break;
//             }
//         }
//     }
//
//     if (branchHit == BR_NONE) {
//         char ipg[24]; memset(ipg, 0, 24);
//         WongWork::CGenUniqueNo::genIPGNo(
//             (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)7,
//             getCurCharacNo(), ipg);
//         unsigned int period = outDef->getUsablePeriod();
//         int newAvatarSlot = liveInv->AddAvatarItem(outDef->get_index(),
//             period, false, '\0', '\0', -1, ipg,
//             (eAvatarItemAddReason)2, false, 0);
//         if (newAvatarSlot == -1) return 4;                 // EBX=4
//         Inven_Item made;
//         made = getCurCharacInvenR()->GetInvenSlot(2, newAvatarSlot);
//         packet.put_header(1, cmdHeader);
//         packet.put_byte(1);
//         packet.put_byte(changeList.size() / 3);
//         for (ci = changeList.begin(); ci != changeList.end();) {
//             packet.put_byte(*ci); ++ci;                    // kind
//             packet.put_short(*ci); ++ci;                   // slot
//             packet.put_int(*ci); ++ci;                     // cnt
//         }
//         packet.put_byte(1);
//         packet.put_byte(1);
//         packet.put_short(newAvatarSlot);
//         packet.put_int(made.get_add_info());
//         packet.put_int(made.m_addInfo2);                  // +0x07 字段照抄
//         packet.put_byte(made.GetUpgrade() & 0xff);
//         packet.put_short(made.m_fieldb & 0xffff);
//         stAmplifyOption_t& amp =
//             *(stAmplifyOption_t*)((char*)&made + 0x12);
//         packet.put_byte(amp.getAbilityType() & 0xff);
//         packet.put_short(amp.getAbilityValue() & 0xffff);
//         packet.put_byte(made.isEmpty() != 0 ? 1 : 0);
//         packet.put_packet(made);
//         packet.finalize(true);
//         Send(packet);
//         SendUpdateItemList((eSendTarget)1, (ENUM_ITEMSPACE)1,
//                            newAvatarSlot);
//         HistoryLog::WriteComposition(*(FILE**)((char*)this + 0x796f8),
//                                      recipe->get_index());
//         return 0;
//     }
//     if (branchHit == BR_AVATAR)
//         return 0x16;                                       // 装扮删除失败
//     return 0x11;                                           // 普通删除失败
// }

// ----------------------------------------------------------------------------
// ORIG 0x08675ac6 CUser::process_use_locked_multibox(
//   int boxSlot, int keySlot, vector<pair<Inven_Item,bool>>& dest) int
int CUser::process_use_locked_multibox(int boxSlot, int keySlot,
                                       std::vector<std::pair<Inven_Item, bool> >& dest)
{
    Inven_Item key;
    key = getCurCharacInvenR()->GetInvenSlot(1, keySlot);

    G_CDataManager();
    CItem* boxDef = G_CDataManager()->find_item(key.m_addInfo);   // -0x18

    CBoosterGage* gage = (CBoosterGage*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(0x11));
    bool overGage = gage->check_max_booster_gage(key.m_addInfo);  // -0x11

    int err = check_locked_multibox_error(boxDef, keySlot, overGage);
    if (err != 0) return err;

    std::vector<std::pair<Inven_Item, bool> > lottery;             // -0x24
    lottery.clear();
    ((CStackableItem*)boxDef)->get_multibox_lottery_item(&lottery);

    err = check_insert_lottery_reward(boxDef, lottery, dest);
    if (err != 0) return err;

    int keyCount = *(int*)((char*)boxDef + 0x1f0);                 // 所需钥匙数
    getCurCharacInvenW()->delete_item((INVEN_TYPE)1, boxSlot, 1,
                                      (eItemDelReason)3, true);
    getCurCharacInvenW()->delete_item((INVEN_TYPE)1, keySlot, keyCount,
                                      (eItemDelReason)3, true);
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG 0x08675d0c CUser::process_use_unlocked_multibox(
//   int boxSlot, int keySlot, vector<pair<Inven_Item,bool>>& dest) int
int CUser::process_use_unlocked_multibox(int boxSlot, int keySlot,
                                         std::vector<std::pair<Inven_Item, bool> >& dest)
{
    Inven_Item key;
    key = getCurCharacInvenR()->GetInvenSlot(1, keySlot);
    G_CDataManager();
    CItem* boxDef = G_CDataManager()->find_item(key.m_addInfo);   // -0x10
    int err = check_unlocked_multibox_error(boxDef, boxSlot);
    if (err != 0) return err;

    std::vector<std::pair<Inven_Item, bool> > lottery;             // -0x1c
    lottery.clear();
    ((CStackableItem*)boxDef)->get_multibox_lottery_item(&lottery);

    err = check_insert_lottery_reward(boxDef, lottery, dest);
    if (err != 0) return err;

    getCurCharacInvenW()->delete_item((INVEN_TYPE)1, boxSlot, 1,
                                      (eItemDelReason)3, true);
    return 0;
}

// >>> W7-BLOCKED CUser::processAwakenReward —— RDARScriptStringManager findString 变参 helper（草稿自造 RDARScriptStringManager_findString）需按权威 const char* findString(int,const char*,bool*) 重排
// ----------------------------------------------------------------------------
// ORIG 0x086792d6 CUser::processAwakenReward(char stage) void
// void CUser::processAwakenReward(char stage)
// {
//     int rewardItemId = 0x3c0;                              // -0x14
//     int sealCount    = 1;                                  // -0x10
//     switch (stage) {                                       // jump table, case 序 0..8
//     case 0:
//         strncpy(mailMsg, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_msg_1", 0), 0xff);
//         strncpy(mailTitle, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_title_1", 0),
//                 0x14);
//         break;
//     case 1: case 7:
//         strncpy(mailMsg, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_msg_2", 0), 0xff);
//         strncpy(mailTitle, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_title_2", 0),
//                 0x14);
//         break;
//     case 2: case 5:
//         strncpy(mailMsg, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_msg_3", 0), 0xff);
//         strncpy(mailTitle, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_title_3", 0),
//                 0x14);
//         break;
//     case 3: case 8:
//         strncpy(mailMsg, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_msg_4", 0), 0xff);
//         strncpy(mailTitle, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_title_4", 0),
//                 0x14);
//         break;
//     case 4:
//         strncpy(mailMsg, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_msg_5", 0), 0xff);
//         strncpy(mailTitle, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_title_5", 0),
//                 0x14);
//         break;
//     case 6:
//         strncpy(mailMsg, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_msg_6", 0), 0xff);
//         strncpy(mailTitle, RDARScriptStringManager_findString(
//                     g_scriptStringManager_, 4, "awaken_mail_title_6", 0),
//                 0x14);
//         break;
//     default:
//         return;
//     }
//     Inven_Item item;
//     G_CDataManager();
//     CItem* def = G_CDataManager()->find_item(rewardItemId);
//     if (def == 0) return;
//     item.m_addInfo = rewardItemId;                        // -0x64
//     def->make_item(item);                                  // vtable+0x8 槽 make_item
//     item.set_add_info(sealCount);
//     ENUM_SERVER_GROUP grp = GetServerGroup();
//     WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
//         mailTitle, item, 0, getCurCharacNo(), mailMsg,
//         strlen(mailMsg), 12, grp, false, false);
// }

// >>> W7-BLOCKED CUser::processFatigueCharac —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ----------------------------------------------------------------------------
// ORIG 0x086925c4 CUser::processFatigueCharac(int usedMin) void
// void CUser::processFatigueCharac(int usedMin)
// {
//     int job = get_charac_job();
//     int giftId = 0;
//     giftId = G_CDataManager()->get_useJobFatigueMatchGift(
//                  (char)job, getCurCharacUsedFatigue() & 0xffff, usedMin);
//     if (giftId == 0) return;
//
//     std::string title;                                     // -0x24
//     std::string fmtKey;                                    // -0x28
//     G_CDataManager()->get_useJobFatigueMatchGiftMsg(job, title, fmtKey);
//
//     Inven_Item item;
//     G_CDataManager();
//     CItem* def = G_CDataManager()->find_item(giftId);
//     unsigned char srvGrp = GetServerGroupToPvP();
//     if (def == 0) return;
//
//     int itemIndex = def->get_index();
//     def->make_item(item);
//     item.set_add_info(1);
//
//     unsigned short fatigue = getCurCharacUsedFatigue() & 0xffff;
//     char msg[255];
//     OS_API::snprintf(msg, 0xff,
//         RDARScriptStringManager_findString(g_scriptStringManager_, 4,
//                                            fmtKey.c_str(), 0),
//         fatigue);
//     if (itemIndex != 0) {
//         unsigned int expire = def->getExpirationDate();
//         WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
//             RDARScriptStringManager_findString(g_scriptStringManager_, 4,
//                                                title.c_str(), 0),
//             item, 0, getCurCharacNo(), msg, strlen(msg), expire,
//             srvGrp, false, true);
//     }
// }

// >>> W7-BLOCKED CUser::processGrowTypeEvent —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ----------------------------------------------------------------------------
// ORIG 0x0867925a CUser::processGrowTypeEvent(unsigned char a, unsigned char b,
//                                              eChangeGrowTypeReason reason)
// void CUser::processGrowTypeEvent(unsigned char p1, unsigned char p2,
//                                  eChangeGrowTypeReason reason)
// {
//     if (reason == 1) return;
//     _onChangeGrowType(p1, p2, reason);
//     EventClassify::CEventScriptMng* emng =
//         G_CDataManager()->get_event_script_mng();
//     emng->process_level_up_reward(this, true);
//     if (p2 == 0)
//         processLevelUpEventReward(true);
// }

// ----------------------------------------------------------------------------
// ORIG 0x08691ec4 CUser::processReturnUser() void
void CUser::processReturnUser()
{
    if (!IsReturnUser()) return;
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x12d);
    packet.put_int(GetReturnUserExpireTime()
                   - GlobalData::s_systemTime_.getCurSec());
    packet.finalize(true);
    Send(packet);

    if (IsReturnUserFirstLogin()) {
        GiveItemsReturnUser();
        GiveEquipItemsReturnUser();
        DB_UpdateReturnUserFirstLogin::makeRequest(get_acc_id(),
            GetReturnUserExpireTime(),
            IsReturnUserFirstLogin() & 0xff);
        SetReturnUserFirstUserLogin(false);
    }
}

// >>> W7-BLOCKED CUser::quest_action —— a4 局部来源、errorCode/giveup 返回语义与 HistoryLog::WriteFineQuest 调用链待核
// ----------------------------------------------------------------------------
// ORIG 0x0866da8a CUser::quest_action(int cmd, int questId, int a3, int a4)
// cmd: 0x21 accept / 0x22 giveup / 0x23 trigger / 0x24 finish(reward)
// void CUser::quest_action(int cmd, int questId, int triggerVal, int opt)
// {
//     G_CDataManager();
//     Quest* q = G_CDataManager()->find_quest(questId);       // -0x30
//     if (q == 0) return;
//     if (q->type == 7) return;                               // q+0x8
//
//     PacketGuard packet;
//     packet.put_header(1, cmd);
//
//     if (cmd == 0x22) {                                       // giveup
//         UserQuest* uq = getCurCharacQuestW();
//         (void)uq->giveup_quest(questId);   // -0x34
//         if (errorCode <= 0) {
//             packet.put_byte(1);
//             packet.put_short(questId);
//             G_CDataManager();
//             Quest* q2 = G_CDataManager()->find_quest(questId);
//             if (q2 == 0) goto LAB_END;
//             if (!q2->m_triggerDataPairs.empty()) {                // q+0x70
//                 bool touched = false;                        // -0x51
//                 std::vector<int> updates;
//                 updates.reserve(q2->m_triggerDataPairs.size() * 3);
//                 CInventory* inv = getCurCharacInvenW();
//                 touched = false;
//                 inv->delete_event_items(q2->m_triggerDataPairs, updates,
//                                         touched, (eMoneySubReason)4,
//                                         (eItemDelReason)4, false);
//                 std::vector<int>::const_iterator ui = updates.begin();
//                 while (ui != updates.end()) {
//                     int spId = *ui; ++ui;
//                     int cntS = *ui; ++ui;                    // short
//                     SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)spId, cntS);
//                     ++ui;                                    // 第三元素只读丢弃
//                 }
//                 SendHasNoGoWithApcQuestInParty(q2);
//             } else {
//                 SendHasNoGoWithApcQuestInParty(q2);
//             }
//         } else {
//             packet.put_byte(0);
//             packet.put_byte(errorCode);
//         }
//         packet.finalize(true);
//         Send(packet);
//     }
//     else if (cmd == 0x21) {                                  // accept
//         G_CDataManager();
//         Quest* qa = G_CDataManager()->find_quest(questId);
//         bool revengeBlock =
//             (qa != 0) && (qa->m_field134 == 1) &&
//             !GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon();
//         bool scheduleBlock =
//             (qa != 0) && (qa->m_field134 == 2) &&
//             !qa->IsOpenScheduleQuest();
//         if (revengeBlock || scheduleBlock) {
//             packet.put_byte(0);
//             packet.put_byte(0x15);
//             packet.finalize(true);
//             Send(packet);
//         } else {
//             UserQuest* uq = getCurCharacQuestW();
//             int local42 = 0;                                  // -0x40
//             int errCode = uq->accept_quest(questId, (char*)&packet, local42);
//             if (errCode > 0) {
//                 packet.put_byte(0);
//                 packet.put_byte(errCode);
//             }
//             packet.finalize(true);
//             Send(packet);
//         }
//     }
//     else if (cmd == 0x23) {                                  // trigger
//         char b = (char)a4;
//         UserQuest* uq = getCurCharacQuestW();
//         int trigResult = uq->set_trigger(questId, (char)triggerVal, b != 0);
//         if (trigResult == -1) {
//             packet.put_byte(0);
//             packet.put_byte(0x16);
//         } else {
//             packet.put_byte(1);
//             packet.put_short(questId);
//             packet.put_int(trigResult);
//             if (trigResult == 0) {
//                 G_CDataManager();
//                 Quest* qt = G_CDataManager()->find_quest(questId);
//                 if (qt == 0) goto LAB_END;
//                 SendHasNoGoWithApcQuestInParty(qt);
//             }
//         }
//         packet.finalize(true);
//         Send(packet);
//     }
//     else if (cmd == 0x24) {                                  // finish
//         unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
//         if ((now - W7_CU_INT(0x79644)) <= 0x3e8) {      // 近期已请求过
//             ++W7_CU_INT(0x79648);
//             if (1 < W7_CU_INT(0x79648)) {
//                 WongWork::CHackAnalyzer* ha = getHackAnalyzer();
//                 ha->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x192,
//                                      1, 0, 0);
//                 packet.put_byte(0);
//                 packet.put_byte(1);
//                 packet.finalize(true);
//                 Send(packet);
//                 goto LAB_END;
//             }
//         } else {
//             W7_CU_INT(0x79648) = 0;
//             W7_CU_INT(0x79644) = GlobalData::s_systemTime_.getCurTickCount();
//         }
//         int err = quest_reward(questId, triggerVal, opt);
//         if (err > 0) {
//             packet.put_byte(0);
//             packet.put_byte(err);
//             packet.finalize(true);
//             Send(packet);
//             HistoryLog::WriteFineQuest(*(FILE**)((char*)this + 0x796f8), questId);
//         } else {
//             std::list<int> mailQuestIds;                     // -0x68
//             stSelectQuestParam selParam(this);               // -0xa0
//             bool hasMailQ = getCurCharacQuestR()
//                 ->get_mail_quest_info(mailQuestIds, selParam);
//             if (hasMailQ) {
//                 std::_List_iterator<int> mi = mailQuestIds.begin();
//                 while (mi != mailQuestIds.end()) {
//                     int mqId = *mi;
//                     G_CDataManager();
//                     Quest* mq = G_CDataManager()->find_quest(mqId);
//                     if (mq != 0 && mq->m_mailFlag == 1 && mq->m_index == questId)
//                         _postQuestTriggerItem(mq);
//                     ++mi;
//                 }
//             }
//             getCurCharacQuestW()->finish_quest(questId);
//             {
//                 PacketGuard qi;
//                 getCurCharacQuestR()->get_quest_info((char*)&qi);
//                 Send(qi);
//             }
//             G_CDataManager();
//             Quest* qz = G_CDataManager()->find_quest(questId);
//             if (qz != 0 && qz->type == 3)
//                 APSystem::CUserProc::ClearActionAndSendtoUser(this,
//                     (APSystem::_ActionGroupIndex)0xe, 0, (ENUM_PACKETCLASS)0);
//             m_historyLog.QuestComplete(questId);
//             _onQuestClear(questId, opt);
//         }
//     }
// LAB_END:
//     ;
// }

// ----------------------------------------------------------------------------
// CUser::quest_reward —— 已按当前权威类型释放（LinkFinal 批）：m_rewardSelectList=vector<pair<int,int>>（ORIG makeRewardItemInfo pair 重载在案）、vtable+0x10 无参 bool 虚调用 = IsAvatarItem（ORIG 0x866fd47 证据）。
// ----------------------------------------------------------------------------
// ORIG 0x0866ea92 CUser::quest_reward(int questId, int selectIdx, int repeatN) int
// 返回 0 ok / 4 背包不足 / 0xa.. / 0x11 材料/条件失败 / 0x15 / 0x16 条件不满足
int CUser::quest_reward(int questId, int selectIdx, int repeatN)
{
    PacketGuard ack;
    ack.put_header(1, 0x24);
    ack.put_byte(1);
    ack.put_short(questId);

    if (!getCurCharacQuestR()->check_cond_clear(questId)) {
        return 0x16;
    }
    G_CDataManager();
    Quest* q = ((QuestList*)(*(int*)((char*)G_CDataManager() + 0x18)))
               ->find_quest(questId);
    if (q == 0) return 0x15;

    bool gm = IsGameMasterMode();
    if (!gm && !q->isRepeatableQuest()) {
        if (((WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4))
                ->isClearedQuest(questId))
            return 0x16;
    }
    if (!(q->m_type == 4 || repeatN == 1)) return 0x16;

 // 需求材料区（needItems +0x7c）：count==0/25 时按 repeatN 缩放校验与预演
    std::vector<std::pair<int,int> > delCond;                  // -0x100
    std::pair<int,int> pairTmp;

    if (!(q->m_triggerType == 0 || q->m_triggerType == 0x19))
        return 0x11;
    {
        std::vector<std::pair<int,int> >::const_iterator di;
        for (di = q->m_triggerDataPairs.begin(); di != q->m_triggerDataPairs.end(); ++di) {
            pairTmp.first  = di->first;
            pairTmp.second = di->second * repeatN;
            delCond.push_back(pairTmp);
        }
        if (!getCurCharacInvenR()->CheckNeedItemList(delCond))
            return 0x11;                                       // ORIG +0x25d
    }

    {
        InventoryMemory qmem;                                  // -0xcb37（ORIG C1 0x822d3b8）
        CInventory shadow;                                     // -0x9f8 副本
        shadow.SetInventoryMemory(&qmem);                      // ORIG 0x822d4f6
        shadow.setCopy(*getCurCharacInvenR());
        std::vector<int> swapList;                             // -0x158
        bool okAll = false;

        if (!shadow.tryDeleteEventItems(delCond))              // 扣料预演失败
            return 0x11;                                       // -0x158 两出口同码

        if (q->m_fieldB8 /*+0xb8 计数哨兵*/ != 0) {                       // +0xb8 非空
            std::vector<RewardItemInfo>* pool =
                getUserQuestRewardItem(q, this);               // -0x88
            std::vector<std::pair<int,int> > insertList;       // -0x10c
            std::vector<RewardItemInfo>::const_iterator ri;
            for (ri = pool->begin(); ri != pool->end(); ++ri) {
                if (ri->m_job == 0xb ||
                    (ri->m_job == (int)get_charac_job() &&
                     (ri->m_growType == -1 ||
                      ri->m_growType == getCurCharFirstGrowType()))) {
                    pairTmp.first  = ri->m_itemIdx;
                    pairTmp.second = ri->m_count * repeatN;
                    insertList.push_back(pairTmp);
                }
            }
            if (shadow.tryInsertEventItems(insertList) !=
                (int)insertList.size())
                return 4;

            selectIdx = q->ConvertRewardSelectIndex(this, selectIdx);
            if ((int)q->m_rewardSelectList.size() <= selectIdx)
                return 0x15;
            if (selectIdx >= 0 && selectIdx != 0xffff) {
                Inven_Item selItem;                            // -0x205..-0x1c5
                std::pair<int, int>* sel = &q->m_rewardSelectList[selectIdx];   // ORIG pair 视图（makeRewardItemInfo pair 重载）
                selItem.m_addInfo = sel->first;                // item idx @+0x02（ORIG -0x1ff ← pair.first）
                G_CDataManager()->getItemList()->create_item(   // ORIG 0x866f069：接收者 = CDataManager+0xc 的 CItemList
                    (ENUM_ITEM_CREATE_TYPE)4, selItem,
                    sel->second * repeatN);                    // count = pair.second×repeatN
                if (shadow.tryInsertItemIntoInventory(selItem) == -1)
                    return 4;
            }
        }
        okAll = true;
    }

    int expGain = 0, goldGain = 0, qpGain = 0, pieceGain = 0;  // -0x118..-0x124
    quest_basic_reward(q, expGain, goldGain, qpGain, pieceGain, false);
    if (expGain != 0) {
        int curLevelExp = 0, newExp = 0;                       // -0x15c/-0x160
        set_charac_party_bonus_exp(0);
        set_charac_member_bonus_exp(0);
        set_charac_fatigue_buf_bonus_exp(0);
        char leveledUp = gain_exp_sp(expGain, curLevelExp, newExp,
                                     (CUser::eExpAddReason)13, 0, false);
        if (leveledUp) {
            if (getCurCharacStamina() < 100)
                setCurCharacStamina((unsigned char)'d');
            int st = get_state();
            if (st == 3) {
                SendNotiPacket((eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
                SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
            } else if (st > 4) {
                SendNotiPacket((eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
            }
        }
        if (m_chState == 3)
            SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
        if ((m_chState == 3) && leveledUp)
            decide_growth_power_reward_system();
    }

    bool touchedSlotEquip = false;                             // -0x125
    std::vector<int> updDelTriple;                             // -0x134(kind,tgt,cnt)*n
    std::vector<int> updAddQuad;                               // -0x140(idx,cnt,…)*n 四元组
    int delObjCnt   = 0;                                       // -0x9c
    int delAddObjCnt= 0;                                       // -0x98

    if ((q->m_triggerType == 0 || q->m_triggerType == 0x19) &&
        q->m_fieldB8 /*+0xb8 计数哨兵*/ != 0x19) {
        delObjCnt = getCurCharacInvenW()->delete_event_items(
            delCond, updDelTriple, touchedSlotEquip,
            (eMoneySubReason)4, (eItemDelReason)4, false);
        if (delObjCnt == 0) {
            cMyTrace tr((char*)__PRETTY_FUNCTION__,
                        0x59ef, 5);
            tr("CUser::quest_reward, QuestID : %d delete_object_count == 0 ",
               questId);
            return 0x11;
        }
        if (getGmQuestFlag())                                  // gm 免扣展示位归零
            delObjCnt = 0;
    }

    long moneyPaidSum  = 0;                                    // -0x94 已发金额
    long moneyOfferSum = 0;                                    // -0x90 应发金额
    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0) {                                  // +0xb8 金钱型奖励
        int moneyLimit = G_CDataManager()->GetMoneyLimitPerLevel(
                            get_charac_level(), get_acc_name()); // -0x80
        long curMoney = getCurCharacInvenR()->get_money();     // -0x7c
        std::vector<std::pair<int,int> > gains;                // -0x16c
        std::vector<RewardItemInfo>* pool =
            getUserQuestRewardItem(q, this);                   // -0x74
        std::vector<RewardItemInfo>::const_iterator ri;
        for (ri = pool->begin(); ri != pool->end(); ++ri) {
            if (ri->m_job == 0xb ||
                (ri->m_job == (int)get_charac_job() &&
                 (ri->m_growType == -1 ||
                  ri->m_growType == getCurCharFirstGrowType()))) {
                pairTmp.first  = ri->m_itemIdx;
                pairTmp.second = ri->m_count * repeatN;
                gains.push_back(pairTmp);
            }
        }
        if (selectIdx >= 0 && selectIdx < (int)q->m_rewardSelectList.size()) {
            std::pair<int, int>* sel = &q->m_rewardSelectList[selectIdx];   // ORIG pair 视图
            pairTmp.first  = sel->first;
            pairTmp.second = sel->second * repeatN;
            gains.push_back(pairTmp);
        }
        std::vector<std::pair<int,int> >::iterator gi;
        for (gi = gains.begin(); gi != gains.end(); ++gi) {
            if (gi->first == 0) {                              // 金钱项：上限封顶
                long offer = goldGain * repeatN;               // local_120*rpt
                moneyOfferSum = offer;
                if (curMoney + offer > (long)moneyLimit) {
                    if ((long)moneyLimit > curMoney) {
                        int allowed = moneyLimit - curMoney;   // -0x70
                        gi->second      = allowed;
                        moneyPaidSum   += allowed;
                        curMoney        = moneyLimit;
                    } else {
                        gi->second = 0;
                    }
                } else {
                    curMoney      += offer;
                    moneyPaidSum  += offer;
                    gi->second     = offer;
                }
            } else {
                SendAradEventItemLog(*(unsigned long*)&gi->first, gi->second);
            }
        }
        delAddObjCnt = getCurCharacInvenW()->insert_event_items(
            gains, updAddQuad, (eMoneyAddReason)3, (eItemAddReason)8,
            false, "", "");
        eventDeliveryQuestClear(questId, repeatN, gains,
                                expGain, goldGain, qpGain);
    }

    ack.put_byte(q->m_triggerType & 0xff);
    ack.put_int(expGain);
    if (q->m_triggerType == 0 || q->m_triggerType == 0x19) {
        if (q->m_triggerType == 0x19) {                       // 每日固定响应
            ack.put_byte(1);
            ack.put_byte(7);
            ack.put_short(0);
            ack.put_int(0);
        } else {
            ack.put_byte(delObjCnt);
            for (size_t k = 0; k < updDelTriple.size();) {     // 三元组
                ack.put_byte(updDelTriple[k]);
                ack.put_short(updDelTriple[k+1]);
                ack.put_int(updDelTriple[k+2]);
                k += 3;
            }
        }
    }
    ack.put_byte(q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ & 0xff);
    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0 && delAddObjCnt >= 0) {
        ack.put_byte(delAddObjCnt);
        for (size_t k = 0; k < updAddQuad.size();) {           // idx/cnt 摊平四元组
            ack.put_short(updAddQuad[k]);
            ack.put_int(updAddQuad[k+1]);
            ack.put_int(updAddQuad[k+2]);
            ack.put_int(updAddQuad[k+3]);
            G_CDataManager();
            CItem* addedDef = G_CDataManager()->find_item(updAddQuad[k]);
            unsigned char ampFlag = 1;
            if (addedDef != 0 &&
                addedDef->IsAvatarItem())                // vtable+0x10 无参 bool 虚调用（ORIG 0x866fd47）
                ampFlag = 2;
            Inven_Item chkItem;
            chkItem = getCurCharacInvenR()->GetInvenSlot(1, updAddQuad[k+1]);
            stAmplifyOption_t& amp =
                *&chkItem.m_amp;
            ack.put_byte(amp.getAbilityType() & 0xff);
            ack.put_short(amp.getAbilityValue() & 0xffff);
            if (addedDef == 0 || !addedDef->is_stackable())
                ack.put_short(0);
            else
                ack.put_short(chkItem.m_addInfo2 & 0xffff);
            k += 4;
        }
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 1) {                                  // 二转
        unsigned char secondType =
            (unsigned char)*(unsigned int*)((char*)q + 0xc0);
        ack.put_byte(secondType);
        set_grow_type(secondType, getCurCharSecondGrowType(), (char*)&ack, (eChangeGrowTypeReason)0);
        PacketGuard bp;
        bp.put_header(0, 2);
        bp.put_byte(0);
        bp.put_short(1);
        make_basic_info((char*)&bp, 0);
        bp.finalize(true);
        G_GameWorld()->send_all(bp, this);

        Stream* ss = GlobalData::s_stream_pool->Acquire("user.cpp", 0x5aed);
        CStreamGuard sig(ss, true);
        sig << (int)0x24;
        sig << (int)GetUID();
        SIG_LOAD_SKILL* sk =
            (SIG_LOAD_SKILL*)sig.GetInBuffer<SIG_LOAD_SKILL>();
        memset(sk, 0, 0x414);
        *reinterpret_cast<unsigned int*>(sk) = getCurCharacNo();   // +0x00 characNo（ORIG 0x867003b）
        sk->m_flag410 = 1;
        *((char*)sk + 0x411) = *((char*)G_CDataManager() + 0xa850);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, sig);
    }
    else if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 2) {                             // 一转
        unsigned char firstType =
            (unsigned char)*(unsigned int*)((char*)q + 0xc0);
        ack.put_byte(firstType);
        set_grow_type(getCurCharFirstGrowType(), firstType, (char*)&ack, (eChangeGrowTypeReason)0);
        PacketGuard bp;
        bp.put_header(0, 2);
        bp.put_byte(0);
        bp.put_short(1);
        make_basic_info((char*)&bp, 0);
        bp.finalize(true);
        G_GameWorld()->send_all(bp, this);
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 10 && *(int*)((char*)q + 0xc0) > 0) {
        int evoInto  = *(int*)((char*)q + 0xfc);
        int evoFrom  = *(int*)((char*)q + 0xc0);
        user_creature::CCreatureMgr* cm =
            getCurCharacInvenW()->GetCreatureMgrW();
        if (!cm->Evolute(evoFrom, evoInto, false))
            return 0x16;
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0x14) {                               // 专家职业
        if (get_charac_level() < *(int*)((char*)q + 0x20)) return 0x16;
        int expertType = *(int*)((char*)q + 0xc0);             // -0x4c
        SetCurCharacExpertJobType(expertType);
        ack.put_byte(expertType);
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x22, 0, (ENUM_PACKETCLASS)0);
        LogHistory("ExpertJob+,%d,%d", get_charac_level(), expertType);
        int posMarker = ack.get_index();                       // 计数回填点一
        int cnt = 0;
        ack.put_byte(0);
        STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(expertType);   // ORIG 0x8670387
        if (script != 0) {
            for (size_t s = 0; s < script->m_growSkillVec.size(); ++s) {
                if (getCurCharacR() != 0) {
                    int subIdx = script->m_growSkillVec[s].first;
                    int lvlReq = script->m_growSkillVec[s].second;
                    SkillSlot* skill = (SkillSlot*)getCurCharacSkillW();
                    int gained = skill->growtype_skill(get_charac_job(),
                                   subIdx, lvlReq, (ENUM_SKILL_TREE_KIND)0);
                    if (gained > 0) {
                        ack.put_byte(gained);
                        ack.put_byte(subIdx);
                        ack.put_byte(lvlReq);
                        ++cnt;
                    }
                }
            }
            ack.set_index(posMarker);
            ack.put_byte(cnt);   // 回填计数
            posMarker = ack.get_index();                       // 回填点二
            cnt = 0;
            ack.put_byte(0);
            for (size_t s = 0; s < script->m_growSkillVec.size(); ++s) {
                if (getCurCharacR() != 0) {
                    int subIdx = script->m_growSkillVec[s].first;
                    int lvlReq = script->m_growSkillVec[s].second;
                    SkillSlot* skill = (SkillSlot*)getCurCharacSkillW();
                    int gained = skill->growtype_skill(get_charac_job(),
                                   subIdx, lvlReq, (ENUM_SKILL_TREE_KIND)1);
                    if (gained > 0) {
                        ack.put_byte(gained);
                        ack.put_byte(subIdx);
                        ack.put_byte(lvlReq);
                        ++cnt;
                    }
                }
            }
            ack.set_index(posMarker);
            ack.put_byte(cnt);   // 回填计数
            expert_job::CCharacExpertJob* ej =
                (expert_job::CCharacExpertJob*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(0));
            ej->InitScript(script);
            SetCurCharacExpertJobExp(0);
        }
        PacketGuard bb;
        bb.put_header(0, 2);
        bb.put_byte(0);
        bb.put_short(1);
        make_basic_info((char*)&bb, 0);
        bb.finalize(true);
        G_GameWorld()->send_all(bb, this);
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0x15) {                               // 副本次数++
        int dungeonId = *(int*)((char*)q + 0xc0);
        ack.put_short(dungeonId);
        AddDungeonClear(dungeonId, 3);
        PacketGuard dp;
        dp.put_header(0, 5);
        dp.put_short(1);
        dp.put_short(dungeonId);
        dp.put_short(4);
        dp.finalize(true);
        Send(dp);
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0x16) {                               // 扩充栏位
        int slotMode = *(int*)((char*)q + 0xc0);
        char curStat = GetCurExEquipSlotStat();
        if (slotMode == 0x14) {
            if (curStat == 0 || curStat == 2)
                SetCurExpEquipSlotStat(curStat + 1);
        } else if (slotMode == 0x15) {
            if (curStat == 0 || curStat == 1)
                SetCurExpEquipSlotStat(curStat + 2);
        }
        ack.put_byte(GetCurExEquipSlotStat() & 0xff);
        ack.put_byte(slotMode);
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0x19) {                               // 宠物进化任务
        std::vector<std::pair<int,int> > evoList;              // -0x1bc
        std::vector<RewardItemInfo>* pool =
            getUserQuestRewardItem(q, this);                   // -0x20
        std::vector<RewardItemInfo>::const_iterator ri;
        for (ri = pool->begin(); ri != pool->end(); ++ri) {
            if (ri->m_job == 0xb ||
                (ri->m_job == (int)get_charac_job() &&
                 (ri->m_growType == -1 ||
                  ri->m_growType == getCurCharFirstGrowType()))) {
                pairTmp.first  = ri->m_itemIdx;
                pairTmp.second = ri->m_count * repeatN;
                evoList.push_back(pairTmp);
            }
        }
        if (selectIdx >= 0 && selectIdx < (int)q->m_rewardSelectList.size()) {
            std::pair<int, int>* sel = &q->m_rewardSelectList[selectIdx];   // ORIG pair 视图
            pairTmp.first  = sel->first;
            pairTmp.second = sel->second * repeatN;
            evoList.push_back(pairTmp);
        }
        if (evoList.size() == 1) {
            std::vector<std::pair<int,int> >::const_iterator ei;
            for (ei = evoList.begin(); ei != evoList.end(); ++ei) {
                G_CDataManager();
                CItem* tgt = G_CDataManager()->find_item(ei->first);
                if (tgt == 0) return 0x16;
                if (tgt->GetItemType() == 0x16) {              // vtable+0xc==22
                    user_creature::CCreatureMgr* cm =
                        getCurCharacInvenW()->GetCreatureMgrW();
                    if (!cm->EventEvoluteCreature(ei->first,
                            *(int*)((char*)q + 0xfc), ei->second))
                        return 0x16;
                    break;
                }
            }
        } else {
            return 0x16;
        }
    }

    ack.finalize(true);
    Send(ack);
    if (touchedSlotEquip)
        send_equip(0x16);
    if (moneyPaidSum < moneyOfferSum)
        SendMoneyFullReason((ENUM_MONEY_FULL_REASON)4,
                            moneyOfferSum, moneyPaidSum);
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG 0x08647ffc CUser::Recv() uint  (报告 CUser/Recv.md)
bool CUser::Recv()
{
    int n = m_network.recv_packet();
    if (n >= 1) {
        if (!Parsing(n)) {
            DisConnSig(DISCONN_SIG_4, false, 0);
            return 0;
        }
        return 1;
    }
    if (n < 0) {
        DisConnSig((DISCONN_SIG)5, false, 0);
        return 0;
    }
    return 1;
}

// >>> W7-BLOCKED CUser::Redeem_Item —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ----------------------------------------------------------------------------
// ORIG 0x08646df2 CUser::Redeem_Item(int redeemId) uint
// unsigned int CUser::Redeem_Item(int redeemId)
// {
//     Redeem_Item::CRedeemItem* mgr =
//         (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(6));     // -0x18
//     if (mgr == 0) {
//         SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
//         return 0;
//     }
//     if (private_store::GetInstancePrivateStoreMgr()
//             ->IsBusyPrivateStore(this)) {
//         cMyTrace t1((char*)__PRETTY_FUNCTION__, 0x529, 5);
//         t1("redeem error : trade : m_id(%s)charac_no(%u)",
//            NumberToString(get_acc_id(), 0), get_charac_no(-1));
//         SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
//         return 0;
//     }
//     if (CheckInTrade()) {
//         cMyTrace t2((char*)__PRETTY_FUNCTION__, 0x52f, 5);
//         t2("redeem error : trade : m_id(%s)charac_no(%u)",
//            NumberToString(get_acc_id(), 0), get_charac_no(-1));
//         SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
//         return 0;
//     }
//     int price = mgr->GetItemPrice(redeemId);                   // -0x14
//     if (price < 0) {
//         SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
//         return 0;
//     }
//     if (getCurCharacMoney() < price) {
//         SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 0x46);
//         return 0;
//     }
//     Inven_Item prize;                                          // -0xcc
//     mgr->GetRedeemInfoR(redeemId, prize);
//     Inven_Item* pPrize = &prize;                               // -0x10
//     if (pPrize->isEmpty()) {
//         SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
//         return 0;
//     }
//     int slotOut = getCurCharacInvenW()->insertItemIntoInventory(
//         *pPrize, (eItemAddReason)0x29, true, true);            // -0xc
//     if (slotOut < 0) {
//         SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 4);
//         return 0;
//     }
//     getCurCharacInvenW()->use_money(price, (eMoneySubReason)0x25, true);
//     G_CDataManager();
//     CItem* prizeDef = G_CDataManager()->find_item(pPrize->m_addInfo);
//     if (!prizeDef->is_stackable())
//         m_historyLog.RedeemItemDel(pPrize->m_addInfo, 1);
//     else
//         m_historyLog.RedeemItemDel(pPrize->m_addInfo,
//                                    pPrize->m_addInfo2);       // +7 字段照抄
//     mgr->RedeemItem(redeemId);
//
//     PacketGuard packet;
//     packet.put_header(1, 0x137);
//     packet.put_byte(1);
//     packet.put_int(slotOut);
//     packet.finalize(true);
//     Send(packet);
//     return 1;
// }

// ----------------------------------------------------------------------------
// ORIG 0x0867efae CUser::registerToBlackList(unsigned int accId) uint
unsigned int CUser::registerToBlackList(unsigned int accId)
{
    if (m_vec8cef4.size() >= 10) {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__, 0x7d62,
            "vt_black_list_.size() >= BLACKLIST_CNT_LIMIT");
        return 0;
    }
    if (isBlackUser(accId)) return 0;
    m_vec8cef4.push_back(accId);
    return 1;
}

// >>> W7-BLOCKED CUser::reqSendMailCertify —— TimerEntry::OBJ_TYPE/TIMER_MESSAGE 枚举实参序列与 capcha 队列契约未核
// ----------------------------------------------------------------------------
// ORIG 0x0868a51a CUser::reqSendMailCertify() uint
// unsigned int CUser::reqSendMailCertify()
// {
//     if (GlobalData::s_capchaDataManager.isEmpty()) return 0;
//     void* captcha = GlobalData::s_capchaDataManager.peekData();// -0xc
//     memcpy(m_capchaCertify, captcha, 10);
//
//     PacketGuard packet;
//     packet.put_header(0, 0x7d);
//     packet.put_byte(5);
//     packet.put_int(*(int*)((char*)captcha + 10));
//     packet.put_binary((char*)captcha + 0xe, *(int*)((char*)captcha + 10));
//     packet.put_byte(0);
//
//     unsigned short v1 = 0, v3 = 0;                             // -0x1a/-0x1e
//     unsigned char v2 = 0;                                      // -0x1b
//     GenerateCleanPadVeriData(v1, v2, v3);
//     packet.put_short(v1);
//     packet.put_byte(v2);
//     packet.put_short(v3);
//     packet.put_byte(m_humanCertifyErrorCnt & 0xff);
//     packet.put_byte(1);
//     packet.finalize(true);
//     Send(packet);
//
//     setHumanCertified(false);
//     m_field8e3ed = false;
//     if (m_humanCertifyTimerKey > 0x7ffffffe)
//         m_humanCertifyTimerKey = 0;
//     ++m_humanCertifyTimerKey;
//     int key = m_humanCertifyTimerKey;
//     G_TimerQueue()->InsertTimer(0 /*OBJ_TYPE_USER*/,
//                                 GetUID(), 9u,
//                                 60, key, (unsigned)-1);
//     return 1;
// }

// >>> W7-BLOCKED CUser::requestCubePremiumInfoIfContracted —— SIG_GET_PREMIUM_SELECTED_CUBE_INFO pad[0x4] 未具名化(SigTypes 属主)
// ----------------------------------------------------------------------------
// ORIG 0x086467ee CUser::requestCubePremiumInfoIfContracted() void
// void CUser::requestCubePremiumInfoIfContracted()
// {
//     if (getCurCharacR() == 0) return;
//     if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x5c)) return;
//
//     Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x475);
//     CStreamGuard guard(s, true);
//     guard << (int)0x332;
//     guard << (int)GetUID();
//     SIG_GET_PREMIUM_SELECTED_CUBE_INFO* sig =
//         (SIG_GET_PREMIUM_SELECTED_CUBE_INFO*)
//             guard.GetInBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO>();
//     sig->curCharacNo = getCurCharacNo();
//     GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
// }

// ----------------------------------------------------------------------------
// ORIG 0x0864f930 CUser::reset_tag_info_to_me(unsigned int tagAccId) void
void CUser::reset_tag_info_to_me(unsigned int tagAccId)
{
    bool touched = false;                                      // -0x9
    std::vector<_Charac_info>& chars = m_characList;           // +0x796e8
    std::vector<_Charac_info>::iterator it;
    for (it = chars.begin(); it != chars.end(); ++it) {
        if (*(unsigned int*)((const char*)&(*it) + 0x14b8) == tagAccId) {                        // +0x14b8
            *(unsigned int*)((char*)&(*it) + 0x14b8) = 0;
            (*(unsigned char*)((char*)&(*it) + 0x14b7)) = 0;                              // +0x14b7
            (*(unsigned char*)((char*)&(*it) + 0x14bc)) = 0; /*viewDirty 未具名*/                              // +0x14bc
            touched = true;
        }
    }
    if (touched)
        ((CCharacterView*)((char*)this + 0x796f4))->enableSaveCharacView();
}

// ----------------------------------------------------------------------------
// ORIG 0x0867f1a4 CUser::resetBlackList() void
void CUser::resetBlackList()
{
    if (!m_vec8cef4.empty())
        m_vec8cef4.clear();
}

// ----------------------------------------------------------------------------
// ORIG 0x0866259e CUser::ResetDailyMission() void
void CUser::ResetDailyMission()
{
    CMissionList_Charac* ml = (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(8));
    ml->Reset_DailyMission();
}

// ----------------------------------------------------------------------------
// ORIG 0x0866c196 CUser::resetGuildDBInfo() void
void CUser::resetGuildDBInfo()
{
    resetCharGuildInfo();                          // CUserCharacInfo 同基址转发
    memset(&m_guildDB, 0, 0xbd);                   // STGuildDBInfoOnly 0xbd 清零
}

// ----------------------------------------------------------------------------
// ORIG 0x0867f720 CUser::resetHumanCertifyData() void
void CUser::resetHumanCertifyData()
{
    memset(m_capchaCertify, 0, 5);
}

// ----------------------------------------------------------------------------
// ORIG 0x0866265a CUser::ResetMissionList() void
void CUser::ResetMissionList()
{
    if (isGMUser()) {
        CMissionList_Charac* ml =
            (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(8));
        ml->ResetMissionList(*this);
    }
}

// >>> W7-BLOCKED CUser::Return_Mercenary_Charac —— UserMercenaryInfoMgr::ReturnCharac(CUser*,uint,competition_reward_result&) 权威头缺 competition_reward_result
// ----------------------------------------------------------------------------
// ORIG 0x0868e692 CUser::Return_Mercenary_Charac(unsigned int uid,
//     competition_reward_result& out) void
// void CUser::Return_Mercenary_Charac(unsigned int mercUid,
//                                     competition_reward_result& reward)
// {
//     m_mercenaryList.ReturnCharac(this, mercUid, reward);       // +0x6ef6c
// }

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ORIG 0x0868148c CUser::SaveBackupSkillInfo(SIG_LOAD_PREMIUM_LETHE* src,
//                                           ENUM_SKILL_TREE_KIND kind)
void CUser::SaveBackupSkillInfo(SIG_LOAD_PREMIUM_LETHE* src,
                                ENUM_SKILL_TREE_KIND kind)
{
    if (getCurCharacR() == 0) return;
    if (kind == (ENUM_SKILL_TREE_KIND)-1 || kind == (ENUM_SKILL_TREE_KIND)0) {
        const char* dst = GetCurCharacBackupSkill();
        if (dst != 0) memcpy((void*)dst, src, 0x199);
    } else {
        const char* dst = GetCurCharacBackupSkill2ND();
        if (dst != 0) memcpy((void*)dst, src, 0x199);
    }
}

// ----------------------------------------------------------------------------
// ORIG 0x086528b4 CUser::saveCharacLinkMessageFlag(bool flag) void
void CUser::saveCharacLinkMessageFlag(bool flag)
{
    setCharacLinkMessageFlag(flag);
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1c66);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<((int)0x1e4);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(get_acc_id());
    guard.operator*()->operator<<(flag);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ----------------------------------------------------------------------------
// ORIG 0x086501c8 CUser::SaveInventoryOnly() uint
unsigned int CUser::SaveInventoryOnly()
{
    if (isLocked4DataLoad()) {
        cMyTrace trace((char*)__PRETTY_FUNCTION__, 0x187a, 5);
        trace("Can not save character until loading done(m_id: %s, charac_no: %d",
              NumberToString(get_acc_id(), 0), getCurCharacNo());
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 7) return 0;

    Stream* s = GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1888);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<(0x23);           // ORIG 0x086501c8：deEv→lsEi 链
    guard.operator*()->operator<<((int)GetUID());  // ORIG 同型
    SIG_SAVE_INVENTORY* inv = (SIG_SAVE_INVENTORY*)
        guard.operator->()->GetInBuffer<SIG_SAVE_INVENTORY>();  // ORIG ptEv 链

    _GetSaveInvenBase(inv);
    inv->m_saveFlag = 2; /*ORIG sig+0x08 save type*/                              // +0x08 save type

    memcpy(inv->m_invenType0,  getCurCharacInvenR()->GetInvenStart(0), 0x53e);
    memcpy(inv->m_invenType1,    getCurCharacInvenR()->GetInvenStart(1), 0x4a58);
    *(unsigned int*)&inv->m_cargoCapacity =
        getCurCharacCargoR()->GetCapacity();
    memcpy(inv->m_cargo,
           getCurCharacCargoR()->get_cargo_start(), 0x2438);
    memcpy(inv->m_invenType3 /*invenRewardZone*/,
           getCurCharacInvenR()->GetInvenStart(3), 0x39aa);
    memcpy(inv->m_creatureTail,
           (char*)getCurCharacInvenR() + 0x55a, 0xf4);

    m_characExpand.update(this, (ENUM_CHARAC_EXPAND_TYPE)9);
    removeRentalItemInfoFromInvenBuf(inv, 1);
    removeRentalItemInfoFromInvenBuf(inv, 2);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 1;
}

// >>> W7-BLOCKED CUser::SaveMercenaryData —— CHARAC_LOAD_MERCENARY save_complete/dirtyByte24 完成标记布局缺失
// ----------------------------------------------------------------------------
// ORIG 0x0868e36a CUser::SaveMercenaryData() uint
// unsigned int CUser::SaveMercenaryData()
// {
//     if (m_mercenaryList.empty()) return 1;
//     Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0xab3b);
//     CStreamGuard guard(s, true);
//     guard << (int)0x2b3;
//     guard << (int)GetUID();
//     SIG_MERCENARY_DB_DATA* db = (SIG_MERCENARY_DB_DATA*)
//         guard.GetInBuffer<SIG_MERCENARY_DB_DATA>();
//     memset(db, 0, 0xbd4);
//
//     unsigned int writeRow = 0;                                 // db[0] 实况行计数
//     for (size_t i = 0; i < m_mercenaryList.size(); ++i) {
//         CHARAC_LOAD_MERCENARY& mc = m_mercenaryList[i];
//         if (mc.dirtyByte24) {                                  // +0x24 脏标记
//             CHARAC_LOAD_MERCENARY* row = (CHARAC_LOAD_MERCENARY*)
//                 ((char*)(db + 4) + writeRow * 0x54);
//             memcpy(row, &mc, 0x54);                            // rep movsl ×0x15
//             writeRow += 1;
//             db[0] = writeRow;
//             mc.save_complete();
//         }
//     }
//     GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
//     return 1;
// }

// ----------------------------------------------------------------------------
// ORIG 0x086505f6 CUser::SaveMileage() void
void CUser::SaveMileage()
{
    if (G_GameWorld()->GetChannelType() == 7) return;

    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x18e4);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<(0x1fc);          // ORIG 0x086505f6：deEv→lsEi 链
    guard.operator*()->operator<<((int)GetUID());  // ORIG 同型
    char* sig = (char*)guard.operator->()->GetInBuffer<SIG_MILEAGE_STRUCT>();  // ORIG ptEv 链
    *(unsigned int*)(sig + 0) = get_acc_id();
    *(unsigned int*)(sig + 4) = getCurCharacNo();
    *(unsigned int*)(sig + 8) = GetMileage();
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ----------------------------------------------------------------------------
// ORIG 0x0868bf14 CUser::select_charac_set_visible_values(char value) void
void CUser::select_charac_set_visible_values(char value)
{
    set_charac_visible_values(value);
    SetVisible();
    if (get_charac_visible_grow_avatar())
        GetGameOptionRef()->set_etc_option((ENUM_OPTION_ETC)1, 1);
    else
        GetGameOptionRef()->set_etc_option((ENUM_OPTION_ETC)1, 0);
}

// >>> W7-BLOCKED CUser::SelectCharac —— _Charac_info levelByte75/raw tag 区、UserQuest::set_parent 及各对象 parent 回写契约未核
// ----------------------------------------------------------------------------
// ORIG 0x0864cd92 CUser::SelectCharac(int mainIdx, int tagIdx) uint
// int CUser::SelectCharac(int mainIdx, int tagIdx)
// {
//     if (getCurCharacR() != 0) {
//         cMyTrace t1((char*)__PRETTY_FUNCTION__, 0x11fb, 5);
//         t1("CUser::SelectCharac m_selected is not null");
//         return 0;
//     }
//     if (!(mainIdx >= 0 && mainIdx < (int)m_characList.size())) {
//         cMyTrace t3((char*)__PRETTY_FUNCTION__, 0x1246, 5);
//         t3("CUser::SelectCharac :charac_index : %d", mainIdx);
//         return 0;
//     }
//     _Charac_info* sel = &m_characList[mainIdx];
//     setCurCharac(sel);
//     if (tagIdx >= 0 && tagIdx < (int)m_characList.size()) {
//         _Charac_info* tag = &m_characList[tagIdx];
//         setTagCharac(tag);
//         getTagCharacInvenR()->set_parent(this);
//         getTagCharacSkillR()->set_parent(this);
//     }
//     if (getCurCharacR() == 0) {
//         cMyTrace t2((char*)__PRETTY_FUNCTION__, 0x120c, 5);
//         t2("CUser::SelectCharac m_selected is null at %d", mainIdx);
//         return 0;
//     }
//     if (getCurCharacR()->levelByte75 < 100) {                  // +0x75
//         int cap600 = 600;                                      // 栈双槽 min/max
//         time_t nowSec = GlobalData::s_systemTime_.getCurSec();
//         time_t lastPlay = getCurCharacLastPlayTick();
//         double diffSec = difftime(nowSec, lastPlay);
//         int elapsed = (int)(diffSec + 0.5);                    // ROUND 宏截断同形
//         if (elapsed > cap600) elapsed = cap600;
//         int restored = 0;
//         if (restored < elapsed) restored = elapsed;
//         int stam = getCurCharacR()->levelByte75 + (restored * 100) / 600;
//         if (stam > 100) stam = 100;
//         setCurCharacStamina((unsigned char)stam);
//     }
//     HistoryLog::WriteSelectCh(*(_IO_FILE**)m_field796f8,
//         getCurCharacName(), get_charac_job(), get_charac_level(),
//         getCurCharacGrowType());
//     getCurCharacInvenR()->set_parent(this);
//     ((SkillSlot*)getCurCharacSkillR())->set_parent(this);
//     getCurCharacQuestR()->set_parent(this);
//     getCurCharacCargoR()->set_parent(this);
//     GameWorld* gw = G_GameWorld();
//     int job = get_charac_job();
//     *(int*)((char*)gw + 4 + (job + 0x58) * 4) += 1;            // 在线职业统计
//     return 1;
// }

// >>> W7-BLOCKED CUser::SellItemTimeChecker —— m_sellLastSec/m_sellCheckCount 字段偏移注记缺失(报告缺失 0x08230256)
// ----------------------------------------------------------------------------
// 报告缺失@0x08230256（nm W sz0x55）SellItemTimeChecker(uint,uint)——objdump 转写：
//   cur = now - [this+0x8e42c]; if(!(cur > interval)) return ++[this+0x8e430];
//   [this+0x8e430]=0; [this+0x8e42c]=now; return [this+0x8e430];
// unsigned int CUser::SellItemTimeChecker(unsigned int nowSec, unsigned int interval)
// {
//     if (!((nowSec - m_sellLastSec) > interval)) {              // ja ⇒ 无符号 >
//         return ++m_sellCheckCount;
//     }
//     m_sellCheckCount = 0;
//     m_sellLastSec = nowSec;
//     return m_sellCheckCount;
// }

// ----------------------------------------------------------------------------
// ORIG 0x0868d1ec CUser::send_broadcast_lottery_gold_message(unsigned long,int)
void CUser::send_broadcast_lottery_gold_message(unsigned long itemIdx, int count)
{
    CItem* def = G_CDataManager()->find_item((int)itemIdx);
    if (def == 0) return;
    if (!def->is_stackable()) return;
    if (*(int*)((char*)def + 0x340) <= 0) return;
    if (*(int*)((char*)def + 0x340) > count) return;

    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(9);
    packet.put_short(get_unique_id() & 0xffff);
    packet.put_int(itemIdx);
    packet.put_int(count);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}

// ----------------------------------------------------------------------------
// ORIG 0x0868bf98 CUser::send_charac_visible_data() void
void CUser::send_charac_visible_data()
{
    PacketGuard packet;
    packet.put_header(0, 0x164);
    packet.put_short(get_unique_id() & 0xffff);
    packet.put_byte((int)get_charac_visible_values());
    packet.finalize(true);

    if (G_GameWorld()->IsPVPChannel()) {
        G_GameWorld()->send_all(packet);
        return;
    }
    Village* village = G_GameWorld()->getUserVillage(this);
    if (village == 0) return;
    if (get_area(false) == village->get_gate_area()) {
        Send(packet);
        return;
    }
    int st = get_state();
    if (st == 5 || (st - 8 < 3U)) {                            // 组队共享可见性
        CParty* party = (CParty*)GetParty();
        if (party != 0) party->send_to_party(packet);
    } else if (st == 3) {
        int area = get_area(false);
        G_GameWorld()->send_to_area((int)getCurCharacVill(), area, packet);
    } else {
        G_GameWorld()->send_all(packet);
    }
}

// ----------------------------------------------------------------------------
// ORIG 0x0868e0ee CUser::Send_SoloPlay_Reject_NotiPack(
//     char const*, ENUM_SOLO_PLAY_REJECT_MODE, ENUM_SOLO_PLAY_REJECT_MSG)
void CUser::Send_SoloPlay_Reject_NotiPack(const char* msg, ENUM_SOLO_PLAY_REJECT_MODE mode,
                                          ENUM_SOLO_PLAY_REJECT_MSG code)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x17d);
    if (msg == 0) {
        packet.put_int(0);
    } else {
        packet.put_int(strlen(msg));
        packet.put_str(msg, strlen(msg));
    }
    packet.put_int(mode);
    packet.put_int(code);
    packet.finalize(true);
    Send(packet);
}

// >>> W7-BLOCKED CUser::sendBingoAddData —— CEventEntity vtable+0x34 直调 CondFn 形态与 getBingoData 视图
// ----------------------------------------------------------------------------
// ORIG 0x08691470 CUser::sendBingoAddData() void
// void CUser::sendBingoAddData()
// {
//     CEventEntity* ev = GlobalData::s_event_manager->GetRepeatEvent(0x6a);
//     if ((*reinterpret_cast<unsigned int**>(ev))[0xd] ==
//             reinterpret_cast<unsigned int>(0)                 // vtable+0x34 槽
//         ) {}
//     typedef bool (*CondFn)(CEventEntity*, int);
//     CondFn condFn = *reinterpret_cast<CondFn*>(
//         reinterpret_cast<char*>(*reinterpret_cast<void***>(ev)) + 0x34);
//     if (!condFn(ev, 0)) return;
//     char* bingo = getBingoData();
//     if (*(bingo + 5) != '\0')
//         sendDecorationObjectCnt(1);
// }
//
// >>> W7-BLOCKED CUser::SendCeraShopGiftStatisticLog —— SIG_STATISTIC_CERASHOP_GIFT_LOG value64 定宽矛盾(fieldmap 行96)
// ----------------------------------------------------------------------------
// ORIG 0x0868a87a CUser::SendCeraShopGiftStatisticLog(uint,uint,ulong,uint)
// void CUser::SendCeraShopGiftStatisticLog(unsigned int arg1, unsigned int arg2,
//                                          unsigned long val64,
//                                          unsigned int arg4)
// {
//     Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0xa2c0);
//     CStreamGuard guard(s, true);
//     guard << (int)0x29c;
//     guard << (int)GetUID();
//     SIG_STATISTIC_CERASHOP_GIFT_LOG* sig =
//         (SIG_STATISTIC_CERASHOP_GIFT_LOG*)guard.GetInBuffer<
//             SIG_STATISTIC_CERASHOP_GIFT_LOG>();
//     sig->accId    = m_accId;
//     sig->characNo = getCurCharacNo();
//     sig->field08  = arg1;
//     sig->field0c  = arg2;
//     sig->value64  = val64;
//     sig->field14  = arg4;
//     GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
// }

// ----------------------------------------------------------------------------
// ORIG 0x086529ca CUser::sendCharacLinkMessage() void
void CUser::sendCharacLinkMessage()
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x122);
    packet.finalize(true);
    Send(packet);
}

// ----------------------------------------------------------------------------
// ORIG 0x0868f0e8 CUser::sendCharacOption() void
void CUser::sendCharacOption()
{
    if (getCurCharacR() == 0) return;
    PacketGuard packet;
    packet.put_header(0, 0x186);
    packet.put_int(0x200);
    packet.put_binary((char*)getCurCharacR() + 0x12b7, 0x200); // 角色选项块
    packet.finalize(true);
    Send(packet);
}

// >>> W7-BLOCKED CUser::SendCoolTimeItemInfo —— ContinuousItemInfo m_endSec 第三字段与权威双 int 冲突(fieldmap UNKNOWN)
// ----------------------------------------------------------------------------
// ORIG 0x086591fe CUser::SendCoolTimeItemInfo(unsigned int nowMs) uint
// unsigned int CUser::SendCoolTimeItemInfo(unsigned int nowMs)
// {
//     PacketGuard packet;
//     packet.put_header(0, 0xac);
//     std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
//     unsigned char cnt = (unsigned char)list->size();
//     packet.put_byte(cnt);
//
//     std::vector<ContinuousItemInfo>::const_iterator it;
//     for (it = list->begin(); it != list->end(); ++it) {
//         int itemId  = it->m_itemIdx;                              // +0
//         int expireAt = it->m_expireTime;                           // +4
//         packet.put_int(itemId);
//         packet.put_int(expireAt - nowMs);
//     }
//     packet.finalize(true);
//     return Send(packet);
// }
//
// >>> W7-BLOCKED CUser::sendDecorationObjectCnt —— CEventEntity vtable+0x34 直调槽与 CondFn cast 形态待 FixEventVtable 属主定论；getBingoData 返回契约
// ----------------------------------------------------------------------------
// ORIG 0x08690d9a CUser::sendDecorationObjectCnt(short count) void
// void CUser::sendDecorationObjectCnt(short count)
// {
//     PacketGuard packet;
//     packet.put_header(0, 0x1ac);
//     packet.put_short(count);
//     packet.finalize(true);
//     Send(packet);
// }
//
//
// ============================================================================
// body_E.cpp —— 续（SetCharacInfo/setCharacInfoDetail/SetEquipment 等）。
// ============================================================================
//
// void CUser::SetCharacInfo(int count, char* data)      // ORIG 0x08649c6a（SetCharacInfo.md）
// {
//     char scratch[0x70c];
//     memset(scratch, 0, 0x70c);                        // avatar 项缓冲（日志/上报载荷）
//     _Charac_info info;
//
//     int curSec = GlobalData::s_systemTime_.getCurSec();
//     for (int i = 0; i < count; ++i) {
//         char* rec = data + i * 0x94;
//         memset(&info, 0, sizeof(info)); /*推断:ORIG Reset 无符号，局部清零*/ *(int*)&info                  = *(int*)rec;                 // +0x00 characNo
//         strncpy(info.m_name, rec + 4, 0x1e);
//         *(char*)((char*)&info + 0x18) = rec[0x18];
//         *(char*)((char*)&info + 0x19) = rec[0x19];
//         *(short*)((char*)&info + 0x1a)= *(short*)(rec + 0x1a);
//         for (int k = 0; k < 0xc; ++k) {
            // 角色快照 int 表（scratch + k*0x3d）
//             *(int*)(scratch + k * 0x3d) = *(int*)(rec + (k + 4) * 8 + 8);
//         }
//         unsigned char grow = (unsigned char)rec[0x22];
//         unsigned int growOut = (grow == 1 || grow == (unsigned char)-1)
//                                    ? grow : 0;
//         CommonTime ct;
//         ct.SetCreateTime(*(long*)(rec + 0x88));
//         char expireFlag = (char)(curSec < *(int*)(rec + 0x90));
//         *(unsigned int*)((char*)&info + 0x14b7 - 0 + /*局部*/ 0) =
//             *(unsigned int*)(rec + 0x90);                            // 删除截止
//         *(unsigned int*)(/*delTime 槽*/ 0) = *(unsigned int*)(rec + 0x8c);
//         unsigned char authen = (unsigned char)((rec[0x1c] & 1) != 0);
//         char powerSide = rec[0x1d];
//         unsigned int wpBonus = *(unsigned int*)(rec + 0x1e);
//         {
//             _Charac_info& dst = m_characList.empty() ? info : info;   // 占位；实现按
//         }                                                             // 报告 push_back 布局落位
//         CInventory::SetInventoryMemory(NULL /*this+0x79794 装载*/ );
//         CCargo::SetInventoryMemory((InventoryMemory*)(/*+0x79794*/ this));
//         m_characList.push_back(info);                                // +0x796e8
//     }
//     if (*(int*)scratch > 0) {                                         // avatar 项计数
//         if (*(int*)scratch < 0x72) {
//             Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0xcb3);
//             CStreamGuard guard(s, true);
//             guard << 0x33;
//             guard << GetUID();
//             guard->put_binary(scratch, 0x70c);
//             GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
//         } else {
//             cMyTrace t(__PRETTY_FUNCTION__, 0xcaf, 5);
//             t("SetCharacInfo AVATAR INFO PROBLEM items<%d> user_no<%s> characnum<%d>",
//               *(int*)scratch, NumberToString(get_acc_id(), 0), count);
//         }
//     }
// }
//
// bool CUser::setCharacInfoDetail(int slot, unsigned int characNo,
//                                 CHARAC_DATA* src)          // ORIG 0x0864ac1a（1500 行报告）
// {
//     if (slot < 0 || slot >= (int)m_characList.size()) {
//         return false;
//     }
//     if (m_characList[slot].m_characNo != (int)characNo) {
//         return false;
//     }
//     _Charac_info& d = m_characList[slot];                  // ORIG 每次 operator[] 再取；
                                                           // O0 契合见 P36/P48
//     *(int*)&d = *(int*)src;                                // characNo
//     if (G_GameWorld()->GetChannelType() != 7) {
//         strncpy(d.m_name, (char*)src + 4, 0x1e);
//     }
//     setCharacFieldB(&d, 0x22, src[0x22]);                  // growType
//     if (src[0x22] == 7) {                                  // 学徒 → 固定节点
//         setCharacFieldB(&d, 0x22, 2);
//         setCharacFieldB(&d, 0x24, 7);
//     }
//     if (src[0x22] == 8) {                                  // 二段成长互换
//         CHARAC_DATA tmp = src[0x22];
//         setCharacFieldB(&d, 0x22, src[0x23]);
//         setCharacFieldB(&d, 0x25, tmp);
//     }
//     if (src[0x22] == 0xc || src[0x22] == 0xd) {
//         setCharacFieldB(&d, 0x22, 0xb);
//     }
//     setCharacFieldB(&d, 0x23, 2);                          // m_field29 区标记(=2)
//     setCharacFieldB(&d, 0x26, src[0x24]);                  // job
//     *(short*)((char*)&d + 0x27) = *(short*)((char*)src + 0x25);
//     *(int*) ((char*)&d + 0x2b) = *(int*)((char*)src + 0x28);
//     setCharacFieldB(&d, 0x29, src[0x27]);                  // movsbl 后 initCharacStat 用
//     setCharacFieldB(&d, 0x75, src[0xb2]);                  // stamina
//     *(unsigned int*)((char*)&d + 0x88) = *(unsigned int*)((char*)src + 0x60);
//     *(unsigned int*)((char*)&d + 0x8c) = *(unsigned int*)((char*)src + 100);
//     *(unsigned short*)((char*)&d + 0x90) = *(unsigned short*)((char*)src + 0x68);
//     *(unsigned short*)((char*)&d + 0x92) = *(unsigned short*)((char*)src + 0x6a);
//     *(unsigned short*)((char*)&d + 0x94) = *(unsigned short*)((char*)src + 0x6c);
//     *(unsigned short*)((char*)&d + 0x96) = *(unsigned short*)((char*)src + 0x6e);
//     memcpy((char*)&d + 0x98,  (char*)src + 0x70, 8);
//     memcpy((char*)&d + 0xa0,  (char*)src + 0x78, 0x22);
//     *(unsigned int*)((char*)&d + 0xc2) = *(unsigned int*)((char*)src + 0x9a);
//     *(unsigned short*)((char*)&d + 0xc6)= *(unsigned short*)((char*)src + 0x9e);
//     *(unsigned short*)((char*)&d + 200) = *(unsigned short*)((char*)src + 0xa0);
//     *(unsigned int*)((char*)&d + 0xca)  = *(unsigned int*)((char*)src + 0xa2);
//     *(unsigned short*)((char*)&d + 0xce)= *(unsigned short*)((char*)src + 0xa6);
//     *(unsigned short*)((char*)&d + 0xd0)= *(unsigned short*)((char*)src + 0xa8);
//     *(unsigned short*)((char*)&d + 0xd2)= *(unsigned short*)((char*)src + 0xaa);
//     *(unsigned short*)((char*)&d + 0xd4)= *(unsigned short*)((char*)src + 0xac);
//     *(unsigned int*)((char*)&d + 0xd6)  = *(unsigned int*)((char*)src + 0xae);
    // —— 六元组入 stat 初始化器（ORIG 调用 InitializeCharacStat::initCharacStat，
    //     定义缺口已通知 Main；声明缺失期间以直接调用呈现）——
//     global::tools::InitializeCharacStat::initCharacStat(
//         *(_Additioal_info*)((char*)&d + 0x88),
//         (unsigned int)d.m_characNo,
//         (int)*(char*)((char*)&d + 0x26),
//         (int)*(short*)((char*)&d + 0x27),
//         (int)((char)(*(char*)((char*)&d + 0x29) << 4) >> 4),
//         (int)((char)(*(char*)((char*)&d + 0x29) * '\x02') >> 5));
//     *(unsigned short*)((char*)&d + 0x33) = *(unsigned short*)((char*)src + 0x2c);
//     *(unsigned short*)((char*)&d + 0x35) = *(unsigned short*)((char*)src + 0x2e);
//     *(unsigned short*)((char*)&d + 0x37) = *(unsigned short*)((char*)src + 0x30);
    // —— 疲劳上限事件 ——
//     {
//         CMaxFatigueFactorEvent* ev =
//             (CMaxFatigueFactorEvent*)GlobalData::s_event_manager->GetRepeatEvent(2);
//         if (ev != 0 && !ev->IsEventing(0)) {
//             *(short*)((char*)&d + 0x35) = (short)DEFAULT_MAX_FATIGUE;
//         } else if (ev != 0) {
//             int factor = ev->GetMaxFatigueFactor();
//             *(short*)((char*)&d + 0x35) =
//                 (short)(factor * DEFAULT_MAX_FATIGUE / 100);
//         }
//     }
//     {
//         CEventBase* ev57 = GlobalData::s_event_manager->GetRepeatEvent(0x57);
//         if (ev57->IsEventing(0)) {
//             short base = *(short*)((char*)&d + 0x35);
//             *(short*)((char*)&d + 0x35) =
//                 (short)(base + *(short*)((char*)G_CDataManager() + 0x51c));
//         }
//     }
//     if (*(short*)((char*)src + 0x32) < 0) {                // 疲劳电池错误分支
//         *(unsigned short*)((char*)&d + 0x4f) = 0;
//         short badVal = *(short*)((char*)src + 0x32);
//         cMyTrace t(__PRETTY_FUNCTION__,
//                    0xf99, 0);
//         t("FATIGUE BATTERY Error : Char No(%d) %d", *(int*)src, (int)badVal);
//     } else {
//         *(unsigned short*)((char*)&d + 0x4f) = *(unsigned short*)((char*)src + 0x32);
//     }
//     *(unsigned int*)((char*)&d + 0xdf4) = 0;
//     *(unsigned int*)((char*)&d + 0xdf8) = 0;
//     *(unsigned int*)((char*)&d + 0x7b)  = *(unsigned int*)((char*)src + 0x4d);
//     {
//         unsigned int lastTick = *(unsigned int*)((char*)&d + 0x7b);
//         cMyTrace t(__PRETTY_FUNCTION__,
//                    0xfb0, 0);
//         t("CharLastPlayTick char(%d) t(%d)", characNo, lastTick);
//     }
//     *(unsigned int*)((char*)&d + 0x7f)  = *(unsigned int*)((char*)src + 0x118);
//     *(unsigned int*)((char*)&d + 0xdbe) = *(unsigned int*)((char*)src + 0x56);
//     setCharacFieldB(&d, 0xdc2, src[0x5a]);
//     *(unsigned short*)((char*)&d + 0x77) = *(unsigned short*)((char*)src + 0x5b);
//     *(unsigned short*)((char*)&d + 0x79) = *(unsigned short*)((char*)src + 0x5d);
//     if (src[0x5f] != 0) {
//         *(unsigned int*)((char*)&d + 0xe22) = *(unsigned int*)src;
//     }
//     ((CommonTime*)((char*)&d + 0x39))->SetCreateTime((long)(src + 0x34));
//     *(unsigned short*)((char*)&d + 0x4d) = *(unsigned short*)((char*)src + 0xb3);
//     *(unsigned int*)((char*)&d + 0x51)  = *(unsigned int*)((char*)src + 0xb5);
//     memcpy((char*)&d + 0x55, (char*)src + 0xb9, 0x10);
//     d.m_reliablePerson.InitTradeGold(*(unsigned int*)((char*)src + 0xc9),
//                                      *(unsigned int*)((char*)src + 0xcd),
//                                      *(unsigned int*)((char*)src + 0xd1));
//     {
//         unsigned int useGold = *(unsigned int*)((char*)src + 0xd1);
//         cMyTrace t(__PRETTY_FUNCTION__,
//                    0xfd7, 0);
//         t("TRADE_RESTIRCTION charac_name(%s), use_gold(%u)", d.m_name, useGold);
//     }
//     *(unsigned int*)((char*)&d + 0xe49) = *(unsigned int*)((char*)src + 0xd5);
//     *(unsigned int*)((char*)&d + 0xe4d) = *(unsigned int*)((char*)src + 0xd9);
//     *(unsigned int*)((char*)&d + 0xe51) = *(unsigned int*)((char*)src + 0xdd);
//     *(unsigned int*)((char*)&d + 0xe55) = *(unsigned int*)((char*)src + 0xe1);
//     *(unsigned int*)((char*)&d + 0xe59) = *(unsigned int*)((char*)src + 0xe5);
//     *(unsigned int*)((char*)&d + 0xe5d) = *(unsigned int*)((char*)src + 0xe9);
//     *(unsigned int*)((char*)&d + 0xe45) = *(unsigned int*)((char*)src + 0xed);
//     *(unsigned int*)((char*)&d + 0xe68) = *(unsigned int*)((char*)src + 0xf1);
//     setCharacFieldB(&d, 0xe70, src[0xf5]);
//     *(unsigned int*)((char*)&d + 0xe71) = *(unsigned int*)((char*)src + 0xf6);
//     *(unsigned int*)((char*)&d + 0xe79) = *(unsigned int*)((char*)src + 0xfa);
//     *(unsigned int*)((char*)&d + 0xe81) = *(unsigned int*)((char*)src + 0x10a);
//     *(unsigned int*)((char*)&d + 0xe7d) = *(unsigned int*)((char*)src + 0x106);
//     *(unsigned short*)((char*)&d + 0x121f) = *(unsigned short*)((char*)src + 0x10e);
//     *(unsigned int*)((char*)&d + 0x1221)   = *(unsigned int*)((char*)src + 0x110);
//     *(unsigned int*)((char*)&d + 0x1225)   = *(unsigned int*)((char*)src + 0x114);
//     setCharacFieldB(&d, 0x1239, src[0x11e]);
//     setCharacFieldB(&d, 0x123e, src[0x11f]);
//     setCharacFieldB(&d, 0x123f, src[0x120]);
//     *(bool*)((char*)&d + 0x1241) = (src[0x121] != 0);
//     *(unsigned short*)((char*)&d + 0x126c) = *(unsigned short*)((char*)src + 0x122);
//     setCharacFieldB(&d, 0x126e, src[0x124]);
//     *(unsigned int*)((char*)&d + 0x126f)  = *(unsigned int*)((char*)src + 0x125);
//     setCharacFieldB(&d, 0x1275, src[0x129]);
//     setCharacFieldB(&d, 0x1276, src[0x12a]);
//     setCharacFieldB(&d, 0x1277, src[299]);
//     setCharacFieldB(&d, 0x127a, src[0x12d]);
//     setCharacFieldB(&d, 0x1278, src[300]);
//     setCharacFieldB(&d, 0x1280, src[0x12f]);
//     *(unsigned int*)((char*)&d + 0x12ad) = *(unsigned int*)((char*)src + 0x130);
//     return true;
// }
//
// void CUser::setCharacLinkBonusSetting(int slot, bool flag)   // ORIG W 0x084ecdb6（报告缺失）
// {
//     if (!(get_charac_count() < slot || slot < 0)) {
//         m_characList[slot].m_field124f = flag ? 1 : 0;       // +0x124f byte
//         m_characterView.enableSaveCharacView();              // +0x796f4
//     }
// }
//
// void CUser::setCharacLinkMessageFlag(bool flag)             // ORIG W 0x084ece20（报告缺失）
// {
//     W7_CU_BYTE(0x703d8) = (unsigned char)(flag ? 1 : 0);                         // +0x703d8 byte
// }
//
// void CUser::SetCharacMercenaryInfo(int count,
//                                    CHARAC_LOAD_MERCENARY* infos)   // ORIG 0x0868e544
// {
//     for (int i = 0; i < count && i < 0x24; ++i) {
//         m_mercenaryMgr.AddCharac(infos[i]);                 // +0x6ef6c UserMercenaryInfoMgr
//     }
// }
//
// void CUser::setCharacSlotCountInfo(unsigned char effectCnt,
//                                    unsigned char limit)     // ORIG 0x0868b480
// {
//     if (effectCnt == 0 || limit == 0) {
//         unsigned char def = getCharacSlotCountDefaultInfo();
//         setSlotEffectCount(def);
//         setCharacSlotLimit(def);
//     } else {
//         setSlotEffectCount(effectCnt);
//         setCharacSlotLimit(limit);
//     }
// }
//
// void CUser::SetEquipment(int slot, void* invenData, int size,
//                          void* outData)                     // ORIG 0x0865da4e
// {
//     for (int i = 0; i <= 9; ++i) {                          // do-while 形态(P25:do-while)
//         Inven_Item* items = (Inven_Item*)invenData;
//         Inven_Item& it = items[i];                          // stride 0x3d
//         bool expired = false;
//         if (*(int*)((char*)invenData + i * 0x3d + 2) == 0) {
//             expired = false;
//         } else {
//             int addInfo = it.get_add_info();
//             if (addInfo < 1) {
//                 expired = false;
//             } else {
//                 int now = OS_API::GetDateTimeTick();
//                 expired = (now > addInfo);
//             }
//         }
//         if (!expired) continue;
//         int* cnt = (int*)outData;
//         _Charac_info& ch = m_characList[slot];
//         *(unsigned int*)((char*)outData + *cnt * 4 + 4) =
//             *(unsigned int*)&ch;                            // 存角色号到输出表
//         *(char*)((char*)outData + *cnt + 0x5a4) = (char)i;
//         it.reset();
//         *cnt = *cnt + 1;
//         break;                                              // ORIG 首个过期后即返回路径
//     }
//     const _Charac_info* chR = getCurCharacIdxElement(slot);
//     getCurCharacInvenW()->SetInvenData(0, invenData, size);
// }
//
// ============================================================================
// body_E.cpp —— 续 3（SetEventCreateDnfReward / Growth / Guild / HackUser /
//                SetInventory）
// ============================================================================
//
// void CUser::SetEventCreateDnfReward(bool flag)       // ORIG 0x08693198（报告全文）
// {
//     m_eventCreateDnfReward = flag;                   // +0x8ec30
// }
//
// void CUser::SetGrowthCreatureEventInfo(
//     const SIG_GIVE_GROWTH_CREATURE_EVENT& sig)       // ORIG 0x08691cde
// {
//     const char* p = (const char*)&sig;
//     for (int i = 0; i < 0xb; ++i) {
//         const char* rec = p + i * 0x2c;
//         if (*(short*)rec == 0 || *(int*)(rec + 4) == 0) {
//             break;
//         }
        // eventCharacInfo 实际 stride 0x2c：+0 short、+4/+8 int、+0xc 起 0x1e 数据。
        // 现头 m_pad[0x10] 与 ORIG 不符——见 fields_E 建议扩为真实布局；
        // 待头定稿后 push_back 改真实字段写入。
//         GrowthCreatureEvent::eventCharacInfo info;
//         *(short*)&info                          = *(short*)rec;
//         *(unsigned int*)((char*)&info + 4)      = *(unsigned int*)(rec + 4);
//         *(unsigned int*)((char*)&info + 8)      = *(unsigned int*)(rec + 8);
//         memcpy((char*)&info + 0xc, rec + 0xc, 0x1e);      // 头 pad 仅 0x10，见 fields_E
//         m_growthCreature.m_vec.push_back(info);           // +0x8ebf8
//     }
// }
//
// void CUser::SetGrowthWeaponEventInfo(
//     const SIG_GROWTH_WEAPON_EVENT& sig)              // ORIG 0x8691bae
// {
//     char* dst = (char*)&m_growthWeapon;              // +0x8ebe4
//     const char* src = (const char*)&sig;
//     dst[0] = src[0];
//     *(unsigned int*)(dst + 4)  = *(unsigned int*)(src + 4);
//     *(unsigned int*)(dst + 8)  = *(unsigned int*)(src + 8);
//     *(unsigned int*)(dst + 12) = *(unsigned int*)(src + 12);
//     *(unsigned short*)(dst + 16) = *(unsigned short*)(src + 16);
//     *(unsigned short*)(dst + 18) = *(unsigned short*)(src + 18);
//
//     CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(100);
//     if (ev->IsEventing(0) && dst[0] != '\x01') {
//         PacketGuard packet;
//         packet.put_header(0, 0x1ba);
//         packet.finalize(true);
//         Send(packet);
//     }
// }
//
// void CUser::SetGuildAgitDBInfo(const STGuildAgitDBInfo& info)   // ORIG W 0x084eced6（报告缺失）
// {
//     memcpy(&m_guildAgitDB, &info, 1);                // 仅 +0x8d0dd 首字节复制
// }
//
// void CUser::SetGuildDBInfo(const STGuildDBInfoOnly& info)       // ORIG W 0x084ece3c（报告缺失）
// {
//     memcpy(&m_guildDB, &info, 0xbd);                 // +0x8d020 整体 0xbd 字节
// }
//
// void CUser::SetGuildInfoProxy(STGuildProxy& proxy)   // ORIG 0x0865cd0a
// {
//     memcpy(&m_guildDB, &proxy, 0x16);                // 名字区 22B 落 +0x8d020
//     m_guildDB.m_fundLevel = proxy.m_level;           // +0x17 → +0x8d03b
//     m_guildDB.m_field95   = proxy.m_powerSide;       // +0x18 → +0x8d0b5
// }
//
// void CUser::setHackUser(unsigned int a, unsigned int b, unsigned int c,
//                         unsigned int d, unsigned short type,
//                         unsigned int param6)         // ORIG 0x0867f748
// {
//     cMyTrace trace("void CUser::setHackUser(unsigned int, unsigned int, unsigned int,"
//                    " unsigned int, short unsigned int, unsigned int)", 0x7e55, 0);
//     char* accStr = NumberToString(get_acc_id(), 0);
//     trace("m_id : %s, SetHackUser : %d(%d)\t  %d(%d)   ApplyFlag = %d\n",
//           accStr, (int)a, (int)b, (int)c, (int)d, (int)type);
//
//     if (type == 2) {                                 // 观察名单
//         stLookOuts entry;
//         new (&entry) stLookOuts();                   // stLookOuts(a,c) 写入首两 dword
//         *(unsigned int*)&entry = a; *((unsigned int*)&entry + 1) = c;
//         m_lookOuts1.push_back(entry);                // +0x8d1b8
//     } else if (type == 3) {                          // IP 计数器
//         if (m_field8e950) return;
//         char ipbuf[0x20];
//         memset(ipbuf + 0x10, 0, 0x10);
//         memset(ipbuf, 0, 0x10);
//         m_net.GetPeerIP2(ipbuf + 0x10, 0x10);
//         Divide_C_ClassIP_EndIP(ipbuf, (char*)0, ipbuf + 0x10);
//         tyIPCounters cnt;                            // tyIPCounters(a,c)
//         *(unsigned int*)&cnt = a; *((unsigned int*)&cnt + 1) = c;
//         m_ipCounters.push_back(cnt);                 // +0x8d1d0
//         m_field8e950 = true;                         // +0x8e950
//         m_historyLog.IPCounterLog_InsertIp((int)a, (int)c, ipbuf + 0x10);
//         DB_HackIPSave::makeRequest(ipbuf, ipbuf + 0x10, a, c);
//         return;
//     } else if (type == 10) {
//         stLookOuts entry;
//         *(unsigned int*)&entry = a; *((unsigned int*)&entry + 1) = c;
//         m_lookOuts2.push_back(entry);                // +0x8d1c4
//         if (isTradePunishType(4)) return;
//         int secs = get_rand_int(0x122) + 10;
//         printf("ARS TRADE PUNISH USER - Rand Sec : %d\n", secs);
//         G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
//                                     get_unique_id(), (TIMER_MESSAGE)0x97,
//                                     secs, GetUID(), 0xffffffff);
//     } else {
//         int secs = get_rand_int(0x122) + 10;
//         m_field8d1a8 = true;                         // +0x8d1a8（._379.m_field0）
//         if (type == 8 || type == 7 || type == 9 || type == 0xc || type == 5) {
//             m_field8e951 = true;                     // +0x8e951
//             if (type == 8) secs = 1;
//         }
//         G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
//                                     get_unique_id(), (TIMER_MESSAGE)0x81,
//                                     secs, GetUID(), 0xffffffff);
//         OnPunishUser();
//         if (!(m_field796d8 != 0 && GetTradePunishType() != 0)) {
//             unsigned short t2 = type;
//             if (m_field796d8 != 0 &&
//                 (type == 1 || type == 4 || type == 9)) {
//                 t2 = 10;
//             }
//             finalizeHackRecord(a, b, c, d, t2, param6);
//             return;
//         }
//         return;
//     }
//     finalizeHackRecord(a, b, c, d, type, param6);
// }
//
// setHackUser 公共尾部（ORIG +0x2a2..+0x2f9）：取对端 IP → DB_InsertAutoPunishFirstUser。
// （原函数内联无独立符号；仅供草稿组织，接入时按原布局合并回 setHackUser。）
//
// int CUser::SetInventory(SIG_LOAD_INVENTORY* sig)     // ORIG 0x0864d160
// {
//     unsigned int money    = *(unsigned int*)((char*)sig + 0x4034);
//     unsigned int coin     = *(unsigned int*)((char*)sig + 0x4038);
//     unsigned int payCoin  = *(unsigned int*)((char*)sig + 0x403c);
//     unsigned int eventCoin= *(unsigned int*)((char*)sig + 0x4040);
//     void* equipZone       = (char*)sig + 0x5949;      // 0x4a58 inven1
//     void* etcZone         = (char*)sig + 0x4044;      // 0x1905 inven2
//     void* avatarZone      = (char*)sig + 0xa3a1;      // 0x53e
//     void* cargoZone       = (char*)sig + 0xa8e4;
//     int   cargoCap        = *(int*)((char*)sig + 0xa8e0);
//     void* creatureZone    = (char*)sig + 0xeed1;      // 0x39aa inven3
//
//     if (getCurCharacR() == 0) {
//         cMyTrace t(__PRETTY_FUNCTION__, 0x1271, 5);
//         t("CUser::SetInventory m_selected is null");
//         return 0;
//     }
//     CInventory* invW = getCurCharacInvenW();
//     for (int i = 0; i < 0xc; ++i) {
//         const CInventory* invR = getCurCharacInvenR();
//         if (*(int*)((char*)invR + i * 0x3d + 0x1e) !=
//             *(int*)((char*)avatarZone + i * 0x3d + 2)) {
//             m_characterView.enableSaveCharacView();
//             break;
//         }
//     }
//     invW->AvatarCoin().Set(*(unsigned int*)((char*)sig + 0x14780));
//     invW->set_money(money);
//     invW->SetCoin(coin);
//     invW->SetPayCoin(payCoin);
//     invW->SetEventCoin(eventCoin);
//     invW->SetInvenData(1, equipZone, 0x4a58);
//     invW->SetInvenData(2, etcZone, 0x1905);
//     memcpy((char*)invW + 0x1c, avatarZone, 0x53e);
//     memcpy((char*)invW + 0x55a, (char*)sig + 0xeed1 + 0x38b6, 0xf4);
//     for (int slot = 0x17; slot < 0x1a; ++slot) {
//         user_creature::CCreatureMgr* mgr =
//             getCurCharacInvenW()->GetCreatureMgrW();
//         mgr->LoadArtifact(slot,
//             (Inven_Item*)((char*)getCurCharacInvenW() + 0x1c + slot * 0x3d));
//     }
//     invW->SetInvenData(3, creatureZone, 0x39aa);
//     CCargo* cargo = getCurCharacCargoW();
//     if (cargoCap == 0) {
//         cargo->SetCapacity(8);
//     } else {
//         cargo->SetCapacity(cargoCap);
//         cargo->set_cargo(cargoZone);
//     }
//     return 1;
// }
//
//
// ============================================================================
// body_F.cpp —— CUser ORIG-only 批次 F 可粘贴实现草稿(47 符号;不入构建)
//
// 取证:docs/class_func_reports/CUser/<fn>.md ×41(汇编+反编译 C 全读);
// 报告缺失 6 个以 nm 定址后 objdump 区间直转(地址见 meta_F.tsv)。
// 目标状态字段名见 fields_F.tsv;缺声明依赖集中列于本文件尾注。
// ============================================================================
//
//
// >>> W7-BLOCKED ^void CUser::SetLimitInfo\( —— LimitItemUsage state/f4/f8 与内嵌 CommonTime 冲突；stLimitItemUsageInfoEx::LimitInfo 未建模
// ---------------------------------------------------------------------------
// 1. CUser::SetLimitInfo @0x086888f8
//    SIG_LIMIT_ITEM_INFO 寻址(ORIG 实证):base+0 计数;entry i 位于
//    base + i*0x10 + 8:{long createTime(+0)/int itemIdx(+4)/int count(+8)}。
// ---------------------------------------------------------------------------
// void CUser::SetLimitInfo(SIG_LIMIT_ITEM_INFO* info)
// {
//     if (info == 0)
//         return;
//
//     char* base = reinterpret_cast<char*>(info);
//     int entryCount = *reinterpret_cast<int*>(base);
//
//     m_limitItemUsage.clear();
//     std::vector<stLimitItemUsageInfoEx::LimitInfo>& limits =
//         G_CDataManager()->GetLimitItemUsageInfo();
//
//     for (std::vector<stLimitItemUsageInfoEx::LimitInfo>::iterator it = limits.begin();
//          it != limits.end(); ++it)
//     {
//         LimitItemUsage usage;
//         bool found = false;
//         for (int i = 0; i < entryCount; ++i)
//         {
//             char* e = base + i * 0x10 + 8;
//             long long createTime =
//                 *reinterpret_cast<long long*>(e);
//             int itemIdx   = *reinterpret_cast<int*>(e + 8);
//             int count     = *reinterpret_cast<int*>(e + 12);
//             if (itemIdx == it->f4)
//             {
//                 usage.state     = it->kind;      // LimitItemUsage+0x00
//                 usage.f8        = it->f8;        // +0x08
//                 usage.f4        = it->f4;        // +0x04
//                 usage.m_addInfo2At10 = count;         // +0x10
//                 usage.SetCreateTime(createTime);
//                 m_limitItemUsage.push_back(usage);
//                 found = true;
//                 break;
//             }
//         }
//         if (!found)
//         {
//             usage.state     = it->kind;
//             usage.f8        = it->f8;
//             usage.f4        = it->f4;
//             usage.m_addInfo2At10 = 0;
//             usage.SetCurTime();
//             m_limitItemUsage.push_back(usage);
//         }
//     }
// }

// ---------------------------------------------------------------------------
// 2. CUser::SetMissionParameter @0x08662558
// ---------------------------------------------------------------------------
void CUser::SetMissionParameter(int kind, int value)
{
    if (!isGMUser())
        return;
    CMissionList_Charac* missions =
        reinterpret_cast<CMissionList_Charac*>(
            GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8));
    missions->GM_SetMissionValue(kind, value);
}

// ---------------------------------------------------------------------------
// 3. CUser::setPcRoomPlayTimeData @0x0868f808
// ---------------------------------------------------------------------------
void CUser::setPcRoomPlayTimeData(unsigned int startTime, unsigned int total,
                                  unsigned char rewardCnt)
{
    m_pcRoomPlayStartTime     = startTime;      // +0x8eb9c
    m_totalPcRoomPlayTime     = static_cast<int>(total);  // +0x8eba0(movl)
    m_pcRoomPlayTimeRewardCnt = rewardCnt;      // +0x8eba4
}

// ---------------------------------------------------------------------------
// 4. CUser::SetPuUser @0x08692b58
// ---------------------------------------------------------------------------
void CUser::SetPuUser(bool flag)
{
    m_field796d8 = (char)(flag ? 1 : 0);
}

// ---------------------------------------------------------------------------
// 5. CUser::SetPvpRecord @0x0864deea —— 返回恒 1(undefined4 mov $1)
// ---------------------------------------------------------------------------
int CUser::SetPvpRecord(PvpResultType& record)
{
    getPVPResultW()->Set(record);
    return 1;
}

// ---------------------------------------------------------------------------
// 6. CUser::setQuestPoint @0x0868acfe
// ---------------------------------------------------------------------------
void CUser::setQuestPoint(int qp)
{
    CQuestShop* shop =
        reinterpret_cast<CQuestShop*>(
            GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb));
    if (shop == 0)
        return;
    shop->setQp(qp);
    shop->sendCharacQp(this, '\0');
}

// ---------------------------------------------------------------------------
// 7. CUser::SetReceivedRequestType @0x082308cc W(报告缺失,objdump 直转)
// ---------------------------------------------------------------------------
void CUser::SetReceivedRequestType(char type)
{
    m_receivedRequestType = type;               // uchar +0x8ec28
}

// ---------------------------------------------------------------------------
// 8. CUser::setRecipeEqiupmentOption @0x0867306e T（报告 docs/CUser/setRecipeEqiupmentOption.md）
//    ORIG stEquipCondition 用法（objdump 实测，cond 按值 0xc 传参）：
//    cond+0=m_upgrade(>0x1f 校验/<<7 表行/trace mat_up)、cond+1=m_tableIdx(非0→amplify 分支
//    与 getCompoundUpgrade 表组)、cond+2=m_amplifySelect(assign type)、cond+4w=m_field4
//    (assign value)、cond+6=m_flag6(trace mat_seal/==1 参与 bind)、cond+7=m_pad7(SetReSealCount)。
//    栈参数：ebp+0x18=invenSpace（update_item 槽号）、ebp+0x1c=stackable、ebp+0x20=matUpgrade
//    （==0xb → 输出强化强制 0）。
// ---------------------------------------------------------------------------
void CUser::setRecipeEqiupmentOption(Inven_Item& outItem, stEquipCondition cond,
                                     int matUpgrade, const CStackableItem* stackable,
                                     int invenSpace)
{
    unsigned short outId = static_cast<unsigned short>(outItem.m_addInfo);   // item+0x02 word
    CItem* outDef = G_CDataManager()->find_item(outId);

    if (outDef == 0)
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fbe, "RECIPE ERROR : Not Exist Output Item");
        return;
    }
    if (outItem.m_field1 != 1)                                               // ITEM_TYPE_EQUIP
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fc4, "RECIPE ERROR : Not Equipment Item");
        return;
    }
    if ((unsigned char)cond.m_upgrade > 0x1f)
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fca, "RECIPE ERROR : Impossible Upgrade Value");
        return;
    }

    // matUpgrade==0xb → 输出强化强制 0；否则按材料升级表 roll
    unsigned char outUpgrade;
    if (matUpgrade == 0xb)
        outUpgrade = 0;
    else
        outUpgrade = static_cast<unsigned char>(getCompoundUpgrade(stackable, cond));

    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x5fdb, 0);
    trace("RECIPE LOG : out_item_id-%u, mat_up_val-%d, out_up_val-%d, mat_seal-%d",
          (unsigned)outId, (unsigned)(unsigned char)cond.m_upgrade,
          (unsigned)outUpgrade, (unsigned)(unsigned char)cond.m_flag6);

    if (cond.m_tableIdx == 0)                                                // cond+1 非0 → amplify
    {
        outItem.SetUpgrade(outUpgrade);
    }
    else
    {
        stAmplifyOption_t& amp = outItem.m_amp;                              // Inven_Item+0x11
        amp.assign(cond.m_amplifySelect, cond.m_field4);                     // type=cond+2, value=cond+4w
        amp.identify();
        outItem.SetUpgrade(outUpgrade);
    }

    // bind 标记：m_flag6==1 且 GetAttachType∈{3,4} → item+0=1，否则 0
    bool bind = false;
    if ((unsigned char)cond.m_flag6 == 1 &&
        (outDef->GetAttachType() == 3 || outDef->GetAttachType() == 4))
        bind = true;
    outItem.m_field0 = bind ? 1 : 0;

    if (outDef->GetAttachType() == 3)
        outItem.SetReSealCount((unsigned char)cond.m_pad7);                  // cond+7 reseal

    getCurCharacInvenW()->update_item((INVEN_TYPE)1, invenSpace, outItem);
}
// ---------------------------------------------------------------------------
// 9. CUser::SetServerGroup @0x082300ac W(报告缺失,objdump 直转)
// ---------------------------------------------------------------------------
void CUser::SetServerGroup(ENUM_SERVER_GROUP group)
{
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x79630) =
        static_cast<int>(group);
}

// ---- body_F.cpp 续篇 A(与主文件按序拼接后编译)----

// ---------------------------------------------------------------------------
// 10. CUser::SetSkill @0x0864db20
//     SIG_LOAD_SKILL 包内布局视图(+0x8 sp1 / +0xc bufA / +0x198 尺寸常量 /
//     +0x1a4 sp2 / +0x1a8 bufB / +0x340 tail1 / +0x344 tail2 / +0x348 cmd 200B)。
// ---------------------------------------------------------------------------
bool CUser::SetSkill(SIG_LOAD_SKILL* skill, SIG_COMBO_SKILL* combo)
{
    char* base = reinterpret_cast<char*>(skill);
    int   sp1   = *reinterpret_cast<int*>(base + 0x8);
    char* bufA  = base + 0xc;
    int   sp2   = *reinterpret_cast<int*>(base + 0x1a4);
    char* bufB  = base + 0x1a8;
    int   tail1 = *reinterpret_cast<int*>(base + 0x340);
    (void)*reinterpret_cast<int*>(base + 0x344);           // ORIG 仅读取未用

    if (getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x13ad, 5);
        trace("CUser::SetSkill User  m_selected is null");
        return false;
    }

    // asm 实参序:(sp1, sp2, tail1, tail1)
    SkillSlot* slot = (SkillSlot*)getCurCharacSkillW();
    slot->set_remain_sp(sp1, sp2, tail1, tail1);

    slot = (SkillSlot*)getCurCharacSkillW();
    slot->set_skill_slot(bufA, bufB, 0x198);

    setSkillCommandData(base + 0x348);

    slot = (SkillSlot*)getCurCharacSkillW();
    slot->SetLoadStrikerSkill(false);

    if (get_charac_job() == 9)
    {
        slot = (SkillSlot*)getCurCharacSkillW();
        slot->loadComboSkill(combo);
        slot = (SkillSlot*)getCurCharacSkillW();
        slot->verifyComboSkill();
    }
    return true;
}

// ---------------------------------------------------------------------------
// 11. CUser::setSkillCommandData @0x08654924
//     memcpy(&m_skillCommands[0],src,200)→解析 50×4B 条目入 getSkillCommand()
//     返回的 std::map<int,STSkillCommandData>→末尾 memset 清零。
//     条目位提取(+0xb/+0xc/+0xd/+0xe 相对 this+0x703d0):
//       cmd = e[3]&0x3f<<8|e[0]; count=e[3]>>5;
//       key[0]=(u16@e+1>>6)&7; key[1]=e[2]>>1&7; key[2]=e[2]>>4&7;
//       key[3]=(u32@e+1>>15)&7; key[4]=e[3]>>2&7(-0x4e/-0x4d 两组照录其位序)。
// ---------------------------------------------------------------------------
void CUser::setSkillCommandData(char* data)
{
    memcpy(&m_skillCommands[0], data, 200);

    std::map<int, STSkillCommandData>* cmds = reinterpret_cast<std::map<int, STSkillCommandData>*>(
        const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return;

    for (int i = 0; i < 50; ++i)
    {
        const unsigned char* e =
            reinterpret_cast<const unsigned char*>(&m_skillCommands[i]);  // 步长 4
        unsigned short cmd = static_cast<unsigned short>(
            (((unsigned short)e[3]) & 0x3f) << 8 | ((unsigned short)e[0]));
        if (cmd == 0)
            break;

        unsigned char count = e[3] >> 5;
        unsigned char keys[4];
        keys[0] = ((((unsigned short)e[1]) | ((unsigned short)e[2] << 8)) >> 6) & 7;
        keys[1] = (unsigned char)((e[2] >> 1) & 7);
        keys[2] = (unsigned char)((e[2] >> 4) & 7);
        keys[3] = (unsigned char)((((unsigned short)e[1]) |
                                   ((unsigned short)e[2] << 8)) >> 15) & 7;
        // 第 5 提取:e[3]>>2&7 对应 STCommand bit29-31(m_key4/m_count 区)

        int key = static_cast<short>(cmd);                  // -0x48 有符号 16 位
        std::map<int, STSkillCommandData>::iterator it = cmds->find(key);
        if (it != cmds->end())
        {
            it->second.m_command = cmd;
            it->second.m_count   = count;
            for (int k = 0; k < static_cast<char>(count); ++k)
                (&it->second.m_key0)[k] = keys[k];          // +7 起逐键写(ORIG 循环)
        }
        else
        {
            STSkillCommandData sc;
            sc.m_command = cmd;
            sc.m_count   = count;
            for (int k = 0; k < static_cast<char>(count); ++k)
                (&sc.m_key0)[k] = keys[k];
            cmds->insert(std::make_pair(key, sc));          // make_pair(short&,data&) 收敛为值对
        }
    }
    memset(&m_skillCommands[0], 0, 200);
}

// >>> W7-BLOCKED CUser::SetSkillLevel —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 12. CUser::SetSkillLevel @0x0867b51c
// ---------------------------------------------------------------------------
// int CUser::SetSkillLevel(int skillIdx, int level)
// {
    // m_field711d8(Client_Spec 内职业可改标志)==1 且当前角色空 → 返 0
//     if (m_clientSpec.m_pad[0] /* +0x711d8 占位位见 fields_F */ != 0 ||
//         getCurCharacR() != 0)
//         return 0;
//
//     CDataManager* dm = G_CDataManager();
//     CSkill* skill = dm->find_skill(get_charac_job(), skillIdx);
//     if (skill == 0)
//         return 0;
//
//     int maxLevel;
//     {
//         char firstGrow = getCurCharFirstGrowType();
//         char secondGrow = getCurCharSecondGrowType();
//         if (secondGrow < 1)
//             maxLevel = skill->get_type_max_level(static_cast<int>(firstGrow));
//         else
//         {
//             maxLevel = skill->get_second_growtype_max_level(
//                 static_cast<int>(firstGrow), static_cast<int>(secondGrow));
//             if (maxLevel == 0)
//                 maxLevel = skill->get_type_max_level(static_cast<int>(firstGrow));
//         }
//     }
    // ORIG: std::min(maxLevel, level) -> std::max(result, 0)
//     level = std::max(std::min(maxLevel, level), 0);
//
//     int treeIdx = GetCurCharacSkillTreeIndex();
//     ((SkillSlot*)getCurCharacSkillW())->growtype_skill(get_charac_job(), skillIdx, level,
//                                          treeIdx);
//     return 1;
// }

// ---------------------------------------------------------------------------
// 13. CUser::setTagCharacInfo @0x08690098
//     m_characList[idx] 元素字段:+0 no / +4 name(strncpy 0x1d) / +0x26 job byte /
//     +0x27 grow ushort / +0x29 扩展 byte / +0x1239 byte / +0x88 起 memcpy 0x52。
//     TAG_CHARAC_DATA 源偏移:+0/+4/+0x21/+0x22(w)/+0x24/+0x25/+0x26(0x52)。
// ---------------------------------------------------------------------------
int CUser::setTagCharacInfo(int idx, unsigned int characNo, TAG_CHARAC_DATA* data)
{
    if (idx < 0 || (size_t)idx >= m_characList.size())
        return 0;
    if (m_characList[idx].m_characNo != characNo)
        return 0;

    char* dst = reinterpret_cast<char*>(&m_characList[idx]);
    char* src = reinterpret_cast<char*>(data);

    *reinterpret_cast<unsigned int*>(dst) =
        *reinterpret_cast<unsigned int*>(src);                       // no
    strncpy(dst + 4, src + 4, 0x1d);                                 // name
    dst[0x26]    = src[0x21];
    *reinterpret_cast<unsigned short*>(dst + 0x27) =
        *reinterpret_cast<unsigned short*>(src + 0x22);
    dst[0x29]    = src[0x24];
    dst[0x1239]  = src[0x25];
    memcpy(dst + 0x88, src + 0x26, 0x52);
    return 1;
}

// >>> W7-BLOCKED CUser::setTagCharacInven —— tag inven 为 Inven_Item 数组视图，草稿按 CInventory 处理语义混乱；GetItemIdByAddInfo 未核
// ---------------------------------------------------------------------------
// 14. CUser::setTagCharacInven @0x08690226
// ---------------------------------------------------------------------------
// int CUser::setTagCharacInven(TAG_CHARAC_INVEN_DATA* data)
// {
//     if (getTagCharacR() == 0)
//     {
//         cMyTrace trace(__PRETTY_FUNCTION__, 0xb166, 5);
//         trace("CUser::setTagCharacInven m_tag_selected is null");
//         return 0;
//     }
//
//     CInventory* inven = getTagCharacInvenW();
//     memcpy(reinterpret_cast<char*>(inven) + 0x1c, data, 0x53e);
//     inven->SetClearAvatar(
//         reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(data) + 0x3ff8));
//
//     char* base = reinterpret_cast<char*>(data);
//     for (int i = 0; i < *reinterpret_cast<int*>(base + 0x1e43); ++i)
//     {
//         if (*reinterpret_cast<int*>(base + i * 0x4b + 0x1e6c) != 0)
//         {
//             int invenSlot = GetItemIdByAddInfo(reinterpret_cast<Inven_Item*>(base + 0x53e),
//                                                0x69,
//                                                *reinterpret_cast<int*>(base + i * 0x4b + 0x1e6c));
//             for (int k = 0; k <= 9; ++k)
//             {
//                 if (*reinterpret_cast<int*>(base + k * 0x3d + 7) ==
//                     *reinterpret_cast<int*>(base + i * 0x4b + 0x1e47))
//                     inven->SetClearAvatar(k, invenSlot);
//             }
//         }
//     }
//
//     int capacity = *reinterpret_cast<int*>(base + 0x4020);
//     if (capacity == 0)
//         getTagCharacCargoW()->SetCapacity(8);
//     else
//         getTagCharacCargoW()->SetCapacity(capacity);
//     return 1;
// }

// ---------------------------------------------------------------------------
// 15. CUser::SetTradePunishOverThirtyDays @0x08689850
// ---------------------------------------------------------------------------
void CUser::SetTradePunishOverThirtyDays(bool flag)
{
    printf(" CUser::SetTradePunishOverThirtydays(%c) \n",
           static_cast<unsigned int>(flag));
    m_postChecksumData.m_tradePunishOverThirtyDays = flag;
}

// ---------------------------------------------------------------------------
// 16. CUser::setUsedBreakAwayLuckPoint @0x086971ee W(报告缺失,objdump 直转)
// ---------------------------------------------------------------------------
void CUser::setUsedBreakAwayLuckPoint(bool flag)
{
    m_usedBreakAwayLuckPoint = flag;
}

// >>> W7-BLOCKED CUser::st_monstorkill —— CUser::st_monstorkill 嵌套类型未建模于权威头
// ---------------------------------------------------------------------------
// 17. CUser::st_monstorkill::Check @0x0823049c size 0x1cd(报告缺失,objdump 直转)
//     嵌套类布局:+0 maxX/+2 minX/+4 maxY/+6 minY/u32 lastTick@+8/int cnt@+0xc/
//     short deltaX@+0x10/short deltaY@+0x12。超时(now-lastTick>limit)复位返 false;
//     窗口收窄后 dx=abs(minX-maxX),dy=abs(minY-maxY);
//     dx>=xLim 或 dy<yLim → 返 true;否则(dx<xLim && dy>=yLim)复位返 false。
// ---------------------------------------------------------------------------
// bool CUser::st_monstorkill::Check(unsigned int now, unsigned short x1,
//                                   unsigned short y1, unsigned short xLim,
//                                   unsigned short yLim, unsigned int limit)
// {
//     if (static_cast<unsigned int>(now - m_lastTick) > limit)
//     {
        // 复位并结算(内联,ORIG 无独立 helper 符号)
//         m_deltaX = 0; m_deltaY = 0; m_count = 0; m_lastTick = now;
//         m_maxX = x1; m_maxY = y1; m_minX = x1; m_minY = y1;
//         return false;
//     }
//
//     ++m_count;
//     if (static_cast<short>(m_maxX) - static_cast<short>(x1) > 0) m_maxX = x1;
//     if (static_cast<short>(m_maxY) - static_cast<short>(y1) > 0) m_maxY = y1;
//     if (static_cast<short>(m_minX) - static_cast<short>(x1) < 0) m_minX = x1;
//     if (static_cast<short>(m_minY) - static_cast<short>(y1) < 0) m_minY = y1;
//
//     m_deltaX = static_cast<short>(static_cast<short>(m_minX) -
//                                   static_cast<short>(m_maxX));
//     m_deltaY = static_cast<short>(static_cast<short>(m_minY) -
//                                   static_cast<short>(m_maxY));
//     {
//         int dx = m_deltaX; dx = dx < 0 ? -dx : dx;      // abs(sar+xor+sub 形态)
//         int dy = m_deltaY; dy = dy < 0 ? -dy : dy;
//         m_deltaX = static_cast<short>(dx);
//         m_deltaY = static_cast<short>(dy);
//     }
//
//     if (static_cast<int>(m_deltaX) >= static_cast<int>((short)xLim))
//         return true;
//     if (static_cast<int>(m_deltaY) < static_cast<int>((short)yLim))
//         return true;
//
    // 复位并结算(dx<xLim 且 dy>=yLim 路径)
//     m_deltaX = 0; m_deltaY = 0; m_count = 0; m_lastTick = now;
//     m_maxX = x1; m_maxY = y1; m_minX = x1; m_minY = y1;   // ORIG 写序 f0/f4/f2/f6
//     return false;
// }

// ---------------------------------------------------------------------------
// 18. CUser::StartRecvEventWaiting @0x0867f1fe
// ---------------------------------------------------------------------------
void CUser::StartRecvEventWaiting()
{
    m_mcap.m_recvEventWaiting = false;                // CMCAPManager +0x18（CUser+0x8e408）

    unsigned short uid  = static_cast<unsigned short>(get_unique_id());
    unsigned int   uidw = GetUID();
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2),
                                uidw,
                                static_cast<TIMER_MESSAGE>(0x67),
                                0x1e, static_cast<int>(uid), 0u);
}

// ---------------------------------------------------------------------------
// 19. CUser::Update_ComboMission @0x0866280a
// ---------------------------------------------------------------------------
void CUser::Update_ComboMission(int comboIdx, short count)
{
    CMissionList_Charac* missions =
        reinterpret_cast<CMissionList_Charac*>(
            GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8));
    missions->Update_ComboMission_Event(*this, comboIdx, count);
    send_MissionList();
}

// ---------------------------------------------------------------------------
// 20. CUser::update_ingame_event_history @0x0868d72c
// ---------------------------------------------------------------------------
void CUser::update_ingame_event_history(int eventKey)
{
    m_map8eb80[eventKey] = true;                         // map<int,bool>@+0x8eb80
}

// ---------------------------------------------------------------------------
// 21. CUser::update_record @0x08676768→0x08676876(map find/highscore/insert)
// ---------------------------------------------------------------------------
int CUser::update_record(GameResultType& result)
{
    std::map<int, GameResultType>* records = getBattleRecordRefW();

    int key = result.GetUniqueKey();                     // -0x78 先物化(P28 声明序近似)
    std::map<int, GameResultType>::iterator it = records->find(key);
    if (it != records->end())
        return it->second.UpdateHighScore(result);       // 高分解就地更新

    int newKey = result.GetUniqueKey();
    records->insert(std::make_pair(newKey, result));     // pair<int,GRT&> converting ctor
    return 0xf;
}

// >>> W7-BLOCKED CUser::updateBingoData —— DB_UpdateBingoEvent 实参形态(int,uint,int&,bool& vs T 表)待核
// ---------------------------------------------------------------------------
// 22. CUser::updateBingoData @0x086914cc
// ---------------------------------------------------------------------------
// void CUser::updateBingoData()
// {
//     if (m_bingo.m_needSave == 0)
//         return;
//
//     bool won     = m_bingo.m_isWin != 0;
//     int  eventId = m_bingo.m_eventId;
//     DB_UpdateBingoEvent::makeRequest(static_cast<int>(GetUID()),
//                                      get_acc_id(), eventId, won);
//     m_bingo.m_needSave = 0;
// }
//
// >>> W7-BLOCKED CUser::UpdateBloodRecord —— stBloodBestRecord map 值结构>=0xc 与权威双字段矛盾
// ---------------------------------------------------------------------------
// 23. CUser::UpdateBloodRecord @0x08651de6
// ---------------------------------------------------------------------------
// void CUser::UpdateBloodRecord()
// {
//     if (getCurCharacR() == 0)
//         return;
//
//     std::map<int, stBloodBestRecord>& bests = m_bloodBestRecord;   // fields_F 目标态
//
//     for (std::map<int, stBloodBestRecord>::iterator it = bests.begin();
//          it != bests.end(); ++it)
//     {
//         Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bbb);
//         CStreamGuard guard(stream, true);
//         guard << 0x140;                                             // 320 sig size
//         guard << GetUID();
//
//         SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD* sig = guard->GetInBuffer<SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>();
//         memset(sig, 0, 0x10);
//         *reinterpret_cast<unsigned int*>(sig)              = getCurCharacNo();
//         *reinterpret_cast<unsigned int*>(sig + 4)          = it->second.m_field8;
//         *reinterpret_cast<unsigned int*>(sig + 8)          = it->second.m_field4;
//         *reinterpret_cast<unsigned int*>(sig + 12)         = it->first;
//         GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2),
//                                      guard);
//     }
//     setBloodBestRecordUpdate(false);
// }
//
// ---- body_F.cpp 续篇 B(接续篇 A 之后拼接)----

// ---------------------------------------------------------------------------
// 24. CUser::updateBlueMarbleEnterCount @0x0869106c
// ---------------------------------------------------------------------------
void CUser::updateBlueMarbleEnterCount()
{
    unsigned char count = getBlueMarbleEnterCount();     // CUserCharacInfo
    unsigned int characNo = get_charac_no(-1);
    DB_UpdateBlueMarbleEnterCount::makeRequest(characNo, count);
}

// ---------------------------------------------------------------------------
// 25. CUser::UpdateBreakAwayDungeonClearCnt @0x086522ea
// ---------------------------------------------------------------------------
void CUser::UpdateBreakAwayDungeonClearCnt()
{
    setBreakAwayDungeonClear(false);

    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1c01);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x145);
    guard.operator*()->operator<<(GetUID());
    guard.operator*()->operator<<(get_acc_id());
    guard.operator*()->operator<<(getBreakAwayDungeonClearCnt());
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
}

// ---------------------------------------------------------------------------
// 26. CUser::UpdateBreakAwayLuckPoint @0x086521ec
// ---------------------------------------------------------------------------
void CUser::UpdateBreakAwayLuckPoint()
{
    setUsedBreakAwayLuckPoint(false);

    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bf2);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x149);
    guard.operator*()->operator<<(GetUID());
    guard.operator*()->operator<<(get_acc_id());
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
}

// ---------------------------------------------------------------------------
// 27. CUser::UpdateCharacLinkBonus @0x0865240c
//     实参写流序:(bonusState@+0x18 先物化) 0x14c/uid/state/exp/gold/bonusState
//     —— asm:local_10=param5 缓存;写入序 sig:uid,state(+0x14 参3),exp(参1),
//     gold(参2),bonusState。按 Ghidra operator<< 顺序照录。
// ---------------------------------------------------------------------------
void CUser::UpdateCharacLinkBonus(unsigned int exp, unsigned int gold,
                                  unsigned int state,
                                  ENUM_CHARAC_LINK_BONUS_STATE bonusState)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1c11);
    CStreamGuard guard(stream, true);
    guard << 0x14c;
    guard << GetUID();
    guard << state;
    guard << exp;
    guard << gold;
    guard << static_cast<unsigned int>(bonusState);
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);

    setCharacLinkBonusExp(0);
    setCharacLinkBonusGold(0);
}

// ---------------------------------------------------------------------------
// 28. CUser::UpdateCharacNameInMercenary @0x084ec5e6 W(报告缺失,objdump 直转)
//     lea this+0x6ef6c → UserMercenaryInfoMgr::UpdateCharacName(uint,string const&)
//     (成员声明 NEEDS_DECL,ORIG _ZN20UserMercenaryInfoMgr16UpdateCharacNameEjRKSs)
// ---------------------------------------------------------------------------
void CUser::UpdateCharacNameInMercenary(unsigned int characNo, const std::string& name)
{
    m_mercenaryMgr.UpdateCharacName(characNo, name);
}

// >>> W7-BLOCKED CUser::updateComboSkill —— SIG_COMBO_SKILL reset 与 getComboSkillSaveData(SkillSlot char* cast 后)成员未建模
// ---------------------------------------------------------------------------
// 29. CUser::updateComboSkill @0x08692396
// ---------------------------------------------------------------------------
// void CUser::updateComboSkill()
// {
//     if (get_charac_job() != 9)
//         return;
//
//     Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0xb697);
//     CStreamGuard guard(stream, true);
//     guard << 0x31d;
//     guard << -1;
//
//     SIG_COMBO_SKILL* sig = guard->GetInBuffer<SIG_COMBO_SKILL>();
//     sig->reset();
//     ((SkillSlot*)getCurCharacSkillR())->getComboSkillSaveData(sig);
//
//     GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
// }

// ---------------------------------------------------------------------------
// 30. CUser::UpdateCurrentCharacRentalInfo @0x08676e3c
//     m_rentalInfo[machineKey][curCharacNo]:可选 clear;
//     对 output.m_deleted 逐项 find_if(FindRentalInfoByItemIndex)+erase;
//     再 push_back 全部 output.m_infos。
// ---------------------------------------------------------------------------
namespace {
class FindRentalInfoByItemIndex   // ORIG 全局 functor ctor @0x08697d2c
{
public:
    explicit FindRentalInfoByItemIndex(int idx) : m_itemIndex(idx) {}
    bool operator()(const InstanceRentalSystem::RentalInfo& info) const
    { return info.m_itemIndex == m_itemIndex; }
private:
    int m_itemIndex;
};
}  // namespace

void CUser::UpdateCurrentCharacRentalInfo(
    const InstanceRentalSystem::RentOutput& output, int machineKey, bool clearFirst)
{
    unsigned int characNo = getCurCharacNo();

    std::vector<InstanceRentalSystem::RentalInfo>& list =
        m_rentalInfo[static_cast<unsigned int>(machineKey)][characNo];

    if (clearFirst)
        list.clear();

    for (std::vector<InstanceRentalSystem::DeletedRentItem>::const_iterator del =
             output.m_deleted.begin();
         del != output.m_deleted.end(); ++del)
    {
        std::vector<InstanceRentalSystem::RentalInfo>::iterator found =
            std::find_if(list.begin(), list.end(),
                         FindRentalInfoByItemIndex(del->m_itemId));
        if (found != list.end())
            list.erase(found);
    }

    for (std::vector<InstanceRentalSystem::RentalInfo>::const_iterator ri =
             output.m_infos.begin();
         ri != output.m_infos.end(); ++ri)
        list.push_back(*ri);
}

// ---------------------------------------------------------------------------
// 31. CUser::UpdateDimensionInout @0x08651fba
// ---------------------------------------------------------------------------
void CUser::UpdateDimensionInout()
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bd0);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x14f);
    guard.operator*()->operator<<(GetUID());

    SIG_UPDATE_DIMENSION* sig = guard->GetInBuffer<SIG_UPDATE_DIMENSION>();
    memset(sig, 0, 0x10);
    *reinterpret_cast<unsigned int*>(sig) = getCurCharacNo();

    for (int i = 0; i <= 5; ++i)
    {
        char v = static_cast<char>(getDemensionInoutValue(static_cast<char>(i)));
        bool abnormal =
            (v < 0) ||
            ((G_CDataManager()->get_dimensionInout(i)) + 10 < static_cast<int>(v));
        if (abnormal)
        {
            reinterpret_cast<char*>(sig)[4 + i] = 0;
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1bde, 0);
            trace("DIMENSION DUNGEON INOUT_COUNT ABNORMAL USER!! CHARAC_NO(%d)",
                  *reinterpret_cast<unsigned int*>(sig));
        }
        else
        {
            reinterpret_cast<char*>(sig)[4 + i] = v;
        }
    }

    reinterpret_cast<char*>(sig)[0xc] =
        static_cast<char>(getUltimateInoutValue('\x02'));
    reinterpret_cast<char*>(sig)[0xd] =
        static_cast<char>(IsSaveDemensionInoutValue());
    reinterpret_cast<char*>(sig)[0xe] =
        static_cast<char>(IsSaveBloodInoutValue());
    SetSaveDemensionInoutValue(false);
    SetSaveBloodInoutValue(false);

    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
}

// ---------------------------------------------------------------------------
// 32. CUser::UpdateEquipslotFlag @0x0868aa54
// ---------------------------------------------------------------------------
int CUser::UpdateEquipslotFlag(char flag)
{
    if (get_charac_level() <= 0x2c)
        return 0;

    if (flag == 0)
    {
        if (getCurEquipslotFlag() == 2)
            return 2;
        setCurEquipslotFlag(1);
        return getCurEquipslotFlag();
    }
    if (flag == 2)
    {
        setCurEquipslotFlag(2);
        return getCurEquipslotFlag();
    }
    return 0x13;
}

// ---------------------------------------------------------------------------
// 33. CUser::UpdateMissionCondition @0x086625c8
// ---------------------------------------------------------------------------
void CUser::UpdateMissionCondition(std::vector<short>& kinds, std::vector<int>& values)
{
    CMissionList_Charac* missions =
        reinterpret_cast<CMissionList_Charac*>(
            GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8));
    missions->Update_RecvPacket_event(*this, kinds, values);
}

// >>> W7-BLOCKED CUser::updatePcRoomPlayTime —— EventClassify::CPcRoomPlayTime get_event_entity 下转与 DB_SavePcRoomPlayTimeEvent 实参
// ---------------------------------------------------------------------------
// 34. CUser::updatePcRoomPlayTime @0x0868f726
// ---------------------------------------------------------------------------
// void CUser::updatePcRoomPlayTime()
// {
//     EventClassify::CEventScriptMng* mng = static_cast<EventClassify::CEventScriptMng*>(G_CDataManager()->get_event_script_mng());
//     EventClassify::CPcRoomPlayTime* entity = mng->get_event_entity(0x5c);
//     if (entity == 0)
//         return;
//     if (entity->verifyUserPcRoomPlayEvent(this) != 0)
//         return;
//
//     collectPcRoomPlayTime();
//     DB_SavePcRoomPlayTimeEvent::makeRequest(get_acc_id(),
//                                             getTotalPcRoomPlayTime(),
//                                             getPcRoomPlayTimeRewardCnt());
// }

// 35. CUser::UpdateRating：唯一 owner 为 LinkClosureA_User.cpp（等价实现去重）。
// ---------------------------------------------------------------------------
// 36. CUser::use_equipment @0x0865ede0
// ---------------------------------------------------------------------------
void CUser::use_equipment(int slot)
{
    int result = getCurCharacInvenW()->use_equip(slot);

    PacketGuard packet;
    packet.put_header(1, 0x33);
    if (result < 1)
    {
        packet.put_byte(1);
        packet.put_byte(slot);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(result);
    }
    packet.finalize(true);
    Send(packet);
}

// ---------------------------------------------------------------------------
// 37. CUser::use_multibox_statistic @0x086767d2
// ---------------------------------------------------------------------------
void CUser::use_multibox_statistic(unsigned int itemIdx)
{
    int kind = 0;
    if (itemIdx == 0x289749)
        kind = 2;
    else if (itemIdx == 0x28982d)
        kind = 1;
    else if (itemIdx != 0x380)
        kind = 0;                                   // ORIG 默认亦 0(asm 语义)

    SendRandomBoxStatistic(kind, (ENUM_RANDOMBOX_ACTION_KIND)1);                // ORIG 第二参恒 1
}

// ---- body_F.cpp 续篇 C(接续篇 B 之后拼接;文件尾)----

// >>> W7-BLOCKED CUser::use_stackable —— GetInvenSlot(sret,invenType,slotNo) 引出参形态与 expert_job::CAlchemist 无对象静态调用形态未核
// ---------------------------------------------------------------------------
// 38. CUser::use_stackable @0x0865e0ae
// ---------------------------------------------------------------------------
// bool CUser::use_stackable(short slotNo, int space, int param3, int param4)
// {
//     int err = 0;
//     if (CheckInTrade())
//         err = 0x13;
//
//     char slotBuf[sizeof(Inven_Item)];
//     getCurCharacInvenR()->GetInvenSlot(  // ORIG GetInvenSlot(out,inven) 引出参形态
//         *reinterpret_cast<Inven_Item*>(slotBuf), space);
//     unsigned long long itemIdx =
//         *reinterpret_cast<unsigned long long*>(slotBuf + 1);   // Inven_Item+1 idx64
//
//     CItem* item = G_CDataManager()->find_item((int)itemIdx);
//     if (item == 0)
//         err = 0x17;
//     else if (item->is_stackable() != 1)
//         err = 0x17;
//
//     if (err == 0 &&
//         expert_job::CAlchemist::CheckUsableStackableItem(this, item) != 1)
//     {
//         PacketGuard packet;
//         packet.put_header(1, 0x2f);
//         packet.put_byte(0);
//         packet.put_byte(7);
//         packet.put_byte(space);
//         packet.put_int(param3);
//         packet.put_int(param4);
//         packet.finalize(true);
//         Send(packet);
//         return true;
//     }
//
//     if (err == 0 && CheckCoolTimeItem(itemIdx))
//     {
//         PacketGuard packet;
//         packet.put_header(1, 0x2f);
//         packet.put_byte(0);
//         packet.put_byte(0x13);
//         packet.put_byte(space);
//         packet.put_int(param3);
//         packet.put_int(param4);
//         packet.finalize(true);
//         Send(packet);
//         return true;
//     }
//
//     if (err == 0)
//         err = getCurCharacInvenW()->use_item(slotNo, 1, space);
//
//     if (err == 0)
//         Global::CBossTowerFunc::UseItem(this,
//             *reinterpret_cast<Inven_Item*>(slotBuf));
//
//     if (err == 0)
//         _processContinuousItem(static_cast<unsigned int>(itemIdx),
//                                reinterpret_cast<CStackableItem*>(item));
//
//     if (err == 0)
//         expert_job::CAlchemist::OnUseStackableItem(
//         this, static_cast<unsigned long long>(itemIdx),
//         reinterpret_cast<CStackableItem*>(item));
//
//     if (err == 0)
//     {
        // 疲劳/LV70 buff 分支(itemIdx 判定链照录)
//         if (itemIdx == 0x1e70)       SendLV70Buff(this, 0x1e77);
//         else if (itemIdx < 0x1e71)
//         {
//             if (itemIdx < 0x1dde)
//             {
//                 if (itemIdx == 0x1d20 || itemIdx == 0x1d5e)
//                 {
//                     int delta = (itemIdx == 0x1d20) ? -0x28 : -10;
//                     setCurCharacFatigue(std::max(
//                         static_cast<int>(getCurCharacFatigue()) + delta, 0));
//                     SendFatigue();
//                 }
//             }
//             else                      // 0x1dde..0x1e70
//             {
//                 setCurCharacFatigue(std::max(
//                     static_cast<int>(getCurCharacFatigue()) - 6, 0));
//                 SendFatigue();
//             }
//         }
//         else
//         {
//             switch (itemIdx)
//             {
//             case 0x1e6d: SendLV70Buff(this, 0x1e74); break;
//             case 0x1e6e: SendLV70Buff(this, 0x1e75); break;
//             default:
//                 if (itemIdx < 0x1f8f)
//                 {
//                     if (itemIdx >= 0x1e71 && itemIdx <= 0x1e73)
//                     {
//                         unsigned buff = (itemIdx == 0x1e72) ? 0x1e78 :
//                                         (itemIdx == 0x1e73) ? 0x1e7a : 0x1e79;
//                         SendLV70Buff(this, buff);
//                     }
//                     else if (itemIdx == 0x201d)
//                     {
//                         setCurCharacFatigue(std::max(
//                             static_cast<int>(getCurCharacFatigue()) - 0x14, 0));
//                         SendFatigue();
//                     }
//                 }
//                 else if (itemIdx == 0x1f8f || itemIdx == 0x27ac5a ||
//                          itemIdx == 0x27addd || itemIdx == 0x292090de)
//                 {
//                     setCurCharacFatigue(std::max(
//                         static_cast<int>(getCurCharacFatigue()) - 0x1e, 0));
//                     SendFatigue();
//                 }
//                 break;
//             case 0x1e72: SendLV70Buff(this, 0x1e79); break;
//             case 0x1e73: SendLV70Buff(this, 0x1e7a); break;
//             }
//         }
//     }
//
//     PacketGuard reply;
//     reply.put_header(1, 0x2f);
//     bool failed = (err != 0);
//     if (failed)
//     {
//         reply.put_byte(0);
//         reply.put_byte(err);
//         reply.put_byte(space);
//         reply.put_int(param3);
//         reply.put_int(param4);
//         reply.finalize(true);
//         Send(reply);
//
//         char reSlot[sizeof(Inven_Item)];
//         getCurCharacInvenR()->GetInvenSlot(*reinterpret_cast<Inven_Item*>(reSlot),
//                                            space);
//         cMyTrace trace(__PRETTY_FUNCTION__, 0x38b5, 5);
//         trace("CUser::use_stackable\tError:%d\tSlot_no:%d\tItem_no:%d",
//               err, (int)slotNo,
//               *reinterpret_cast<int*>(reSlot + 3));
//     }
//     else
//     {
//         reply.put_byte(1);
//         reply.put_short(slotNo);
//         reply.put_byte(space);
//         reply.put_int(param3);
//         reply.put_int(param4);
//         reply.finalize(true);
//         Send(reply);
//         ARAD::ARAD_ITEM::arad_secret_shop_event_item(this, itemIdx,
//                                                      reinterpret_cast<CStackableItem*>(item));
//     }
//     return !failed;
// }

// ---------------------------------------------------------------------------
// 39. CUser::UseCeraPoint @0x08692b16
// ---------------------------------------------------------------------------
unsigned int CUser::UseCeraPoint(unsigned int amount)
{
    if (m_ceraPoint < amount)
        m_ceraPoint = 0;
    else
        m_ceraPoint = m_ceraPoint - amount;
    return m_ceraPoint;
}

// ---------------------------------------------------------------------------
// 40. CUser::UseLimitItem @0x08688ae6
// ---------------------------------------------------------------------------
unsigned char CUser::UseLimitItem(int itemIdx, bool sendNoti)
{
    unsigned char ok = 1;
    for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin();
         it != m_limitItemUsage.end(); ++it)
    {
        if (it->m_field15 != itemIdx)
            continue;
        if (it->isUsable() != 1)
            ok = 0;
        if (sendNoti)
            SendLimitItemUsageNotiInfo(itemIdx,
                                       *reinterpret_cast<signed char*>(
                                           reinterpret_cast<char*>(&*it) + 0x11),
                                       it->m_field19);
        if (it->useIt())
        {
            long long t = *reinterpret_cast<long long*>(
                reinterpret_cast<char*>(&*it) + 0xc);
            DB_UpdateLimitItemUsage::makeRequest(get_charac_no(-1),
                *reinterpret_cast<signed char*>(reinterpret_cast<char*>(&*it) + 0x10),
                itemIdx, it->m_field19, t);
            return 1;
        }
        return 0;
    }
    return ok;
}

// ---------------------------------------------------------------------------
// 41. CUser::UseMileage @0x08650a10
// ---------------------------------------------------------------------------
void CUser::UseMileage(int cost)
{
    int after = m_mileage - cost;
    LogHistory("Mileage %s,%d,%d,%d", "Use", cost, m_mileage,
               ~(after >> 31) & after);          // max(after,0) 位形态照录
    if (after < 0)
        m_mileage = 0;
    else
        m_mileage = after;
    SaveMileage();                               // NEEDS_DECL(见尾注)
}

// >>> W7-BLOCKED CUser::UseQuestItem —— getCurCharacInvenSpace 发明调用与邮件包字段链未核
// ---------------------------------------------------------------------------
// 42. CUser::UseQuestItem @0x08668d1a(主干分支映射全录;局部数组名随栈语义)
// ---------------------------------------------------------------------------
// void CUser::UseQuestItem(int itemIdx)
// {
//     if (get_state() != 3)
//         return;
//
//     PacketGuard packet;
//     if (CheckInTrade())
//     {
//         InterfacePacketBuf::clear(&packet);
//         SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x20), 0x13, packet);
//         return;
//     }
//
//     char curSlot[sizeof(Inven_Item)];
//     getCurCharacInvenR()->GetInvenSlot(*reinterpret_cast<Inven_Item*>(curSlot),
//                                        getCurCharacInvenSpace());
//     unsigned int curIdx =
//         *reinterpret_cast<unsigned int*>(curSlot + 4);     // local_9b
//
//     char grow = 0;              // local_51:目标职业档位
//     int  subType = 0;           // local_4c:二转/子类
//     int  keepSkill = 0;         // local_50:保留技能包标志
//     char doChangeGrow = 0;      // local_45
//
//     struct MapEntry { unsigned int lo, hi; char g; int s; int k; };
//     static const MapEntry kMap[] = {
//         {0xfc9,0xfc9,0,1,0},{0xfca,0xfca,0,2,0},{0xfcb,0xfcb,0,3,0},{0xfcc,0xfcc,0,4,0},
//         {0xfcd,0xfcd,1,2,0},{0xfce,0xfce,1,1,0},{0xfcf,0xfcf,1,3,0},{0xfd0,0xfd0,1,4,0},
//         {0xfd1,0xfd1,2,1,0},{0xfd2,0xfd2,2,3,0},{0xfd3,0xfd3,2,2,0},{0xfd4,0xfd4,2,4,0},
//         {0xfef,0xfef,3,1,0},{0xff0,0xff0,3,3,0},{0xff1,0xff1,3,2,0},{0xff2,0xff2,3,4,0},
//         {0xff6,0xff6,0,4,1},{0xff7,0xff7,0,1,1},{0xff8,0xff8,0,2,1},
//         {0xffb,0xffb,1,2,1},{0xffc,0xffc,1,1,1},{0xffd,0xffd,1,3,1},{0xffe,0xffe,1,4,1},
//         {0xfff,0xfff,2,1,1},{0x1000,0x1000,2,3,1},{0x1001,0x1001,2,2,1},{0x1002,0x1002,2,4,1},
//         {0x1003,0x1003,3,1,1},{0x1004,0x1004,3,3,1},{0x1005,0x1005,3,2,1},{0x1006,0x1006,3,4,1},
//         {0x1044,0x1044,4,2,0},{0x1045,0x1045,4,1,0},{0x1046,0x1046,4,3,0},
//         {0x1047,0x1047,4,3,0},{0x1048,0x1048,4,4,0},
//         {0x105b,0x105b,1,4,1},{0x105c,0x105c,1,1,1},{0x105d,0x105d,1,2,1},{0x105e,0x105e,1,3,1},
//         {0x105f,0x105f,1,4,1},{0x1060,0x1060,2,1,1},{0x1061,0x1061,2,2,1},{0x1062,0x1062,2,3,1},
//         {0x1063,0x1063,2,4,1},{0x1064,0x1064,3,1,1},{0x1065,0x1065,3,2,1},
//         {0x1079,0x1079,3,3,1},{0x107a,0x107a,3,4,1},{0x107b,0x107b,0,1,1},{0x107c,0x107c,0,2,1},
//         {0x107d,0x107d,0,3,1},
//         {0x1f1c,0x1f1c,3,4,1},{0x1f1d,0x1f1d,4,1,1},{0x1f1e,0x1f1e,4,2,1},{0x1f1f,0x1f1f,4,3,1},
//         {0x1f20,0x1f20,5,1,0},{0x1f21,0x1f21,5,3,1},
//         {0x1f22,0x1f22,5,2,0},{0x1f23,0x1f23,5,4,0},{0x1f24,0x1f24,5,1,1},
//         {0x1f25,0x1f25,5,3,1},{0x1f26,0x1f26,5,2,1},{0x1f27,0x1f27,5,4,1},
//         {0x1f28,0x1f28,6,1,0},{0x1f29,0x1f29,6,2,0},
//         {0x1f2a,0x1f2a,6,1,1},{0x1f2b,0x1f2b,6,2,1},
//         {0x1f50,0x1f50,7,1,0},{0x1f51,0x1f51,7,3,0},{0x1f52,0x1f52,7,2,0},
//         {0x1f53,0x1f53,7,4,0},{0x1f54,0x1f54,7,1,1},{0x1f55,0x1f55,7,3,1},
//         {0x1f56,0x1f56,7,2,1},{0x1f57,0x1f57,7,4,1},
//     };
//
    // ---- ORIG 判定链(Lab 二叉树)重构为线性 guard(语义等价):----
//     bool reject = false;
//     do {
//         const MapEntry* hit = 0;
//         for (size_t mi = 0; mi < sizeof(kMap)/sizeof(kMap[0]); ++mi)
//         {
//             if (kMap[mi].lo <= curIdx && curIdx <= kMap[mi].hi)
//             { hit = &kMap[mi]; break; }
//         }
//         if (hit == 0)                       // 未知道具 → 0x20/0x11 错误包
//         { reject = true; break; }
//
//         grow      = hit->g;
//         subType   = hit->s;
//         keepSkill = hit->k;
//         doChangeGrow = 1;
//
        // ORIG 判定链:六段“均不在”|| firstGrow!=0 为真才进入 grow 边界判断;
        // 否则(处于 special 区间的初代角色)直接发 0x20/0x11 错误包。
        // 因 map 必中某段(未知道具此前已 reject),outsideAll 恒 false,
        // 门槧行为上等价于「非初代不可用」。
//         bool outsideAll =
//             (curIdx < 0x105c || 0x1065 < curIdx) &&
//             (curIdx < 0x1079 || 0x107d < curIdx) &&
//             (curIdx < 0x1f1c || 0x1f1f < curIdx) &&
//             (curIdx < 0x1f24 || 0x1f27 < curIdx) &&
//             (curIdx < 0x1f2a || 0x1f2b < curIdx) &&
//             (curIdx < 0x1f54 || 0x1f57 < curIdx);
//         if (!(outsideAll || getCurCharFirstGrowType() != 0))
//         { reject = true; break; }
//
//         if (static_cast<char>(grow) < 0 || static_cast<char>(grow) > 10)
//         { reject = true; break; }
//         if (grow != get_charac_job())
//         { reject = true; break; }
//     } while (0);
//
//     if (reject)
//     {
//         InterfacePacketBuf::clear(&packet);
//         packet->put_header( 1, 0x20);
//         packet->put_byte( 0);
//         packet->put_byte( 0x11);
//         packet->finalize( true);
//         Send(packet);
//         return;
//     }
//
//     {
//         int levelLow  = G_CDataManager()->get_level_exp(get_charac_level());
//         int levelHigh = G_CDataManager()->get_level_exp(get_charac_level() + 1);
//         unsigned int exp = getCurCharacExp();
//         if (exp < (unsigned int)levelLow)
//             setCurCharacExp(levelLow);
//         else if ((unsigned int)levelHigh < exp)
//             setCurCharacExp(levelHigh - 1);
//
//         int slots[312];          // local_68c
//         int idxs[312];           // local_b6c
//         memset(slots, 0, sizeof(slots));
//         memset(idxs, 0, sizeof(idxs));
//         int n = 0;
//
//         for (int s = 3; s < 9; ++s)
//         {
//             char buf[sizeof(Inven_Item)];
//             getCurCharacInvenR()->GetInvenSlot(*reinterpret_cast<Inven_Item*>(buf),
//                                                getCurCharacInvenSpace());
//             unsigned int id = *reinterpret_cast<unsigned int*>(buf + 4);
//             int cnt = *reinterpret_cast<int*>(buf + 12);
//             bool match =
//                 keepSkill != 0
//                   ? (id > 0x105b && id < 0x1066 && id >= 0x1079 && id <= 0x107d &&
//                      id >= 0x1f1c && id <= 0x1f1f && id >= 0x1f24 && id <= 0x1f27 &&
//                      id >= 0x1f2a && id <= 0x1f2b && id >= 0x1f54 && id <= 0x1f57)
//                   : (((id > 0xfc8 && id < 0xfd5) || (id > 0xfee && id < 0xff3) ||
//                       (id > 0xff6 && id < 0x1007) || (id > 0x1044 && id < 0x1049) ||
//                       (id > 0x1f1f && id < 0x1f24) || (id > 0x1f27 && id < 0x1f2a) ||
//                       (id > 0x1f4f && id < 0x1f54)));
//             if (match) { slots[n] = s; idxs[n] = cnt; ++n; }
//         }
//         for (int s = 0x99; s < 0xc9; ++s)
//         {
//             char buf[sizeof(Inven_Item)];
//             getCurCharacInvenR()->GetInvenSlot(*reinterpret_cast<Inven_Item*>(buf),
//                                                getCurCharacInvenSpace());
//             unsigned int id = *reinterpret_cast<unsigned int*>(buf + 4);
//             int cnt = *reinterpret_cast<int*>(buf + 12);
//             bool match =
//                 keepSkill != 0
//                   ? (id > 0x105b && id < 0x1066 && id >= 0x1079 && id <= 0x107d &&
//                      id >= 0x1f1c && id <= 0x1f1f && id >= 0x1f24 && id <= 0x1f27 &&
//                      id >= 0x1f2a && id <= 0x1f2b && id >= 0x1f54 && id <= 0x1f57)
//                   : (((id > 0xfc8 && id < 0xfd5) || (id > 0xfee && id < 0xff3) ||
//                       (id > 0xff6 && id < 0x1007) || (id > 0x1044 && id < 0x1049) ||
//                       (id > 0x1f1f && id < 0x1f24) || (id > 0x1f27 && id < 0x1f2a) ||
//                       (id > 0x1f4f && id < 0x1f54)));
//             if (match) { slots[n] = s; idxs[n] = cnt; ++n; }
//         }
//
//         if (n > 0)
//         {
//             InterfacePacketBuf::clear(&packet);
//             packet->put_header( 1, 0x12);
//             packet->put_byte( 1);
//             packet->put_byte( 0);
//             packet->put_byte( n);
//             for (int i = 0; i < n; ++i)
//             {
//                 getCurCharacInvenW()->delete_item(static_cast<INVEN_TYPE>(1),
//                                                   slots[i], idxs[i],
//                                                   static_cast<eItemDelReason>(10),
//                                                   true);
//                 packet->put_short( slots[i]);
//                 packet->put_int( idxs[i]);
//             }
//             packet->finalize( true);
//             Send(packet);
//         }
//
//         WongWork::CSkillChanger changer;
//         if (doChangeGrow == 1)
//         {
//             changer.CountElixer(this);
//             ((SkillSlot*)getCurCharacSkillW())->debugCheckGrowTypeSkill(get_charac_level(),
//                                                           get_charac_job(),
//                                                           subType, keepSkill);
//         }
//         else
//         {
//             ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
//             ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
//             addSkillOnCreateCharacter(((SkillSlot*)getCurCharacSkillW())->get_skillslot_buf(
//                                           static_cast<ENUM_SKILL_TREE_KIND>(0)),
//                                       get_charac_job());
//             addSkillOnCreateCharacter(((SkillSlot*)getCurCharacSkillW())->get_skillslot_buf(
//                                           static_cast<ENUM_SKILL_TREE_KIND>(1)),
//                                       get_charac_job());
//             changer.SkillInitialize(this, 0, true);
//         }
//
//         WongWork::CCalcCharacStat::InitializeStat(getCurCharacAddInfoW(),
//                                                   get_charac_job());
//         WongWork::CCalcCharacStat::CalcStat(getCurCharacAddInfoW(), get_charac_job(),
//                                             subType, keepSkill, get_charac_level());
//
//         int* needElixer = changer.GetNeedElixerCount();
//         int* curElixer  = changer.GetElixerCount();
//         int* ability    = changer.GetNeedElixerAbility();
//         for (int a = 1; a <= *ability; ++a)
//         {
//             if (curElixer[a] < needElixer[a])
//                 WongWork::CCalcCharacStat::ApplyElixerItem(getCurCharacAddInfoW(),
//                                                            ability[a],
//                                                            needElixer[a] - curElixer[a]);
//         }
//
//         set_grow_type((unsigned char)(subType & 0xff), (unsigned char)(keepSkill & 0xff), 0, (eChangeGrowTypeReason)1);
//         _postCheckForceChangeGrowType(subType, keepSkill);
//         SendNotiPacket(static_cast<eSendTarget>(0), 2, 0);
//         send_skill_info();
//         SendNotiPacket(static_cast<eSendTarget>(1), 2, 1);
//     }
//     return;
// }
//
// >>> W7-BLOCKED CUser::ValidateStrikerInfo —— _Charac_info striker 字段注记与 tagAccId@+0x14b8 冲突,pad 区未建模
// ---------------------------------------------------------------------------
// 43. CUser::ValidateStrikerInfo @0x08690be6
//     m_characList 元素内 +0x14b7(strikerSkillIndex uchar)/+0x14b8(linkedCharacNo uint)
// ---------------------------------------------------------------------------
// void CUser::ValidateStrikerInfo()
// {
//     StrikerScript* script = G_CDataManager()->m_strikerScript;
//
//     for (size_t i = 0; i < m_characList.size(); ++i)
//     {
//         if (m_characList[i].m_strikerLinkedCharacNo == 0)
//             continue;
//
//         int linkedIdx = get_charac_idx(m_characList[i].m_strikerLinkedCharacNo);
//         if (linkedIdx == -1)
//         {
//             m_characList[i].m_strikerLinkedCharacNo = 0;
//             m_characList[i].m_strikerSkillIndex = 0;
//             continue;
//         }
//
//         unsigned char skillIdx = m_characList[i].m_strikerSkillIndex;
//         char secondGrow = m_characList[linkedIdx].m_secondGrowType;   // +0x29 高低 4bit 取低
//         secondGrow = (char)((secondGrow << 4) >> 4);
//         char firstGrow = m_characList[linkedIdx].m_firstGrowType;     // +0x26
//
//         if (script->checkSkillIndex(firstGrow, secondGrow, skillIdx) != 1)
//         {
//             m_characList[i].m_strikerLinkedCharacNo = 0;
//             m_characList[i].m_strikerSkillIndex = 0;
//         }
//     }
// }

// >>> W7-BLOCKED CUser::VerifyCleanPadVeriData —— cleanpadVeri A/B/C 三值字段未建模
// ---------------------------------------------------------------------------
// 44. CUser::VerifyCleanPadVeriData @0x08649b44
// ---------------------------------------------------------------------------
// bool CUser::VerifyCleanPadVeriData(unsigned int value)
// {
//     return value == static_cast<unsigned int>(m_cleanpadVeriA) +
//                               static_cast<unsigned int>(m_cleanpadVeriB) +
//                               static_cast<unsigned int>(m_cleanpadVeriC);
// }

// ---------------------------------------------------------------------------
// 45. CUser::verifyDoingQuest @0x0868d144
// ---------------------------------------------------------------------------
void CUser::verifyDoingQuest()
{
    for (int i = 0; i <= 0x13; ++i)
    {
        UserQuest* qr = getCurCharacQuestR();
        int doingIdx = qr->m_quest[i];               // 类内 +0x7538
        if (doingIdx == 0)
            continue;
        if (qr->isClearQuest(doingIdx))
        {
            UserQuest* qw = getCurCharacQuestW();
            qw->m_quest[i]   = 0;
            qw->m_trigger[i] = 0;                    // 类内 +0x7588
        }
    }
}

// >>> W7-BLOCKED CUser::VerifyPresentAvengerTitle —— ARAD::Arad_EventPeriodDataManager 权威头(isApplied/checkCreateTime)未接入本 TU
// ---------------------------------------------------------------------------
// 46. CUser::VerifyPresentAvengerTitle @0x0868b552(报告全文)
//     与 RestoreGainExpProducers 批次同名实现可能重复——宿主择一保留。
// ---------------------------------------------------------------------------
// char CUser::VerifyPresentAvengerTitle()
// {
//     unsigned int accId = get_acc_id();
//     unsigned int createTime = getCurCharacCreateTime();
//     ARAD::Arad_EventPeriodDataManager* period =
//         ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
//             std::string("Arad_EventPeriodDataManager"));
//     if (period->checkCreateTime(static_cast<ENUM_REPEAT_EVENT_CODE>(0x8e),
//                                 createTime, accId) != 1)
//         return 0;
//
//     bool gate = false;
//     if (!(get_charac_level() < 0x14) && get_charac_job() == 4 &&
//         getCurCharFirstGrowType() == 0)
//         gate = true;
//     if (!gate)
//         return 0;
//
//     unsigned int nowSec = GlobalData::s_systemTime_.getCurSec();
//     bool applied = false;
//     if (getFlagPresentAvengerTitle() != 1)
//     {
//         ARAD::Arad_EventPeriodDataManager* p2 =
//             ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
//                 std::string("Arad_EventPeriodDataManager"));
//         applied = p2->isApplied(static_cast<ENUM_REPEAT_EVENT_CODE>(0x8e), nowSec) != 0;
//     }
//     if (!applied)
//         return 0;
//
//     Inven_Item item;
//     int wantIdx = 0x28d3af;                          // local_90 常量比较值(独立于产物)
//     G_CDataManager()->getItemList()->create_item(
//         static_cast<ENUM_ITEM_CREATE_TYPE>(0), item, 1);
//
//     if (wantIdx == 0 || wantIdx == -1)
//         return 0;
//
//     setFlagPresentAvengerTitle(true);
//
//     char title[0x15];
//     char mailText[0x100];
//     memset(title, 0, sizeof(title));
//     memset(mailText, 0, sizeof(mailText));
//     strncpy(title, g_scriptStringManager_.findString(4, "ready_to_avenger_title", 0), 0x14);
//     strncpy(mailText, g_scriptStringManager_.findString(4, "ready_to_avenger_mail", 0), 0xff);
//
//     WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
//         title, item, 0, getCurCharacNo(), mailText,
//         static_cast<int>(strlen(mailText)), 0, GetServerGroup(), false, false);
//
//     DB_SaveGiveAvengerTitleFlag::makeRequest(GetUID(), get_acc_id(),
//                                              getCurCharacNo());
//
//     cMyTrace trace(__PRETTY_FUNCTION__, 0xa6eb, 0);
//     trace("avenger plan event item post m_id(%s),charac_no(%u), itemidx(%u)",
//           NumberToString(get_acc_id(), 0), getCurCharacNo(),
//           static_cast<unsigned int>(*reinterpret_cast<int*>(&item)));
//     return 1;
// }

// ---------------------------------------------------------------------------
// 47. CUser::verifyStackableItemActionPlace @0x0868f69a(纯尾调)
// ---------------------------------------------------------------------------
int CUser::verifyStackableItemActionPlace(StackableAction::Type type, const CItem* item)
{
    return verifyStackableItemProcess(type, item);
}

// ============================================================================
// NEEDS_DECL 汇总(宿主/对应域批次落地后本文件可直接编译):
//  1. CUser.h 补本批新声明 + fields_F.tsv 字段(m_serverGroup 等)。
//  2. UserMercenaryInfoMgr::UpdateCharacName(unsigned int,const std::string&)
//     ORIG 0x081b462e。
//  3. LimitItemUsage 方法 SetCreateTime(long long)/SetCurTime()/isUsable()/useIt()
//     与字段 state/f4/f8/createTime64/countAt10/m_field15/m_field19 权威化
//     (ORIG 符号 ctor/isUsable/useIt/isAnotherDay 已核)。
//  4. CInventory::GetItemIdByAddInfo 自由函数形态
//     int GetItemIdByAddInfo(Inven_Item*, int, int)(ORIG 0x084e91a4)。
//  5. use_stackable 链依赖:expert_job::CAlchemist::{CheckUsableStackableItem,
//     OnUseStackableItem}、Global::CBossTowerFunc::UseItem、SendLV70Buff、
//     ARAD::ARAD_ITEM::arad_secret_shop_event_item(ORIG 均有符号)。
//  6. UseQuestItem 链依赖:WongWork::CSkillChanger::{CountElixer,
//     GetNeedElixerCount,GetElixerCount,GetNeedElixerAbility}、
//     WongWork::CCalcCharacStat::{InitializeStat,CalcStat,ApplyElixerItem}(static)、
//     CUser::{set_grow_type,_postCheckForceChangeGrowType,SendNotiPacket,
//     send_skill_info}(部分头已有)。
//  7. VerifyPresentAvengerTitle 链:g_scriptStringManager_(RDARScriptStringManager)、
//     WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(...bb)、
//     DB_SaveGiveAvengerTitleFlag::makeRequest(int,uint,uint)、
//     NumberToString(unsigned,int)、ARAD EventPeriod 链(isApplied/checkCreateTime
//     由 FixAradEventPeriodMethods/JoyousDolphin 批次提供)。
//  8. TimerQueue InsertTimer 形态按 TimerDispatcher 头最终签名;TIMER_MESSAGE/OBJ_TYPE cast。
// ============================================================================

// ============================================================================
// ================== W7-BigThree 专项（increase_status / init_skill_tree /====
// ================== InitOnAccept，ORIG-only 清零 Wave7）=====================
// 每函数逐份完整读取 docs/class_func_reports/CUser/<Func>.md（## 汇编 + ##
// 反编译 C）。被调 ORIG 符号经 nm -C 核实；具名字段取自 CUser.h/CDataManager.h
// 权威布局；init_skill_tree 相关依赖落点：addSkillOnCreateCharacter/growtype_skill
// /CSkillChanger::SkillInitialize → SkillSlot.cpp；get_give_skill → CCharacter.cpp。
// ============================================================================

#include "STExpertJobScript.h"   // STExpertJobScript::m_growSkillVec（+0x0c vector<pair<int,int>>）

// ----------------------------------------------------------------------------
// CUser::init_skill_tree —— ORIG T 0x0866bce8 _ZN5CUser15init_skill_treeE20ENUM_SKILL_TREE_KIND
// 报告 docs/class_func_reports/CUser/init_skill_tree.md 全文转写：
//   getCurCharacR==0 → return；
//   clear_all_skills(kind) → set_parent(this)；
//   kind==-1/0 → addSkillOnCreateCharacter(skillW+0x46, job)，否则 skillW+0x1de；
//   get_give_skill(first,second,giveVec,0) 成功→ growtype_skill×giveVec；
//     expert: GetExpertJobScript(GetCurCharacExpertJobType()) 非空→对 m_growSkillVec
//     每个 (skillIdx) 以 GetCurExpertJobLevel(Exp) 作 level 调 growtype_skill 两树；
//     CSkillChanger SkillInitialize(this, kind==-1/0?1:2, false)；
//   失败→ LogManager::logFormat(1,"user.cpp",PRETTY,0x5328,"User %s - ...")。
// ----------------------------------------------------------------------------
void CUser::init_skill_tree(ENUM_SKILL_TREE_KIND kind)
{
    if (getCurCharacR() == 0)
        return;

    ((SkillSlot*)getCurCharacSkillW())->clear_all_skills(kind);
    ((SkillSlot*)getCurCharacSkillW())->set_parent(this);

    // asm 0822f140=getCurCharacSkillW / 080fdf20=get_charac_job（nm 实证 thunk 序），
    // 反编译为两分支各 addSkillOnCreateCharacter((_Mastered_skill*)(skillW+{0x46|0x1de}), job)。
    if ((int)kind == -1 || (int)kind == 0)
    {
        addSkillOnCreateCharacter((_Mastered_skill*)(getCurCharacSkillW() + 0x46),
                                  get_charac_job());
    }
    else
    {
        addSkillOnCreateCharacter((_Mastered_skill*)(getCurCharacSkillW() + 0x1de),
                                  get_charac_job());
    }

    std::vector<std::pair<int,int> > giveVec;
    // ORIG 0x0866bce8：giveVec 后立即构造独立 pair 局部（C1 符号），
    // 循环内 giveVec[i] 逐字段拷入（mov/mov），非引用。
    std::pair<int,int> pr;
    char secondGrow = getCurCharSecondGrowType();
    char firstGrow = getCurCharFirstGrowType();
    int job = get_charac_job();
    // ORIG：*(int*)(mgr+0x14)+job*0x7dc —— mgr->m_characList（CCharacList*）->m_characs[job]
    bool got = G_CDataManager()->m_characList->m_characs[job].
                   get_give_skill((int)firstGrow, (int)secondGrow, giveVec, 0);
    if (!got)
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__, 0x5328,
            "User %s - CUser::init_skill_tree", getCurCharacName());
        return;
    }

    for (unsigned int i = 0; i < giveVec.size(); ++i)
    {
        // ORIG 循环体顺序：先 get_charac_job、再 getCurCharacSkillW，
        // slot->growtype_skill(job, pair.first, pair.second, kind)。
        pr = giveVec[i];
        int j = get_charac_job();
        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second, kind);
    }

    // Expert job 补技能树：GetExpertJobScript(0x0822b5f2 W _ZN12CDataManager18GetExpertJobScriptEi)
    // 返回 STExpertJobScript*（+0x0c = m_growSkillVec）。
    STExpertJobScript* expert =
        (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
    if (expert != 0)
    {
        for (unsigned int i = 0; i < expert->m_growSkillVec.size(); ++i)
        {
            int exp = GetCurCharacExpertJobExp();
            int lvl = GetCurExpertJobLevel(exp);
            int j = get_charac_job();
            std::pair<int,int>& pr = expert->m_growSkillVec[i];
            ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                               (ENUM_SKILL_TREE_KIND)1);
        }
    }

    WongWork::CSkillChanger changer;
    changer.SkillInitialize(this, ((int)kind == -1 || (int)kind == 0) ? 1 : 2, false);
}

#include "GlobalData.h"        // GlobalData::s_systemTime_ / s_pSecuProtectionField 等
#include "CEnvironment.h"      // G_CEnvironment / Server_Envir 字段
#include "Cipher.h"            // G_Cipher
#include "CMailBoxHelper.h"    // WongWork::CMailBoxHelper::AssignMailBox / CMailBox::Init
#include "GameMasterCmd.h"     // CGameMasterCharacter（InitOnAccept static local）
#include "CPremiumLetheManager.h" // CPremiumLetheManager 完整类型（increase_status lethe 链）

unsigned int get_seed_from_date();   // ORIG T 0x086b1d25 _Z18get_seed_from_datev
// cSecuUserAction —— ORIG 0x086978ba C1(空体)/0x0827b3f4 Init(+0 清零)。
// ORIG 侧仅 sizeof=4 的状态对象；InitOnAccept 0x086493c8 +0x4a1 处 new。
// 唯一声明点暂落本 TU（建议后续由 secu 域批次收敛到独立头）。
class cSecuUserAction
{
public:
    cSecuUserAction() {}                        // ORIG W 0x086978ba：空体
    void Init() { m_state = 0; }                // ORIG T 0x0827b3f4：+0x00 = 0
    // ORIG T 0x0827b402：return m_state == 1（cmp $0x1; setne 反相）。
    bool IsSecurityStateOff();

private:
    int m_state;   // +0x00（IsSecurityStateOff ==1 判定）
};

// ORIG T 0x0827b402（外联定义发射 T 符号）。
bool cSecuUserAction::IsSecurityStateOff()
{
    return m_state == 1;
}

// ----------------------------------------------------------------------------
// CUser::InitOnAccept —— ORIG T 0x086493c8 _ZN5CUser12InitOnAcceptEi
// 报告 docs/class_func_reports/CUser/InitOnAccept.md 全文转写：
//   socket 挂接/标志清零 → PacketVerifier::Init → mailBox 分配&Init →
//   密钥握手包(Cipher 原始钥) → seed/随机数/stun 列表包 → Send/
//   StartRecvEventWaiting/setStdDropRate(100) → secu 系列子对象初始化 →
//   GM 调试角色指针(static local gameMasterCharacter, guard 版式与 ORIG 一致)。
// ----------------------------------------------------------------------------
int CUser::InitOnAccept(int socketFd)
{
    m_network.set_socket(socketFd, false);
    m_chState = 1;
    m_winPointBuffer = 0;
    m_packetVerifier.Init();
    m_network.initNetworkBuffer();
    m_tradeCount = 0;

    // game option 尾部区逐字节清零（ORIG 0x70390..0x703b8；CGameOption +0x62c..+0x657）。
    m_gameOption.m_gmQuestFlag = 0;
    m_gameOption.m_forceDropFlag = 0;
    m_gameOption.m_playExpAdd = 0;                     // +0x703a4（movl 直接 dword 写）
    m_gameOption.m_breakAwayDungeonClear = 0;          // +0x703a8（movb 单字节写）
    m_gameOption.m_breakAwayDungeonClearCnt = 0;       // +0x703ac
    memset(m_gameOption.m_pad64c, 0, sizeof(m_gameOption.m_pad64c));  // +0x703b0
    m_gameOption.m_breakAwayQuestTime = 0;             // +0x703b4
    m_gameOption.m_breakAwayRewardOrder = 0;           // +0x703b8（int 宽写；CGameOption+0x654 具名化 by CU_D）

    m_multiboxLotteryFailCnt = 0;                      // +0x703bc
    m_breakAway.clear();                               // STBreakAwayRewards::clear W 0x0831d454
    m_breakAwayRewardData = 0;                         // +0x703c0（movb；setBreakAwayRewardData ORIG 0x0831d50c）
    m_usedBreakAwayLuckPoint = 0;                      // +0x703c1

    memset(m_skillCommands, 0, sizeof(m_skillCommands));  // ORIG memset(this+0x703db,0,200)
                                                          // （50×STCommand pack(1)=200B）

    m_premium.m_mailBox = (void*)WongWork::CMailBoxHelper::AssignMailBox();
    if (m_premium.m_mailBox != 0)
    {
        // asm 直接以 *(this+0x71b98) 为 this 调 WongWork::CMailBox::Init()
        ((WongWork::CMailBox*)m_premium.m_mailBox)->Init();
    }

    PacketGuard packet;                                // -0x20
    {
        packet.put_header(0, 1);                       // try 区起点 0864954b
        unsigned char origKey[1024];                   // -0x420（local_424，1024B）
        G_Cipher()->GetOriginalKey(origKey);
        int totalKeyLen = G_Cipher()->GetTotalKeyLength();
        packet.put_binary((char*)origKey, totalKeyLen);

        int nameLen = strlen(G_CEnvironment()->get_channel_name());
        packet.put_int(nameLen);
        packet.put_str(G_CEnvironment()->get_channel_name(), nameLen);

        m_clientSpec.m_seedFromDate = get_seed_from_date();    // +0x711e8（Client_Spec +0xc0c）
        packet.put_int(m_clientSpec.m_seedFromDate);

        m_checkSumSeed = get_rand_int(0);                      // +0x796e0
        packet.put_int(m_checkSumSeed);

        packet.put_byte(G_CEnvironment()->m_serverEnvir.m_serverGroup);      // env+0x378
        packet.put_byte(G_CEnvironment()->m_serverEnvir.m_channelNo);        // env+0x1b0
        packet.put_byte(G_CEnvironment()->m_serverEnvir.m_processSequence);  // env+0x1a8

        packet.put_int(GlobalData::s_systemTime_.getCurSec());

        int stunCnt = (int)G_CEnvironment()->m_serverEnvir.m_stunIP.size();  // env+0x360
        packet.put_int((int)G_CEnvironment()->m_serverEnvir.m_stunIP.size());
        if (stunCnt != 0)
        {
            for (unsigned int i = 0; i < G_CEnvironment()->m_serverEnvir.m_stunIP.size(); ++i)
            {
                char* ip = G_CEnvironment()->m_serverEnvir.m_stunIP[i].m_ip;
                packet.put_int((int)strlen(ip));
                packet.put_str(ip, 0x10);                      // 固定 16 字节宽
            }
            packet.put_int(G_CEnvironment()->m_serverEnvir.m_stunPort[0]);   // env+0x36c
            packet.put_int(G_CEnvironment()->m_serverEnvir.m_stunPort[1]);
        }
        packet.put_int(0x25e);
        packet.put_int(0x232);
        packet.finalize(true);
    }
    Send(packet);

    StartRecvEventWaiting();
    setStdDropRate(100);

    m_postChecksumData.m_tradePunishType = 0;          // +0x79628
    m_humanCertifyTimerKey = 0;                        // +0x8e0f8
    m_humanCertifyErrorCnt = -1;                       // +0x8e0fc
    m_humanCertified = false;                          // +0x8e100

    m_mcap.m_secuUserAction = new cSecuUserAction();
    m_mcap.m_secuUserAction->Init();

    m_pad8e101[0x2eb] = 0;                             // ORIG movb $0x0,0x8e3ec（pad 末字节）
    m_field8e3ed = true;                               // +0x8e3ed
    memset(m_pad8e101, 0, 0x145);                      // memset(this+0x8e101,0,0x145)
    memset(m_pad8e101 + 0x145, 0, 0x1a6);              // memset(this+0x8e246,0,0x1a6)
    m_mcap.reset();                                    // WongWork::CMCAPManager::reset W 0x08694fa0
    m_postChecksumData.m_field31 = false;              // +0x7962d

    getSecurityCard()->init();                         // thunk 0x0822fd20/WongWork::CSecurityCard::init 0x085fdd68
    getPad()->init();                                  // Sanicova::CPad::init 0x0859957c
    m_mcap.m_field1c = 0;                              // +0x8e40c（GetAutoClientRate 清零）

    // gm 调试角色（函数级 static，guard/data 名与 ORIG _ZGVZN5CUser12InitOnAcceptEiE…同版式）
    static CGameMasterCharacter gameMasterCharacter;
    m_postChecksumData.m_gameMasterCharac = &gameMasterCharacter;   // +0x7964c

    m_mcap.m_capchaReqTick = 0;                        // +0x8e94c
    m_mcap.m_field544 = false;                         // +0x8e934
    m_mcap.m_hackIpLogged = false;                     // +0x8e950
    m_mcap.m_field561 = false;                         // +0x8e951

    m_hades.m_secuReward = false;                      // +0x8e97c
    m_hades.m_secuType = 0;                            // +0x8e97d
    m_hades.Init(this);                                // XNuclear::CHades::Init T 0x084b91e8
    m_goldControl.Init(this);                          // Secu_GoldControl::Init T 0x08287230
    m_hackLogCheck.Init(this);                         // Secu_HackLogCheck::Init T 0x0827a7d0
    m_mcap.m_field546 = 0;                             // +0x8e936（short）
    m_mcap.m_lastLotteryTime = 0;                      // +0x8e938
    m_mcap.m_pad554[0] = 0;                            // +0x8e944
    m_piSender.m_nonClientFlag = 0;                    // +0x8eaf4
    m_piSender.m_nonClientRandInt = 0;                 // +0x8eaf8
    m_debugCommand = 0;                                // +0x8e090
    m_ceraUserInfo.m_updateOntime = false;             // +0x8d24c（UserInfo+0x68 bool）
    m_accHacking.init(this);                           // Secu_AccountHacking::init T 0x08278a98
    m_gameOption.m_forceDropFlag = 0;                  // +0x70391（第二处 movb $0x0）
    memset(&m_garenaAuthData, 0, sizeof(m_garenaAuthData));  // ORIG memset(this+0x6ef92,0,0x68)（InitOnAccept.md:589）

    return 1;                                          // asm mov $0x1,%ebx → %eax 返回 1
}

// ----------------------------------------------------------------------------
// CUser::increase_status
// ----------------------------------------------------------------------------
// CUser::increase_status —— ORIG T 0x086657fc _ZN5CUser15increase_statusEs
// 报告 docs/class_func_reports/CUser/increase_status.md 全文转写（汇编+C）：
//   GetInvenSlot 取槽物品 → itemIdx 判定表设定 (type,value) → 公共门槛段 →
//   find_item/GetUsablePvPRank/GetItemType/delete_item → switch(type) 动作 →
//   公共回包 + m_chState 5/8 队伍战房广播。
// ----------------------------------------------------------------------------
void CUser::increase_status(short slotNo)
{
    PacketGuard packet;

    if (CheckInTrade() != 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
        return;                                              // L086686d4
    }

    Inven_Item slotItem;
    slotItem = getCurCharacInvenR()->GetInvenSlot(1, slotNo);
    const unsigned int itemIdx = (unsigned int)slotItem.m_addInfo;

    char type = -1;                                          // -0xd1
    unsigned int value = 0;                                  // -0xd8

    // ---- itemIdx → (type,value) 判定表（ORIG 0866589d..086668f0 比较树序）----
    if (itemIdx == 0x4b5)
    {
        type = 0x11; value = 5;
    }
    else if (itemIdx < 0x4b6)
    {
        if (itemIdx == 0x40c)
        {
            type = 1; value = 10000;
        }
        else if (itemIdx < 0x40d)
        {
            if (itemIdx == 0xc9) goto L08665e38;
            if (itemIdx > 0xc9)
            {
                if (itemIdx != 0x3c3)
                {
                    if (itemIdx > 0x3c3)
                    {
                        if (itemIdx == 0x40a) { type = 1; value = 100; }
                        else if (itemIdx < 0x40b)
                        {
                            if (itemIdx != 0x407) goto L08666751;
                            type = 0; value = 5;
                        }
                        else { type = 1; value = 1000; }     // 0x40b..0x40c
                    }
                    else
                    {
                        if ((itemIdx != 0x394) && (itemIdx != 0x3c0)) goto L08666751;
                        goto L0866607b;
                    }
                    goto L08666990;
                }
                goto L0866607b;                              // 0x3c3
            }
            if (itemIdx == 0x2a) { type = 0x15; value = 1; goto L08666990; }  // L086665ed
            if (itemIdx < 0x2b)
            {
                if ((itemIdx == 3) || (itemIdx == 0x1c))
                {
L0866607b:
                    if (GlobalData::s_pSecuProtectionField->Check(
                            this, (SECURITY_PROTCTION)0x2c) != 0)
                    {
                        unsigned char errSecu =
                            (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                                this, (SECURITY_PROTCTION)0x2c);
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
                        return;                              // L086686d4
                    }
                    if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
                    {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
                        return;
                    }
                    if (CheckItemLock(1, (int)slotNo) != 0)
                    {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd5, packet);
                        return;
                    }
                    if (!WongWork::CSkillChanger::CheckCondition(this))
                    {
                        setCurCharacExp(G_CDataManager()->get_level_exp(get_charac_level()));
                    }
                    if ((itemIdx == 0x4b6) || (itemIdx == 0x4e5))
                    {
                        if (get_charac_level() < 0x32) return;    // asm 直跳 L086686d4
                        if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                        {
                            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd8, packet);
                            return;
                        }
                        value = 1; type = 0x12;
                    }
                    else if (itemIdx == 0x394) { value = 1; type = 0xa; }
                    else if (itemIdx == 0x3c0)
                    {
                        value = 1; type = 0x18;
                        CQuestShop* questShop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
                        if (questShop != 0)
                        {
                            questShop->clearQP(this);
                            questShop->sendCharacQp(this, '\0');
                        }
                    }
                    else if (itemIdx == 0x3c3) { value = 1; type = 0xa; }
                    else if (itemIdx == 0x28982c) { value = 1; type = 0xa; }
                    else { value = 0; type = 0xa; }
                    goto L08666990;
                }
            }
            else if ((itemIdx == 0x6d) ||
                     ((itemIdx > 0x6c) && ((itemIdx - 0xa1) < 2)))
            {
                goto L0866607b;
            }
            goto L08666751;
        }
        if (itemIdx == 0x413) { type = 2; value = 0xfa; }
        else if (itemIdx < 0x414)
        {
            if (itemIdx == 0x40f) { type = 4; value = 0x32; }
            else if (itemIdx < 0x410)
            {
                if (itemIdx == 0x40d) { type = 1; value = 100000; }
                else
                {
                    if (itemIdx != 0x40e) goto L08666751;
                    type = 0; value = 0x14;
                }
            }
            else if (itemIdx == 0x411) { type = 5; value = 0x32; }
            else if (itemIdx < 0x412) { type = 6; value = 0x32; }
            else { type = 7; value = 0x32; }
        }
        else if (itemIdx == 0x416) { type = 9; value = 10; }
        else
        {
            if (itemIdx > 0x416)
            {
                if (itemIdx > 0x4af)
                {
                    if (itemIdx < 0x4b3) goto L0866662f;     // 0x4b0..0x4b2
                    if (itemIdx == 0x4b4) { type = 0x11; value = 1; goto L08666990; }
                }
                goto L08666751;
            }
            if (itemIdx == 0x414) { type = 3; value = 0xfa; }
            else
            {
                if (itemIdx != 0x415) goto L08666751;
                type = 8; value = 10;
            }
        }
    }
    else if (itemIdx < 0x204d)
    {
        if (itemIdx < 0x204b)
        {
            if (itemIdx == 0x1bbd)
            {
                if (get_state() != 3)
                {
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
                    return;
                }
                type = 0xd; value = 8;
            }
            else if (itemIdx < 0x1bbe)
            {
                if (itemIdx == 0x4d0) { type = 1; value = 10000000; }
                else if (itemIdx < 0x4d1)
                {
                    if (itemIdx == 0x4b6) goto L0866607b;
                    if (itemIdx != 0x4cf) goto L08666751;
                    type = 1; value = 1000000;
                }
                else
                {
                    if (itemIdx == 0x4e5) goto L0866607b;
                    if (itemIdx == 0xc84)
                    {
                        if (get_charac_guildkey() == 0)
                        {
                            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x40, packet);
                            return;
                        }
                        type = 0xb; value = 10;
                    }
                    else
                    {
                        if (itemIdx != 0x4df) goto L08666751;
                        type = 0x13; value = 10;
                    }
                }
            }
            else if (itemIdx == 0x1c0d) { type = 0xd; value = 5; }
            else if (itemIdx < 0x1c0e)
            {
                if (itemIdx == 0x1bc1)
                {
                    if (get_charac_guildkey() == 0)
                    {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x40, packet);
                        return;
                    }
                    type = 0xb; value = 0x32;
                }
                else
                {
                    if (itemIdx != 0x1c0c) goto L08666751;
                    type = 0xd; value = 1;
                }
            }
            else if (itemIdx == 0x1f16) { type = 0x14; value = 0; }
            else
            {
                if (itemIdx == 0x1f71) goto L08665c97;
                if (itemIdx != 0x1c82) goto L08666751;
                if (get_state() != 3)
                {
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
                    return;
                }
                type = 0xd; value = 2;
            }
        }
        else
        {
L08665c97:
            bool reject = false;
            if (!CalLevelUpItemCheck(itemIdx))
            {
                reject = false;                              // asm: 不报错，走公共段
            }
            else
            {
                int valueTmp = (int)value;
                reject = (CalLevelUpItemState(type, valueTmp, 0x13, 0x3b) != 1);
                value = (unsigned int)valueTmp;
            }
            if (reject)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
                return;
            }
        }
    }
    else if (itemIdx == 0x28bdb8)
    {
L08665e38:
        CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0xf);
        if ((repeatEvent == 0) || (!repeatEvent->IsEventing(0)))
        {
            packet.clear();
            packet.put_header(1, 0x20);
            packet.put_byte(0);
            packet.put_byte(0x13);
            packet.finalize(true);
            Send(packet);
            return;
        }
        short totalFatigue = (short)getCurCharacTotalFatigue();
        if (totalFatigue == 0)
        {
            packet.clear();
            packet.put_header(1, 0x20);
            packet.put_byte(0);
            packet.put_byte(0x43);
            packet.finalize(true);
            Send(packet);
            return;
        }
        type = 0x0e; value = 0;
    }
    else if (itemIdx < 0x28bdb9)
    {
        if (itemIdx == 0x28974b)
        {
L08666320:
            if (G_GameWorld()->IsPvPSkilTreeChannel() != 1)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
                return;
            }
            if (GlobalData::s_pSecuProtectionField->Check(
                    this, (SECURITY_PROTCTION)0x2c) != 0)
            {
                unsigned char errSecu =
                    (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                        this, (SECURITY_PROTCTION)0x2c);
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
                return;
            }
            if (!WongWork::CSkillChanger::CheckCondition(this))
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 1, packet);
                return;
            }
            value = 0; type = 0x16;
            if (itemIdx == 0x289788) value = 1;
        }
        else
        {
            if (itemIdx < 0x28974c)
            {
                if ((itemIdx - 0x204e) < 2) goto L08665c97;
                goto L08666751;
            }
            else
            {
                if (itemIdx == 0x289788) goto L08666320;
                if (itemIdx == 0x28982c) goto L0866607b;
                if (itemIdx != 0x28974c) goto L08666751;
                if (G_GameWorld()->IsPvPSkilTreeChannel() != 1)
                {
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
                    return;
                }
                if (GlobalData::s_pSecuProtectionField->Check(
                        this, (SECURITY_PROTCTION)0x2c) != 0)
                {
                    unsigned char errSecu =
                        (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                            this, (SECURITY_PROTCTION)0x2c);
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
                    return;
                }
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd8, packet);
                    return;
                }
                value = 0; type = 0x17;
                goto L08666990;
            }
        }
        
    }
    else if (itemIdx == 0x989a13)
    {
L08665d24:
        int valueTmp = (int)value;
        bool ok54 = (CalLevelUpItemState(type, valueTmp, 1, 0x54) == 1);
        value = (unsigned int)valueTmp;
        if (!ok54)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
        goto L08666990;
    }
    else
    {
        if (itemIdx < 0x989a14)
        {
            if (itemIdx == 0x28d14d) { type = 0x15; value = 1; goto L08666990; }
            if (itemIdx == 0x28d2bc) goto L08665c97;
            goto L08666751;
        }
        if (itemIdx == 0x2920915c)
        {
L0866662f:
            if (!IsGuildMaster())
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xdb, packet);
                return;
            }
            STGuildDBInfoOnly* guildDB = (STGuildDBInfoOnly*)GetGuildDBInfo();
            int needFund =
                G_CDataManager()->m_guildParameter.getGuildLevelUpParam(guildDB->m_fundLevel + 1);
            if (needFund == 0)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd2, packet);
                return;
            }
            if ((unsigned int)needFund <= guildDB->m_fundExp)      // +0x29 无符号比较
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd2, packet);
                return;
            }
            type = 0x10;
            value = G_CDataManager()->m_guildParameter.getGuildExpBook(guildDB->m_fundLevel + 1);
            goto L08666990;
        }
        if (itemIdx != 0x292091ad)
        {
            if (itemIdx == 0x292090e1) goto L08665d24;
            goto L08666751;
        }
        type = 0x19; value = 1;
        goto L08666990;
    }

L08666751:
    // 默认路径：查 CDataManager::m_field50cc 命中或 GetIncreaseStatus 描述
    {
        CDataManager* mgr = G_CDataManager();
        if (itemIdx == (unsigned int)mgr->m_growthCapsule.m_capsuleId)
        {
            CItem* itemA = mgr->find_item(mgr->m_growthCapsule.m_capsuleId);
            if (itemA == 0)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (!itemA->is_stackable())
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (get_charac_level() < itemA->getUsableLevel())
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (get_charac_level() > 0x47)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            type = 1; value = 0;
            int vtmp = 0;
            if (!itemA->GetIncreaseStatusIntData(0, vtmp))
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            value = (unsigned int)vtmp;
        }
        else
        {
            CItem* itemB = mgr->find_item(itemIdx);
            if ((itemB != 0) && (itemB->GetIncreaseStatusType() != 0) &&
                (itemB->GetIncreaseStatusType() == 1))
            {
                int vtmp = 0;
                itemB->GetIncreaseStatusIntData(0, vtmp);
                value = (unsigned int)vtmp;
                type = 1;
            }
        }
    }

L08666990:
    if (type == -1)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
        return;                                              // L086686d4
    }
    if (type == 0x0f)
    {
        if (get_state() != 3)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
        if ((int)(getCurCharacFatigue() & 0xffff) < (int)value)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x5f, packet);
            return;
        }
    }
    if (((int)type == 0) || ((int)type == 0x11) || ((int)type == 1))
    {
        if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
    }

    CItem* actionItem = G_CDataManager()->find_item(itemIdx);
    if (actionItem == 0) return;                             // L086686d4
    if (actionItem->GetUsablePvPRank() > get_pvp_grade())
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 1, packet);
        return;
    }
    if (actionItem->GetItemType() != 0x15)
    {
        CInventory* invenW = getCurCharacInvenW();
        if (!invenW->delete_item((INVEN_TYPE)1, slotNo, 1, (eItemDelReason)10, true))
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
            return;
        }
    }

    switch ((unsigned char)type)
    {
    case 0:                                                  // SP（asm +0x1435）
    {
        gain_sp(value);
        history_log_sp(-1, value, (eSPAddReason)1);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(0);
        packet.put_short(0);
        packet.finalize(true);
        Send(packet);
        return;                                              // asm jmp L086686d4
    }
    case 1:                                                  // EXP 类（asm +0x167f）
    {
        int bonusA = 0;                                      // -0xdc
        int bonusB = 0;                                      // -0xe0
        int spBefore = 0;                                    // -0xe4
        int spAfter = 0;                                     // -0xe8
        if (get_charac_level() > 0x54) value = 0;
        if (itemIdx == (unsigned int)G_CDataManager()->m_growthCapsule.m_capsuleId)
            rewardExp(value, bonusA, bonusB, spBefore, spAfter, (::eExpAddReason)3, true);
        else
            rewardExp(value, bonusA, bonusB, spBefore, spAfter, (::eExpAddReason)0, true);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(spAfter - spBefore);
        packet.put_short(bonusB);
        packet.finalize(true);
        Send(packet);
        return;
    }
    case 2:                                                  // AddInfoRef +0x00（int）
    {
        *(int*)((char*)getCurCharacAddInfoRefW()) += value;
        break;
    }
    case 3:                                                  // +0x04（int）
    {
        *(int*)((char*)getCurCharacAddInfoRefW() + 4) += value;
        break;
    }
    case 4:                                                  // +0x08（short）
    {
        *(short*)((char*)getCurCharacAddInfoRefW() + 8) += (short)value;
        break;
    }
    case 5:                                                  // +0x0a
    {
        *(short*)((char*)getCurCharacAddInfoRefW() + 10) += (short)value;
        break;
    }
    case 6:                                                  // +0x0c
    {
        *(short*)((char*)getCurCharacAddInfoRefW() + 12) += (short)value;
        break;
    }
    case 7:                                                  // +0x0e
    {
        *(short*)((char*)getCurCharacAddInfoRefW() + 14) += (short)value;
        break;
    }
    case 8:                                                  // +0x42（int）
    {
        *(int*)((char*)getCurCharacAddInfoRefW() + 0x42) += value;
        break;
    }
    case 9:                                                  // 四属性循环 (+8)*2
    {
        for (int i = 0; i < 4; ++i)
        {
            *(short*)((char*)getCurCharacAddInfoRefW() + (i + 8) * 2) += (short)value;
        }
        break;
    }
    case 0xa:
    case 0x18:                                               // 转职/成长洗点重建技能树
    {
        bool ok = false;
        std::vector<std::pair<int,int> > giveVec;
        if (value == 1)
        {
            SkillSlot* skillSlot = (SkillSlot*)getCurCharacSkillW();
            skillSlot->clear_all_skills_both();
            skillSlot = (SkillSlot*)getCurCharacSkillW();
            skillSlot->set_parent(this);
            addSkillOnCreateCharacter((_Mastered_skill*)(getCurCharacSkillW() + 0x46),
                                      get_charac_job());
            addSkillOnCreateCharacter((_Mastered_skill*)(getCurCharacSkillW() + 0x1de),
                                      get_charac_job());
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int job2 = get_charac_job();
            bool got = G_CDataManager()->m_characList->m_characs[job2].
                           get_give_skill((int)firstGrow, (int)secondGrow, giveVec, 0);
            if (got)
            {
                for (unsigned int i = 0; i < giveVec.size(); ++i)
                {
                    int j = get_charac_job();
                    std::pair<int,int>& pr = giveVec[i];
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second,
                                                                       (ENUM_SKILL_TREE_KIND)0);
                    j = get_charac_job();
                    pr = giveVec[i];
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second,
                                                                       (ENUM_SKILL_TREE_KIND)1);
                }
                STExpertJobScript* expert = (STExpertJobScript*)
                    G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
                if (expert != 0)
                {
                    for (unsigned int i = 0; i < expert->m_growSkillVec.size(); ++i)
                    {
                        int lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        int j = get_charac_job();
                        std::pair<int,int>& pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)0);
                        lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        j = get_charac_job();
                        pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                WongWork::CSkillChanger changer;
                changer.SkillInitialize(this, 0, false);
                send_skill_info();
                CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    if (lethe != 0)
                    {
                        lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)0);
                        lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)0);
                    lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)1);
                }
                ok = true;
            }
            else
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x4aee,
                    "User %s - CUser::set_grow_type %d", getCurCharacName(), (int)type);
                ok = false;
            }
        }
        else
        {
            ENUM_SKILL_TREE_KIND treeKind =
                (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills(treeKind);
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            bool firstTree = ((GetCurCharacSkillTreeIndex() == -1) ||
                              (GetCurCharacSkillTreeIndex() == 0));
            if (firstTree)
            {
                addSkillOnCreateCharacter((_Mastered_skill*)(getCurCharacSkillW() + 0x46),
                                          get_charac_job());
            }
            else
            {
                addSkillOnCreateCharacter((_Mastered_skill*)(getCurCharacSkillW() + 0x1de),
                                          get_charac_job());
            }
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int job2 = get_charac_job();
            bool got = G_CDataManager()->m_characList->m_characs[job2].
                           get_give_skill((int)firstGrow, (int)secondGrow, giveVec, 0);
            if (got)
            {
                for (unsigned int i = 0; i < giveVec.size(); ++i)
                {
                    ENUM_SKILL_TREE_KIND kindI =
                        (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
                    int j = get_charac_job();
                    std::pair<int,int>& pr = giveVec[i];
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second, kindI);
                }
                STExpertJobScript* expert = (STExpertJobScript*)
                    G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
                if (expert != 0)
                {
                    for (unsigned int i = 0; i < expert->m_growSkillVec.size(); ++i)
                    {
                        int lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        int j = get_charac_job();
                        std::pair<int,int>& pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)0);
                        lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        j = get_charac_job();
                        pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                WongWork::CSkillChanger changer;
                int initKind = ((GetCurCharacSkillTreeIndex() == -1) ||
                                (GetCurCharacSkillTreeIndex() == 0)) ? 1 : 2;
                changer.SkillInitialize(this, initKind, false);
                send_skill_info();
                CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    if (lethe != 0)
                    {
                        lethe->UpdateBackupSkillFlag(
                            this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                    }
                }
                else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    lethe->InitLetheSkill(
                        this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                }
                ok = true;
            }
            else
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x4b48,
                    "User %s - CUser::set_grow_type %d", getCurCharacName(), (int)type);
                ok = false;
            }
        }
        if (!ok) return;                                     // 失败：仅析构 packet 返回
        break;
    }
    case 0xb:                                                // 公会经验（+0xc92）
    {
        if (get_charac_guildkey() != 0) add_guild_exp(value);
        break;
    }
    case 0xc:                                                // stamina（+0xcb3）
    {
        setCurCharacStamina((unsigned char)value);
        break;
    }
    case 0xd:                                                // event coin（+0xcd9）
    {
        int coinNow = getCurCharacInvenR()->GetEventCoin();
        getCurCharacInvenW()->SetEventCoin(coinNow + value);
        m_historyLog.EventCoinAdd(getCurCharacInvenR()->GetEventCoin(), value,
                                  (eCoinAddReason)2);
        SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 1);
        break;
    }
    case 0x0e:                                               // fatigue 回复（+0xd43）
    {
        RecoverFatigue(value);
        SendFatigue();
        break;
    }
    case 0x0f:                                               // fatigue 扣减（+0xd55）
    {
        int remain = (int)(getCurCharacFatigue() & 0xffff) - (int)value;
        if (remain < 0) remain = 0;                          // ORIG max(int) 内联形态
        setCurCharacFatigue(remain);
        SendFatigue();
        break;
    }
    case 0x10:                                               // 公会经验上报（+0xdd8）
    {
        if (get_charac_guildkey() != 0)
        {
            unsigned int characNo = get_charac_no(-1);
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
            proxy->SendIncreaseGuildExp((unsigned int)get_charac_guildkey(), characNo,
                                        value, 1);
        }
        break;
    }
    case 0x11:                                               // SFP（+0xe64）
    {
        gain_sfp(value);
        history_log_sfp(-1, value, (eSPAddReason)1);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(0);
        packet.put_short(0);
        packet.finalize(true);
        Send(packet);
        return;
    }
    case 0x12:                                               // SFP 技能树重建（+0xf12 区）
    {
        int initKind = ((GetCurCharacSkillTreeIndex() == -1) ||
                        (GetCurCharacSkillTreeIndex() == 0)) ? 3 : 4;
        ((SkillSlot*)getCurCharacSkillW())->clear_sfp_skills(
            (char)get_charac_job(),
            (ENUM_SKILL_TREE_KIND)((GetCurCharacSkillTreeIndex() == -1) ? 0 : 0));
        WongWork::CSkillChanger changer;
        changer.SkillInitialize(this, initKind, false);
        send_skill_info();
        break;
    }
    case 0x13:                                               // power war point（+0x1180）
    {
        IncreasePowerWarPoint((short)value);
        break;
    }
    case 0x14:                                               // GM 状态校正（+0x119c）
    {
        adjust_charac_stat();
        if (m_chState == 3)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            Send(packet);
        }
        break;
    }
    case 0x15:                                               // coin（+0x125b）
    {
        int coinNow = getCurCharacInvenR()->GetCoin();
        getCurCharacInvenW()->SetCoin(coinNow + value);
        m_historyLog.CoinAdd(getCurCharacInvenR()->GetCoin(), value, (eCoinAddReason)3);
        SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 1);
        break;
    }
    case 0x16:                                               // PvP 技能树（+0x1303）
    {
        if (value == 1)
        {
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            givePvPSkillTree(0, true, 3);
            int pvpGrade = get_pvp_grade();
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int point = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                get_charac_job(), (int)firstGrow, (int)secondGrow, pvpGrade, false);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(point,
                                                                       (ENUM_SKILL_TREE_KIND)0);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(point,
                                                                       (ENUM_SKILL_TREE_KIND)1);
            send_skill_info();
            CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
            if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                if (lethe != 0)
                {
                    lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)0);
                    lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)1);
                }
            }
            else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)0);
                lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)1);
            }
        }
        else
        {
            ENUM_SKILL_TREE_KIND treeKind =
                (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills(treeKind);
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            int initKind = ((GetCurCharacSkillTreeIndex() == -1) ||
                            (GetCurCharacSkillTreeIndex() == 0)) ? 1 : 2;
            givePvPSkillTree(0, true, initKind);
            int pvpGrade = get_pvp_grade();
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int point = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                get_charac_job(), (int)firstGrow, (int)secondGrow, pvpGrade, false);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
                point, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
            send_skill_info();
            CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
            if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                if (lethe != 0)
                {
                    lethe->UpdateBackupSkillFlag(
                        this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                }
            }
            else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                lethe->InitLetheSkill(
                    this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
            }
        }
        break;
    }
    case 0x17:                                               // one day lethe（+0x1800）
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x4c0a, 0);
        trace("ONE_DAY_LETHE : BUY_SKILL_CONFIRM_TICKET, char(%s), char_no(%d), style(%d)",
              getCurCharacName(), getCurCharacNo(), GetCurCharacSkillTreeIndex());
        CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
        if (lethe != 0) lethe->ConfirmSkillReq(this);
        break;
    }
    default:
        break;
    case 0x19:                                               // avatar coin（+0x13e1）
    {
        AvatarCoin* avatarCoin =
            (AvatarCoin*)((char*)getCurCharacInvenW() + 0x658);   // CInventory::m_avatarCoin
        avatarCoin->Add(value);
        AvatarCoin::SaveToDB(this);
        AvatarCoin::SendSyncPacket(this);
        AvatarCoin::HistoryLog::AddLog(this, value);
        break;
    }
    }

L_common_tail_6751_end_return:
    // ---- 公共回包与队伍/战房广播（ORIG +0x2c39 尾段）----
    packet.clear();
    packet.put_header(1, 0x20);
    packet.put_byte(1);
    packet.put_short(slotNo);
    packet.put_byte(type);
    packet.put_int(value);
    packet.put_short(0);
    packet.put_short(0);
    packet.finalize(true);
    Send(packet);

    if (m_chState == 5)
    {
        CParty* party = (CParty*)GetParty();
        if (party != 0)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            party->send_to_party(packet);
        }
    }
    else if (m_chState == 8)
    {
        WarRoom* warRoom = (WarRoom*)GetWarRoom();
        if (warRoom != 0)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            warRoom->SendToRoom(packet);
        }
    }
}

// W9 batch A: report-backed short accessors and container helpers.
void CUser::complete_reward_list(std::vector<std::pair<int, int> >& toDoList,
                                 std::vector<std::pair<int, int> >& doneList)
{
    for (size_t i = 0; i < toDoList.size(); ++i)
        for (size_t j = 0; j < doneList.size(); ++j)
            if (toDoList[i].first == doneList[j].first)
                toDoList[i].second -= doneList[j].second;
}

bool CUser::IsWaitingStateCharac(unsigned int mercSlot)
{
    return m_mercenaryMgr.IsWaitingStateCharac(mercSlot);
}

void* CUser::GetUserSecretShop()
{
    CParty* party = static_cast<CParty*>(GetParty());
    if (party == 0)
        return 0;
    return party->GetUserSecretShop(this);
}

int CUser::getQuestPoint()
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData(
        static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xb));
    if (shop == 0)
        return 0;
    return shop->getQp();
}

bool CUser::IsState(ch_state state) const
{
    return m_chState == state;
}

unsigned char CUser::GetChangedGiftFatigueQuantity() const
{
    return m_field8ebb0;
}

unsigned char CUser::GetReceivedRequestType() const
{
    return m_receivedRequestType;
}

unsigned char CUser::getPcRoomAuth() const
{
    return m_pcRoomAuth;
}

bool CUser::IsPCRoomUser() const
{
    return false;
}

unsigned short CUser::GetSlotIDX() const
{
    return static_cast<unsigned short>(m_slotIDX);
}

void CUser::SetAge(unsigned int age)
{
    m_clientSpec.m_age = static_cast<int>(age);
}

void CUser::SetCurCharacUsedGiftFatigueQuantity(unsigned int quantity)
{
    if (m_field8ebac != static_cast<int>(quantity))
    {
        m_field8ebac = static_cast<int>(quantity);
        SetChangedGiftFatigueQuantity(true);
    }
}

int CUser::getAddQuestItemDropRateFromPremium(ENUM_PREMIUM_TYPE type)
{
    return m_premium.GetAddQuestItemDropRate(static_cast<int>(type));
}

void CUser::setCharacLinkBonusSetting(int slot, bool flag)
{
    if (slot >= 0 && slot < static_cast<int>(m_characList.size()))
    {
        m_characList[slot].m_linkBonusSetting = flag;
        m_characterView.enableSaveCharacView();
    }
}

bool CUser::isCharacLinkBonusSetting(char slot)
{
    if (slot < 0 || slot >= static_cast<char>(m_characList.size()))
        return false;
    return m_characList[static_cast<int>(slot)].m_linkBonusSetting;
}

void CUser::setBreakAwayRewardData(bool flag)
{
    m_breakAwayRewardData = flag ? 1 : 0;
}

void CUser::setAdvanceAltarIndex(short index)
{
    m_advanceAltarIdx = index;
}

int CUser::getLinkCharacNo(char index)
{
    if (index < 0 || index >= static_cast<char>(m_characList.size()))
        return 0;
    return m_characList[static_cast<int>(index)].m_characNo;
}

// ---------------------------------------------------------------------------
// Bingo 域 TU-local 视图（BingoEventLink4.cpp 持同名 local 类与全局符号定义，
// 两 TU 各自声明不冲突；仅消费 getBingoSystem/calBoradMatchLine 全局 T 符号）。
// CBingo::initUserData 为 ORIG 真实符号 _ZN6CBingo12initUserDataER9BingoData
// @0x080cadd4（报告全文），当前仓库无 producer，由本 TU 补唯一真实定义。
// ---------------------------------------------------------------------------
class CBingo
{
public:
    int calBoradMatchLine(int board);        // ORIG T 0x080cab5a（定义 BingoEventLink4.cpp）

    int initUserData(BingoData& data);       // ORIG T 0x080cadd4（本 TU 定义；ORIG 返回 1）
};

class BingoEvent   // LinkClosureD：原 BingoEventLocalView（_ZN19... mangled 无 ORIG 对应）
{                  // 改名 BingoEvent 使 getBingoSystem mangled 落 ORIG _ZN10BingoEvent14getBingoSystemEv
public:
    void* getBingoSystem();                  // ORIG T 0x080c9fea（定义 BingoEventLink4.cpp）
};

int CBingo::initUserData(BingoData& data)
{
    // initUserData.md：calBoradMatchLine(*((int*)&data)) 结果写 data+0x04（char 截断），
    // 返回 1。
    *reinterpret_cast<char*>(reinterpret_cast<char*>(&data) + 0x04) =
        static_cast<char>(calBoradMatchLine(
            *reinterpret_cast<int*>(reinterpret_cast<void*>(&data))));
    return 1;
}

// ============================================================================
// CU_Fix-2 批次 —— TranslateLoginInfo 链接失败中的 CUser 域 16 符号
// （docs/class_func_reports/CUser/<Func>.md 全文 ## 汇编+## 反编译 C 已逐份读取）
// ============================================================================

void CUser::SetBingoData(SIG_LOGIN_DATA& sig)
{
    // SetBingoData.md @0x0869126e：clear(BingoData@+0x8ebdc)、
    // m_eventId←sig.m_bingoEventId(+0x413c)、m_isWin←sig.m_bingoWinFlag(+0x4138)!=0。
    // 注：BingoData::clear 当前 owner（CUser.cpp）为空桩占位，语义修复归其域批次。
    m_bingo.clear();
    m_bingo.m_eventId = sig.m_bingoEventId;
    m_bingo.m_isWin = (sig.m_bingoWinFlag != 0);
}

void CUser::SetBirth(const char* birth)
{
    // SetBirth.md @0x0867ecf0：strncpy 局部 2 字节缓冲后 atoi；
    // 低字节写 m_clientSpec.m_birthMonth(+0x8ebe→全址0x711d5)、
    // m_clientSpec.m_birthDay(全址 0x711d4)。
    char digits[3];
    digits[0] = '\0';
    memcpy(digits, birth + 2, 2);
    digits[2] = '\0';
    m_clientSpec.m_birthMonth = static_cast<char>(atoi(digits));
    memcpy(digits, birth + 4, 2);
    digits[2] = '\0';
    m_clientSpec.m_birthDay = static_cast<char>(atoi(digits));
}

void CUser::SetCharacMercenaryInfo(int count, CHARAC_LOAD_MERCENARY* infos)
{
    // SetCharacMercenaryInfo.md @0x0868e544：i<count && i<0x24 循环，
    // 逐元素 UserMercenaryInfoMgr::AddCharac(infos[i])（stride 0x54）。
    for (int i = 0; i < count && i < 0x24; ++i)
    {
        m_mercenaryMgr.AddCharac(infos[i]);
    }
}

void CUser::setCharacSlotCountInfo(unsigned char effectCnt, unsigned char limit)
{
    // setCharacSlotCountInfo.md @0x0868b480：零参回退默认值分支。
    if (effectCnt == 0 || limit == 0)
    {
        const int def = getCharacSlotCountDefaultInfo();
        setSlotEffectCount(static_cast<unsigned char>(def));
        setCharacSlotLimit(static_cast<unsigned char>(def));
    }
    else
    {
        setSlotEffectCount(effectCnt);
        setCharacSlotLimit(limit);
    }
}

int CUser::getCharacSlotCountDefaultInfo()
{
    // getCharacSlotCountDefaultInfo.md @0x0868b4fc：
    //   G_CEnvironment()+0x1a8 ==1→1 / ==2→3；
    //   否则 G_CEnvironment()+0x378 ==5||==0xb→0x18，其余→0x11。
    // 【推断字段注记】CEnvironment 两处环境槽位未在权威头建模（0x1a8/0x378），
    // 按 §4.3 例外保留地址形式访问。
    char* env = reinterpret_cast<char*>(G_CEnvironment());
    int v = *reinterpret_cast<int*>(env + 0x1a8);
    if (v == 1)
        return 1;
    if (v == 2)
        return 3;
    v = *reinterpret_cast<int*>(env + 0x378);
    if (v == 5 || v == 0xb)
        return 0x18;
    return 0x11;
}

void CUser::SetEventCreateDnfReward(bool flag)
{
    // SetEventCreateDnfReward.md @0x08693198：单字节成员写。
    m_eventCreateDnfReward = flag;
}

void CUser::SetGrowthCreatureEventInfo(const SIG_GIVE_GROWTH_CREATURE_EVENT& sig)
{
    // SetGrowthCreatureEventInfo.md @0x08691cde：stride 0x2c×11 循环，
    // serverGroup/characNo 任一为 0 停止；构造 eventCharacInfo 入 m_growthCreature.m_vec(+0x8ebf8)。
    for (int i = 0; i < 11; ++i)
    {
        const SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo& src =
            sig.m_entries[i];
        if (src.m_serverGroup == 0 || src.m_characNo == 0)
            break;
        GrowthCreatureEvent::eventCharacInfo info;
        info.m_serverGroup = src.m_serverGroup;
        info.m_characNo = src.m_characNo;
        info.m_job = src.m_job;
        memcpy(info.m_name, src.m_name, 0x1e);
        m_growthCreature.m_vec.push_back(info);
    }
}

void CUser::SetGrowthWeaponEventInfo(const SIG_GROWTH_WEAPON_EVENT& sig)
{
    // SetGrowthWeaponEventInfo.md @0x8691bae：sig(+0/+4/+8/+0xc/+0x10/+0x12)
    // 六字段搬运至 GrowthWeaponEvent(+0x8ebe4)；repeat 事件 100(0x64) 进行中且
    // m_eventFlag != 1 时发送 noti 0x1ba（ORIG 对事件指针无空检直接虚调）。
    m_growthWeapon.m_eventFlag = sig.m_data[0];
    unsigned int v32;
    unsigned short v16;
    memcpy(&v32, sig.m_data + 4, 4);
    m_growthWeapon.m_field4 = v32;
    memcpy(&v32, sig.m_data + 8, 4);
    m_growthWeapon.m_field8 = v32;
    memcpy(&v32, sig.m_data + 12, 4);
    m_growthWeapon.m_fieldc = v32;
    memcpy(&v16, sig.m_data + 16, 2);
    m_growthWeapon.m_field10 = v16;
    memcpy(&v16, sig.m_data + 18, 2);
    m_growthWeapon.m_field12 = v16;

    if (GlobalData::s_event_manager->GetRepeatEvent(100)->IsEventing(
            static_cast<CUser*>(0))
        && m_growthWeapon.m_eventFlag != 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0x1ba);
        packet.finalize(true);
        Send(packet);
    }
}

void CUser::ValidateStrikerInfo()
{
    // ValidateStrikerInfo.md @0x08690be6：遍历 m_characList(+0x796e8)，
    // striker 字段对 _Charac_info 元素内 m_field14b7(u8 skillIndex)/m_field14b8(u32
    // linkedCharacNo)【推断注记：报告语义 strikerSkillIndex/strikerLinkedCharacNo】：
    // linked 找不到或 StrikerScript::checkSkillIndex 校验失败即双清零。
    for (size_t i = 0; i < m_characList.size(); ++i)
    {
        if (m_characList[i].m_field14b8 == 0)
            continue;
        const int linkedIdx =
            get_charac_idx(static_cast<unsigned int>(m_characList[i].m_field14b8));
        if (linkedIdx == -1)
        {
            m_characList[i].m_field14b8 = 0;
            m_characList[i].m_field14b7 = 0;
            continue;
        }
        const unsigned char skillIdx =
            static_cast<unsigned char>(m_characList[i].m_field14b7);
        const _Charac_info& linked = m_characList[linkedIdx];
        const char secondGrow =
            static_cast<char>((linked.m_field29 << 4) >> 4);   // +0x29 低 4bit
        const char firstGrow = linked.m_field26;               // +0x26
        StrikerScript* script = G_CDataManager()->m_altarConfig.m_strikerScript;
                                                           // CDataManager+0x6390
        if (!script->checkSkillIndex(firstGrow, secondGrow,
                                     static_cast<int>(skillIdx)))
        {
            m_characList[i].m_field14b8 = 0;
            m_characList[i].m_field14b7 = 0;
        }
    }
}

void CUser::sendReturnUserFirstLogin()
{
    // sendReturnUserFirstLogin.md @0x0868a9a4：IsReturnUser && IsReturnUserFirstLogin
    // 双门槛后发 noti 0x1bc 空载荷包。
    if (!IsReturnUser() || !IsReturnUserFirstLogin())
        return;
    PacketGuard packet;
    packet.put_header(0, 0x1bc);
    packet.finalize(true);
    Send(packet);
}

void CUser::set_acc_info(unsigned int accId, const char* accName)
{
    // set_acc_info.md @0x8649bfc：m_accId(+0x704ac)=accId；
    // strncpy(m_accName(+0x704b0), name, 0x14)；min(strlen,0x14) 处补 '\0'。
    m_accId = accId;
    strncpy(m_accName, accName, 0x14);
    size_t len = strlen(accName);
    if (len > 0x14)
        len = 0x14;
    m_accName[len] = '\0';
}

void CUser::set_fatigue_count_db(int type, int value)
{
    // set_fatigue_count_db.md @0x868d4a2：int 实参低字节分别写
    // m_serverFatigueDbType(+0x8eb4d ← type) 与 m_serverFatigueValue(+0x8eb4c ← value)。
    m_serverFatigueDbType = static_cast<char>(type);
    m_serverFatigueValue = static_cast<char>(value);
}

void CUser::set_ingame_event_history(int eventType, int v1, int v2)
{
    // set_ingame_event_history(iii).md @0x868d580：map<int,pair<int,int>> m_map8eb68
    // find/insert-or-update；随后 update_ingame_event_history(eventType) 置脏标记。
    std::map<int, std::pair<int, int> >::iterator it = m_map8eb68.find(eventType);
    if (it == m_map8eb68.end())
        m_map8eb68.insert(std::make_pair(eventType, std::make_pair(v1, v2)));
    else
    {
        it->second.first = v1;
        it->second.second = v2;
    }
    update_ingame_event_history(eventType);
}

void CUser::set_ingame_event_history(SIG_INGAME_EVENT_HISTORY* sig)
{
    // set_ingame_event_history(SIG*).md @0x868d664：条目 stride 0xc ×30 循环；
    // key==0 跳过；get_event_script_mng()->is_current_ingame_event_record_type(key)
    // 通过才逐条转发 set_ingame_event_history(key,+4,+8)。
    if (sig == 0)
        return;
    for (int i = 0; i < 0x1e; ++i)
    {
        const unsigned char* rec = sig->m_data + i * 0xc;
        int key;
        memcpy(&key, rec, 4);
        if (key == 0)
            continue;
        EventClassify::CEventScriptMng* scriptMng =
            static_cast<EventClassify::CEventScriptMng*>(
                G_CDataManager()->get_event_script_mng());
        if (!scriptMng->is_current_ingame_event_record_type(key))
            continue;
        int v1;
        int v2;
        memcpy(&v1, rec + 4, 4);
        memcpy(&v2, rec + 8, 4);
        set_ingame_event_history(key, v1, v2);
    }
}

void CUser::setCharacLevelUpGift_AccountOnce(SIG_LEVELUP_GIFT_ACCOUNT_ONCE& sig)
{
    // setCharacLevelUpGift_AccountOnce.md @0x8691dca：条目 stride 8 至多 11 条，
    // entry.second(+4)==0 停止；make_pair(entry.first, entry.second) 插入
    // multimap<int,int> m_levelUpGift.m_map(+0x8ec04)。
    for (int i = 0; i < 11; ++i)
    {
        const unsigned char* rec =
            reinterpret_cast<const unsigned char*>(&sig) + i * 8;
        int second;
        memcpy(&second, rec + 4, 4);
        if (second == 0)
            break;
        int first;
        memcpy(&first, rec, 4);
        m_levelUpGift.m_map.insert(std::make_pair(first, second));
    }
}

void CUser::initBingoData()
{
    // initBingoData.md @0x86912be：repeat 0x6a(BingoEvent) 进行中才继续；
    // BingoEvent::getBingoSystem()（全局符号 T 0x080c9fea，定义 BingoEventLink4.cpp）
    // 返回 shared_ptr<CBingo> 槽 {px,pn}，经 px 调 CBingo::initUserData(*bingoData)
    // （本 TU 补真实定义，见下），最后 sendBingoData()（定义 BingoEventLink4.cpp）。
    CEventBase* probe = GlobalData::s_event_manager->GetRepeatEvent(0x6a);
    // ORIG 0x086912be：首次 GetRepeatEvent 后无空判直接虚调 IsEventing；
    // 事件进行中**二次** GetRepeatEvent（此处于判空）后经 shared_ptr
    // operator->（pt 显式符号）解引用 CBingo。NEW 曾复用 probe 且直读 px。
    if (!probe->IsEventing(static_cast<CUser*>(0)))
        return;
    BingoEvent* event = reinterpret_cast<BingoEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (event == 0) return;
    BingoData* data = static_cast<BingoData*>(getBingoData());
    // 注：boost::shared_ptr<CBingo> operator-> 因 TU-local CBingo 不完整无法实例化，
    // 直读 px 槽（语义等价；ORIG pt 符号差异记深查）
    CBingo* bingo = *reinterpret_cast<CBingo**>(event->getBingoSystem());
    bingo->initUserData(*data);
    sendBingoData();
}


// ---------------------------------------------------------------------------
// CUser::setBreakAwayRewardOrder @0x0831d4de（ORIG W sz0x11；报告缺失，
// nm 定址后 objdump 忠实转写：【推断】目标仅 4 字节 int 透传写 this+0x703b8，
// 该槽位在 CUser.h 处于 break-away 奖励域前、未建模字段区）
// ---------------------------------------------------------------------------
void CUser::setBreakAwayRewardOrder(ENUM_BREAK_AWAY_REWARD_ORDER order)
{
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x703b8) =
        static_cast<int>(order);
}

// ============================================================================
// LinkClosureA-2 批次（2026-08-27）—— 全量链接收敛批 A：CUser 方法族 producer。
// 每函数：报告 docs/class_func_reports/CUser/<Func>.md（## 汇编+## 反编译 C 已完整阅读）
// 或报告缺失时 nm -S + objdump 忠实转写（标注）；nm 核验 mangled/地址。
// ============================================================================

// ---- LinkClosureA 消费的外部符号（ORIG nm 实证；实现 owner 待 Main 分派）----
class CEventManager_fwd_check;   // GetRepeatEvent/IsEventing 经 CGuildServerProxy.h 权威声明消费

// ---------------------------------------------------------------------------
// CUser::get_SoloPlay_msgType(MSG_REQ_TO_PEER const&) T 0x0868dd76 sz0x4c
// 报告：docs/class_func_reports/CUser/get_SoloPlay_msgType.md
// 注：MSG_REQ_TO_PEER 当前仅有前向声明（ORIG 亦无完整布局符号），+0xf 为
// SoloPlay 请求子类型 byte（movzbl+movsbl 双指令形态，与 ORIG 一致）。
// ---------------------------------------------------------------------------
int CUser::get_SoloPlay_msgType(const MSG_REQ_TO_PEER& msg)
{
    int v = static_cast<signed char>(
        *reinterpret_cast<const unsigned char*>(
            reinterpret_cast<const char*>(&msg) + 0xf));
    if (v == 1) {
        return 0;
    }
    if (v > 1) {
        if (v == 2) {
            return 2;
        }
        if (v == 3) {
            return 3;
        }
    }
    else if (v == 0) {
        return 1;
    }
    return 4;
}

// ---------------------------------------------------------------------------
// CUser::SendItemMakingSkillInfo() T 0x0868b00a sz0x39
// 报告：docs/class_func_reports/CUser/SendItemMakingSkillInfo.md
// GetCharacExpandDataR(this,10)（CUser.h:2715 声明，返回 CExpandEquipslot*，
// ORIG Ghidra 视图即 CItemMakingSkill* 槽）→ 非空则 CItemMakingSkill::
// Send_ItemMakingSkillList(this)。
// ---------------------------------------------------------------------------
void CUser::SendItemMakingSkillInfo()
{
    CItemMakingSkill* skill =
        reinterpret_cast<CItemMakingSkill*>(GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)10));
    if (skill != 0) {
        skill->Send_ItemMakingSkillList(this);
    }
}

// ---------------------------------------------------------------------------
// CUser::sendBingoAddData() T 0x08691470 sz0x5b
// 报告：docs/class_func_reports/CUser/sendBingoAddData.md
// GetRepeatEvent(0x6a)->IsEventing(0)（vptr+0x34 槽实证，P5 直呼）；
// getBingoData()->m_isWin（+0x5 movzbl）非零才 sendDecorationObjectCnt(1)。
// ---------------------------------------------------------------------------
void CUser::sendBingoAddData()
{
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x6a);
    if (event->IsEventing(0)) {
        BingoData* data = static_cast<BingoData*>(getBingoData());
        if (data->m_isWin) {
            sendDecorationObjectCnt(1);
        }
    }
}

// ---------------------------------------------------------------------------
// CUser::SetGuildInfoProxy(STGuildProxy& proxy) T 0x0865cd0a sz0x48
// 报告：docs/class_func_reports/CUser/SetGuildInfoProxy.md
// ORIG：m_guildDB.m_fundLevel←proxy.m_level(+0x17)；memcpy(m_guildDB,&proxy,0x16)；
// m_guildDB.m_field95←proxy.m_powerSide(+0x18)。STGuildProxy 定义在
// CGuildServerProxy.h:39（0x16=名字区）。
// ---------------------------------------------------------------------------
void CUser::SetGuildInfoProxy(STGuildProxy& proxy)
{
    m_guildDB.m_fundLevel = proxy.m_level;
    memcpy(&m_guildDB, &proxy, 0x16);
    m_guildDB.m_field95 = static_cast<char>(proxy.m_powerSide);
}

// ---------------------------------------------------------------------------
// CUser::checkCharacLinkType(char linkPos, int slot) T 0x08652b90 sz0xfe
// 报告：docs/class_func_reports/CUser/checkCharacLinkType.md
// getCharacLinkType()==2：slot≤linkPos+1 或 linkPos 奇 → 拒；再验
// m_characList[linkPos+1].m_characLinkType==3。
// getCharacLinkType()==3：linkPos-1<0 或 linkPos 偶 → 拒；验 [-1]==2。
// ---------------------------------------------------------------------------
bool CUser::checkCharacLinkType(char linkPos, int slot)
{
    int curLinkType = static_cast<int>(getCharacLinkType());
    if (slot <= static_cast<int>(linkPos) || linkPos < 0) {
        return false;
    }
    if (curLinkType == 2) {
        if (static_cast<int>(linkPos) + 1 >= slot) {
            return false;
        }
        if ((linkPos & 1) != 0) {
            return false;
        }
        if (m_characList[linkPos + 1].m_characLinkType != 3) {
            return false;
        }
    }
    else if (curLinkType == 3) {
        if (static_cast<int>(linkPos) - 1 < 0) {
            return false;
        }
        if ((linkPos & 1) == 0) {
            return false;
        }
        if (m_characList[linkPos - 1].m_characLinkType != 2) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// CUser::SendDungeonClearList() T 0x086781b4 sz0xcd
// 报告：docs/class_func_reports/CUser/SendDungeonClearList.md
// put_header(0,5)→put_short(getDungeonClearSize)→enumDungeonClear(this,
// fnEnumDungeonClear,&packet)→finalize(true)→Send。回调自由函数 ORIG 名
// fnEnumDungeonClear（_Z18fnEnumDungeonClearscPc 0x08678166；CUser.h 现声明
// fnEnumDungeonClears 为 19 字符错拼，mangled 不匹配——已报 Main 修正）。
// ---------------------------------------------------------------------------
bool fnEnumDungeonClear(short dungeonIdx, char diff, char* userData);   // T 0x08678166

void CUser::SendDungeonClearList()
{
    PacketGuard packet;
    packet.put_header(0, 5);
    const WongWork::CDungeonClear* clear = getDungeonClearRefR();
    packet.put_short(static_cast<int>(clear->getDungeonClearSize()));
    clear->enumDungeonClear(this, ::fnEnumDungeonClear, &packet);
    Send(packet);
}

// ---------------------------------------------------------------------------
// CUser::SendEventBringUpInfo() T 0x08689968 sz0xba
// 报告：docs/class_func_reports/CUser/SendEventBringUpInfo.md
// getCurCharacNo()!=0 才发：put_header(0,0x196)→put_byte(
// getCurUseBringUpMaterialTotal)→finalize→Send。
// ---------------------------------------------------------------------------
void CUser::SendEventBringUpInfo()
{
    if (getCurCharacNo() != 0) {
        PacketGuard packet;
        packet.put_header(0, 0x196);
        packet.put_byte(static_cast<int>(getCurUseBringUpMaterialTotal()));
        packet.finalize(true);
        Send(packet);
    }
}

// ---------------------------------------------------------------------------
// CUser::SendTagCharacInfo() T 0x086903f8 sz0xf8
// 报告：docs/class_func_reports/CUser/SendTagCharacInfo.md
// getTagCharacR()!=0：byte@+0x14bc 非 1 则 W 侧置 1；clear→put_header(0,0x19e)
// →put_short(1)→MakeTagCharacInfo→finalize→Send。
// ---------------------------------------------------------------------------
void CUser::SendTagCharacInfo()
{
    const _Charac_info* tagR = getTagCharacR();
    if (tagR != 0) {
        if (static_cast<unsigned char>(tagR->m_pad14bc[0]) != 1) {
            _Charac_info* tagW = getTagCharacW();
            tagW->m_pad14bc[0] = 1;
        }
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x19e);
        packet.put_short(1);
        MakeTagCharacInfo(packet);
        packet.finalize(true);
        Send(packet);
    }
}

// ---------------------------------------------------------------------------
// ORIG 反汇编：getCurCharacR()==0 时 sete+test+jne 早退；Acquire 结果直接
// 作为 CStreamGuard 构造实参（无中间栈变量）。
void CUser::requestCubePremiumInfoIfContracted()
{
    if (getCurCharacR() == 0)
        return;
    if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x5c))
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("user.cpp", 0x475), true);
    guard.operator*()->operator<<(0x332);
    guard.operator*()->operator<<(static_cast<int>(GetUID()));
    SIG_GET_PREMIUM_SELECTED_CUBE_INFO* sig =
        guard->GetInBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO>();
    sig->m_characNo = static_cast<unsigned int>(getCurCharacR()->m_characNo);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
 // ---------------------------------------------------------------------------
// CUser::setCharacSlotLimit(unsigned char limit) W 0x0869755c sz0x1b
// 报告缺失（nm 0x0869755c 定址后 objdump 忠实转写）：
//   mov %al,-0x4; mov 0xc(%ebp)→eax? 实参经 -0x4 截断回读 movzbl 后写
//   this+0x8d245 = m_ceraUserInfo.m_field61（与 getCharacSlotLimit 读侧对称）。
// ---------------------------------------------------------------------------
void CUser::setCharacSlotLimit(unsigned char limit)
{
    m_ceraUserInfo.m_field61 = limit;
}

// ---------------------------------------------------------------------------
// CUser::EnableCharacInfo(CACHE_SECTION section) W 0x081253f4 sz0x13
// 报告缺失（nm 0x081253f4 定址后 objdump 忠实转写）：
//   movb $1, 0x8d0fd(%edx,%eax,1) → this + section + 0x8d0fd
//   即 m_pad8d0fd[section] = 1（缓存段使能标志数组）。
// ---------------------------------------------------------------------------
void CUser::EnableCharacInfo(CACHE_SECTION section)
{
    m_pad8d0fd[static_cast<int>(section)] = 1;
}

// ---------------------------------------------------------------------------
// CUser::SendRevengeDungeon() T 0x08656abc sz0x156
// 报告：docs/class_func_reports/CUser/SendRevengeDungeon.md
// GetRewardType()==1/2 写 type byte（其它 return）；remain=max(0,close-cur)；
// 10 字节 {type,1,remain:int,reserved:int} 经 put_binary(0x1ec 包)发送。
// ---------------------------------------------------------------------------
void CUser::SendRevengeDungeon()
{
    if (getCurCharacR() != 0) {
        char data[10];
        memset(data, 0, 10);
        int rewardType = GlobalData::s_villageMonsterMgr->GetRewardType();
        if (rewardType == 1) {
            data[0] = 0;
        }
        else if (rewardType == 2) {
            data[0] = 1;
        }
        else {
            return;
        }
        unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
        unsigned int closeTime = GlobalData::s_revengeDungeonMgr->GetCloseTime();
        int remain;
        if (closeTime < curSec) {
            remain = 0;
        }
        else {
            remain = static_cast<int>(closeTime - curSec);
        }
        data[1] = 1;
        *reinterpret_cast<int*>(data + 2) = remain;
        *reinterpret_cast<int*>(data + 6) = 0;
        PacketGuard packet;
        packet.put_header(0, 0x1ec);
        packet.put_binary(data, 10);
        packet.finalize(true);
        Send(packet);
    }
}

// ---------------------------------------------------------------------------
// CUser::setTagCharacInven(TAG_CHARAC_INVEN_DATA* data) T 0x08690226 sz0x1d1
// 报告：docs/class_func_reports/CUser/setTagCharacInven.md
// 前任 W7-BLOCKED 解除：调用目标已逐一 nm 解析（0x084e9990
// CInventory::SetClearAvatar(int,int)、0x084e99b4 SetClearAvatar(ulong*)、
// 0x084e91a4 GetItemIdByAddInfo(Inven_Item*,int,int)、0x08694a0c
// CCargo::SetCapacity(int)）。getTagCharacInvenW 返回 Inven_Item* 即
// CInventory 首址视图。TAG_CHARAC_INVEN_DATA 为 0x4024 字节 blob
//（SigTypes_C.h m_data），字段经 m_data+偏移具名成员索引访问。
// ---------------------------------------------------------------------------
int GetItemIdByAddInfo(Inven_Item* items, int space, int addInfo);   // W 0x084e91a4（owner：Item 域批次）

int CUser::setTagCharacInven(TAG_CHARAC_INVEN_DATA* data)
{
    if (getTagCharacR() == 0) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xb166, 5);
        trace("CUser::setTagCharacInven m_tag_selected is null");
        return 0;
    }
    Inven_Item* invenRaw = getTagCharacInvenW();
    memcpy(reinterpret_cast<char*>(invenRaw) + 0x1c, data, 0x53e);
    CInventory* inven = reinterpret_cast<CInventory*>(invenRaw);
    inven->SetClearAvatar(reinterpret_cast<unsigned long*>((char*)data + 0x3ff8));
    for (int i = 0; i < *reinterpret_cast<int*>((char*)data + 0x1e43); ++i) {
        int invenSlot = 0;
        if (*reinterpret_cast<int*>((char*)data + i * 0x4b + 0x1e6c) != 0) {
            invenSlot = GetItemIdByAddInfo(
                reinterpret_cast<Inven_Item*>((char*)data + 0x53e), 0x69,
                *reinterpret_cast<int*>((char*)data + i * 0x4b + 0x1e6c));
            for (int j = 0; j < 10; ++j) {
                if (*reinterpret_cast<int*>((char*)data + j * 0x3d + 7) ==
                    *reinterpret_cast<int*>((char*)data + i * 0x4b + 0x1e47)) {
                    inven->SetClearAvatar(j, invenSlot);
                }
            }
        }
    }
    int capacity = *reinterpret_cast<int*>((char*)data + 0x4020);
    if (capacity == 0) {
        getTagCharacCargoW()->SetCapacity(8);
    }
    else {
        getTagCharacCargoW()->SetCapacity(capacity);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// CUser::SetInventory(SIG_LOAD_INVENTORY* sig) T 0x0864d160 sz0x35a
// 报告：docs/class_func_reports/CUser/SetInventory.md
// SIG_LOAD_INVENTORY 权威字段（SigTypes_C.h）填充；CInventory +0x1c/+0x55a
// 两个 memcpy 目标暂为布局偏移（字段化 owner=CInventory 域批次）。
// ---------------------------------------------------------------------------
int CUser::SetInventory(SIG_LOAD_INVENTORY* sig)
{
    unsigned int money = sig->m_money;
    unsigned int coin = sig->m_coin;
    unsigned int payCoin = sig->m_payCoin;
    unsigned int eventCoin = sig->m_eventCoin;
    char* inven1 = (char*)sig->m_invenType1;
    char* inven2 = (char*)sig->m_invenType2;
    char* equipMirror = (char*)sig->m_equipMirror;
    char* cargo = (char*)sig->m_cargo;
    int cargoCapacity = static_cast<int>(sig->m_cargoCapacity);
    char* inven3 = (char*)sig->m_invenType3;
    if (getCurCharacR() == 0) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1271, 5);
        trace("CUser::SetInventory m_selected is null");
        return 0;
    }
    for (int i = 0; i < 0xc; ++i) {
        const CInventory* invenR = getCurCharacInvenR();
        if (*reinterpret_cast<const int*>(reinterpret_cast<const char*>(invenR) + i * 0x3d + 0x1e) !=
            *reinterpret_cast<int*>(equipMirror + i * 0x3d + 2)) {
            m_characterView.enableSaveCharacView();
            break;
        }
    }
    CInventory* inven = getCurCharacInvenW();
    reinterpret_cast<AvatarCoin*>(reinterpret_cast<char*>(inven) + 0x658)
        ->Set(sig->m_avatarCoin);
    inven = getCurCharacInvenW();
    inven->set_money(money);
    inven = getCurCharacInvenW();
    inven->SetCoin(coin);
    inven = getCurCharacInvenW();
    inven->SetPayCoin(payCoin);
    inven = getCurCharacInvenW();
    inven->SetEventCoin(eventCoin);
    inven = getCurCharacInvenW();
    inven->SetInvenData(1, inven1, 0x4a58);
    inven = getCurCharacInvenW();
    inven->SetInvenData(2, inven2, 0x1905);
    inven = getCurCharacInvenW();
    memcpy(reinterpret_cast<char*>(inven) + 0x1c, equipMirror, 0x53e);
    inven = getCurCharacInvenW();
    memcpy(reinterpret_cast<char*>(inven) + 0x55a, inven3 + 0x38b6, 0xf4);
    for (int i = 0x17; i < 0x1a; ++i) {
        CInventory* invenW = getCurCharacInvenW();
        user_creature::CCreatureMgr* mgr = invenW->GetCreatureMgrW();
        mgr->LoadArtifact(i,
            reinterpret_cast<Inven_Item*>(reinterpret_cast<char*>(invenW) + i * 0x3d + 0x1c));
    }
    inven = getCurCharacInvenW();
    inven->SetInvenData(3, inven3, 0x39aa);
    if (cargoCapacity == 0) {
        getCurCharacCargoW()->SetCapacity(8);
    }
    else {
        getCurCharacCargoW()->SetCapacity(cargoCapacity);
        getCurCharacCargoW()->set_cargo(cargo);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// CUser::SetLimitInfo(SIG_LIMIT_ITEM_INFO* info) T 0x086888f8 sz0x1ed
// 报告：docs/class_func_reports/CUser/SetLimitInfo.md
// 清空 m_limitItemUsage(+0x8cf68) 后遍历 CDataManager::GetLimitItemUsageInfo
// (m_info0.m_list)：sig 条目 itemId 匹配 LimitInfo.m_a → 填 usage{kind,m_b,m_a,
// entry.m_field0} + SetCreateTime(entry.m_createTime)；未匹配 → m_field19=0 +
// SetCurTime()。push_back 到 m_limitItemUsage。
// ---------------------------------------------------------------------------
void CUser::SetLimitInfo(SIG_LIMIT_ITEM_INFO* info)
{
    if (info != 0) {
        m_limitItemUsage.clear();
        stLimitItemUsageInfoEx* usageInfo = G_CDataManager()->GetLimitItemUsageInfo();
        std::vector<stLimitItemUsageInfo::LimitInfo>::iterator it =
            usageInfo->m_info0.m_list.begin();
        for (; it != usageInfo->m_info0.m_list.end(); ++it) {
            LimitItemUsage usage;
            bool matched = false;
            for (int i = 0; i < info->m_count; ++i) {
                if (info->m_entries[i].m_itemId == it->m_a) {
                    usage.m_field10 = it->m_kind;
                    *reinterpret_cast<int*>(
                        reinterpret_cast<char*>(&usage) + 0x11) = it->m_b;
                    usage.m_field15 = it->m_a;
                    usage.m_field19 = info->m_entries[i].m_field0;
                    reinterpret_cast<CommonTime*>(&usage)
                        ->SetCreateTime(info->m_entries[i].m_createTime);
                    m_limitItemUsage.push_back(usage);
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                usage.m_field10 = it->m_kind;
                *reinterpret_cast<int*>(
                    reinterpret_cast<char*>(&usage) + 0x11) = it->m_b;
                usage.m_field15 = it->m_a;
                usage.m_field19 = 0;
                reinterpret_cast<CommonTime*>(&usage)->SetCurTime();
                m_limitItemUsage.push_back(usage);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CUser::SelectCharac(int idx, int tagIdx) T 0x0864cd92 sz0x3cd
// 报告：docs/class_func_reports/CUser/SelectCharac.md
// 选择主角色/tag 角色：设 curCharac/set_parent 链、stamina 离线恢复
//（elapsed=min(600,difftime); min(stamina+elapsed*100/600,100)）、
// HistoryLog::WriteSelectCh、GameWorld::m_channelUserCount[job]++。
// ---------------------------------------------------------------------------
int CUser::SelectCharac(int idx, int tagIdx)
{
    if (getCurCharacR() != 0) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x11fb, 5);
        trace("CUser::SelectCharac m_selected is not null");
        return 0;
    }
    bool idxValid = (idx >= 0) && (static_cast<int>(m_characList.size()) > idx);
    if (idxValid) {
        setCurCharac(&m_characList[idx]);
        bool tagValid = (tagIdx >= 0) && (static_cast<int>(m_characList.size()) > tagIdx);
        if (tagValid) {
            setTagCharac(&m_characList[tagIdx]);
            const CInventory* tagInven =
                reinterpret_cast<const CInventory*>(getTagCharacInvenR());
            const_cast<CInventory*>(tagInven)->set_parent(this);
            const SkillSlot* tagSkill =
                reinterpret_cast<const SkillSlot*>(getTagCharacSkillR());
            const_cast<SkillSlot*>(tagSkill)->set_parent(this);
        }
        if (getCurCharacR() == 0) {
            cMyTrace trace2(__PRETTY_FUNCTION__, 0x120c, 5);
            trace2("CUser::SelectCharac m_selected is null at %d", idx);
            return 0;
        }
        if (getCurCharacR()->m_stamina < 100) {
            int limits[2];
            unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
            long lastTick = getCurCharacLastPlayTick();
            double elapsed = difftime(static_cast<time_t>(curSec),
                                      static_cast<time_t>(lastTick));
            int minv = std::min(limits[1], limits[0]);
            int zero = 0;
            minv = std::max(zero, minv);
            limits[0] = minv;
            int svals[2];
            svals[0] = static_cast<int>(getCurCharacR()->m_stamina)
                       + (limits[0] * 100) / 600;
            svals[1] = 100;
            minv = std::min(svals[1], svals[0]);
            setCurCharacStamina(static_cast<unsigned char>(minv));
        }
        char growType = static_cast<char>(getCurCharacGrowType());
        int level = get_charac_level();
        int job = get_charac_job();
        const char* name = getCurCharacName();
        HistoryLog::WriteSelectCh(*reinterpret_cast<FILE* const*>(&m_field796f8),
                                  name, job, level, static_cast<int>(growType));
        const CInventory* curInven = getCurCharacInvenR();
        const_cast<CInventory*>(curInven)->set_parent(this);
        const SkillSlot* curSkill =
            reinterpret_cast<const SkillSlot*>(getCurCharacSkillR());
        const_cast<SkillSlot*>(curSkill)->set_parent(this);
        UserQuest* quest = getCurCharacQuestR();
        quest->set_parent(this);   // 声明已由 Main 代落 CUser.h（ORIG W _ZN10UserQuest10set_parentEP5CUser）
        const CCargo* curCargo = getCurCharacCargoR();
        const_cast<CCargo*>(curCargo)->set_parent(this);
        job = get_charac_job();
        G_GameWorld()->m_channelUserCount[job] += 1;
        return 1;
    }
    cMyTrace trace3(__PRETTY_FUNCTION__, 0x1246, 5);
    trace3("CUser::SelectCharac :charac_index : %d", idx);
    return 0;
}

// ---------------------------------------------------------------------------
// CUser::set_MultimailMsgData(MSG_MULTI_MAILBOX_SEND* data) T 0x0868a4e8 sz0x32
// 报告：docs/class_func_reports/CUser/set_MultimailMsgData.md
// 0x8e3ec multimail flag=1 + memcpy(0x8e246, data, 0x1a6)。0x8e 区访问与
// InitOnAccept 既有写法一致（m_pad8e101 为 0x8e101 起块视图）。
// ---------------------------------------------------------------------------
void CUser::set_MultimailMsgData(MSG_MULTI_MAILBOX_SEND* data)
{
    m_pad8e101[0x2eb] = 1;                                 // ORIG movb $1,0x8e3ec
    memcpy(m_pad8e101 + 0x145, data, 0x1a6);               // ORIG memcpy(this+0x8e246,..)
}

// ---------------------------------------------------------------------------
// CUser::SendMail(bool certified) T 0x08689f12 sz0x3ca
// 报告：docs/class_func_reports/CUser/SendMail.md
// 0x8e101 MSG_MAILBOX_SEND 载荷（m_pad8e101 块视图）：name@+0x11/nameLen@+0xd/
// money@+0x2e/itemId@+0x39；multimail flag@0x8e3ec 分流；疲劳礼物金
// limit=(long long)(money*(0.7+(float)(rand*0.01)))（0x8cf3988=0.01、
// 0x8cf3990=0.7 实测）；SIG_FIND_FACTORY_HUB 0x24 块入 DB_Q。
// ---------------------------------------------------------------------------
void CUser::SendMail(bool certified)
{
    m_field8e3ed = true;                                   // ORIG movb $1,0x8e3ed
    if (m_pad8e101[0x2eb] != 0) {                          // +0x8e3ec multimail flag
        SendMultiMail(certified);
        return;
    }
    if (certified) {
        MSG_CHECK_MAIL_RECEIVER check;
        check.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_0>(
            *reinterpret_cast<MSG_MAILBOX_SEND*>(m_pad8e101));
        int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(this, m_pad8e101 + 0x11, &check);
        if (rc == 0) {
            bool fatigueGift = false;                      // local_21
            int itemId = *reinterpret_cast<int*>(m_pad8e101 + 0x39);      // +0x8e13a
            CItem* item = G_CDataManager()->find_item(itemId);
            if (item != 0) {
                bool skip;
                if (item->IsAvatarItem() || item->get_rarity() > 1) {
                    skip = false;
                }
                else {
                    skip = true;
                }
                if (skip) {
                    fatigueGift = true;
                }
            }
            if (itemId == 0) {
                fatigueGift = true;
            }
            IncrementTradeGold(*reinterpret_cast<unsigned int*>(m_pad8e101 + 0x2e),
                               static_cast<int>(fatigueGift));
        }
        else {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x61, static_cast<unsigned char>(rc));
        }
        if (getCurCharacR() != 0) {
            unsigned short usedFatigue = getCurCharacUsedFatigue();
            int randv = get_rand_int(0x19);
            CInventory* inven = const_cast<CInventory*>(getCurCharacInvenR());
            int money = inven->get_money();
            long long limit = static_cast<long long>(
                static_cast<double>(money) * (0.7 + static_cast<float>(
                    static_cast<double>(randv) * 0.01)));
            if (static_cast<unsigned int>(limit) <=
                *reinterpret_cast<unsigned int*>(m_pad8e101 + 0x2e)) {   // +0x8e12f
                Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0xa12b);
                CStreamGuard guard(stream, true);
                guard.operator*()->operator<<(0x18a);
                guard.operator*()->operator<<(static_cast<int>(GetUID()));
                SIG_FIND_FACTORY_HUB* sig =
                    guard->GetInBuffer<SIG_FIND_FACTORY_HUB>();
                memset(sig, 0, 0x24);
                *reinterpret_cast<unsigned int*>(sig->m_pad) = get_acc_id();
                strncpy(sig->m_pad + 4, m_pad8e101 + 0x11,
                        *reinterpret_cast<unsigned int*>(m_pad8e101 + 0xd));  // +0x8e10e
                sig->m_pad[0x21] = 0;
                int r = get_rand_int(0x14);
                if (r + 0x78 <= static_cast<int>(usedFatigue)) {
                    sig->m_pad[0x21] = 1;
                }
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
        }
        memset(m_pad8e101, 0, 0x145);                      // 清 MSG_MAILBOX_SEND 载荷
        return;
    }
    SendCmdErrorPacket((ENUM_CMDPACKET)0x61, 0x87);
    memset(m_pad8e101, 0, 0x145);
}

// ---------------------------------------------------------------------------
// CUser::sendDecorationObjectCnt(short count) T 0x08690d9a sz0x96
// 报告缺失（nm 0x08690d9a 定址后 objdump 忠实转写）：
// put_header(0,0x1ac)→put_short(count)→finalize(true)→Send。
// ---------------------------------------------------------------------------
void CUser::sendDecorationObjectCnt(short count)
{
    PacketGuard packet;
    packet.put_header(0, 0x1ac);
    packet.put_short(static_cast<int>(count));
    packet.finalize(true);
    Send(packet);
}

// ---------------------------------------------------------------------------
// CUser::CreateCharac(SIG_CREATE_CHARAC* sig) T 0x0864a186 sz0x2ab
// 报告：docs/class_func_reports/CUser/CreateCharac.md
// 栈上 _Charac_info：Reset→字段化填充（characNo/name/job/技能表 0x52 拷贝/
// stamina=100/疲劳上限/0xff 禁玩标记等）→局部 CInventory/CCargo 缓冲绑
// InventoryMemory→push_back→CharacAdd(size-1)→enableSaveCharacView→
// _onCreateCharacter→返回 0。局部对象无显式构造的缓冲区按 ORIG 原样
// reinterpret_cast（ORIG 无 CInventory/CCargo C1 调用）。
// ---------------------------------------------------------------------------
int CUser::CreateCharac(SIG_CREATE_CHARAC* sig)
{
    _Charac_info info;
    info.Reset();
    info.m_characNo = sig->m_characNo;
    strncpy(info.m_name, sig->m_name, 0x1e);
    info.m_field22 = 1;
    info.m_field23 = 2;
    info.m_field24 = 0;
    info.m_field25 = 0;
    info.m_field26 = static_cast<char>(sig->m_slot);           // job
    info.m_level = 1;
    info.m_field2b = 0;
    info.m_field29 = 0;
    info.m_stamina = 100;
    reinterpret_cast<CommonTime*>(reinterpret_cast<char*>(&info) + 0x39)->SetCurTime();
    info.m_field33 = 0;
    info.m_field35 = static_cast<unsigned short>(DEFAULT_MAX_FATIGUE);
    info.m_field37 = 0;
    info.m_field4f = 0;
    info.m_pad83[0] = 0;                                       // +0x83
    *reinterpret_cast<int*>(&info.m_pad83[1]) = 0;             // +0x84（随后被 0x52 覆盖）
    char job = static_cast<char>(sig->m_slot);
    memcpy(reinterpret_cast<char*>(&info) + 0x84,
           *reinterpret_cast<char**>(reinterpret_cast<char*>(G_CDataManager()) + 0x14)
               + job * 0x7dc, 0x52);
    info.m_resetDwordDf4 = 0;
    info.m_resetDwordDf8 = 0;
    info.m_pade85[0xa] = 0;                                    // +0xe8f
    info.m_field1239 = 0xff;
    info.m_pad1240 = static_cast<char>(sig->m_eventCharacFlag != 0);
    char invenBuf[0xcd0];
    reinterpret_cast<CInventory*>(invenBuf)->SetInventoryMemory(&m_inventoryMemory);
    char cargoBuf[0x4a];
    reinterpret_cast<CCargo*>(cargoBuf)->SetInventoryMemory(&m_inventoryMemory);
    m_characList.push_back(info);
    m_historyLog.CharacAdd(static_cast<int>(m_characList.size()) - 1);
    m_characterView.enableSaveCharacView();
    _onCreateCharacter(sig->m_characNo, sig->m_name, job);
    return 0;
}

// ---------------------------------------------------------------------------
// CUser::use_stackable(short slot, int space, int p3, int p4) T 0x0865e0ae sz0x8e5
// 报告：docs/class_func_reports/CUser/use_stackable.md（## 汇编+## 反编译 C 已完整阅读）
// 校验链（trade/find_item/is_stackable/CAlchemist/CoolTime）→use_item→
// BossTower/连续道具/CAlchemist 消费→LV70Buff 与疲劳药品表→结果包 0x2f。
// ---------------------------------------------------------------------------
void SendLV70Buff(CUser* user, unsigned long buff);   // ORIG T 0x0865defc（owner：Buff 域批次）

bool CUser::use_stackable(short slot, int space, int p3, int p4)
{
    int err = 0;
    if (CheckInTrade()) {
        err = 0x13;
    }
    GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    Inven_Item item = const_cast<CInventory*>(getCurCharacInvenR())
                          ->GetInvenSlot(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot);
    CItem* itemDef = G_CDataManager()->find_item(item.m_addInfo);
    if (itemDef == 0) {
        err = 0x17;
    }
    else if (!itemDef->is_stackable()) {
        err = 0x17;
    }
    if (err == 0 && expert_job::CAlchemist::CheckUsableStackableItem(this, itemDef) != 1) {
        PacketGuard packet;
        packet.put_header(1, 0x2f);
        packet.put_byte(0);
        packet.put_byte(7);
        packet.put_byte(space);
        packet.put_int(p3);
        packet.put_int(p4);
        packet.finalize(true);
        Send(packet);
        return true;
    }
    if (err == 0 && CheckCoolTimeItem(static_cast<unsigned long>(item.m_addInfo))) {
        PacketGuard packet;
        packet.put_header(1, 0x2f);
        packet.put_byte(0);
        packet.put_byte(0x13);
        packet.put_byte(space);
        packet.put_int(p3);
        packet.put_int(p4);
        packet.finalize(true);
        Send(packet);
        return true;
    }
    if (err == 0) {
        CInventory* inven = getCurCharacInvenW();
        err = inven->use_item(slot, 1, space);
    }
    if (err == 0) {
        Global::CBossTowerFunc::UseItem(this, item);
    }
    if (err == 0) {
        _processContinuousItem(static_cast<unsigned int>(item.m_addInfo),
                               static_cast<CStackableItem*>(itemDef));
    }
    if (err == 0) {
        expert_job::CAlchemist::OnUseStackableItem(
            this, static_cast<unsigned long>(item.m_addInfo), itemDef);
    }
    if (err == 0) {
        unsigned long idx = static_cast<unsigned long>(item.m_addInfo);
        if (idx == 0x1e70) {
            SendLV70Buff(this, 0x1e77);
        }
        else if (idx < 0x1e71) {
            if (idx < 0x1dde) {
                if (idx < 0x1ddc) {
                    if (idx == 0x1d20) {
                        int remain = getCurCharacFatigue() - 0x28;
                        int zero = 0;
                        setCurCharacFatigue(std::max(zero, remain));
                        SendFatigue();
                    }
                    else if (idx == 0x1d5e) {
                        int remain = getCurCharacFatigue() - 10;
                        int zero = 0;
                        setCurCharacFatigue(std::max(zero, remain));
                        SendFatigue();
                    }
                }
                else {
                    int remain = getCurCharacFatigue() - 6;
                    int zero = 0;
                    setCurCharacFatigue(std::max(zero, remain));
                    SendFatigue();
                }
            }
            else if (idx == 0x1e6e) {
                SendLV70Buff(this, 0x1e75);
            }
            else if (idx < 0x1e6f) {
                if (idx == 0x1e6d) {
                    SendLV70Buff(this, 0x1e74);
                }
            }
            else {
                SendLV70Buff(this, 0x1e76);
            }
        }
        else {
            if (idx != 0x1f8f) {
                if (idx < 0x1f90) {
                    if (idx == 0x1e72) {
                        SendLV70Buff(this, 0x1e79);
                    }
                    else if (idx < 0x1e72) {
                        SendLV70Buff(this, 0x1e78);
                    }
                    else if (idx == 0x1e73) {
                        SendLV70Buff(this, 0x1e7a);
                    }
                    goto sendResult;
                }
                if (idx != 0x27ac5a) {
                    if (idx < 0x27ac5b) {
                        if (idx == 0x201d) {
                            int remain = getCurCharacFatigue() - 0x14;
                            int zero = 0;
                            setCurCharacFatigue(std::max(zero, remain));
                            SendFatigue();
                        }
                        goto sendResult;
                    }
                    if ((idx != 0x27addd) && (idx != 0x292090de)) {
                        goto sendResult;
                    }
                }
            }
            int remain = getCurCharacFatigue() - 0x1e;
            int zero = 0;
            setCurCharacFatigue(std::max(zero, remain));
            SendFatigue();
        }
    }
sendResult:
    {
        PacketGuard packet;
        packet.put_header(1, 0x2f);
        if (err != 0) {
            packet.put_byte(0);
            packet.put_byte(err);
            packet.put_byte(space);
            packet.put_int(p3);
            packet.put_int(p4);
            packet.finalize(true);
            Send(packet);
            Inven_Item slotItem = const_cast<CInventory*>(getCurCharacInvenR())
                                      ->GetInvenSlot(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x38b5, 5);
            trace("CUser::use_stackable\tError:%d\tSlot_no:%d\tItem_no:%d",
                  err, static_cast<int>(slot), slotItem.m_addInfo);
        }
        else {
            packet.put_byte(1);
            packet.put_short(static_cast<int>(slot));
            packet.put_byte(space);
            packet.put_int(p3);
            packet.put_int(p4);
            packet.finalize(true);
            Send(packet);
            ARAD::ARAD_ITEM::arad_secret_shop_event_item(
                this, static_cast<unsigned long>(item.m_addInfo),
                static_cast<CStackableItem*>(itemDef));
        }
    }
    return err == 0;
}

// ---------------------------------------------------------------------------
// CMaxFatigueFactorEvent：ORIG 仅 GetMaxFatigueFactor（W 0x0812ce54）被消费，
// 权威类未建模；本 TU 以最小类声明 + reinterpret_cast 视图消费（GetRepeatEvent(2)
// 返回的 CEventBase* 实际指向该事件对象）。
// ---------------------------------------------------------------------------
class CMaxFatigueFactorEvent
{
public:
    int GetMaxFatigueFactor();   // ORIG W _ZN22CMaxFatigueFactorEvent19GetMaxFatigueFactorEv
};

// tagCharacVisibleFlag：ORIG W 0x08695c6c reset / 0x08695c60 ctor；权威类未建模，
// 本 TU 最小声明 + reinterpret_cast 视图消费（_Charac_info +0xe40 区）。
class tagCharacVisibleFlag
{
public:
    tagCharacVisibleFlag();   // ORIG W _ZN20tagCharacVisibleFlagC1Ev @0x08695c60（LinkE2）
    void reset();   // ORIG W _ZN20tagCharacVisibleFlag5resetEv
};

// ---------------------------------------------------------------------------
// CUser::setCharacInfoDetail(int slot, unsigned int characNo, CHARAC_DATA* src)
// T 0x0864ac1a sz0xfe9 —— 报告：setCharacInfoDetail.md（## 汇编+## 反编译 C 全文）。
// slot/characNo 校验 → CHARAC_DATA 全字段搬运（成长型特化 7/8/0xc/0xd）→
// initCharacStat → 疲劳上限事件(2/0x57) → 疲劳电池错误分支 → LastPlayTick 等。
// ---------------------------------------------------------------------------
bool CUser::setCharacInfoDetail(int slot, unsigned int characNo, CHARAC_DATA* src)
{
    if (slot < 0 || slot >= static_cast<int>(m_characList.size())) {
        return false;
    }
    if (m_characList[slot].m_characNo != static_cast<int>(characNo)) {
        return false;
    }
    _Charac_info& d = m_characList[slot];
    char* dst = reinterpret_cast<char*>(&d);
    const char* s = reinterpret_cast<const char*>(src);
    *reinterpret_cast<int*>(dst) = *reinterpret_cast<const int*>(s);   // characNo
    if (G_GameWorld()->GetChannelType() != 7) {
        strncpy(d.m_name, s + 4, 0x1e);
    }
    dst[0x22] = s[0x22];                                   // growType
    if (s[0x22] == 7) {                                    // 学徒 → 固定节点
        dst[0x22] = 2;
        dst[0x24] = 7;
    }
    if (s[0x22] == 8) {                                    // 二段成长互换
        char tmp = s[0x22];
        dst[0x22] = s[0x23];
        dst[0x25] = tmp;
    }
    if (s[0x22] == 0xc || s[0x22] == 0xd) {
        dst[0x22] = 0xb;
    }
    dst[0x23] = 2;
    dst[0x26] = s[0x24];                                   // job
    *reinterpret_cast<short*>(dst + 0x27) = *reinterpret_cast<const short*>(s + 0x25);
    *reinterpret_cast<int*>(dst + 0x2b) = *reinterpret_cast<const int*>(s + 0x28);
    dst[0x29] = s[0x27];
    dst[0x75] = s[0xb2];                                   // stamina
    *reinterpret_cast<unsigned int*>(dst + 0x88) = *reinterpret_cast<const unsigned int*>(s + 0x60);
    *reinterpret_cast<unsigned int*>(dst + 0x8c) = *reinterpret_cast<const unsigned int*>(s + 100);
    *reinterpret_cast<unsigned short*>(dst + 0x90) = *reinterpret_cast<const unsigned short*>(s + 0x68);
    *reinterpret_cast<unsigned short*>(dst + 0x92) = *reinterpret_cast<const unsigned short*>(s + 0x6a);
    *reinterpret_cast<unsigned short*>(dst + 0x94) = *reinterpret_cast<const unsigned short*>(s + 0x6c);
    *reinterpret_cast<unsigned short*>(dst + 0x96) = *reinterpret_cast<const unsigned short*>(s + 0x6e);
    memcpy(dst + 0x98, s + 0x70, 8);
    memcpy(dst + 0xa0, s + 0x78, 0x22);
    *reinterpret_cast<unsigned int*>(dst + 0xc2) = *reinterpret_cast<const unsigned int*>(s + 0x9a);
    *reinterpret_cast<unsigned short*>(dst + 0xc6) = *reinterpret_cast<const unsigned short*>(s + 0x9e);
    *reinterpret_cast<unsigned short*>(dst + 200) = *reinterpret_cast<const unsigned short*>(s + 0xa0);
    *reinterpret_cast<unsigned int*>(dst + 0xca) = *reinterpret_cast<const unsigned int*>(s + 0xa2);
    *reinterpret_cast<unsigned short*>(dst + 0xce) = *reinterpret_cast<const unsigned short*>(s + 0xa6);
    *reinterpret_cast<unsigned short*>(dst + 0xd0) = *reinterpret_cast<const unsigned short*>(s + 0xa8);
    *reinterpret_cast<unsigned short*>(dst + 0xd2) = *reinterpret_cast<const unsigned short*>(s + 0xaa);
    *reinterpret_cast<unsigned short*>(dst + 0xd4) = *reinterpret_cast<const unsigned short*>(s + 0xac);
    *reinterpret_cast<unsigned int*>(dst + 0xd6) = *reinterpret_cast<const unsigned int*>(s + 0xae);
    char job = dst[0x26];
    char sub = dst[0x29];
    short level = *reinterpret_cast<short*>(dst + 0x27);
    unsigned int no = static_cast<unsigned int>(d.m_characNo);
    global::tools::InitializeCharacStat::initCharacStat(
        *reinterpret_cast<_Additioal_info*>(dst + 0x88), no,
        static_cast<int>(job), static_cast<int>(level),
        static_cast<int>(static_cast<signed char>(sub << 4) >> 4),
        static_cast<int>(static_cast<signed char>(sub * 2) >> 5));
    *reinterpret_cast<unsigned short*>(dst + 0x33) = *reinterpret_cast<const unsigned short*>(s + 0x2c);
    *reinterpret_cast<unsigned short*>(dst + 0x35) = *reinterpret_cast<const unsigned short*>(s + 0x2e);
    *reinterpret_cast<unsigned short*>(dst + 0x37) = *reinterpret_cast<const unsigned short*>(s + 0x30);
    {
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(2);
        if (ev != 0) {
            if (!ev->IsEventing(0)) {
                *reinterpret_cast<short*>(dst + 0x35) = static_cast<short>(DEFAULT_MAX_FATIGUE);
            }
            else {
                int factor = reinterpret_cast<CMaxFatigueFactorEvent*>(ev)->GetMaxFatigueFactor();
                *reinterpret_cast<short*>(dst + 0x35) =
                    static_cast<short>(factor * DEFAULT_MAX_FATIGUE / 100);
            }
        }
    }
    {
        CEventBase* ev57 = GlobalData::s_event_manager->GetRepeatEvent(0x57);
        if (ev57->IsEventing(0)) {
            short base = *reinterpret_cast<short*>(dst + 0x35);
            *reinterpret_cast<short*>(dst + 0x35) = static_cast<short>(
                base + *reinterpret_cast<short*>(reinterpret_cast<char*>(G_CDataManager()) + 0x51c));
        }
    }
    if (*reinterpret_cast<const short*>(s + 0x32) < 0) {   // 疲劳电池错误分支
        *reinterpret_cast<unsigned short*>(dst + 0x4f) = 0;
        short badVal = *reinterpret_cast<const short*>(s + 0x32);
        cMyTrace t(__PRETTY_FUNCTION__,
                   0xf99, 0);
        t("FATIGUE BATTERY Error : Char No(%d) %d",
          *reinterpret_cast<const int*>(s), static_cast<int>(badVal));
    }
    else {
        *reinterpret_cast<unsigned short*>(dst + 0x4f) =
            *reinterpret_cast<const unsigned short*>(s + 0x32);
    }
    *reinterpret_cast<unsigned int*>(dst + 0xdf4) = 0;
    *reinterpret_cast<unsigned int*>(dst + 0xdf8) = 0;
    *reinterpret_cast<unsigned int*>(dst + 0x7b) = *reinterpret_cast<const unsigned int*>(s + 0x4d);
    {
        unsigned int lastTick = *reinterpret_cast<unsigned int*>(dst + 0x7b);
        cMyTrace t(__PRETTY_FUNCTION__,
                   0xfb0, 0);
        t("CharLastPlayTick char(%d) t(%d)", characNo, lastTick);
    }
    *reinterpret_cast<unsigned int*>(dst + 0x7f) = *reinterpret_cast<const unsigned int*>(s + 0x118);
    *reinterpret_cast<unsigned int*>(dst + 0xdbe) = *reinterpret_cast<const unsigned int*>(s + 0x56);
    dst[0xdc2] = s[0x5a];
    *reinterpret_cast<unsigned short*>(dst + 0x77) = *reinterpret_cast<const unsigned short*>(s + 0x5b);
    *reinterpret_cast<unsigned short*>(dst + 0x79) = *reinterpret_cast<const unsigned short*>(s + 0x5d);
    if (s[0x5f] != 0) {
        *reinterpret_cast<unsigned int*>(dst + 0xe22) = *reinterpret_cast<const unsigned int*>(s);
    }
    reinterpret_cast<CommonTime*>(dst + 0x39)->SetCreateTime(
        const_cast<char*>(s + 0x34));
    *reinterpret_cast<unsigned short*>(dst + 0x4d) = *reinterpret_cast<const unsigned short*>(s + 0xb3);
    *reinterpret_cast<unsigned int*>(dst + 0x51) = *reinterpret_cast<const unsigned int*>(s + 0xb5);
    memcpy(dst + 0x55, s + 0xb9, 0x10);
    reinterpret_cast<CReliablePerson*>(dst + 0x65)->InitTradeGold(
        *reinterpret_cast<const unsigned int*>(s + 0xc9),
        *reinterpret_cast<const unsigned int*>(s + 0xcd),
        *reinterpret_cast<const unsigned int*>(s + 0xd1));
    {
        unsigned int useGold = *reinterpret_cast<const unsigned int*>(s + 0xd1);
        cMyTrace t(__PRETTY_FUNCTION__,
                   0xfd7, 0);
        t("TRADE_RESTIRCTION charac_name(%s), use_gold(%u)", d.m_name, useGold);
    }
    *reinterpret_cast<unsigned int*>(dst + 0xe49) = *reinterpret_cast<const unsigned int*>(s + 0xd5);
    *reinterpret_cast<unsigned int*>(dst + 0xe4d) = *reinterpret_cast<const unsigned int*>(s + 0xd9);
    *reinterpret_cast<unsigned int*>(dst + 0xe51) = *reinterpret_cast<const unsigned int*>(s + 0xdd);
    *reinterpret_cast<unsigned int*>(dst + 0xe55) = *reinterpret_cast<const unsigned int*>(s + 0xe1);
    *reinterpret_cast<unsigned int*>(dst + 0xe59) = *reinterpret_cast<const unsigned int*>(s + 0xe5);
    *reinterpret_cast<unsigned int*>(dst + 0xe5d) = *reinterpret_cast<const unsigned int*>(s + 0xe9);
    *reinterpret_cast<unsigned int*>(dst + 0xe45) = *reinterpret_cast<const unsigned int*>(s + 0xed);
    *reinterpret_cast<unsigned int*>(dst + 0xe68) = *reinterpret_cast<const unsigned int*>(s + 0xf1);
    dst[0xe70] = s[0xf5];
    *reinterpret_cast<unsigned int*>(dst + 0xe71) = *reinterpret_cast<const unsigned int*>(s + 0xf6);
    *reinterpret_cast<unsigned int*>(dst + 0xe79) = *reinterpret_cast<const unsigned int*>(s + 0xfa);
    *reinterpret_cast<unsigned int*>(dst + 0xe81) = *reinterpret_cast<const unsigned int*>(s + 0x10a);
    *reinterpret_cast<unsigned int*>(dst + 0xe7d) = *reinterpret_cast<const unsigned int*>(s + 0x106);
    *reinterpret_cast<unsigned short*>(dst + 0x121f) = *reinterpret_cast<const unsigned short*>(s + 0x10e);
    *reinterpret_cast<unsigned int*>(dst + 0x1221) = *reinterpret_cast<const unsigned int*>(s + 0x110);
    *reinterpret_cast<unsigned int*>(dst + 0x1225) = *reinterpret_cast<const unsigned int*>(s + 0x114);
    dst[0x1239] = s[0x11e];
    dst[0x123e] = s[0x11f];
    dst[0x123f] = s[0x120];
    *reinterpret_cast<bool*>(dst + 0x1241) = (s[0x121] != 0);
    *reinterpret_cast<unsigned short*>(dst + 0x126c) = *reinterpret_cast<const unsigned short*>(s + 0x122);
    dst[0x126e] = s[0x124];
    *reinterpret_cast<unsigned int*>(dst + 0x126f) = *reinterpret_cast<const unsigned int*>(s + 0x125);
    dst[0x1275] = s[0x129];
    dst[0x1276] = s[0x12a];
    dst[0x1277] = s[299];
    dst[0x127a] = s[0x12d];
    dst[0x1278] = s[300];
    dst[0x1280] = s[0x12f];
    *reinterpret_cast<unsigned int*>(dst + 0x12ad) = *reinterpret_cast<const unsigned int*>(s + 0x130);
    return true;
}

// ---------------------------------------------------------------------------
// _Charac_info::Reset() T 0x086822b8 sz0x7d9
// 报告：docs/class_func_reports/_Charac_info/Reset.md（## 汇编+## 反编译 C 全文）。
// 全字段清零 + CommonTime::clear + _Additioal_info::clear + CInventory::reset +
// SkillSlot::clear_all_skills_both + CCargo::reset + 两 map clear +
// tagCharacVisibleFlag::reset + stAuraAvatarOption::reset。
// ---------------------------------------------------------------------------
void _Charac_info::Reset()
{
    char* p = reinterpret_cast<char*>(this);
    *reinterpret_cast<int*>(p) = 0;                          // m_characNo
    memset(p + 4, 0, 0x1e);                                  // m_name
    p[0x22] = 0; p[0x23] = 0; p[0x26] = 0;
    *reinterpret_cast<short*>(p + 0x27) = 0;
    p[0x29] = 0; p[0x2a] = 0;
    *reinterpret_cast<int*>(p + 0x2b) = 0;
    *reinterpret_cast<int*>(p + 0x2f) = 0;
    *reinterpret_cast<short*>(p + 0x33) = 0;
    *reinterpret_cast<short*>(p + 0x35) = 0;
    *reinterpret_cast<short*>(p + 0x37) = 0;
    reinterpret_cast<CommonTime*>(p + 0x39)->clear();
    *reinterpret_cast<short*>(p + 0x77) = 0;
    *reinterpret_cast<short*>(p + 0x79) = 0;
    *reinterpret_cast<int*>(p + 0x7b) = 0;
    *reinterpret_cast<int*>(p + 0x7f) = 0;
    reinterpret_cast<_Additioal_info*>(p + 0x88)->clear();
    reinterpret_cast<CInventory*>(p + 0xda)->reset();
    reinterpret_cast<SkillSlot*>(p + 0x892)->clear_all_skills_both();
    reinterpret_cast<CCargo*>(p + 0xdaa)->reset();
    *reinterpret_cast<int*>(p + 0xdbe) = 0;
    p[0xdc2] = 0;
    *reinterpret_cast<int*>(p + 0xe02) = 0;
    *reinterpret_cast<int*>(p + 0xdf4) = 0;
    *reinterpret_cast<int*>(p + 0xdf8) = 0;
    *reinterpret_cast<int*>(p + 0xdfe) = 0;
    *reinterpret_cast<int*>(p + 0xe06) = 0;
    *reinterpret_cast<int*>(p + 0xe0a) = 0;
    memset(p + 0xe0e, 0, 0x14);
    *reinterpret_cast<int*>(p + 0xe22) = 0;
    *reinterpret_cast<int*>(p + 0xe26) = 0;
    *reinterpret_cast<int*>(p + 0xe2a) = 0;
    *reinterpret_cast<int*>(p + 0xe33) = 0;
    *reinterpret_cast<int*>(p + 0xe37) = 0;
    *reinterpret_cast<int*>(p + 0xe2e) = 0;
    p[0xe32] = 0;
    p[0xe3b] = 0;
    p[0x83] = 0;
    *reinterpret_cast<int*>(p + 0x84) = 0;
    *reinterpret_cast<int*>(p + 0xe3c) = 0;
    *reinterpret_cast<int*>(p + 0xde9) = 0;
    *reinterpret_cast<short*>(p + 0xded) = 0;
    *reinterpret_cast<int*>(p + 0xdef) = 0;
    p[0xdf3] = 0;
    *reinterpret_cast<short*>(p + 0xdfc) = 0;
    *reinterpret_cast<int*>(p + 0xe41) = 0;
    *reinterpret_cast<int*>(p + 0xe49) = 0;
    *reinterpret_cast<int*>(p + 0xe4d) = 0;
    *reinterpret_cast<int*>(p + 0xe45) = 0;
    *reinterpret_cast<int*>(p + 0xe51) = 0;
    *reinterpret_cast<int*>(p + 0xe55) = 0;
    *reinterpret_cast<int*>(p + 0xe59) = 0;
    *reinterpret_cast<int*>(p + 0xe61) = 0;
    memset(p + 0xe8d, 0, 0xc);
    p[0xe90] = 1;
    p[0x24] = 0;
    p[0xe65] = 0;
    *reinterpret_cast<int*>(p + 0xe5d) = 0;
    p[0xe66] = 0;
    p[0xe67] = 0;
    *reinterpret_cast<int*>(p + 0xe68) = 0;
    *reinterpret_cast<int*>(p + 0xe6c) = 0;
    *reinterpret_cast<int*>(p + 0xe71) = 0;
    *reinterpret_cast<int*>(p + 0xe75) = 0;
    *reinterpret_cast<int*>(p + 0xe79) = 0;
    *reinterpret_cast<int*>(p + 0xe7d) = 0;
    *reinterpret_cast<int*>(p + 0xe81) = 0;
    *reinterpret_cast<int*>(p + 0xe85) = 0;
    *reinterpret_cast<int*>(p + 0xe89) = 0;
    p[0xe99] = 0;
    *reinterpret_cast<int*>(p + 0x71) = 0;
    p[0xeb2] = 0;
    p[0xeb4] = 0;
    p[0xeb8] = 0;
    p[0xeb7] = 0;
    memset(p + 0xeb9, 0, 6);
    p[0xebf] = 0;
    memset(p + 0xec0, 0, 3);
    p[0xec3] = 0;
    *reinterpret_cast<int*>(p + 0xec4) = -1;
    *reinterpret_cast<int*>(p + 0xec8) = 400000000;
    m_bloodBestRecords.clear();                              // +0xecc
    p[0xee4] = 0;
    *reinterpret_cast<int*>(p + 0xee5) = 0;
    *reinterpret_cast<short*>(p + 0x4f) = 0;
    p[0xee9] = 0;
    p[0xeea] = 0;
    *reinterpret_cast<short*>(p + 0xeeb) = 0;
    memset(p + 0xeed, 0, 0x199);
    memset(p + 0x55, 0, 0x10);                               // m_reliablePerson
    *reinterpret_cast<short*>(p + 0x121f) = 0;
    *reinterpret_cast<int*>(p + 0x1221) = 0;
    *reinterpret_cast<int*>(p + 0x1225) = 0;
    *reinterpret_cast<short*>(p + 0x1229) = 0;
    p[0x122b] = 0;
    *reinterpret_cast<short*>(p + 0x122c) = 0;
    *reinterpret_cast<int*>(p + 0x122e) = 0;
    *reinterpret_cast<short*>(p + 0x1232) = 0;
    *reinterpret_cast<short*>(p + 0x1234) = 0;
    *reinterpret_cast<short*>(p + 0x1236) = 0;
    p[0x1238] = 0;
    p[0x1239] = 0xff;
    *reinterpret_cast<short*>(p + 0x123a) = 0;
    *reinterpret_cast<short*>(p + 0x123c) = 0;
    p[0x123e] = 0;
    p[0x1241] = 1;
    p[0x1242] = 0;
    p[0x1243] = 0;
    p[0x1244] = 0;
    p[0x1245] = 0xff;
    p[0x1246] = 0;                                           // m_characLinkType
    *reinterpret_cast<int*>(p + 0x1247) = 0;
    *reinterpret_cast<int*>(p + 0x124b) = 0;
    p[0x124f] = 0;                                           // m_linkBonusSetting
    *reinterpret_cast<int*>(p + 0x1250) = 0;
    m_skillCommandsMap.clear();                              // +0x1254
    *reinterpret_cast<short*>(p + 0x126c) = 0;               // m_field126c
    p[0x126e] = 0;
    *reinterpret_cast<int*>(p + 0x126f) = 0;
    p[0x1273] = 0;
    p[0x1274] = 0;
    p[0x1275] = 0;
    p[0x1276] = 0;
    p[0x1277] = 0;
    p[0x1278] = 0;
    p[0x1279] = 0;
    p[0x127a] = 0;
    reinterpret_cast<tagCharacVisibleFlag*>(p + 0xe40)->reset();
    p[0x127b] = 0;
    *reinterpret_cast<int*>(p + 0x127c) = 0;
    p[0x1280] = 0;
    reinterpret_cast<stAuraAvatarOption*>(p + 0x1281)->reset();
    *reinterpret_cast<int*>(p + 0x1291) = 0;
    *reinterpret_cast<int*>(p + 0x128d) = 0;
    *reinterpret_cast<int*>(p + 0x1295) = 0;
    *reinterpret_cast<int*>(p + 0x1299) = 0;
    memset(p + 0x129d, 0, 8);
    memset(p + 0x12a5, 0, 8);
    *reinterpret_cast<int*>(p + 0x12ad) = 0;
    *reinterpret_cast<int*>(p + 0x12b1) = 0;
    *reinterpret_cast<short*>(p + 0x12b5) = 0;
    memset(p + 0x12b7, 0, 0x200);
    p[0x14b7] = 0;                                           // m_field14b7
    *reinterpret_cast<int*>(p + 0x14b8) = 0;                 // m_field14b8
    p[0x14bc] = 0;                                           // m_pad14bc[0]
    p[0x14bd] = 0;
    *reinterpret_cast<int*>(p + 0x14be) = 0;
    *reinterpret_cast<int*>(p + 0x14c2) = 0;
    *reinterpret_cast<int*>(p + 0x14c6) = 0;                 // m_viewState
    *reinterpret_cast<short*>(p + 0x14ca) = 0;
    *reinterpret_cast<short*>(p + 0x14cc) = 0;
    p[0x14ce] = 0;
    p[0x14cf] = 0;
    p[0x14d0] = 0;
}

// ---------------------------------------------------------------------------
// CUser::SendMultiMail(bool certified) T 0x0868a2dc sz0x1d9
// 报告：docs/class_func_reports/CUser/SendMultiMail.md
// 0x8e246 MSG_MULTI_MAILBOX_SEND 块（m_pad8e101+0x145 视图）：name@+0x11、
// money@+0x2f、count@+0x33、条目 stride 0xb@+0x37（itemId 首字段）。
// ---------------------------------------------------------------------------
void CUser::SendMultiMail(bool certified)
{
    char* block = m_pad8e101 + 0x145;                        // +0x8e246
    if (certified) {
        MSG_CHECK_MAIL_RECEIVER check;
        check.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_2>(
            *reinterpret_cast<MSG_MULTI_MAILBOX_SEND*>(block));
        int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(this, block + 0x11, &check);
        if (rc == 0) {
            bool fatigueGift = false;
            int count = static_cast<int>(static_cast<unsigned char>(block[0x33]));
            for (int i = 0; i < count; ++i) {
                int itemId = *reinterpret_cast<int*>(block + 0x37 + i * 0xb);
                CItem* item = G_CDataManager()->find_item(itemId);
                if (item != 0) {
                    bool skip;
                    if (item->IsAvatarItem()) {
                        skip = false;
                    }
                    else if (item->get_rarity() > 1) {
                        skip = false;
                    }
                    else {
                        skip = true;
                    }
                    if (skip) {
                        fatigueGift = true;
                    }
                }
                if (itemId == 0) {
                    fatigueGift = true;
                }
                IncrementTradeGold(*reinterpret_cast<unsigned int*>(block + 0x2f),
                                   static_cast<int>(fatigueGift));
            }
        }
        else {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, static_cast<unsigned char>(rc));
        }
        memset(block, 0, 0x1a6);
    }
    else {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x87);
        memset(block, 0, 0x1a6);
    }
}

// ---------------------------------------------------------------------------
// CUser::_onCreateCharacter(unsigned int characNo, char* name, char job)
// T 0x0864a432 sz0x178 —— 报告：docs/class_func_reports/CUser/_onCreateCharacter.md
// job==4 且 curSec<0x4936f351：create_item(3,item,1) 造 0x1d19 蛋 →
// 系统邮件（game_server_msg_56/57）→ _processUnlimitChallenge。
// ---------------------------------------------------------------------------
void CUser::_onCreateCharacter(unsigned int characNo, char* name, char job)
{
    (void)name;
    if (job == 4) {
        unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
        if (curSec < 0x4936f351u) {
            Inven_Item item;
            item.m_addInfo = 0x1d19;
            G_CDataManager()->getItemList()->create_item(
                (ENUM_ITEM_CREATE_TYPE)3, item, 1);
            if (item.m_addInfo == -1) {
                return;
            }
            char title[0x15];
            char body[0x100];
            const char* s56 = g_scriptStringManager_.findString(4, "game_server_msg_56", 0);
            strncpy(title, s56, 0x14);
            const char* s57 = g_scriptStringManager_.findString(4, "game_server_msg_57", 0);
            strncpy(body, s57, 0xff);
            int bodyLen = static_cast<int>(strlen(body));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, 0, characNo, body, bodyLen, 0,
                (ENUM_SERVER_GROUP)0, false, false);
        }
    }
    _processUnlimitChallenge(characNo, job);
}

// ===========================================================================
// LinkE2 区（2026-08-28）
//  1. tagCharacVisibleFlag ctor/reset（ORIG W 0x08695c60 / 0x08695c6c，各 movb $0）
//  2. _Charac_info::_Charac_info()（ORIG W 0x08695cf8 sz0x1f7）—— objdump 逐指令
//     转写：按 ORIG call 序对 +0x29..+0x14ca 各子对象构造（CUser.h 中为 pad
//     视图区，故 reinterpret_cast + placement new 显式调用），尾调 Reset()
//     （T 0x086822b8，定义于本 TU）。
//  3. _Charac_info::~_Charac_info()（ORIG W 0x08697982 sz0x129）—— ORIG 仅显式
//     析构 7 个非平凡子对象（其余平凡，ORIG 未调用），逆序：
//     map@0x1254 → map@0xecc → vec@0xea6 → vec@0xe9a → CCargo@0xdaa →
//     SkillSlot@0x892 → CInventory@0xda。
//  4. SIG_LOAD_PREMIUM_LETHE 完整建模（此前全树仅前向声明）：ctor W 0x0848ff7c
//     为 203×2B _Mastered_skill 数组循环构造（0x196 + 3 pad = 0x199；+0x1086 处
//     第二实例恰抵 +0x121f，与 CUser.h pad 区边界互证）。
// ===========================================================================

// ---- SIG_LOAD_ETC ctor（ORIG W 0x08491f86；子对象区按 ORIG call 序
//      placement new 显式构造；具名成员 m_tagEtc/m_pvpResult/m_records/
//      m_attendanceEvent 由编译器自动构造）----
SIG_LOAD_ETC::SIG_LOAD_ETC()
{
    char* p = reinterpret_cast<char*>(this);
    new (p + 0x2308) SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO();  // 0x848f8c8
    new (p + 0x24a0) SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO();  // 0x848f8c8
    new (p + 0x29fd) SIG_LOAD_PREMIUM_LETHE();                  // 0x848ff7c
    new (p + 0x2b97) SIG_LOAD_PREMIUM_LETHE();                  // 0x848ff7c
    for (int i = 0; i < 0x31; ++i)
        new (p + 0x3e70 + i * 0xc) stNPCRelationship_t();       // 0x848fa50
    new (p + 0x40ce) SIG_REDEEMINFO_LIST_TO_DB();               // 0x848f894
    new (p + 0x466a) stTitleBook();                             // 0x848f62a
    new (p + 0xb1a4) stAchievement_DBData();                    // 0x828d8d8
    new (p + 0xc9af) SIG_SAVE_PC_ROOM_PLAY_TIME();              // 0x848fb64
    new (p + 0xc9bc) std::bitset<32>();                         // 0x84954de
    new (p + 0xc9c0) SIG_LOAD_SKILL();                          // 0x8491ee0
    new (p + 0xcdd5) SIG_REQUEST_PVP_MISSION_LIST();            // 0x848f8d4
    new (p + 0xd5a7) SIG_QUEST_SHOP_DATA();                     // 0x848f92e
    new (p + 0xd6fc) SIG_ITEM_MAKING_SKILL();                   // 0x848fa82
    new (p + 0xd718) SIG_REQUEST_TOD_USER_STATE();              // 0x8491f68
    new (p + 0xd73c) SIG_ITEM_DICTIONARY_FLAG();                // 0x848fab2
    new (p + 0xeac8) CharacterOption();                         // 0x822abec
    new (p + 0xecc8) SIG_BOOSTER_GAGE_DATA();                   // 0x848fb4c
    new (p + 0xecd0) SIG_BEST_CLEAR_TIME();                     // 0x848fb02
    new (p + 0x14aaa) SIG_COMBO_SKILL();                        // 0x848fbbe
    new (p + 0x14b5c) seriaRoom_AniDeco();                      // 0x822ee18
}

CharacterOption::CharacterOption()
{
    memset(this, -1, 0x200);     // ORIG 0x0822abec：memset(this,0xffffffff,0x200)
}

// ORIG 0x08695ca4 _ZN19ExpBonusPointResultC1Ev/C2Ev：sub $0x18 后 call clear。
ExpBonusPointResult::ExpBonusPointResult()
{
    clear();
}

// ORIG 0x08695cb8 _ZN19ExpBonusPointResult5clearEv：+0x00/+0x04 两 int 清零。
void ExpBonusPointResult::clear()
{
    m_base = 0;
    m_add = 0;
}

tagCharacVisibleFlag::tagCharacVisibleFlag()
{
    *reinterpret_cast<unsigned char*>(this) = 0;   // ORIG 0x08695c60：movb $0,(%eax)
}

void tagCharacVisibleFlag::reset()
{
    *reinterpret_cast<unsigned char*>(this) = 0;   // ORIG 0x08695c6c：movb $0,(%eax)
}

// ---- _Charac_info ctor/dtor（LinkE2）----
_Charac_info::_Charac_info()
{
    char* p = reinterpret_cast<char*>(this);
    new (p + 0x29) STGrowthType();                        // call 0x8486bce
    new (p + 0x2a) STGrowthType();                        // call 0x8486bce
    new (p + 0x88) _Additioal_info();                     // call 0x8348254（inline memset 0x52 同语义）
    new (p + 0x0da) CInventory();                         // call 0x822d52c
    new (p + 0x892) SkillSlot();                          // call 0x8694d6a
    new (p + 0xdaa) CCargo();                             // call 0x86949da
    new (p + 0xe40) tagCharacVisibleFlag();               // call 0x8695c60
    new (p + 0xe8d) STPowerWarCharacData();               // call 0x86959dc
    new (p + 0xe9a) std::vector<ContinuousItemInfo>();    // call 0x86980de
    new (p + 0xea6) std::vector<ContinuousItemInfo>();    // call 0x86980de
    new (&m_bloodBestRecords) std::map<int, stBloodBestRecord>();   // +0xecc call 0x8698150
    new (p + 0xeed) SIG_LOAD_PREMIUM_LETHE();             // call 0x848ff7c
    new (p + 0x1086) SIG_LOAD_PREMIUM_LETHE();            // call 0x848ff7c
    new (&m_skillCommandsMap) std::map<int, STSkillCommandData>();  // +0x1254 call 0x86981ca
    new (p + 0x129d) WpBonusPointResult();                // call 0x8695c78
    new (p + 0x12a5) ExpBonusPointResult();               // call 0x8695ca4
    new (p + 0x12b7) CharacterOption();                   // call 0x822abec
    new (p + 0x14ca) seriaRoom_AniDeco();                 // call 0x822ee18
    Reset();                                              // call 0x86822b8
}

_Charac_info::~_Charac_info()
{
    typedef std::map<int, STSkillCommandData> SkillCommandMap;
    typedef std::map<int, stBloodBestRecord> BloodBestMap;
    char* p = reinterpret_cast<char*>(this);
    reinterpret_cast<SkillCommandMap*>(p + 0x1254)->~SkillCommandMap();   // call 0x8695ce4
    reinterpret_cast<BloodBestMap*>(p + 0xecc)->~BloodBestMap();          // call 0x8695cd0
    reinterpret_cast<std::vector<ContinuousItemInfo>*>(p + 0xea6)->~vector();   // call 0x86980f2
    reinterpret_cast<std::vector<ContinuousItemInfo>*>(p + 0xe9a)->~vector();   // call 0x86980f2
    reinterpret_cast<CCargo*>(p + 0xdaa)->~CCargo();                      // call 0x86949f8
    reinterpret_cast<SkillSlot*>(p + 0x892)->~SkillSlot();                // call 0x8694e80
    reinterpret_cast<CInventory*>(p + 0xda)->~CInventory();               // call 0x822d600
}
// ORIG 0x08699863：编译器生成的拷贝构造。平凡区（含 _Additioal_info@0x88、
// CCargo@0xdaa 等无堆成员子对象）逐字段拷贝；非平凡子对象调用拷贝构造。
_Charac_info::_Charac_info(const _Charac_info& other)
{
    typedef std::map<int, stBloodBestRecord> BloodBestMap;
    typedef std::map<int, STSkillCommandData> SkillCommandMap;
    char* p = reinterpret_cast<char*>(this);
    const char* o = reinterpret_cast<const char*>(&other);
    memcpy(p, o, 0xda);                                                // +0x00..+0xd9 平凡区
    new (p + 0xda) CInventory(*reinterpret_cast<const CInventory*>(o + 0xda));    // call 0x8699476
    new (p + 0x892) SkillSlot(*reinterpret_cast<const SkillSlot*>(o + 0x892));    // call 0x86996f6
    memcpy(p + 0xdaa, o + 0xdaa, 0xe9a - 0xdaa);                       // CCargo 等平凡区
    new (p + 0xe9a) std::vector<ContinuousItemInfo>(
        *reinterpret_cast<const std::vector<ContinuousItemInfo>*>(o + 0xe9a));    // call 0x869f61c
    new (p + 0xea6) std::vector<ContinuousItemInfo>(
        *reinterpret_cast<const std::vector<ContinuousItemInfo>*>(o + 0xea6));    // call 0x869f61c
    new (reinterpret_cast<BloodBestMap*>(p + 0xecc))
        BloodBestMap(*reinterpret_cast<const BloodBestMap*>(o + 0xecc));          // call 0x869f6d0
    memcpy(p + 0xeed, o + 0xeed, 0x1254 - 0xeed);                       // +0xeed..+0x1253 平凡区
    new (reinterpret_cast<SkillCommandMap*>(p + 0x1254))
        SkillCommandMap(*reinterpret_cast<const SkillCommandMap*>(o + 0x1254));   // call 0x869f6ea
    memcpy(p + 0x129d, o + 0x129d, 0x14d1 - 0x129d);                    // 尾部平凡区
}

// ORIG 0x0869da62：编译器生成的赋值。顺序：平凡区（+0x88 区 0x52 memcpy）→
// CInventory/SkillSlot 赋值 → +0xdaa 区 0x11 memcpy → 双 vector/双 map 赋值。
_Charac_info& _Charac_info::operator=(const _Charac_info& other)
{
    typedef std::map<int, stBloodBestRecord> BloodBestMap;
    typedef std::map<int, STSkillCommandData> SkillCommandMap;
    char* p = reinterpret_cast<char*>(this);
    const char* o = reinterpret_cast<const char*>(&other);
    if (p == o)
        return *this;
    memcpy(p, o, 0x88);                                                // +0x00..+0x87 平凡区
    memcpy(p + 0x88, o + 0x88, 0x52);                                  // _Additioal_info
    *reinterpret_cast<CInventory*>(p + 0xda) =
        *reinterpret_cast<const CInventory*>(o + 0xda);                // call 0x869d73c
    *reinterpret_cast<SkillSlot*>(p + 0x892) =
        *reinterpret_cast<const SkillSlot*>(o + 0x892);                // call 0x869d91e
    memcpy(p + 0xdaa, o + 0xdaa, 0x11);                                // CCargo 平凡区
    *reinterpret_cast<std::vector<ContinuousItemInfo>*>(p + 0xe9a) =
        *reinterpret_cast<const std::vector<ContinuousItemInfo>*>(o + 0xe9a);     // call 0x86a32b0
    *reinterpret_cast<std::vector<ContinuousItemInfo>*>(p + 0xea6) =
        *reinterpret_cast<const std::vector<ContinuousItemInfo>*>(o + 0xea6);     // call 0x86a32b0
    *reinterpret_cast<BloodBestMap*>(p + 0xecc) =
        *reinterpret_cast<const BloodBestMap*>(o + 0xecc);             // call 0x86a34fe
    *reinterpret_cast<SkillCommandMap*>(p + 0x1254) =
        *reinterpret_cast<const SkillCommandMap*>(o + 0x1254);         // call 0x86a351c
    memcpy(p + 0x129d, o + 0x129d, 0x14d1 - 0x129d);                   // 尾部平凡区
    return *this;
}

// ============================================================================
// CUser::giveCharacLinkBonusExp(unsigned int bonusExp)
// ORIG T _ZN5CUser21giveCharacLinkBonusExpEj @0x08652564 (0x34f)。
// 报告 docs/class_func_reports/CUser/giveCharacLinkBonusExp.md 全文（汇编+反编译 C）已读。
// 流程：记录 SP → calcHelpAbuseRatio → gain_exp_sp(reason=3) → 升级广播 basic info →
//       链接角色名 0x123 包 → SendNotiPacket(1,2,1) → send_skill_info。
// ============================================================================
void CUser::giveCharacLinkBonusExp(unsigned int bonusExp)
{
    PacketGuard packet;

    int sfp = 0;                                       // local_30[0]（sfpRef）
    int sp = 0;                                        // local_30[1]（spRef）
    const int skillTreeIdx = GetCurCharacSkillTreeIndex();
    SkillSlot* skillR = (SkillSlot*)getCurCharacSkillR();
    const int spBefore = skillR->get_remain_sp_at_index(
        (ENUM_SKILL_TREE_KIND)skillTreeIdx);
    calcHelpAbuseRatio(bonusExp, 0);                   // SUB41(param_1,0) → bool false
    const char gained = gain_exp_sp(
        (int)bonusExp, sp, sfp, (CUser::eExpAddReason)3, 0, true);

    SkillSlot* skillR2 = (SkillSlot*)getCurCharacSkillR();   // P48 二次独立调用
    const int spAfter = skillR2->get_remain_sp_at_index(
        (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());

    if (gained != '\0')
    {
        packet.clear();                                // InterfacePacketBuf::clear
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        make_basic_info((char*)&packet, '\0');
        packet.finalize(true);
        G_GameWorld()->send_all(packet, this);         // send_all 带 excepted user
    }

    char linkName[0x1e];                               // local_4e
    memset(linkName, 0, sizeof(linkName));
    const int linkSlot = (int)getLinkCharacSlotIndex();
    if (linkSlot >= 0 &&
        (int)m_characList.size() > linkSlot)
    {
        strcpy(linkName, (const char*)(reinterpret_cast<char*>(&m_characList[linkSlot]) + 4));
    }

    packet.clear();
    packet.put_header(0, 0x123);
    packet.put_int((int)strlen(linkName));
    packet.put_str(linkName, (int)strlen(linkName));
    packet.put_int(0);
    packet.put_int((int)bonusExp);
    packet.put_short((short)(spAfter - spBefore));
    packet.put_short((short)sfp);
    packet.finalize(true);
    Send(packet);

    SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    send_skill_info();
}

// ============================================================================
// CUser::quest_action(int cmd, int questId, int triggerVal, int opt)
// ORIG T _ZN5CUser12quest_actionEiiii @0x0866da8a。
// 报告 docs/class_func_reports/CUser/quest_action.md 全文已读；草稿（本文件
// quest_action W7-BLOCKED 注释区）按报告全文转写，本定义为其真实落地。
// cmd：0x21 accept / 0x22 giveup / 0x23 trigger / 0x24 finish(reward)。
// ============================================================================
void CUser::quest_action(int cmd, int questId, int triggerVal, int opt)
{
    Quest* q = G_CDataManager()->find_quest(questId);
    if (q == 0)
        return;
    if (q->m_type == 7)
        return;

    PacketGuard packet;
    packet.put_header(1, cmd);

    if (cmd == 0x22)                                            // giveup
    {
        getCurCharacQuestW()->giveup_quest(questId);
        packet.put_byte(1);
        packet.put_short(questId);
        Quest* q2 = G_CDataManager()->find_quest(questId);
        if (q2 != 0)
        {
            if (!q2->m_triggerDataPairs.empty())
            {
                bool touched = false;
                std::vector<int> updates;
                updates.reserve(q2->m_triggerDataPairs.size() * 3);
                getCurCharacInvenW()->delete_event_items(
                    q2->m_triggerDataPairs, updates, touched,
                    (eMoneySubReason)4, (eItemDelReason)4, false);
                std::vector<int>::const_iterator ui = updates.begin();
                while (ui != updates.end())
                {
                    const int spId = *ui; ++ui;
                    const int cntS = *ui; ++ui;
                    SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)spId, cntS);
                    ++ui;                                       // 第三元素只读丢弃
                }
            }
            SendHasNoGoWithApcQuestInParty(q2);
        }
        packet.finalize(true);
        Send(packet);
    }
    else if (cmd == 0x21)                                       // accept
    {
        Quest* qa = G_CDataManager()->find_quest(questId);
        const bool revengeBlock =
            (qa != 0) && (qa->m_field134 == 1) &&
            !GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon();
        const bool scheduleBlock =
            (qa != 0) && (qa->m_field134 == 2) &&
            !qa->IsOpenScheduleQuest();
        if (revengeBlock || scheduleBlock)
        {
            packet.put_byte(0);
            packet.put_byte(0x15);
            packet.finalize(true);
            Send(packet);
        }
        else
        {
            int local42 = 0;
            const int errCode =
                getCurCharacQuestW()->accept_quest(
                    questId, (char*)&packet, local42);
            if (errCode > 0)
            {
                packet.put_byte(0);
                packet.put_byte(errCode);
            }
            packet.finalize(true);
            Send(packet);
        }
    }
    else if (cmd == 0x23)                                       // trigger
    {
        const char b = (char)opt;
        const int trigResult =
            getCurCharacQuestW()->set_trigger(
                questId, (char)triggerVal, b != 0);
        if (trigResult == -1)
        {
            packet.put_byte(0);
            packet.put_byte(0x16);
        }
        else
        {
            packet.put_byte(1);
            packet.put_short(questId);
            packet.put_int(trigResult);
            if (trigResult == 0)
            {
                Quest* qt = G_CDataManager()->find_quest(questId);
                if (qt != 0)
                    SendHasNoGoWithApcQuestInParty(qt);
            }
        }
        packet.finalize(true);
        Send(packet);
    }
    else if (cmd == 0x24)                                       // finish(reward)
    {
        const unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
        if ((now - m_postChecksumData.m_field48) <= 0x3e8)      // 0x79644 近期已请求
        {
            ++m_postChecksumData.m_field4c;                     // 0x79648
            if (1 < m_postChecksumData.m_field4c)
            {
                WongWork::CHackAnalyzer* ha = getHackAnalyzer();
                ha->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x192,
                                     1, 0, 0);
                packet.put_byte(0);
                packet.put_byte(1);
                packet.finalize(true);
                Send(packet);
                return;
            }
        }
        else
        {
            m_postChecksumData.m_field4c = 0;
            m_postChecksumData.m_field48 =
                GlobalData::s_systemTime_.getCurTickCount();
        }

        const int err = quest_reward(questId, triggerVal, opt);
        if (err > 0)
        {
            packet.put_byte(0);
            packet.put_byte(err);
            packet.finalize(true);
            Send(packet);
            HistoryLog::WriteFineQuest((FILE*)m_field796f8, questId);
        }
        else
        {
            std::list<int> mailQuestIds;
            stSelectQuestParam selParam(this);
            const bool hasMailQ = getCurCharacQuestR()
                                      ->get_mail_quest_info(mailQuestIds,
                                                            selParam);
            if (hasMailQ)
            {
                for (std::list<int>::const_iterator mi = mailQuestIds.begin();
                     mi != mailQuestIds.end(); ++mi)
                {
                    Quest* mq = G_CDataManager()->find_quest(*mi);
                    if (mq != 0 && mq->m_mailFlag == 1 && mq->m_index == questId)
                        _postQuestTriggerItem(mq);
                }
            }
            getCurCharacQuestW()->finish_quest(questId);
            {
                PacketGuard qi;
                getCurCharacQuestR()->get_quest_info((char*)&qi);
                Send(qi);
            }
            Quest* qz = G_CDataManager()->find_quest(questId);
            if (qz != 0 && qz->m_type == 3)
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    this, (APSystem::_ActionGroupIndex)0xe, 0,
                    (ENUM_PACKETCLASS)0);
            m_historyLog.QuestComplete(questId);
            _onQuestClear(questId, opt);
        }
    }
}

// ============================================================================
// CUser::SetCharacInfo(int count, char* data)   ORIG T 0x08649c6a。
// 报告 docs/class_func_reports/CUser/SetCharacInfo.md 全文（汇编+反编译 C）已读。
// 逐记录（stride 0x94）填充栈上 _Charac_info（Reset→characNo/name/字段区/
// 角色快照表/建号时间/删除截止/禁玩与视图标记→CInventory/CCargo 绑共享
// InventoryMemory）后 push_back 至 m_characList(+0x796e8)。avatar 上报分支
// 依赖 scratch(local_748) 计数——ORIG 中该缓冲仅 memset 未写入，分支恒假，
// 按 ORIG 原样保留（Ghidra 同口径死分支）。
// ============================================================================
void CUser::SetCharacInfo(int count, char* data)
{
    char scratch[0x70c];
    memset(scratch, 0, 0x70c);                         // local_748：恒零载荷（ORIG 同）
    _Charac_info info;
    const int curSec = GlobalData::s_systemTime_.getCurSec();

    for (int i = 0; i < count; ++i)
    {
        char* rec = data + i * 0x94;
        info.Reset();                                  // _Charac_info::Reset
        char* p = reinterpret_cast<char*>(&info);
        *reinterpret_cast<int*>(p) = *reinterpret_cast<int*>(rec);      // characNo
        strncpy(reinterpret_cast<char*>(p + 4), rec + 4, 0x1e);         // m_name
        p[0x18] = rec[0x18];
        p[0x19] = rec[0x19];
        *reinterpret_cast<short*>(p + 0x1a) =
            *reinterpret_cast<short*>(rec + 0x1a);

        for (int k = 0; k < 0xc; ++k)                  // 背包槽镜像表（0x3d stride）
        {
            *reinterpret_cast<int*>(p + 0xf8 + k * 0x3d) =
                *reinterpret_cast<int*>(rec + (k + 4) * 8 + 8);
        }
        for (int k = 0; k < 0xc; ++k)                  // 外观槽数组（8 字节槽，info+0x75c）
        {
            *reinterpret_cast<int*>(p + 0x75e + k * 8) =
                *reinterpret_cast<int*>(rec + (k + 4) * 8 + 8);
            *reinterpret_cast<short*>(p + 0x762 + k * 8) =
                *reinterpret_cast<short*>(rec + (k + 4) * 8 + 0xc);
            *reinterpret_cast<short*>(p + 0x764 + k * 8) =
                *reinterpret_cast<short*>(rec + (k + 4) * 8 + 0xe);
        }

        // growType：==1 或 ==0xff 原样，否则 0（→ info+0x14c6）
        const unsigned char grow =
            static_cast<unsigned char>(rec[0x22]);
        *reinterpret_cast<int*>(p + 0x14c6) =
            (grow == 1 || grow == static_cast<unsigned char>(-1))
                ? static_cast<int>(grow) : 0;

        reinterpret_cast<CommonTime*>(p + 0x39)
            ->SetCreateTime(*reinterpret_cast<long*>(rec + 0x88));

        const char expireFlag = (char)(curSec <
            *reinterpret_cast<int*>(rec + 0x90));
        p[0xe66] = expireFlag;                                        // local_db3
        *reinterpret_cast<int*>(p + 0x12ad) =
            *reinterpret_cast<int*>(rec + 0x90);                      // 删除截止
        *reinterpret_cast<int*>(p + 0xe5d) =
            *reinterpret_cast<int*>(rec + 0x8c);                      // local_dbc
        p[0x1240] = static_cast<char>((rec[0x1c] & 1) != 0);          // 禁玩标记
        p[0x14b7] = rec[0x1d];                                        // powerSide
        *reinterpret_cast<int*>(p + 0x14b8) =
            *reinterpret_cast<int*>(rec + 0x1e);                      // wpBonus

        {   // local_dd9：&0xfd | 1 | bit*2（rec[0x1c] bit4）
            const unsigned char bit =
                static_cast<unsigned char>(
                    ((static_cast<unsigned char>(rec[0x1c]) >> 4) << 7) >> 7);
            unsigned char& dst =
                *reinterpret_cast<unsigned char*>(p + 0xe40);
            dst = static_cast<unsigned char>((dst & 0xfd) | 1 | (bit * 2));
        }
        p[0x1c04] = expireFlag;                                       // local_15

        reinterpret_cast<CInventory*>(p + 0xda)
            ->SetInventoryMemory(&m_inventoryMemory);                 // this+0x79794
        reinterpret_cast<CCargo*>(p + 0xdaa)
            ->SetInventoryMemory(&m_inventoryMemory);
        m_characList.push_back(info);
    }

    if (*reinterpret_cast<int*>(scratch) > 0)          // ORIG 恒假死分支（原样保留）
    {
        if (*reinterpret_cast<int*>(scratch) < 0x72)
        {
            Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0xcb3);
            CStreamGuard guard(s, true);
            guard << 0x33;
            guard << (int)GetUID();
            guard->put_binary(scratch, 0x70c);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        }
        else
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0xcaf, 5);
            t("SetCharacInfo AVATAR INFO PROBLEM items<%d> user_no<%s> characnum<%d>",
              *reinterpret_cast<int*>(scratch),
              NumberToString(get_acc_id(), 0), count);
        }
    }
}

// ============================================================================
// CUser::UseQuestItem(int itemIdx)  ORIG T 0x08668d1a (0x16e4)。
// 报告 docs/class_func_reports/CUser/UseQuestItem.md（2005 行，汇编+反编译 C
// 全文已读）。觉醒/转职道具链：state==3 门控→交易中拒绝(0x20/0x13)→
// GetInvenSlot(1,itemIdx) 取当前道具并按 ORIG 二叉判定树线性化为
// kUseQuestItemMap（grow/subType/keepSkill 三元组）→ grow 边界与职业守卫
// （失败发 0x20/0x11 错误包）→ 经验钳制到等级区间→扫描槽 3..8 与 0x99..0xc8
// 收集可删道具→0x12 包→CSkillChanger/CCalcCharacStat 技能与属性重算→
// set_grow_type/_postCheckForceChangeGrowType/双 SendNotiPacket。
// ============================================================================
void CUser::UseQuestItem(int itemIdx)
{
    if (get_state() != 3)
        return;

    PacketGuard packet;
    if (CheckInTrade())
    {
        packet.clear();
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
        return;
    }

    const Inven_Item cur = getCurCharacInvenR()->GetInvenSlot(1, itemIdx);
    const unsigned int curIdx = (unsigned int)cur.m_addInfo;   // local_9b

    char grow = 0;                    // local_51：目标职业档位
    int  subType = 0;                 // local_4c：二转/子类
    int  keepSkill = 0;               // local_50：保留技能包标志
    char doChangeGrow = 0;            // local_45

    struct MapEntry { unsigned int lo, hi; int g, s, k; };
    static const MapEntry kUseQuestItemMap[] = {
        {0xfc9,0xfc9,0,1,0},{0xfca,0xfca,0,2,0},{0xfcb,0xfcb,0,3,0},{0xfcc,0xfcc,0,4,0},
        {0xfcd,0xfcd,1,2,0},{0xfce,0xfce,1,1,0},{0xfcf,0xfcf,1,3,0},{0xfd0,0xfd0,1,4,0},
        {0xfd1,0xfd1,2,1,0},{0xfd2,0xfd2,2,3,0},{0xfd3,0xfd3,2,2,0},{0xfd4,0xfd4,2,4,0},
        {0xfef,0xfef,3,1,0},{0xff0,0xff0,3,3,0},{0xff1,0xff1,3,2,0},{0xff2,0xff2,3,4,0},
        {0xff6,0xff6,0,4,1},{0xff7,0xff7,0,1,1},{0xff8,0xff8,0,2,1},
        {0xffb,0xffb,1,2,1},{0xffc,0xffc,1,1,1},{0xffd,0xffd,1,3,1},{0xffe,0xffe,1,4,1},
        {0xfff,0xfff,2,1,1},{0x1000,0x1000,2,3,1},{0x1001,0x1001,2,2,1},{0x1002,0x1002,2,4,1},
        {0x1003,0x1003,3,1,1},{0x1004,0x1004,3,3,1},{0x1005,0x1005,3,2,1},{0x1006,0x1006,3,4,1},
        {0x1044,0x1044,4,2,0},{0x1045,0x1045,4,1,0},{0x1046,0x1046,4,3,0},
        {0x1047,0x1047,4,3,0},{0x1048,0x1048,4,4,0},
        {0x105b,0x105b,1,4,1},{0x105c,0x105c,1,1,1},{0x105d,0x105d,1,2,1},{0x105e,0x105e,1,3,1},
        {0x105f,0x105f,1,4,1},{0x1060,0x1060,2,1,1},{0x1061,0x1061,2,2,1},{0x1062,0x1062,2,3,1},
        {0x1063,0x1063,2,4,1},{0x1064,0x1064,3,1,1},{0x1065,0x1065,3,2,1},
        {0x1079,0x1079,3,3,1},{0x107a,0x107a,3,4,1},{0x107b,0x107b,0,1,1},{0x107c,0x107c,0,2,1},
        {0x107d,0x107d,0,3,1},
        {0x1f1c,0x1f1c,3,4,1},{0x1f1d,0x1f1d,4,1,1},{0x1f1e,0x1f1e,4,2,1},{0x1f1f,0x1f1f,4,3,1},
        {0x1f20,0x1f20,5,1,0},{0x1f21,0x1f21,5,3,1},
        {0x1f22,0x1f22,5,2,0},{0x1f23,0x1f23,5,4,0},{0x1f24,0x1f24,5,1,1},
        {0x1f25,0x1f25,5,3,1},{0x1f26,0x1f26,5,2,1},{0x1f27,0x1f27,5,4,1},
        {0x1f28,0x1f28,6,1,0},{0x1f29,0x1f29,6,2,0},
        {0x1f2a,0x1f2a,6,1,1},{0x1f2b,0x1f2b,6,2,1},
        {0x1f50,0x1f50,7,1,0},{0x1f51,0x1f51,7,3,0},{0x1f52,0x1f52,7,2,0},
        {0x1f53,0x1f53,7,4,0},{0x1f54,0x1f54,7,1,1},{0x1f55,0x1f55,7,3,1},
        {0x1f56,0x1f56,7,2,1},{0x1f57,0x1f57,7,4,1},
    };
    const MapEntry* hit = 0;
    for (unsigned int mi = 0; mi < sizeof(kUseQuestItemMap) / sizeof(kUseQuestItemMap[0]);
         ++mi)
    {
        if (kUseQuestItemMap[mi].lo <= curIdx && curIdx <= kUseQuestItemMap[mi].hi)
        {
            hit = &kUseQuestItemMap[mi];
            break;
        }
    }
    if (hit == 0)
        return;                                        // ORIG 未知道具 → 静默返回

    grow = static_cast<char>(hit->g);
    keepSkill = hit->k;
    doChangeGrow = 1;

    const bool outsideAll =
        (curIdx < 0x105c || 0x1065 < curIdx) &&
        (curIdx < 0x1079 || 0x107d < curIdx) &&
        (curIdx < 0x1f1c || 0x1f1f < curIdx) &&
        (curIdx < 0x1f24 || 0x1f27 < curIdx) &&
        (curIdx < 0x1f2a || 0x1f2b < curIdx) &&
        (curIdx < 0x1f54 || 0x1f57 < curIdx);

    bool reject = false;
    if (!(outsideAll || getCurCharFirstGrowType() != 0))
        reject = true;
    else if (grow < 0 || grow > 10)
        reject = true;
    else if (grow != get_charac_job())
        reject = true;

    if (reject)
    {
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.finalize(true);
        Send(packet);
        return;
    }

    {   // 经验钳制到当前等级区间
        const int levelLow = G_CDataManager()->get_level_exp(get_charac_level());
        const int levelHigh = G_CDataManager()->get_level_exp(get_charac_level() + 1);
        const unsigned int exp = (unsigned int)getCurCharacExp();
        if (exp < (unsigned int)levelLow)
            setCurCharacExp(levelLow);
        else if ((unsigned int)levelHigh < exp)
            setCurCharacExp(levelHigh - 1);

        int slots[312];                                // local_68c（ORIG 0x138 个 int）
        int idxs[312];                                 // local_b6c
        memset(slots, 0, sizeof(slots));
        memset(idxs, 0, sizeof(idxs));
        int n = 0;                                     // local_3c

        for (int s = 3; s < 9; ++s)
        {
            const Inven_Item it = getCurCharacInvenR()->GetInvenSlot(1, s);
            const unsigned int id = (unsigned int)it.m_addInfo;
            const int cnt = it.m_addInfo2;             // +0x07（local_d3）
            bool match;
            if (keepSkill == 0)
            {
                match = ((id > 0xfc8 && id < 0xfd5) || (id > 0xfee && id < 0xff3) ||
                         (id > 0xff6 && id < 0x1007) || (id > 0x1044 && id < 0x1049) ||
                         (id > 0x1f1f && id < 0x1f24) || (id > 0x1f27 && id < 0x1f2a) ||
                         (id > 0x1f4f && id < 0x1f54));
            }
            else
            {
                match = (id > 0x105b && id < 0x1066 &&
                         (id < 0x1079 || id > 0x107d) &&
                         (id < 0x1f1c || id > 0x1f1f) &&
                         (id < 0x1f24 || id > 0x1f27) &&
                         (id < 0x1f2a || id > 0x1f2b) &&
                         (id < 0x1f54 || id > 0x1f57));
            }
            if (match) { slots[n] = s; idxs[n] = cnt; ++n; }
        }
        for (int s = 0x99; s < 0xc9; ++s)
        {
            const Inven_Item it = getCurCharacInvenR()->GetInvenSlot(1, s);
            const unsigned int id = (unsigned int)it.m_addInfo;
            const int cnt = it.m_addInfo2;
            bool match;
            if (keepSkill == 0)
            {
                match = ((id > 0xfc8 && id < 0xfd5) || (id > 0xfee && id < 0xff3) ||
                         (id > 0xff6 && id < 0x1007) || (id > 0x1044 && id < 0x1049) ||
                         (id > 0x1f1f && id < 0x1f24) || (id > 0x1f27 && id < 0x1f2a) ||
                         (id > 0x1f4f && id < 0x1f54));
            }
            else
            {
                match = (id > 0x105b && id < 0x1066 &&
                         (id < 0x1079 || id > 0x107d) &&
                         (id < 0x1f1c || id > 0x1f1f) &&
                         (id < 0x1f24 || id > 0x1f27) &&
                         (id < 0x1f2a || id > 0x1f2b) &&
                         (id < 0x1f54 || id > 0x1f57));
            }
            if (match) { slots[n] = s; idxs[n] = cnt; ++n; }
        }

        if (n > 0)
        {
            packet.clear();
            packet.put_header(1, 0x12);
            packet.put_byte(1);
            packet.put_byte(0);
            packet.put_byte(n);
            for (int i = 0; i < n; ++i)
            {
                getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slots[i],
                                                  idxs[i], (eItemDelReason)10,
                                                  true);
                packet.put_short(slots[i]);
                packet.put_int(idxs[i]);
            }
            packet.finalize(true);
            Send(packet);
        }

        WongWork::CSkillChanger changer;
        if (doChangeGrow == 1)
        {
            changer.CountElixer(this);
            SkillSlot* skillW = (SkillSlot*)getCurCharacSkillW();
            skillW->debugCheckGrowTypeSkill(get_charac_level(), get_charac_job(),
                                            subType, keepSkill);
        }
        else
        {
            SkillSlot* skillW = (SkillSlot*)getCurCharacSkillW();
            skillW->clear_all_skills_both();
            skillW->set_parent(this);
            addSkillOnCreateCharacter(
                reinterpret_cast<_Mastered_skill*>(reinterpret_cast<char*>(skillW) + 0x46),
                get_charac_job());
            addSkillOnCreateCharacter(
                reinterpret_cast<_Mastered_skill*>(reinterpret_cast<char*>(skillW) + 0x1de),
                get_charac_job());
            changer.SkillInitialize(this, 0, true);
        }

        WongWork::CCalcCharacStat::InitializeStat(
            reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoW()),
            get_charac_job());
        WongWork::CCalcCharacStat::CalcStat(
            reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoW()),
            get_charac_job(), subType, keepSkill, get_charac_level());

        int* needElixer = changer.GetNeedElixerCount();
        int* curElixer = changer.GetElixerCount();
        int* ability = changer.GetNeedElixerAbility();
        for (int a = 1; a <= *ability; ++a)
        {
            if (curElixer[a] < needElixer[a])
            {
                WongWork::CCalcCharacStat::ApplyElixerItem(
                    reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoW()),
                    ability[a], needElixer[a] - curElixer[a]);
            }
        }

        set_grow_type((unsigned char)(subType & 0xff),
                      (unsigned char)(keepSkill & 0xff), 0,
                      (eChangeGrowTypeReason)1);
        _postCheckForceChangeGrowType(this, subType, keepSkill);
        SendNotiPacket((eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
        send_skill_info();
        SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    }
}

// ============================================================================
// CUser::compound_item(int itemIdx, bool useRecipe2ndCheck, unsigned short
//                      count, int subCmd)
// ORIG T _ZN5CUser13compound_itemEibti @0x0867218c (0xee1)。
// 报告 docs/class_func_reports/CUser/compound_item.md（1397 行，汇编+反编译 C
// 全文已读）合成主链：_GetRecipeItem→avatar 分流→_CheckCompoundSkill→
// 材料表/结果表复制与倍乘→空间与装备条件校验→影子背包预演扣料→
// stExpertCompoundResultVariation 随机产出→expert_job 成功统计→0x帧包→
// _ProcCompoundCore→SendUpdateItem/send_equip/GainItemMakingSkillPoint/
// WriteComposition。
// ============================================================================
int CUser::compound_item(int itemIdx, bool useRecipe2ndCheck,
                         unsigned short count, int subCmd)
{
    int recipeKeyOut = 0;                                          // local_80
    CStackableItem* recipe =                                       // local_5c（ORIG -0x58 存 CItem* 返回，直接当 CStackableItem 布局访问）
        static_cast<CStackableItem*>(_GetRecipeItem(
            (unsigned long)itemIdx, useRecipe2ndCheck, recipeKeyOut));
    if (recipe == 0)
        return 0x11;

    const RecipeInfo& recipeInfo = recipe->m_recipe174;            // +0x174
    if (is_recipe_items_avatar_type(recipeInfo.m_vecC))            // +0x180（ORIG 汇编）
    {
        return process_recipe_avatar_items(itemIdx, useRecipe2ndCheck,
                                           count, subCmd, recipe);
    }
    if (!_CheckCompoundSkill(recipe, useRecipe2ndCheck))           // xor1/test → !result
        return 0x16;

    int blankSlots[11];                                            // local_124
    memset(blankSlots, 0, sizeof(blankSlots));
    int needSlots[11];                                             // local_150
    memset(needSlots, 0, sizeof(needSlots));
    int keepSlots[11];                                             // local_17c
    memset(keepSlots, 0, sizeof(keepSlots));

    int freeQuick = getCurCharacInvenR()->QuickEmptyCount();       // local_58

    std::vector<std::pair<int,int> > delList(recipeInfo.m_vec0);   // local_8c ← +0x174
    if (!delList.empty())                                          // ORIG empty()==0 → 自我倍增
        delList.insert(delList.end(), delList.begin(), delList.end());
    std::vector<std::pair<int,int> > delListScaled(delList);       // local_98
    if (subCmd == 0xf2)
        CalculateExpertJobMaterials(delListScaled);
    std::vector<std::pair<int,int> > resultMulList(recipeInfo.m_vecC);  // local_a4 ← +0x180

    _CalcItemVectorMultiply(delListScaled, (int)count);
    _CalcItemVectorMultiply(resultMulList, (int)count);
    if (_IsCompoundResult2MoreEquip(resultMulList))
        return 0x17;
    if (!getCurCharacInvenR()->CheckNeedItemList(delListScaled))   // ORIG 以 local_98 校验
        return 0x15;

    stEquipCondition condObj;                                      // local_ac
    Inven_Item condItem;                                           // local_1b9
    const int checkRet = getCurCharacInvenR()->CheckEquipmentItemCondition(
        delListScaled, condObj, condItem);                         // ORIG 以 local_98 校验
    if (checkRet != 0)
        return checkRet;

    getCurCharacInvenR()->CalBlankSlot(blankSlots, delListScaled);
    if ((!useRecipe2ndCheck) && (recipeKeyOut == 1))
        blankSlots[2] = blankSlots[2] + 1;
    getCurCharacInvenR()->CalNeedSlot(needSlots, delList);

    freeQuick = freeQuick + blankSlots[0];
    for (int si = 1; si < 0xb; ++si)
    {
        if (keepSlots[si] + blankSlots[si] < needSlots[si])
        {
            freeQuick = freeQuick + blankSlots[si] +
                        (keepSlots[si] - needSlots[si]);
            if (freeQuick < 0)
                return 4;
        }
    }

    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation variation;  // local_b4
    // getCurCharacInvenRefR 现返回 const CInventory*（ORIG 符号实证）。
    CInventory* invRef = const_cast<CInventory*>(getCurCharacInvenRefR());
    const int expertExp = GetCurCharacExpertJobExp();
    const int expertLevel = GetCurExpertJobLevel(expertExp);
    const int expertType = GetCurCharacExpertJobType();
    invRef->GetExpertJobCompoundResultVariation(
        (ENUM_EXPERT_JOB_TYPE)expertType, expertLevel, variation);

    char limitStackHit = 0;                                        // local_49
    int failCode = 0;                                              // unaff_EBX
    bool shadowOk = false;                                         // bVar2
    CInventory shadowInv;                                          // local_9b0
    InventoryMemory shadowMem;                                     // local_caef
    shadowInv.SetInventoryMemory(&shadowMem);
    shadowInv.setCopy(*const_cast<CInventory*>(getCurCharacInvenRefR()));
    if (shadowInv.tryDeleteEventItems(delListScaled))
    {
        if (useRecipe2ndCheck)
        {
            shadowOk = true;
        }
        else if (shadowInv.delete_item((INVEN_TYPE)1, itemIdx,
                                       (int)count, (eItemDelReason)8, false))
        {
            shadowOk = true;
        }
        else
        {
            failCode = 0x11;
        }

        if (shadowOk)
        {
            if (variation.m_field4 <= 0.0f)                        // local_b0
            {
                shadowOk = (shadowInv.tryInsertEventItems(resultMulList) != 0);
                if (!shadowOk)
                    failCode = 4;
            }
            else
            {
                std::vector<std::pair<int,int> > scaled(resultMulList);  // local_dc
                for (std::vector<std::pair<int,int> >::iterator it = scaled.begin();
                     it != scaled.end(); ++it)
                {
                    CItem* def = G_CDataManager()->find_item(it->first);  // local_40（ORIG Ghidra 视图 CStackableItem*）
                    if (def != 0)
                    {
                        if (def->is_stackable())
                        {
                            if (static_cast<CStackableItem*>(def)
                                    ->getStackableLimit() == 1)
                                limitStackHit = 1;
                        }
                        else
                        {
                            limitStackHit = 1;
                        }
                    }
                    if (limitStackHit != 1)
                    {
                        // ORIG 0x86727a7..0x86727f9：
                        // pair.second = (int)ROUND(field4 / 100.0f) * pair.second
                        it->second = static_cast<int>(variation.m_field4 / 100.0f + 0.5f)
                                     * it->second;
                    }
                }
                if (shadowInv.tryInsertEventItems(scaled) == 0)
                    failCode = (limitStackHit == 0) ? 100 : 4;
                else
                    shadowOk = true;
            }
        }
    }
    else
    {
        failCode = 0x11;
    }

    if (!shadowOk)
        return failCode;

    char bSendEquip = 0;                                           // local_b5
    PacketGuard packet;                                            // local_c4
    packet.put_header(1, subCmd);
    packet.put_byte(1);
    if (recipeInfo.m_field25 != 0)                                 // recipe+0x199
    {
        packet.put_byte((int)resultMulList.size());
        for (std::vector<std::pair<int,int> >::const_iterator ri = resultMulList.begin();
             ri != resultMulList.end(); ++ri)
        {
            packet.put_int(ri->first);
            packet.put_int(ri->second);
        }

        int perExp = 0;                                            // local_ec
        int successCnt = 0;                                        // local_3c
        int expGain = 0;                                           // local_38
        int bonusCnt = 0;                                          // local_34
        const float ratio = variation.m_field4 / 100.0f;           // local_30
        for (int ci = 0; ci < (int)count; ++ci)
        {
            if (expert_job::CExpertJob::IsSuccess(this, recipe, perExp))
            {
                expGain = expGain + perExp;
                const int dice = get_rand_int(100);
                if (((float)dice < variation.m_field0) && (limitStackHit != 1))
                    bonusCnt = bonusCnt + 1;
                successCnt = successCnt + 1;
            }
        }
        if (successCnt < 1)
        {
            resultMulList.clear();
        }
        else
        {
            expert_job::CExpertJob::IncreaseExpertJobExp(
                this, GetCurCharacExpertJobExp() + expGain);
            for (std::vector<std::pair<int,int> >::iterator it = resultMulList.begin();
                 it != resultMulList.end(); ++it)
            {
                const int perCount = it->second / (int)count;      // local_28
                it->second = (it->second / (int)count) * successCnt;
                if (0 < bonusCnt)
                {
                    // ORIG：ROUND(perCount * ratio - perCount) * bonusCnt
                    const int extra =
                        static_cast<int>((float)perCount * ratio - (float)perCount + 0.5f)
                        * bonusCnt;
                    it->second = it->second + extra;
                }
                CExpertJobStatistic* stat = GetInstanceExpertJobStatistic();
                stat->increaseItemCount((ENUM_EXPERT_JOB_TYPE)expertType,
                                        (unsigned long)it->first, it->second);
            }
        }
        packet.put_int(successCnt);
        packet.put_int((int)count - successCnt);
        if (recipeInfo.m_field25 != 0)
            packet.put_byte(bonusCnt);
    }

    std::vector<int> updatedSlots;                                 // local_d0
    updatedSlots.clear();
    if (!_ProcCompoundCore(delListScaled, resultMulList, updatedSlots,
                           itemIdx, useRecipe2ndCheck, &packet,
                           *(bool*)&bSendEquip, count, condObj, recipe,
                           condItem))
        return 0x11;

    packet.finalize(true);
    Send(packet);
    if (!updatedSlots.empty())
    {
        for (std::vector<int>::const_iterator ui = updatedSlots.begin();
             ui != updatedSlots.end(); ++ui)
        {
            const int slot = *ui;                                  // local_20
            if (-1 < slot)
            {
                const Inven_Item it = getCurCharacInvenR()->GetInvenSlot(1, slot);
                if (it.m_field1 == 1)                              // local_1f5：equip 类型
                    SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
            }
        }
    }
    if (bSendEquip != '\0')
        send_equip(0x16);
    _onCompoundItem(recipe->get_index());
    if (recipe->GetNeedLevel() != 0)
    {
        CItemMakingSkill* skill =
            reinterpret_cast<CItemMakingSkill*>(
                GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)10));
        skill->GainItemMakingSkillPoint(recipe, this);
    }
    HistoryLog::WriteComposition((FILE*)m_field796f8, recipe->get_index());
    return 0;
}


// ---------------------------------------------------------------------------
// LinkF1 批次（2026-08-28）三小函数（报告 docs/class_func_reports/CUser/）。
// ---------------------------------------------------------------------------

// ORIG T _ZN5CUser10AddMileageEi14eCoinAddReason @0x08650954（报告 AddMileage.md）：
// LogHistory("Mileage %s,%d,%d,%d,%d","Add",amount,m_mileage,m_mileage+amount,reason)
// → m_mileage(+0x796d4) += amount → SaveMileage()。
void CUser::AddMileage(int amount, eCoinAddReason reason)
{
    LogHistory("Mileage %s,%d,%d,%d,%d", "Add", amount, m_mileage,
               m_mileage + amount, reason);
    m_mileage += amount;
    SaveMileage();
}

// ORIG T _ZN5CUser27is_recipe_items_avatar_typeERKSt6vectorISt4pairIiiESaIS2_EE
// @0x0868c45e（报告 is_recipe_items_avatar_type.md）：逐项 find_item，任一
// CItem 虚槽 +0x10（IsAvatarItem）非零即返回 1；全部未命中返回 0。
bool CUser::is_recipe_items_avatar_type(
    const std::vector<std::pair<int, int> >& items)
{
    for (std::vector<std::pair<int, int> >::size_type i = 0; i < items.size(); ++i)
    {
        CItem* item = G_CDataManager()->find_item(items[i].first);
        if (item != 0 && item->IsAvatarItem())
            return true;
    }
    return false;
}

// ORIG 反汇编对 get_state() 短路求值两次调用（==5 或 ==10 分支），不做缓存。
void CUser::SendHasNoGoWithApcQuestInParty(Quest const* quest)
{
    if (get_state() == 5 ||
        (get_state() == 10 && quest->m_triggerType == 3 &&
         quest->m_triggerSubType == 0))
    {
        CParty* party = reinterpret_cast<CParty*>(GetParty());
        if (party == 0)
            return;
        if (!party->HasNoQuestGoWithApcInParty(quest))
            return;
        PacketGuard packet;
        packet.put_header(0, 0xa8);
        packet.put_int(quest->m_index);
        packet.finalize(true);
        party->send_to_party(packet);
    }
}

// ---------------------------------------------------------------------------
// LinkF1 批次（2026-08-28）：CUser::process_recipe_avatar_items —— ORIG T
// _ZN5CUser26process_recipe_avatar_itemsEibhiPK14CStackableItem @0x0868c4f4。
// 报告 docs/class_func_reports/CUser/process_recipe_avatar_items.md 全文转写
// （草稿 /tmp/df_batches/linkf1/process_recipe_avatar_items.body.cpp + notes.md，
// 含 3 处 Ghidra 实参修正与 ORIG 立即数枚举 static_cast 标注）。
// ---------------------------------------------------------------------------
int CUser::process_recipe_avatar_items(int mainSlot, bool param2,
                                       unsigned short count, int cmdHeader,
                                       CStackableItem const* recipe)
{
    // 0x0868c4f4+0x01f: recipe == NULL -> 0x11
    if (recipe == NULL) {
        return 0x11;
    }
    // +0x023: param2 != 0 -> 0x13
    if (param2) {
        return 0x13;
    }

    // +0x042: 拷贝 recipe+0x174 -> materials（CStackableItem::m_recipe174.m_vec0）
    // +0x06e: 拷贝 recipe+0x180 -> outputs（m_recipe174.m_vecC）
    std::vector<std::pair<int, int> > materials(recipe->m_recipe174.m_vec0);
    std::vector<std::pair<int, int> > outputs(recipe->m_recipe174.m_vecC);
    // +0x076: outputs.size() > 1 -> 0x11
    if (outputs.size() > 1) {
        return 0x11;
    }

    // +0x0a1: vector<int>（槽位更新三元组，元素按 [space, slot, count] 排布）
    std::vector<int> updatedSlots;
    // +0x0af: 显式 clear（ORIG 空构造后即 clear，忠实保留）
    updatedSlots.clear();

    // +0x0bd..+0x105: 栈上副本库存
    //   CInventory::CInventory() 0x0822d52c
    //   InventoryMemory::InventoryMemory() 0x0822d3b8
    //   CInventory::SetInventoryMemory 0x0822d4f6
    //   CUserCharacInfo::getCurCharacInvenRefR 0x081803ca
    //   CInventory::setCopy 0x084fa314
    CInventory inv;
    InventoryMemory invMem;
    inv.SetInventoryMemory(&invMem);
    // getCurCharacInvenRefR 的头签名是 const Inven_Item*，但 ORIG 将返回值按
    // const CInventory& 传给 setCopy（同一 m_inven 子对象，两种视图同址）。
    inv.setCopy(*(const CInventory*)getCurCharacInvenRefR());

    // +0x14a: inv.delete_item(1 /*INVEN_TYPE_EQUIP*/, mainSlot, count, 8, false)
    // +0x15d: 失败 -> 0x11（eItemDelReason 枚举缺成员 8，按 ORIG 立即数强转）
    if (!inv.delete_item(INVEN_TYPE_EQUIP, mainSlot,
                         (int)(unsigned int)count, (eItemDelReason)8, false)) {
        return 0x11;
    }
    // +0x167..+0x1b5: push_back(0 /*equip space*/, mainSlot, count)
    updatedSlots.push_back(0);
    updatedSlots.push_back(mainSlot);
    updatedSlots.push_back((int)(unsigned int)count);

    // +0x1cf..+0x439: 材料循环（materials.size() 次）
    {
        int matIdx = 0;
        while ((int)materials.size() > matIdx) {
            std::pair<int, int> const& mat = materials[matIdx];

            // +0x1ed: G_CDataManager() 0x080cc19b
            // +0x1f9: CDataManager::find_item(int) 0x0835fa32
            CDataManager* dataMgr = G_CDataManager();
            CItem* item = dataMgr->find_item(mat.first);

            if (item != NULL) {
                // +0x210: 虚调用 *(vtable+0x10) -> CItem::IsAvatarItem()
                if (!item->IsAvatarItem()) {
                    // 普通材料：在副本库存中删除事件物品
                    // +0x319: inv.get_inven_slot_no(mat.first) 0x0850cd62
                    int slot = inv.get_inven_slot_no(mat.first);
                    if (slot == -1) {
                        return 0xd5;
                    }
                    // +0x381: inv.tryDeleteEventItems(one) 0x085058da
                    std::vector<std::pair<int, int> > one;
                    one.push_back(mat);
                    if (!inv.tryDeleteEventItems(one)) {
                        // +0x394: 失败 -> 0x11（就地退出，不做产出）
                        return 0x11;
                    }
                    // +0x3a3..+0x3ef: push_back(0, slot, mat.second)
                    updatedSlots.push_back(0);
                    updatedSlots.push_back(slot);
                    updatedSlots.push_back(mat.second);
                } else {
                    // avatar 材料
                    // +0x249: inv.get_avatar_slot_no(mat.first) 0x0850cd1c
                    int slot = inv.get_avatar_slot_no(mat.first);
                    if (slot == -1) {
                        return 0xd5;
                    }
                    // +0x281: CheckItemLock(2 /*INVEN_TYPE_AVATAR*/, slot)
                    if (CheckItemLock(INVEN_TYPE_AVATAR, slot)) {
                        return 0xd5;
                    }
                    // +0x2a8..+0x2f5: push_back(1 /*avatar space*/, slot, 1)
                    updatedSlots.push_back(1);
                    updatedSlots.push_back(slot);
                    updatedSlots.push_back(1);
                }
            }
            ++matIdx;
        }
    }

    // +0x471..+0x489: 取产出物品（注意：ORIG 未判 outputs.size() 是否为 0，
    // 直接读 outputs[0]；size>1 已被拒，size==0 属 ORIG 原始 UB，忠实转写）
    CDataManager* dataMgr = G_CDataManager();
    CItem* outItem = dataMgr->find_item(outputs[0].first);
    // +0x489: outItem == NULL -> 0x11
    if (outItem == NULL) {
        return 0x11;
    }

    // +0x4a2: PacketGuard::PacketGuard() 0x0858dd4c
    PacketGuard guard;
    // +0x4ad: getCurCharacInvenW() 0x080da28e
    CInventory* invW = getCurCharacInvenW();

    // +0x4cd..+0x6eb: 按三元组在真实库存中删除
    {
        std::vector<int>::const_iterator it = updatedSlots.begin();
        while (it != updatedSlots.end()) {
            int space = *it;
            ++it;
            int slot = *it;
            ++it;
            int delCount = *it;
            ++it;

            if (space == 0) {
                // +0x578: getCurCharacInvenW() 0x080da28e（ORIG 分支内重取）
                CInventory* invW2 = getCurCharacInvenW();
                // +0x5ac: delete_item(1 /*EQUIP*/, slot, delCount, 8, true)
                if (!invW2->delete_item(INVEN_TYPE_EQUIP, slot, delCount,
                                        (eItemDelReason)8, true)) {
                    // +0x5c9: trace（ORIG 字符串原样）
                    cMyTrace trace(
                        "int CUser::process_recipe_avatar_items(int, bool, "
                        "RequestItemCountType, int, const CStackableItem*)",
                        0xa8c1, 5);
                    trace("CUser::process_recipe_avatar_items, "
                          "getCurCharacInvenW()->delete_item failed , %d %d",
                          getCurCharacNo(), slot);
                    return 0x11;
                }
            } else { // space == 1（ORIG 仅判 ==1 / !=0 两路，此为 !=0 路）
                // +0x62b: getCurCharacInvenR() 0x080da27e
                // +0x64c: GetInvenSlot(2, slot) 0x084fb918 —— 读出当前 avatar 物品
                const CInventory* invR = getCurCharacInvenR();
                Inven_Item curItem = invR->GetInvenSlot(INVEN_TYPE_AVATAR, slot);
                // +0x681: invW->delete_item(2, slot, 1, 3, true)
                if (!invW->delete_item(INVEN_TYPE_AVATAR, slot, 1,
                                       eItemDelReason_3, true)) {
                    return 0x16;
                }
                // +0x6a3: mgr->UnRegistItem(curItem.m_addInfo2) 0x082f9402
                //   （Ghidra 反编译漏掉该 int 实参，汇编 0x6a8 以 -0x12e=Inven_Item+0x07 传入）
                WongWork::CAvatarItemMgr* mgr = invW->GetAvatarItemMgrW();
                mgr->UnRegistItem(curItem.m_addInfo2);
                // +0x6e6: invW->SendAvatarEvent(0x36, addInfo2, 1, slot+10, 0)
                invW->SendAvatarEvent(0x36, curItem.m_addInfo2, 1,
                                      slot + 10, 0);
            }
        }
    }

    // +0x720..+0x76b: 生成 IPG 编号（缓冲 0x18 字节清零）
    char ipgBuf[0x18];
    for (int k = 0; k < 0x18; ++k) {
        ipgBuf[k] = 0;
    }
    // +0x766: CGenUniqueNo::genIPGNo(7, getCurCharacNo(), ipgBuf) 0x084b7750
    //   （ORIG 第 1 参立即数 7，枚举现有成员只到 6，按 ORIG 强转）
    char* ipgNo = WongWork::CGenUniqueNo::genIPGNo(
        (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)7,
        (unsigned int)getCurCharacNo(), ipgBuf);

    // +0x773: CItem::get_index(outItem) 0x08110c48
    int outIdx = outItem->get_index();
    // +0x78d: CItem::getUsablePeriod(outItem) 0x08110c60
    int usablePeriod = outItem->getUsablePeriod();

    // +0x7d1: invW->AddAvatarItem(outIdx, usablePeriod, false, 0, -1,
    //                            ipgNo, 2, false, 0) 0x08509b9e
    //   （eAvatarItemAddReason 缺成员 2，按 ORIG 立即数强转）
    int newSlot = invW->AddAvatarItem(outIdx, usablePeriod, false, 0,
                                      (char)-1, ipgNo,
                                      (eAvatarItemAddReason)2, false, 0);
    // +0x7e9: newSlot == -1 -> 0x4
    if (newSlot == -1) {
        return 0x4;
    }

    // +0x810: getCurCharacInvenR() + GetInvenSlot(2, newSlot) —— 读回新 avatar
    Inven_Item newItem =
        getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, newSlot);

    // +0x835..+0x84b: put_byte(1); put_byte((int)updatedSlots.size() / 3)
    //   （ORIG 用 0xaaaaaaab 魔数除 3）
    guard.put_header(1, cmdHeader);     // +0x9fa 区段前：put_header(1, param_4)
    guard.put_byte(1);
    guard.put_byte((int)(updatedSlots.size() / 3));

    // +0x9d4..+0x99e: 三元组逐个 put_byte(space)/put_short(slot)/put_int(count)
    {
        std::vector<int>::const_iterator it = updatedSlots.begin();
        while (it != updatedSlots.end()) {
            int space = *it;
            ++it;
            int slot = *it;
            ++it;
            int cnt = *it;
            ++it;
            guard.put_byte(space);
            guard.put_short(slot);
            guard.put_int(cnt);
        }
    }

    // +0x9f7..+0xa2f: 产出物品字段
    guard.put_byte(1);
    guard.put_byte(1);
    guard.put_short(newSlot);
    guard.put_int(newItem.m_addInfo);    // Inven_Item+0x02（item idx）
    guard.put_int(newItem.m_addInfo2);   // Inven_Item+0x07
    // +0xa2f: Inven_Item::GetUpgrade() 0x080f506c -> put_byte
    guard.put_byte((int)newItem.GetUpgrade() & 0xff);
    // +0xa5c: put_short(newItem.m_fieldb)（Inven_Item+0x0b endurance）
    guard.put_short((int)newItem.m_fieldb);
    // +0xa6e: stAmplifyOption_t::getAbilityType() 0x08150732 -> put_byte
    guard.put_byte((int)newItem.m_amp.getAbilityType() & 0xff);
    // +0xa94: stAmplifyOption_t::getAbilityValue() 0x08150772 -> put_short
    guard.put_short((int)newItem.m_amp.getAbilityValue() & 0xffff);
    // +0xaba: put_byte(newItem.m_field0 != 0 ? 1 : 0)
    guard.put_byte(newItem.m_field0 != 0 ? 1 : 0);
    // +0xaea: put_packet(newItem) 0x0815098e
    guard.put_packet(newItem);
    // +0xb13: finalize(true) 0x080cb958
    guard.finalize(true);

    // +0xb28: Send(guard) 0x086485ba
    Send(guard);
    // +0xb4a: SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)1, newSlot)
    //   0x0867c65a（ENUM_ITEMSPACE 枚举缺成员 1，按 ORIG 强转）
    SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)1, newSlot);

    // +0xb4f: HistoryLog::WriteComposition(*(FILE**)(this+0x796f8),
    //                                      recipe->get_index()) 0x084ba174
    //   （m_field796f8 为 int 存文件句柄，与 ORIG *(int*)(this+0x796f8) 一致）
    HistoryLog::WriteComposition((FILE*)(unsigned)m_field796f8,
                                 recipe->get_index());

    // +0xb6f: return 0
    return 0;
}
// ---------------------------------------------------------------------------
// 全局自由函数 deleteSpecificItems @0x086cbc85（_Z19deleteSpecificItemsP5CUserPKSt6vector…）
// GameWorld::deleteSpecificItems @0x086ccbf4 按 m_UserInWorld 遍历调用本函数。
// 报告 docs/class_func_reports/_lt_global_gt_/deleteSpecificItems.md 全文转写：
//   1) get_state()<=2 直接返回 1（不发包）。
//   2) CUser::deleteSpecificItem(list, delList) 收集被删物品（space,slot）对。
//   3) 29 个 PacketGuard 各按 ENUM_ITEMSPACE 槽位组装 0xe 号包：包头(0,0xe)、
//      put_byte(space)、记 get_index()、put_short(0) 占位。
//   4) 按 space 分派：2=CCargo MakeItemPacket；0xc=AccountCargo MakeItemPacket；
//      其余映射 invenType（3→0、7→3、1→2、其它→1）后 CInventory::GetInvenSlot，
//      槽空时手写空槽字段序列 + put_packet(g_emptySlot)，否则 MakeItemPacket。
//   5) count>0 的槽 put_short(index, count)、finalize(true)、user->Send(guard)。
//   6) 循环结束返回 1。
// ORIG 汇编实证：包数组 29×0xc（rep stos 清零 + C1 循环）；count/index 各 29×4
// 清零；空间映射 movl 常量链；g_emptySlot 全局（CAccountCargo.cpp:22 定义）。
extern Inven_Item g_emptySlot;   // ORIG 全局 @0x943ddc0（CAccountCargo.cpp 定义）

int deleteSpecificItems(CUser* user,
                        const std::vector<std::pair<int, int> >* list)
{
    if (user->get_state() <= 2)
        return 1;

    std::vector<std::pair<int, int> > delList;
    user->deleteSpecificItem(*list, delList);

    PacketGuard guards[0x1d];
    int index[0x1d];
    int count[0x1d];
    for (int i = 0; i < 0x1d; ++i)
    {
        index[i] = 0;
        count[i] = 0;
    }

    for (int i = 0; i < 0x1d; ++i)
    {
        guards[i].put_header(0, 0xe);
        guards[i].put_byte(i);
        index[i] = guards[i].get_index();
        guards[i].put_short(0);
    }

    for (std::vector<std::pair<int, int> >::const_iterator it =
             delList.begin(); it != delList.end(); ++it)
    {
        int space = it->first;
        int slot  = it->second;

        if (space == 2)
        {
            user->getCurCharacCargoR()->MakeItemPacket(&guards[2], slot);
            count[2] += 1;
        }
        else if (space == 0xc)
        {
            if (user->IsExistAccountCargo())
            {
                user->GetAccountCargo()->MakeItemPacket(&guards[0xc], slot);
                count[0xc] += 1;
            }
        }
        else
        {
            int invenType = 1;
            if (space == 3)
                invenType = 0;
            else if (space == 7)
                invenType = 3;
            else if (space == 1)
                invenType = 2;

            Inven_Item item =
                user->getCurCharacInvenR()->GetInvenSlot(invenType, slot);
            if (item.m_addInfo == 0)
            {
                guards[space].put_short(slot);
                guards[space].put_int(-1);
                guards[space].put_int(0);
                guards[space].put_byte(0);
                guards[space].put_short(0);
                guards[space].put_byte(0);
                guards[space].put_int(0);
                guards[space].put_byte(0);
                guards[space].put_short(0);
                guards[space].put_byte(0);
                guards[space].put_packet(g_emptySlot);
            }
            else
            {
                user->getCurCharacInvenR()->MakeItemPacket(
                    (INVEN_TYPE)invenType, slot, guards[space]);
            }
            count[space] += 1;
        }
    }

    for (int i = 0; i < 0x1d; ++i)
    {
        if (count[i] != 0)
        {
            guards[i].put_short(index[i], count[i]);
            guards[i].finalize(true);
            user->Send(guards[i]);
        }
    }

    return 1;
}

