// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Arad_ItemBonus.h"   // ARAD::ARAD_ITEM（use_stackable 尾部事件物品发放）
// ============================================================================
// CUserOnlyOrig.cpp —— CUser 域「仅 ORIG」批次（Wave6 W6_CUser，worklist
// /tmp/only_orig/CUser.txt 269 符号，2026-08-27）
//
// 六批转写草稿（/tmp/df_batches/onlyorig_cuser body_A..F，含 compound_item 的
// m_battleField 具名视图修正版）统一落库为单一 TU；声明并入 CUser.h 尾部
// 「Wave6-CUser 新增声明」区。每函数逐份读取 docs/class_func_reports/CUser/<Func>.md
// （## 汇编 + ## 反编译 C）或按 nm -S + objdump 忠实转写并标注地址。
// thunk/依赖唯一 owner 见各函数头注释与 meta_*.tsv。
// ============================================================================
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <boost/shared_ptr.hpp>   // CBingo::getBingoSystem()->operator->（ORIG 0x080ce766）

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
#include "CUserOnlyOrig/_postCheckForceChangeGrowType.inc"
#include "CUserOnlyOrig/get_rand_int.inc"
class CUserEventUtil { public: static void updateCharacterDayEvent(CUser*, int); }; // T 0x08688230
#include "CUserOnlyOrig/GiveItemsReturnUser.inc"
namespace ARAD { class Arad_EventPeriodDataManager; }
#include "CUserOnlyOrig/_postCheckForceChangeGrowType_1.inc"
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
// 泳道口径：ORIG 无 SkillCmdKey 的 `_ZL` 局部符号（nm 判据）⇒ 该访问在 ORIG 是
// 反编译期合成的成员寻址，非真实函数。O0 下 static/static inline 仍落 `_ZL` 符号 ⇒
// 改宏展开。语句表达式的值是 rvalue（不能直接作被赋方），故末表达式取「目标字节的
// 地址」再由宏外层解引用 ⇒ 与「返回 unsigned char&」等价：既可读亦可赋。
// d/i 各只求值一次（引用/const 局量绑定），分支次序与原 switch 逐字一致（含 default
// 钳制到 m_key4）。已用独立探针验证左值赋值、求值次数与钳制语义。
// 状态：本 TU 内该宏当前**仅被注释代码引用**（下方 SkillCmdKey 调用点均在转写注释里），
// 即宏体尚未走过真实 TU 展开——语义正确性由独立探针（同族 g++ 4.4 / -std=gnu++98 / -O0）
// 保证；若后续启用调用点，须重跑本 TU 三数确认。
#define SkillCmdKey(d, i)                                                \
    (*({                                                                 \
        STSkillCommandData& _skcd = (d);                                 \
        const int _ski = (i);                                            \
        &(_ski == 0 ? _skcd.m_key0 :                                     \
          _ski == 1 ? _skcd.m_key1 :                                     \
          _ski == 2 ? _skcd.m_key2 :                                     \
          _ski == 3 ? _skcd.m_key3 : _skcd.m_key4);                      \
    }))

// body_A.cpp —— OnlyOrig_CUser.CuBatchA 转写草稿（Wave5: CUser 域 ORIG-only 批次 A）
// 依据 docs/class_func_reports/CUser/ 各报告（## 汇编 + ## 反编译 C 全文已读）；
// AddDailyBadge / ClearChildrensDayEvent / CheckPossibleRequestRenameState 三符号报告缺失，
// 按 nm -S 定址后 objdump -d 忠实转写（见各函数头注释）。
// 本文件为可粘贴草稿；签名与 ORIG mangled 一致。被调方若属本批直接调用；外域依赖照常书写。


// cSecuUserAction —— ORIG 0x086978ba C1(空体) / 0x0827b3f4 Init(+0 清零，**T 强符号**)。
// [A06 2026-09-10 P327 T1 调序] 本类定义与 `Init` 的**外联定义**前移至本 TU 首个强符号
// 之前：ORIG 的 GI 键名 = `_GLOBAL__I__ZN15cSecuUserAction4InitEv`（ORIG nm 0x0827b45d
// `t 0x1c`），而 P327 要求该键名取自「TU 内第一个强符号·外部链接·本 TU 定义」的实体 ⇒
// `cSecuUserAction::Init` 必须是**外联强符号**（ORIG 侧即 `T 0x0827b3f4 size 0xe`）。
// 旧形把 Init 写成类内 inline（发 W 弱符号）⇒ 不参与 GI 取名，键名错落到
// `_GLOBAL__I__ZN5CUser23_CalcItemVectorMultiplyERSt6vectorISt4pairIiiESaIS2_EEi`（仅 NEW）。
// 纪律：只移动**函数定义**，不移动任何带动态初始化的对象（P327）。
class cSecuUserAction
{
public:
    cSecuUserAction() {}                        // ORIG W 0x086978ba：空体（保持类内 inline）
    void Init();                                // ORIG T 0x0827b3f4：+0x00 = 0（外联，见下）
    // ORIG T 0x0827b402：return m_state == 1（cmp $0x1; setne 反相）。
    bool IsSecurityStateOff();

private:
    int m_state;   // +0x00（IsSecurityStateOff ==1 判定）
};

#include "CUserOnlyOrig/Init.inc"


#include "CUserOnlyOrig/_CalcItemVectorMultiply.inc"


#include "CUserOnlyOrig/_CheckCompoundSkill.inc"


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

#include "CUserOnlyOrig/_GetRecipeItem.inc"


#include "CUserOnlyOrig/_IsCompoundResult2MoreEquip.inc"


#include "CUserOnlyOrig/_onChangeGrowType.inc"


#include "CUserOnlyOrig/_onQuestClear.inc"


#include "CUserOnlyOrig/_ProcCompoundCore.inc"



#include "CUserOnlyOrig/_processCharacterDayEvent.inc"


#include "CUserOnlyOrig/_processFatigueBuffEvent.inc"


#include "CUserOnlyOrig/_processUnlimitChallenge.inc"


#include "CUserOnlyOrig/_saveConditionEventInfo.inc"


#include "CUserOnlyOrig/_saveProperDungeonClearCount.inc"


#include "CUserOnlyOrig/_updateSaveOption.inc"
{
    // ORIG 0x08652dde：Acquire 结果 %eax 直落 guard ctor（技巧36 删中转，
    // guard 槽 -0x18→-0x14 对齐）
    // [b28d-22 N104] 残差=EH 尾块相对序：ORIG 正常路径尾（dtor+epilogue）居函数最末、
    // unwind 块（0x08652ef0）居中；4.4 把 unwind 压最末、正常尾就地内联（块内容逐条
    // 全等，仅两尾块互换）。gap=1、槽位集相同（-0x14/-0xc）、P167 CLEAR ⇒ 封顶。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1d0d), true);
    guard.operator*()->operator<<(0x2b5);
    guard.operator*()->operator<<((int)GetUID());
    // SIG_CHARACTER_OPTION 协议视图：+0x00 characNo + 0x04 CharacterOption(0x200)。
    struct SIG_CHARACTER_OPTION_VIEW
    {
        // [b28d-solo-CUserOnlyOrig P211 档③] getCurCharacNo 返 unsigned int ⇒ 字段同型化
        // 复现 ORIG 0x08652dde LHS-first 直存（`mov -0xc(%ebp),%edx; mov %eax,(%edx)`；
        // int 字段异型产 RHS-first + `mov %eax,%edx` 中转）。
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
    // [G6Sub3 2026-09-10 复核] 80/81、帧 0x20、槽位集(-0x14/-0xc)全等；除上方 N104 的 EH
    // 尾块相对序（4.4 把 unwind 块中置 ⇒ 正常路径多发 1 条 `jmp`，= 全部 gap）外，其余残差
    // **全部**是数据地址差：`_ZN10GlobalData13s_stream_poolE` @0x940bd6c、
    // `_ZN10GlobalData10s_msgq_mgrE` @0x940bd68、"user.cpp" 字符串（.o 段内显示为
    // `mov 0x0,%eax` / `$0x253`）⇒ N132。无其它真实结构差异（指令/助记符序列逐条同构）。
}

#include "CUserOnlyOrig/add_matched_list.inc"


#include "CUserOnlyOrig/AddCurCharacMercenaryInfo.inc"


#include "CUserOnlyOrig/AddDailyBadge.inc"


#include "CUserOnlyOrig/AddTicket.inc"


#include "CUserOnlyOrig/alonePlay_Mode.inc"


#include "CUserOnlyOrig/alonePlay_Mode_1.inc"


#include "CUserOnlyOrig/apply_elixir_item.inc"


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

#include "CUserOnlyOrig/CalLevelUpItemCheck.inc"


#include "CUserOnlyOrig/CalLevelUpItemState.inc"


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

#include "CUserOnlyOrig/check_insert_lottery_reward.inc"


#include "CUserOnlyOrig/check_locked_multibox_error.inc"


#include "CUserOnlyOrig/check_multibox_error.inc"


#include "CUserOnlyOrig/check_unlocked_multibox_error.inc"


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

#include "CUserOnlyOrig/checkCodeChecksum.inc"


#include "CUserOnlyOrig/checkCodeChecksumRange.inc"


#include "CUserOnlyOrig/CheckDeleteCharacTime.inc"


#include "CUserOnlyOrig/CheckDungeonClear.inc"


#include "CUserOnlyOrig/CheckGuildLevelUp.inc"


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

#include "CUserOnlyOrig/checkInformNoticeFlag.inc"


#include "CUserOnlyOrig/CheckItemLockSlotSpace.inc"


#include "CUserOnlyOrig/CheckLimitMiniumAge.inc"


#include "CUserOnlyOrig/CheckPossibleRequestRenameState.inc"


#include "CUserOnlyOrig/checkSelectCharac.inc"


#include "CUserOnlyOrig/CheckTicket.inc"


#include "CUserOnlyOrig/ClearChildrensDayEvent.inc"


#include "CUserOnlyOrig/ClearMission.inc"


#include "CUserOnlyOrig/ClearMissionKind.inc"


#include "CUserOnlyOrig/ClearSkills.inc"


#include "CUserOnlyOrig/collectPcRoomPlayTime.inc"

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

#include "CUserOnlyOrig/compoundGuildMessage.inc"


#include "CUserOnlyOrig/compoundSystemMessage.inc"


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

#include "CUserOnlyOrig/copyItemOption.inc"


#include "CUserOnlyOrig/count_specific_items.inc"


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

#include "CUserOnlyOrig/decideCharacLinkMessage.inc"


#include "CUserOnlyOrig/decideLinkCharacSlotIndexes.inc"


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

#include "CUserOnlyOrig/DeleteCharac.inc"


#include "CUserOnlyOrig/DeleteCurrentCharacRentalItemInfo.inc"
{
    // [b28d-22 修法+真语义 bug] ORIG 0x08677081/0x08677093：**外层** find key = 形参 key
    // （`mov 0xc(%ebp),%eax; mov %eax,-0x18; lea -0x18(%ebp),%edx`）；0x0867706e/0x086770ef：
    // **内层** find key = cur->m_characNo（`mov -0xc(%ebp),%eax; mov (%eax),%eax; mov %eax,-0x1c`）。
    // 旧版两级 key 写反（行为错误）。ORIG 0x086770d8 `jne <尾>` 早退形 + 每处 `lea 0x6ef78(%eax)`
    // 重算（P183 禁引用缓存）。
    const _Charac_info* cur = getCurCharacR();
    if (cur == NULL) return;
    unsigned int accId = cur->m_characNo;
    if (accId == 0) return;
    typedef std::map<unsigned int,
            std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> > >
        RentalOuterMap;                                     // m_rentalInfo (+0x6ef78)
    RentalOuterMap::iterator it = m_rentalInfo.find(key);
    if (it == m_rentalInfo.end()) return;
    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> >::iterator it2 =
        it->second.find(accId);
    // ORIG 0x086770d8 `test %al,%al; jne <跳过>` ⇒ ==end 早退形（P222 正向包裹）。
    if (it2 == it->second.end()) return;
    it->second.erase(it2);
}

#include "CUserOnlyOrig/deleteToBlackList.inc"


#include "CUserOnlyOrig/DelTradePunishType.inc"


#include "CUserOnlyOrig/discard_mouse_register.inc"


#include "CUserOnlyOrig/DoItemLock.inc"


#include "CUserOnlyOrig/DoItemUnlock.inc"


#include "CUserOnlyOrig/DoItemUnlockCancel.inc"


#include "CUserOnlyOrig/QuestClearDeliveryMsg.inc"


#include "CUserOnlyOrig/eventDeliveryQuestClear.inc"

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

#include "CUserOnlyOrig/get_ingame_event_history.inc"

// 无参版 Ev @0x868d4f0（报告在案）：return (this + 0x8eb68) 即上述 map 引用 getter。

#include "CUserOnlyOrig/getCompoundUpgrade.inc"

#include "CUserOnlyOrig/getInDungeonItemSellCount.inc"


#include "CUserOnlyOrig/set_charac_guildkey_in_login.inc"

#include "CUserOnlyOrig/incPcRoomPlayTimeRewardCnt.inc"

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

#include "CUserOnlyOrig/isSuccessCompound.inc"


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
//                     *reinterpret_cast<CBattle_Field*>(&party->m_battleField);
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

#include "CUserOnlyOrig/isEnableGiveGiftToNPC.inc"


// ============================ 交易金币累加 ==================================

#include "CUserOnlyOrig/IncrementTradeGold.inc"



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

#include "CUserOnlyOrig/LoadMileage.inc"


#include "CUserOnlyOrig/lost_ext.inc"


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

#include "CUserOnlyOrig/OnItemUnlockWaitTimeout.inc"


#include "CUserOnlyOrig/OnPunishUser.inc"


#include "CUserOnlyOrig/OnRestrictTrade.inc"


#include "CUserOnlyOrig/onSelectCharacter.inc"


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

#include "CUserOnlyOrig/printBlackList.inc"


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

#include "CUserOnlyOrig/process_use_locked_multibox.inc"


#include "CUserOnlyOrig/process_use_unlocked_multibox.inc"


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

#include "CUserOnlyOrig/processReturnUser.inc"


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

#include "CUserOnlyOrig/quest_reward.inc"


#include "CUserOnlyOrig/Recv.inc"


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

#include "CUserOnlyOrig/registerToBlackList.inc"


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
//     m_sendMailCertified = false;
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

#include "CUserOnlyOrig/reset_tag_info_to_me.inc"


#include "CUserOnlyOrig/resetBlackList.inc"


#include "CUserOnlyOrig/ResetDailyMission.inc"


#include "CUserOnlyOrig/resetGuildDBInfo.inc"


#include "CUserOnlyOrig/resetHumanCertifyData.inc"


#include "CUserOnlyOrig/ResetMissionList.inc"


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

#include "CUserOnlyOrig/SaveBackupSkillInfo.inc"


#include "CUserOnlyOrig/saveCharacLinkMessageFlag.inc"


#include "CUserOnlyOrig/SaveInventoryOnly.inc"


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

#include "CUserOnlyOrig/SaveMileage.inc"


#include "CUserOnlyOrig/select_charac_set_visible_values.inc"


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
//     HistoryLog::WriteSelectCh(*(_IO_FILE**)m_historyLogFp,
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

#include "CUserOnlyOrig/send_broadcast_lottery_gold_message.inc"


#include "CUserOnlyOrig/send_charac_visible_data.inc"


#include "CUserOnlyOrig/Send_SoloPlay_Reject_NotiPack.inc"


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

#include "CUserOnlyOrig/sendCharacLinkMessage.inc"


#include "CUserOnlyOrig/sendCharacOption.inc"


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
//         if (!(m_puUser != 0 && GetTradePunishType() != 0)) {
//             unsigned short t2 = type;
//             if (m_puUser != 0 &&
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

#include "CUserOnlyOrig/SetMissionParameter.inc"


#include "CUserOnlyOrig/setPcRoomPlayTimeData.inc"


#include "CUserOnlyOrig/SetPuUser.inc"


#include "CUserOnlyOrig/SetPvpRecord.inc"


#include "CUserOnlyOrig/setQuestPoint.inc"


#include "CUserOnlyOrig/SetReceivedRequestType.inc"


#include "CUserOnlyOrig/setRecipeEqiupmentOption.inc"

#include "CUserOnlyOrig/SetServerGroup.inc"


// ---- body_F.cpp 续篇 A(与主文件按序拼接后编译)----

#include "CUserOnlyOrig/SetSkill.inc"


#include "CUserOnlyOrig/setSkillCommandData.inc"


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

#include "CUserOnlyOrig/setTagCharacInfo.inc"


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

#include "CUserOnlyOrig/SetTradePunishOverThirtyDays.inc"


#include "CUserOnlyOrig/setUsedBreakAwayLuckPoint.inc"


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

#include "CUserOnlyOrig/StartRecvEventWaiting.inc"


#include "CUserOnlyOrig/Update_ComboMission.inc"


#include "CUserOnlyOrig/update_ingame_event_history.inc"


#include "CUserOnlyOrig/update_record.inc"


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

#include "CUserOnlyOrig/updateBlueMarbleEnterCount.inc"


#include "CUserOnlyOrig/UpdateBreakAwayDungeonClearCnt.inc"


#include "CUserOnlyOrig/UpdateBreakAwayLuckPoint.inc"


#include "CUserOnlyOrig/UpdateCharacLinkBonus.inc"


#include "CUserOnlyOrig/UpdateCharacNameInMercenary.inc"


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
// FindRentalInfoByItemIndex：ORIG 形态为**全局作用域**仿函数（nm 实证，均 W）：
//   0x08697d2c _ZN25FindRentalInfoByItemIndexC1Ei / C2Ei
//   0x08697d3a _ZNK25FindRentalInfoByItemIndexclERKN20InstanceRentalSystem10RentalInfoE
//   0x0869b808 _ZSt7find_ifI...RentalInfo...E25FindRentalInfoByItemIndex...
//   0x086a0fe8 _ZSt9__find_ifI...25FindRentalInfoByItemIndex...
// 匿名 namespace 会把四者全部改写成 `_ZN12_GLOBAL__N_1...` ⇒ 发明 ORIG 不存在的符号。
// 故定义于全局作用域。跨 TU 同形约束：CUser.cpp:7724 有一份**同名同体**定义，inline
// 成员各自发射 W 后 link 期合并的前提是两侧类体逐 token 相同（ctor 初始化列表、
// operator() 体、成员名/类型/顺序全部一致）—— 两份已按 `sed '/^struct/,/^};$/p'` +
// diff 核对：类体逐字一致（含换行排版），无 ODR 隐患。
// 任何一方改动成员或语义时 MUST 同步另一方，否则同名异构 = ODR 隐患。
struct FindRentalInfoByItemIndex
{
    explicit FindRentalInfoByItemIndex(int itemIndex) : m_itemIndex(itemIndex) {}
    bool operator()(const InstanceRentalSystem::RentalInfo& info) const
    {
        return info.m_itemIndex == m_itemIndex;
    }

    int m_itemIndex;
};

#include "CUserOnlyOrig/UpdateCurrentCharacRentalInfo.inc"


// ---------------------------------------------------------------------------
// 31. CUser::UpdateDimensionInout @0x08651fba
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08651fba：p167=CLEAR，gap=10；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08651fba：N82 封顶——帧差（147 vs 157）+ 槽序
// （p167 CLEAR，gap 10）。
namespace
{
// ORIG 0x086520b8/0x086520d3/0x08652103 直证：+0x4 起的 6 字节数组按**单 disp**
// `movb $0x0,0x4(%edx,%eax,1)` 访问（P311 成员数组视图），+0xc/+0xd/+0xe 亦为
// `mov %al,0xc(%edx)` 单 disp 直落。用具名视图替代 `((char*)sig)[4+i]`（后者在 4.4 生
// `add $0x4,%edx; add %edx,%eax` 两步 + 跨 call 的 lea 地址缓存）。
struct UpdateDimensionInoutSigView
{
    unsigned int m_characNo;    // +0x0
    char m_inoutValue[6];       // +0x4..+0x9
    char m_reservedA[2];        // +0xa..+0xb（ORIG 不写）
    // 三字段按 ORIG 0x0865217a 段形定 uchar：源 uchar 返回值同型直落单步
    // `mov %al,0xc(%edx)`；声明 char 会多一条 uchar→char 的 `mov %eax,%edx` 中转。
    unsigned char m_ultimateInout;  // +0xc
    unsigned char m_saveDimension;  // +0xd
    unsigned char m_saveBlood;      // +0xe
};
}

#include "CUserOnlyOrig/UpdateDimensionInout.inc"


#include "CUserOnlyOrig/UpdateEquipslotFlag.inc"


#include "CUserOnlyOrig/UpdateMissionCondition.inc"


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

#include "CUserOnlyOrig/use_equipment.inc"


#include "CUserOnlyOrig/use_multibox_statistic.inc"


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

#include "CUserOnlyOrig/UseCeraPoint.inc"


#include "CUserOnlyOrig/UseLimitItem.inc"


#include "CUserOnlyOrig/UseMileage.inc"


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

#include "CUserOnlyOrig/verifyDoingQuest.inc"


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

#include "CUserOnlyOrig/verifyStackableItemActionPlace.inc"


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

#include "CUserOnlyOrig/init_skill_tree.inc"


#include "GlobalData.h"        // GlobalData::s_systemTime_ / s_pSecuProtectionField 等
#include "CEnvironment.h"      // G_CEnvironment / Server_Envir 字段
#include "Cipher.h"            // G_Cipher
#include "CMailBoxHelper.h"    // WongWork::CMailBoxHelper::AssignMailBox / CMailBox::Init
#include "GameMasterCmd.h"     // CGameMasterCharacter（InitOnAccept static local）
#include "CPremiumLetheManager.h" // CPremiumLetheManager 完整类型（increase_status lethe 链）

// [R21a14 ④ §27.2 类型轴] 原为 `unsigned int`（TU 局部声明）与权威定义
// GlobalData/get_seed_from_date.inc `int get_seed_from_date()` 冲突 ⇒ 调用点
// InitOnAccept 0x08649?? 处 store 被 4.4 判「异型」产 RHS-first+`mov %eax,%edx`
// 中转；ORIG 0x0864964x 实为 `mov 0x8(%ebp),%edx; mov %eax,0x711e8(%edx)`（LHS-first
// 直存 ⇒ int 与 int 同型）。改回 int 后与 ORIG 逐条同形（探针
// /tmp/verify357/R21a14/probe/p.cpp fa(RHS+中转) vs fb(LHS 直存) 复现）。
int get_seed_from_date();   // ORIG T 0x086b1d25 _Z18get_seed_from_datev
// cSecuUserAction 类定义 + `Init` 外联定义已**前移至本 TU 文件头**（P327 T1 调序，
// 使 GI 键名 = `_GLOBAL__I__ZN15cSecuUserAction4InitEv` 与 ORIG 一致，见文件头注释）。
// ORIG 侧仅 sizeof=4 的状态对象；InitOnAccept 0x086493c8 +0x4a1 处 new。

#include "CUserOnlyOrig/IsSecurityStateOff.inc"


#include "CUserOnlyOrig/InitOnAccept.inc"


#include "CUserOnlyOrig/increase_status.inc"


#include "CUserOnlyOrig/complete_reward_list.inc"


#include "CUserOnlyOrig/IsWaitingStateCharac.inc"


#include "CUserOnlyOrig/GetUserSecretShop.inc"


#include "CUserOnlyOrig/getQuestPoint.inc"


#include "CUserOnlyOrig/IsState.inc"


#include "CUserOnlyOrig/GetChangedGiftFatigueQuantity.inc"


#include "CUserOnlyOrig/GetReceivedRequestType.inc"


#include "CUserOnlyOrig/getPcRoomAuth.inc"


#include "CUserOnlyOrig/IsPCRoomUser.inc"


#include "CUserOnlyOrig/GetSlotIDX.inc"


#include "CUserOnlyOrig/SetAge.inc"


#include "CUserOnlyOrig/SetCurCharacUsedGiftFatigueQuantity.inc"


#include "CUserOnlyOrig/getAddQuestItemDropRateFromPremium.inc"


#include "CUserOnlyOrig/setCharacLinkBonusSetting.inc"


#include "CUserOnlyOrig/isCharacLinkBonusSetting.inc"


#include "CUserOnlyOrig/setBreakAwayRewardData.inc"


#include "CUserOnlyOrig/setAdvanceAltarIndex.inc"


#include "CUserOnlyOrig/getLinkCharacNo.inc"


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
    boost::shared_ptr<CBingo>* getBingoSystem();  // ORIG T 0x080c9fea（定义 BingoEventLink4.cpp）
};

#include "CUserOnlyOrig/initUserData.inc"


// ============================================================================
// CU_Fix-2 批次 —— TranslateLoginInfo 链接失败中的 CUser 域 16 符号
// （docs/class_func_reports/CUser/<Func>.md 全文 ## 汇编+## 反编译 C 已逐份读取）
// ============================================================================

#include "CUserOnlyOrig/SetBingoData.inc"


#include "CUserOnlyOrig/SetBirth.inc"
{
    // SetBirth.md @0x0867ecf0：strncpy 局部 2 字节缓冲后 atoi；
    // 低字节写 m_birthMonth(+0x8ebe→全址0x711d5)、m_birthDay(全址 0x711d4)。
    // ORIG 0x0867ecf0：入口 `movw $0x0,-0xb(%ebp); movb $0x0,-0x9(%ebp)` 2B+1B 拆分零写
    // （4.1 对 3B 零初始化做 subword store，奇址 -0xb 直落）；memcpy 只覆 [0..1]，尾 0 保持
    // ⇒ 显式 digits[2]='\0' 不存在于 ORIG。4.4 packed struct 复现奇址 HImode store。
    struct __attribute__((packed)) Digits3 {
        short w;
        char b2;
    } digits;
    digits.w = 0;
    digits.b2 = 0;
    // [b28d-solo-CUserOnlyOrig P167 重载选错真 bug] ORIG 0x0867ed18 / 0x0867ed4b
    // 均 call strncpy@plt（0807d8d0），非 memcpy（我方旧写法 EXTRA memcpy ×2、
    // MISSING strncpy ×2）。n=2 且源非零时两者等价，按 ORIG 照抄。
    // [A06 2026-09-10] 此处改用**裸指针算术** `birth + 2`：4.1 生两段 `add $0x2,%eax`、
    // 4.4 折叠为 `lea 0x2(%eax),%edx`（P319 反向），两者条数相等（各 2）；旧
    // `(unsigned int)birth + 2` 整型形虽复现 add，却令 4.4 多一条 `mov %eax,%edx`
    // 中转（35 vs ORIG 33）。取条数口径 ⇒ 裸指针算术（33=33）。
    // [M8COB 2026-09-11 试形·回滚] `&birth[2]` 下标形与 `birth + 2` 同产 `lea 0x2(%eax),%edx`
    // （33=33、AE 残差 12 行同前）⇒ 4.4 不产 ORIG 的就地 `add $0x2,%eax`（4.1 求值序：
    // src 先入 %eax 并就地加、dest 的 lea 延后）⇒ 属跨版本求值序族，回退原形。
    strncpy(reinterpret_cast<char*>(&digits), birth + 2, 2);
    m_birthMonth = static_cast<char>(atoi(reinterpret_cast<char*>(&digits)));
    strncpy(reinterpret_cast<char*>(&digits), birth + 4, 2);
    m_birthDay = static_cast<char>(atoi(reinterpret_cast<char*>(&digits)));
}

#include "CUserOnlyOrig/SetCharacMercenaryInfo.inc"


#include "CUserOnlyOrig/setCharacSlotCountInfo.inc"


#include "CUserOnlyOrig/getCharacSlotCountDefaultInfo.inc"


#include "CUserOnlyOrig/SetEventCreateDnfReward.inc"


#include "CUserOnlyOrig/SetGrowthCreatureEventInfo.inc"


#include "CUserOnlyOrig/SetGrowthWeaponEventInfo.inc"
{
    // SetGrowthWeaponEventInfo.md @0x8691bae：sig(+0/+4/+8/+0xc/+0x10/+0x12)
    // 六字段搬运至 GrowthWeaponEvent(+0x8ebe4)；repeat 事件 100(0x64) 进行中且
    // m_eventFlag != 1 时发送 noti 0x1ba（ORIG 对事件指针无空检直接虚调）。
    // [b28d-solo-CUserOnlyOrig] ORIG 0x08691bae 每字段 `mov 0xc(%ebp),%eax;
    // mov 0x4(%eax),%edx; mov %edx,0x8ebe8(%eax)` 直读直存（无 memcpy/无槽）；
    // SIG_GROWTH_WEAPON_EVENT 是 uchar[0x14]，用具名载荷视图内联 cast 逐字段访问。
    // 字段序照 ORIG 发射序：+4、+8、+0x10、+0xc、+0x12。
    struct GrowthWeaponSigView {
        bool m_eventFlag;            // +0x00（与目标 m_eventFlag 同为 bool ⇒ P300 RHS-first 直存）
        char m_pad1[3];              // +0x01..0x03
        unsigned int m_f4;           // +0x04
        unsigned int m_f8;           // +0x08
        unsigned int m_fc;           // +0x0c
        short m_f10;                 // +0x10（与目标 m_field10 同型，P211 档③）
        short m_f12;                 // +0x12（与目标 m_field12 同型）
    };
    m_growthWeapon.m_eventFlag = sig.m_eventFlag;
    m_growthWeapon.m_field4    = sig.m_field4;
    m_growthWeapon.m_field8    = sig.m_field8;
    m_growthWeapon.m_field10   = sig.m_field10;
    m_growthWeapon.m_fieldc    = sig.m_fieldc;
    m_growthWeapon.m_field12   = sig.m_field12;

    if (GlobalData::s_event_manager->GetRepeatEvent(100)->IsEventing(
            static_cast<CUser*>(0)))
    {
        // [b28d-solo-CUserOnlyOrig P225 推广] 拆嵌套 if：ORIG 两条件各发一段
        // `xor $1/test/je`；4.4 对 `a && b` 恒折直跳/物化岛，嵌套 if 逐条复现。
        // [P292/P168a] ORIG 0x08691c48 `movzbl 0x8ebe4(%eax),%eax; xor $0x1,%eax;
        // test %al,%al; je` = bool 字段 + `!field`（旧 uchar + `!=1` 产 `cmp $1,%al`）。
        if (!m_growthWeapon.m_eventFlag)
        {
            PacketGuard packet;
            packet.put_header(0, 0x1ba);
            packet.finalize(true);
            Send(packet);
        }
    }
}

#include "CUserOnlyOrig/ValidateStrikerInfo.inc"


#include "CUserOnlyOrig/sendReturnUserFirstLogin.inc"


#include "CUserOnlyOrig/set_acc_info.inc"


#include "CUserOnlyOrig/set_fatigue_count_db.inc"


#include "CUserOnlyOrig/set_ingame_event_history.inc"


#include "CUserOnlyOrig/set_ingame_event_history_1.inc"
{
    // set_ingame_event_history(SIG*).md @0x868d664：条目 stride 0xc ×30 循环；
    // key==0 跳过；get_event_script_mng()->is_current_ingame_event_record_type(key)
    // 通过才逐条转发 set_ingame_event_history(key,+4,+8)。
    if (sig == 0)
        return;
    // [b28d-solo-CUserOnlyOrig P167/P95 真形] ORIG 0x0868d664 无任何 memcpy
    // （旧写法 EXTRA memcpy ×3、OUR calls 7 vs ORIG 4）：每条目三次完整
    // `idx*3; shl 2; add sig` 链直读 int 字段 ⇒ 局部具名视图 + 逐字段表达式
    // （P0 无 view 指针槽；ORIG 直接 `add 0xc(%ebp),%eax`）。
    struct IngameEventRecord { int m_key; int m_v1; int m_v2; };
    // [A06 2026-09-10 P319 第二形 试行记录·已回退] ORIG 每处寻址尾为 `add 0xc(%ebp),%eax`
    // （整数加法、形参作内存操作数，O13/O22/O37/O44/O51）。实测 `(unsigned int)sig + i*0xc`
    // 回 cast 形在 4.4 产 **3 条**（`mov %eax,%edx; mov 0xc(%ebp),%eax; lea (%edx,%eax,1),%eax`）
    // 反劣于原下标形的 2 条（`mov 0xc(%ebp),%ecx; lea (%ecx,%eax,1)`）⇒ 回退，记 N18a 族
    // （4.1 `add mem,%eax` 单条不可造）。
    // 视图元素仍具名（§4.3 合规）。
    // ORIG 0x0868d714 尾测 `cmpl $0x1d,-0xc; setle %al; test %al,%al; jne`
    // = 有符号 `i <= 0x1d`（P2/P51）；`i < 0x1e` 在 4.4 生 jcc 直跳无 setle。
    for (int i = 0; i <= 0x1d; ++i)
    {
        // [G6Sub3 2026-09-10 P296 新旋钮·待父泳道测] ORIG 0x0868d681/868d694/868d6c2 三处失败
        // 出口**同落** 868d710(=i++)、体尾**无**中继岛；旧 `continue` 形令 4.4 在体尾多发
        // `jmp +2; nop` 岛（OUR 79 vs ORIG 70 中的 2 条）。按 P296 改嵌套正向 if 包裹 ⇒ 期望
        // 消 jmp+nop；若四数不改善即回退。
        // ORIG 0x0868d690 `test %eax,%eax; je 868d710`（key==0 → i++）= 正极性 `!= 0` 守卫。
        if (reinterpret_cast<const IngameEventRecord*>(sig)[i].m_key != 0)
        {
            // ORIG 0x0868d6a5 `mov (%eax),%ebx` 跨 G_CDataManager/get_event_script_mng
            // 保活 ⇒ 链式内联实参（P52），勿用具名 scriptMng 槽。
            // [L6 判据①/P222 极性重核] ORIG 0x0868d6c0 `call is_current_ingame_event_record_type;
            // test %al,%al; je 868d710` = **正极性**守卫（体在 je 之后直落）；旧 `if (!…) continue;`
            // 使 4.4 反产 `xor $1,%eax; test %al,%al; jne`。
            if (static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())->
                    is_current_ingame_event_record_type(
                        reinterpret_cast<const IngameEventRecord*>(sig)[i].m_key))
            {
                set_ingame_event_history(
                    reinterpret_cast<const IngameEventRecord*>(sig)[i].m_key,
                    reinterpret_cast<const IngameEventRecord*>(sig)[i].m_v1,
                    reinterpret_cast<const IngameEventRecord*>(sig)[i].m_v2);
            }
        }
    }
}

#include "CUserOnlyOrig/setCharacLevelUpGift_AccountOnce.inc"


#include "CUserOnlyOrig/initBingoData.inc"



#include "CUserOnlyOrig/setBreakAwayRewardOrder.inc"
{
    // ORIG 0x0831d4de：enum=enum 同型赋值 this 先载（CGameOption.h 字段已具型）
    m_gameOption.m_breakAwayRewardOrder = order;
}

// ============================================================================
// LinkClosureA-2 批次（2026-08-27）—— 全量链接收敛批 A：CUser 方法族 producer。
// 每函数：报告 docs/class_func_reports/CUser/<Func>.md（## 汇编+## 反编译 C 已完整阅读）
// 或报告缺失时 nm -S + objdump 忠实转写（标注）；nm 核验 mangled/地址。
// ============================================================================

// ---- LinkClosureA 消费的外部符号（ORIG nm 实证；实现 owner 待 Main 分派）----
class CEventManager_fwd_check;   // GetRepeatEvent/IsEventing 经 CGuildServerProxy.h 权威声明消费

#include "CUserOnlyOrig/get_SoloPlay_msgType.inc"
{
    // ORIG 0x0868dd7c：`movzbl 0xf(%eax),%eax; movsbl %al,%eax` 单条成员 disp
    // 装载（§4.3 例外视图，MSG_REQ_TO_PEER 无布局符号）+ 值留 %eax 二分下探
    // （4.1 switch 决策树 `cmp $1; je; cmp $1; jg; test; je;…`，技巧34）
    struct PeerTypeView { char m_pad[0xf]; unsigned char m_reqType; };
    // [b28d-solo-CUserOnlyOrig P279/P0] ORIG 无 v 槽（`movzbl 0xf(%eax),%eax;
    // movsbl %al,%eax` 后值留 %eax 直接二分）⇒ switch 直接吃表达式。
    switch (static_cast<signed char>(
        (reinterpret_cast<const PeerTypeView*>(&msg))->m_reqType)) {
    // ORIG 岛序 0x29 ret1 / 0x30 ret0 / 0x37 ret2 / 0x3e ret3 / 0x45 ret4
    // ⇒ 源 case 书写序 0,1,2,3（body 块按声明序发射）
    case 0: return 1;
    case 1: return 0;
    case 2: return 2;
    case 3: return 3;
    default: return 4;
    }
}

#include "CUserOnlyOrig/SendItemMakingSkillInfo.inc"


#include "CUserOnlyOrig/sendBingoAddData.inc"


#include "CUserOnlyOrig/SetGuildInfoProxy.inc"


#include "CUserOnlyOrig/checkCharacLinkType.inc"


#include "CUserOnlyOrig/fnEnumDungeonClear.inc"

#include "CUserOnlyOrig/SendDungeonClearList.inc"


#include "CUserOnlyOrig/SendEventBringUpInfo.inc"


#include "CUserOnlyOrig/SendTagCharacInfo.inc"


#include "CUserOnlyOrig/requestCubePremiumInfoIfContracted.inc"

#include "CUserOnlyOrig/setCharacSlotLimit.inc"


#include "CUserOnlyOrig/EnableCharacInfo.inc"


#include "CUserOnlyOrig/SendRevengeDungeon.inc"
{
    // ORIG 0x08656acf `sete %al; test; jne END` ⇒ 正向 ==0 早退（技巧6/9）
    if (getCurCharacR() == 0)
        return;
    // ORIG 0x08656aec lea -0x22：10B 记录 {type@0,flag@1,remain@2,reserved@6}（pack 1）
    //（pack(1) ⇒ 4.4 按 align 1 紧排，槽基址 -0x22 对齐 ORIG）
#pragma pack(push, 1)
    struct RevengeData { char m_type; char m_flag; int m_remain; int m_reserved; };
#pragma pack(pop)
    RevengeData data;
    memset(&data, 0, 10);                       // ORIG 0x08656af2 memset(.,0,0xa)
    // ORIG 0x08656b07-0x08656b23：switch 下探具名槽 -0x18 `mov -0x18,%eax;
    // cmp $1; je 岛; cmp $2; je 岛; jmp END`（技巧34，直写 switch 精确复现）
    int rewardType = GlobalData::s_villageMonsterMgr->GetRewardType();
    switch (rewardType) {
    case 1: data.m_type = 0; break;
    case 2: data.m_type = 1; break;
    default: return;
    }
    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    unsigned int closeTime = GlobalData::s_revengeDungeonMgr->GetCloseTime();
    // ORIG 0x08656b43 dead init + `jb 岛(置0)` / 减法链 fall-through：then 在前
    int remain = 0;
    if (closeTime >= curSec) {
        remain = static_cast<int>(closeTime - curSec);
    } else {
        remain = 0;
    }
    data.m_flag = 1;                             // ORIG +0xae movb $1,-0x21
    data.m_remain = remain;                      // ORIG +0xb2
    data.m_reserved = 0;                         // ORIG +0xb8
    PacketGuard packet;
    packet.put_header(0, 0x1ec);
    // [Lane6 2026-09-10 P0] 删 `char* dp = (char*)&data;` 具名指针局部：ORIG 0x08656ba1
    // 无 -0xc 槽（`lea -0x22(%ebp),%eax; movl $0xa,0x8(%esp); mov %eax,0x4(%esp)` 直落
    // 实参）⇒ 原码是内联实参；具名缓存多 store+load 各 1 并把槽基顶深 4。
    // [A06 2026-09-10 实参求值序] ORIG 0x08656b9x：`lea -0x22(%ebp),%eax`（先算 &=data）
    // → `movl $0xa,0x8(%esp)` → `mov %eax,0x4(%esp)`；写 `(char*)&data` 时 4.4 反为
    // 「先压常量后 lea」。改 `&data.m_type`（char*，元素首址同值）令 4.4 先物化基址。
    // 注：本函数因切分工具把函数体内**列 0 的 `#pragma pack`/`struct`** 误判为 TU 级
    // 边界，函数体尾部留在**聚合器**而 .inc 只有函数头 ⇒ 改动跨 .inc 与 .cpp 两处。
    // [CoBatchB 2026-09-11 负结论·已回滚] `&data.m_type`（元素首址同值）实测 **97→97 中性**
    // （4.4 仍先压常量）⇒ 回滚；ORIG 0x08656ba1 `lea` 先于 0x08656ba4 `movl $0xa` 属 P319
    // 跨版本实参装载序，源码不可控。
    // [g06-D 2026-09-11 试形①·已回滚] Main 已批准 thin-inc 例外，实测把 size 由
    //   `sizeof(data)` 改为字面量 `10`（参数 int，mangling E15InterfacePacketBuf10put_binaryEPci）
    //   **中性**：仍 97/97、ndiff=2、仍是「先 `movl $0xa,0x8(%esp)` 后 `lea -0x22`」（ORIG 反之）。
    //   ⇒ 与该件历史两次试形（reinterpret_cast 形 / `&data.m_type` 形）一致，属 **P319/P75
    //   跨版本实参装载时刻**（4.1 把简单地址 lea 提前到实参 store 之前），源码不可控。
    packet.put_binary(reinterpret_cast<char*>(&data), sizeof(data));   // ORIG E15InterfacePacketBuf10put_binaryEPci
    packet.finalize(true);
    Send(packet);
}

#include "CUserOnlyOrig/GetItemIdByAddInfo.inc"

#include "CUserOnlyOrig/setTagCharacInven.inc"


#include "CUserOnlyOrig/SetInventory.inc"


#include "CUserOnlyOrig/SetLimitInfo.inc"

#undef CUOO_LIM

#include "CUserOnlyOrig/SelectCharac.inc"


#include "CUserOnlyOrig/set_MultimailMsgData.inc"


#include "CUserOnlyOrig/SendMail.inc"


#include "CUserOnlyOrig/sendDecorationObjectCnt.inc"


#include "CUserOnlyOrig/CreateCharac.inc"


#include "CUserOnlyOrig/SendLV70Buff.inc"

#include "CUserOnlyOrig/use_stackable.inc"


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

// tagCharacVisibleFlag：权威类定义已入 CUser.h:530（L31CharacPad `_Charac_info` 成员化），
// 本 TU 删除局部重复声明（否则 redefinition）；成员函数定义见下（`m_flag = 0;`）。

#include "CUserOnlyOrig/setCharacInfoDetail.inc"


#include "CUserOnlyOrig/Reset.inc"


#include "CUserOnlyOrig/SendMultiMail.inc"


#include "CUserOnlyOrig/_onCreateCharacter.inc"


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

#include "CUserOnlyOrig/SIG_LOAD_ETC.inc"


#include "CUserOnlyOrig/CharacterOption.inc"


#include "CUserOnlyOrig/ExpBonusPointResult.inc"


#include "CUserOnlyOrig/clear.inc"


#include "CUserOnlyOrig/tagCharacVisibleFlag.inc"


#include "CUserOnlyOrig/reset.inc"



// ---------------------------------------------------------------------------
// _Charac_info 拷贝构造/赋值 —— ORIG 0x08699863（C1ERKS_）/0x0869da62（aS=）
// 均为编译器生成版（标量+字符数组按 dword 逐字段拷贝，map/vector 子对象按
// 成员声明序调拷贝构造/赋值；ORIG 汇编头部 7×int+movzwl 的 m_name 形态即
// GCC 数组成员隐式拷贝）。手写 memcpy 块版本与编译器 codegen 结构不同，
// 已删除，改为隐式定义；下列 helper 强制本 TU 落出两个弱符号。
// ---------------------------------------------------------------------------
// [b28d-22 N82/N83] ORIG 0x08699xxx `_ZN12_Charac_infoaSERKS_`（957 条）/`C1ERKS_`（1012 条）：
// ORIG 对 +0x88 起 0x148 字节走 `mov $0x52,%ebx` 块拷贝循环；重建 CUser.h（冻结）把该区
// 建模为具名标量字段，4.4 O0 只能逐字段拷贝；op=/拷贝构造均为编译器隐式生成 ⇒ 语义完成、
// byte 层 N82/N83 封顶（P167 差集见 TSV）。
// 泳道口径：原 `static _cuoo_emit_charac_info_ops` + `g_cuoo_charac_info_emit` 两个 force
// 符号在 ORIG 均无（nm 判据）⇒ 已删除。两弱符号改由本 TU 的自然宿主发射：
// `std::vector<_Charac_info> CUser::m_characList` 的 push_back/M_insert_aux→
// new_allocator::construct 引 C1ERKS_，erase→move_backward/__copy_move 引 aSERKS_
// （见本 TU 上方 `for (std::vector<_Charac_info>::iterator it = m_characList.begin(); ...)`）。

#include "CUserOnlyOrig/_Charac_info.inc"


#include "CUserOnlyOrig/dtor__Charac_info.inc"

// ORIG 0x08699863：编译器生成的拷贝构造。平凡区（含 _Additioal_info@0x88、
// CCargo@0xdaa 等无堆成员子对象）逐字段拷贝；非平凡子对象调用拷贝构造。

// ORIG 0x0869da62：编译器生成的赋值。顺序：平凡区（+0x88 区 0x52 memcpy）→
// CInventory/SkillSlot 赋值 → +0xdaa 区 0x11 memcpy → 双 vector/双 map 赋值。

#include "CUserOnlyOrig/giveCharacLinkBonusExp.inc"


#include "CUserOnlyOrig/quest_action.inc"


#include "CUserOnlyOrig/SetCharacInfo.inc"


#include "CUserOnlyOrig/UseQuestItem.inc"


#include "CUserOnlyOrig/compound_item.inc"



// ---------------------------------------------------------------------------
// LinkF1 批次（2026-08-28）三小函数（报告 docs/class_func_reports/CUser/）。
// ---------------------------------------------------------------------------

#include "CUserOnlyOrig/AddMileage.inc"


#include "CUserOnlyOrig/is_recipe_items_avatar_type.inc"


#include "CUserOnlyOrig/SendHasNoGoWithApcQuestInParty.inc"


#include "CUserOnlyOrig/process_recipe_avatar_items.inc"

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

#include "CUserOnlyOrig/deleteSpecificItems.inc"

