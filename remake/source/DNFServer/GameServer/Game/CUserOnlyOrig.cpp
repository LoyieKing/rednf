// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG T 0x0827b3f4 size 0xe：`mov 0x8(%ebp),%eax; movl $0x0,(%eax); ret`。
// 置为本 TU 第一个强符号（GI 键名载体）。
#include "CUserOnlyOrig/Init.inc"
}

// ---------------------------------------------------------------------------
// 1) CUser::_CalcItemVectorMultiply(vector<pair<int,int>>&, int)
//    报告：docs/class_func_reports/CUser/_CalcItemVectorMultiply.md
//    ORIG 0x08672042：循环 it->second *= mult。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/_CalcItemVectorMultiply.inc"
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
// [b28d-22 N82] ORIG 0x08670f1e：p167=M0/E6:_ZNSt6vectorISt4pairIiiESaIS1_EED1Ev;_ZNSt6vectorISt4pairIiiESaIS1_EEC1ERKS3_，gap=43；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08670f1e：修法（本轮）——check_skill_list 按值参数
// 的实参改匿名右值临时（直构进参数槽），消 EXTRA `vector<pair<int,int>>::D1Ev`×4 +
// `C1ERKS_`×2 ⇒ p167 CLEAR（19=19），178→138 条。残余帧差 0x10（sub $0x50 vs $0x60，
// N115 outgoing 预留）+ setcc 物化 ⇒ N82/N115 封顶。
#include "CUserOnlyOrig/_CheckCompoundSkill.inc"
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
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08670e38：本波三修归零（逐条全等，仅尾对齐 nop）——
// ①去 ret 预清（ORIG 无 `movl $0`，P0/P198①）；②去 inven 具名局部改内联实参（P0）；
// ③GetInvenSlot 返回值改**值类型局部**（P278，消引用指针槽 -0xc）+ 补具名
// `CStackableItem* stack`（P0，ORIG -0x10 ret/-0xc stack 双槽）。
#include "CUserOnlyOrig/_GetRecipeItem.inc"
}

// ---------------------------------------------------------------------------
// 6) CUser::_IsCompoundResult2MoreEquip(vector<pair<int,int>> const&)
//    报告：docs/class_func_reports/CUser/_IsCompoundResult2MoreEquip.md。ORIG 0x086720d0。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x086720d0：p167=CLEAR，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x086720d0：N82/N86 封顶——迭代器 sret 槽序/帧差
// （4.1 恒临时槽+拷贝，4.4 直构进目标槽；p167 CLEAR，gap 1）。
#include "CUserOnlyOrig/_IsCompoundResult2MoreEquip.inc"
}

// ---------------------------------------------------------------------------
// 7) CUser::_onChangeGrowType(uchar, uchar newSecond, eChangeGrowTypeReason reason)
//    报告：docs/class_func_reports/CUser/_onChangeGrowType.md。ORIG 0x08664080。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08664080：N82 封顶——帧/槽差（p167 CLEAR）。
#include "CUserOnlyOrig/_onChangeGrowType.inc"
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
// [b28d-22 N7/N82] ORIG 0x08664412：p167=CLEAR，gap=0；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig NEAR] ORIG 0x08664412：助记符序列相同；残余为寄存器/栈槽分配跨版本差（P112/N82 族），语义等价。
#include "CUserOnlyOrig/_onQuestClear.inc"
}

// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08671102：N82/N89 封顶——p167 差集未清零
// （memset/vector begin/end 族），gap -5。
// （memset/vector begin/end 族，见 /tmp/b28d_solo_CUserOnlyOrig_result.tsv）；
// 残余为帧槽/模板内联跨版本差（N82/N89 族），语义完成。
#include "CUserOnlyOrig/_ProcCompoundCore.inc"
}


// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08659490：p167=CLEAR，gap=5；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08659490：N82 封顶——帧/槽差 + x87（p167 CLEAR，gap 5）。
#include "CUserOnlyOrig/_processCharacterDayEvent.inc"
}

// ---------------------------------------------------------------------------
// 12) CUser::_processFatigueBuffEvent(uint, uint) —— 同型（DIV=.rodata, 1/2/3→0x1d95）
//     报告：docs/class_func_reports/CUser/_processFatigueBuffEvent.md。ORIG 0x086598ba。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/_processFatigueBuffEvent.inc"
}

// ---------------------------------------------------------------------------
// 13) CUser::_processUnlimitChallenge(unsigned int, char)
//     报告：docs/class_func_reports/CUser/_processUnlimitChallenge.md。ORIG 0x0864a5aa。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/_processUnlimitChallenge.inc"
}

// ---------------------------------------------------------------------------
// 14) CUser::_saveConditionEventInfo(uint, short, short)
//     报告：docs/class_func_reports/CUser/_saveConditionEventInfo.md。ORIG 0x086819fa。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/_saveConditionEventInfo.inc"
}

// ---------------------------------------------------------------------------
// 15) CUser::_saveProperDungeonClearCount(uint, short)
//     报告：docs/class_func_reports/CUser/_saveProperDungeonClearCount.md。ORIG 0x08681b3a。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/_saveProperDungeonClearCount.inc"
}

// ---------------------------------------------------------------------------
// 16) CUser::_updateSaveOption()
//     报告：docs/class_func_reports/CUser/_updateSaveOption.md。ORIG 0x08652dde。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08652dde：p167=CLEAR，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08652dde：N104 封顶——EH 尾块相对序：ORIG 正常路径
// 尾（dtor+epilogue）居函数最末、unwind 块居中；4.4 把 unwind 压最末、正常尾就地内联
// （块内容逐条全等，仅两尾块互换；p167 CLEAR，槽位集相同）。
#include "CUserOnlyOrig/_updateSaveOption.inc"
}

// ---------------------------------------------------------------------------
// 17) CUser::add_matched_list(vector<pair<int,int>>&, int key, int addValue)
//     报告：docs/class_func_reports/CUser/add_matched_list.md。ORIG 0x086650b2。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/add_matched_list.inc"
}

// ---------------------------------------------------------------------------
// 18) CUser::AddCurCharacMercenaryInfo()
//     报告：docs/class_func_reports/CUser/AddCurCharacMercenaryInfo.md。ORIG 0x0868e596。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/AddCurCharacMercenaryInfo.inc"
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
#include "CUserOnlyOrig/AddDailyBadge.inc"
}

// ---------------------------------------------------------------------------
// 20) CUser::AddTicket(int ticketIdx, bool notify)
//     报告：docs/class_func_reports/CUser/AddTicket.md。ORIG 0x08678282。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08678282：p167=M0/E5:_ZNSt20_List_const_iteratorIiEC1ERKSt14_List_iteratorIiE;_ZNKSt20_List_const_iteratorIiEdeEv;_ZNSt20_List_const_iteratorIiEppEv，gap=-1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08678282：N82/P28 封顶——list 迭代器族槽序
// （p167 M0/E5 迭代器转换构造，gap -1）。
#include "CUserOnlyOrig/AddTicket.inc"
}

// ---------------------------------------------------------------------------
// 21) CUser::alonePlay_Mode()
//     报告：docs/class_func_reports/CUser/alonePlay_Mode.md。ORIG 0x0868dc7a。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868dc7a：N 封顶——ORIG `movswl -0xa(%ebp),%eax;
// cmp $0x1,%eax`（4.1 short 槽比较 32 位化）；4.4 对 short 局部恒融 `cmpw $1,mem`
// （P310 形 int 局部+显式 cast 实测仍融/多槽，探针 /tmp/b28d_solo_cuo/p310.cpp g1..g6）。
#include "CUserOnlyOrig/alonePlay_Mode.inc"
}

// ---------------------------------------------------------------------------
// 22) CUser::alonePlay_Mode(MSG_REQ_TO_PEER const&)
//     报告：alonePlay_Mode_0868dcbc.md。ORIG 0x0868dcbc。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868dcbc：N 封顶——同 alonePlay_Mode()：ORIG
// `movswl -0xa(%ebp),%eax; cmp $1,%eax` vs 4.4 `cmpw $1,mem`（short 比较 lowering 跨版本差）。
#include "CUserOnlyOrig/alonePlay_Mode_1.inc"
}

// ---------------------------------------------------------------------------
// 23) CUser::apply_elixir_item(_Additioal_info&, pair<int,int>&)
//     报告：docs/class_func_reports/CUser/apply_elixir_item.md。ORIG 0x08665274。
//     switch(first)：0x40f..0x412 → 短字段+count*50；0x413/0x414 → int+count*250；
//     0x415 → int@+0x42+count*10；0x416 → 循环 4 个 short+=count*10。
//     _Additioal_info 内字段由 FixAdditionalInfoLeaves 批建模；此处按 m_pad 视图改具名草案。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08665274：p167=CLEAR，gap=-2；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08665274：N7 封顶——数组值/索引的 callee-saved
// 角色互换（ORIG ecx=索引/ebx=值，4.4 ebx=索引/ecx=值）+ lea-vs-add；130 vs 128 条。
#include "CUserOnlyOrig/apply_elixir_item.inc"
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
#include "CUserOnlyOrig/CalLevelUpItemCheck.inc"
}

// ---------------------------------------------------------------------------
// 27) CUser::CalLevelUpItemState(char&, int&, int, int)
//     报告：docs/class_func_reports/CUser/CalLevelUpItemState.md。ORIG 0x08689d74。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08689d74：p167=CLEAR，gap=-4；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08689d74：N82 封顶——push edi/esi callee-saved 差
// （p167 CLEAR，gap 4）。
#include "CUserOnlyOrig/CalLevelUpItemState.inc"
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
// [b28d-22 N82] ORIG 0x08676296：p167=M0/E6:_ZN9__gnu_cxx17__normal_iteratorIPKSt4pairI10Inven_ItembESt6vectorIS3_SaIS3_EEEC1IPS3_EERKNS0_IT_NS_11__enable_ifIXsrSt10__are_sameISC_SB_E7__valueES8_E6__typeEEE;_ZNK9__gnu_cxx17__normal_iteratorIPKSt4pairI10Inven_ItembESt6vectorIS3_SaIS3_EEEptEv;_ZN9__gnu_cxx17__normal_iteratorIPKSt4pairI10Inven_ItembESt6vectorIS3_SaIS3_EEEppEv，gap=-50；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08676296：N86/P28 封顶——p167 MISSING
// `normal_iterator<...>::operator++(int)`×2（ppEi），EXTRA `operator->`×2（ptEv）；
// 4.1 后置 ++ 独立 sret 临时槽不可复现。
#include "CUserOnlyOrig/check_insert_lottery_reward.inc"
}

// ---------------------------------------------------------------------------
// 30) CUser::check_locked_multibox_error(CItem const*, int slotCntHint, bool doubleMode)
//     报告：docs/class_func_reports/CUser/check_locked_multibox_error.md。ORIG 0x08675fd2。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08675fd2：p167=CLEAR，gap=4；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08675fd2：N82 封顶——槽基差 8（ORIG -0x59 vs
// 4.4 -0x61）+ 帧差（p167 CLEAR）。
#include "CUserOnlyOrig/check_locked_multibox_error.inc"
}

// ---------------------------------------------------------------------------
// 31) CUser::check_multibox_error(int requiredEmptySlots)
//     报告：docs/class_func_reports/CUser/check_multibox_error.md。ORIG 0x08675edc。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08675edc：P168a 裸短路链（callee bool）一次归零
// （具名 uchar r + 逗号绑定形令 4.4 落 -0x9 槽 + cmpb；见函数内注释）。
#include "CUserOnlyOrig/check_multibox_error.inc"
}

// ---------------------------------------------------------------------------
// 32) CUser::check_unlocked_multibox_error(CItem const*, int addInfo)
// 32) CUser::check_unlocked_multibox_error(CItem const*, int addInfo)
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/check_unlocked_multibox_error.inc"
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
#include "CUserOnlyOrig/checkCodeChecksum.inc"
}

// ---------------------------------------------------------------------------
// 35) CUser::checkCodeChecksumRange(CodeHackChecksumEx const&, CodeHackCheckRangeEx const&)
//     报告：docs/class_func_reports/CUser/checkCodeChecksumRange.md。ORIG 0x0867ea5c。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0867ea5c：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/checkCodeChecksumRange.inc"
}

// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0864a830：dead gap 局部 4→8 字节
// （unsigned int→unsigned long long）复现 4.1「tt 槽基 -0x20 + 上方 8B 空洞」
// （帧 0x34 不变：4.4 带 call 帧恒 16k+4 量化，locals+outgoing 44→48 仍落 0x34）
// ⇒ AE。P232/P31 扩展：dead 局部宽度按 ORIG 空洞宽度定。
#include "CUserOnlyOrig/CheckDeleteCharacTime.inc"
}

// ---------------------------------------------------------------------------
// 37) CUser::CheckDungeonClear(int questId)
//     报告：docs/class_func_reports/CUser/CheckDungeonClear.md。ORIG 0x0866d934。
// ---------------------------------------------------------------------------
// CUser.h:3620 已改 int（Main 落，ORIG 0x0866d96a 尾 call item_reward_quest; leave 直返 int）
#include "CUserOnlyOrig/CheckDungeonClear.inc"
}

// ---------------------------------------------------------------------------
// 38) CUser::CheckGuildLevelUp(unsigned int& priceOut)
//     报告：docs/class_func_reports/CUser/CheckGuildLevelUp.md。ORIG 0x0866c27e。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0866c27e：本波五修归零（113=113 逐条全等，check_tu AE+1）
// ——①param 静态类型改指针（ORIG `mov 0x4(%eax)` 单条 disp）；②price 预清两句式（P198①）；
// ③needFund/db 内联表达式（P0/P48，消 8B 槽深差）；④比较正极性 `price > fundMoney`（P196）；
// ⑤trace 全表达式临时链（P227，变参值 %ebx 跨 ctor 保活）+ price int 化（P211 档③）+
// GuildParamView.m_value int 化（P254/P265 两段中转）。
#include "CUserOnlyOrig/CheckGuildLevelUp.inc"
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
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0868f9e8：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/checkInformNoticeFlag.inc"
}

#include "CUserOnlyOrig/CheckItemLockSlotSpace.inc"
}

// ---------------------------------------------------------------------------
// 42) CUser::CheckLimitMiniumAge(int ageLimit, int& uniqueLow16)
//     报告：docs/class_func_reports/CUser/CheckLimitMiniumAge.md。ORIG 0x0864fc3e。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/CheckLimitMiniumAge.inc"
}

// ---------------------------------------------------------------------------
// 43) CUser::CheckPossibleRequestRenameState(int slot) const
//     【报告缺失@0x0822ff62 size 0x60】objdump 全文转写。
//     slot 越界(<0 或 >=m_characList.size()) → false；
//     否则返回 m_characList[slot].m_viewState(+0x14c6)==0xff。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0822ff62：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/CheckPossibleRequestRenameState.inc"
}

// ---------------------------------------------------------------------------
// 44) CUser::checkSelectCharac(int slot)
//     报告：docs/class_func_reports/CUser/checkSelectCharac.md。ORIG 0x0864aaf6。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864aaf6：p167=M0/E0:，gap=-7；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864aaf6：修法（本轮）——trace 字符串改
// __PRETTY_FUNCTION__ 直压立即数（P118/P139，消静态槽装载）；lvl 改表达式直用 + 嵌套 if
// （P200/P225）复现 `movzwl 0x27(%eax),%eax; movswl %ax,%ebx` + 两次完整下标链 operator[]。
// 残余：首守卫 `jle` vs 4.4 `jg`（P258 德摩根 or 形）+ setcc 物化 + 帧差
// （ORIG push ebx/sub $0x24 vs 4.4 sub $0x38，N115 outgoing 预留）⇒ N82/N115 封顶。
#include "CUserOnlyOrig/checkSelectCharac.inc"
}

// ---------------------------------------------------------------------------
// 45) CUser::CheckTicket(int ticketIdx)
//     报告：docs/class_func_reports/CUser/CheckTicket.md。ORIG 0x086785f4。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/CheckTicket.inc"
}

// ---------------------------------------------------------------------------
// 46) CUser::ClearChildrensDayEvent() 【报告缺失@0x0832815a size 0x63】objdump 全文转写。
//     标志 @+0x8e098 清零后，将 +0x23824/4 与 +0x23830/4 两表（每项 stride 4，
//     固定内偏移 0xc / 0x4 处的 int）置 -1，i = 0..9（cmpl $9; setle 循环界）。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/ClearChildrensDayEvent.inc"
}

// ---------------------------------------------------------------------------
// 47) CUser::ClearMission(short missionId)
//     报告：docs/class_func_reports/CUser/ClearMission.md。ORIG 0x08662606。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/ClearMission.inc"
}

// ---------------------------------------------------------------------------
// 48) CUser::ClearMissionKind(short kind)
//     报告：docs/class_func_reports/CUser/ClearMissionKind.md。ORIG 0x086626a0。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/ClearMissionKind.inc"
}

// ---------------------------------------------------------------------------
// 49) CUser::ClearSkills()
//     报告：docs/class_func_reports/CUser/ClearSkills.md。ORIG 0x0867b4c4。
//     条件（xor $1 后 jne 双取反 ⇒「必须同时不成立」）：
//       flag(+0x711d8)==0 且 getCurCharacR()!=0 → 返回 false；
//       否则 SkillSlotW.debugCommandResetSkill() 并返回 true。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0867b4c4：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/ClearSkills.inc"
}

#include "CUserOnlyOrig/collectPcRoomPlayTime.inc"
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
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0869304e：P285 空 else{return;} 复现正常路径尾
// `jmp <epilogue>; nop` 岛（纯正向 if 被 4.4 jump-thread）⇒ AE，见函数内注释。
#include "CUserOnlyOrig/compoundGuildMessage.inc"
}

// ---------------------------------------------------------------------------
// 5. CUser::compoundSystemMessage(Inven_Item const&) T 0x08692f62
#include "CUserOnlyOrig/compoundSystemMessage.inc"
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
// [b28d-22 N82] ORIG 0x08671eb2：p167=CLEAR，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08671eb2：N7/N18a 封顶——位域 and/or 段寄存器分配
// 互换（ORIG eax→ecx→edx 链 vs 4.4 edx 直算）+ 局部槽序；95 vs 96 条。
#include "CUserOnlyOrig/copyItemOption.inc"
}

// ---------------------------------------------------------------------------
// 10. CUser::count_specific_items(vector<int>&, vector<pair<int,int>>&) T 0x0866514a
#include "CUserOnlyOrig/count_specific_items.inc"
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
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08652a58：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/decideCharacLinkMessage.inc"
}

// ---------------------------------------------------------------------------
// 15. CUser::decideLinkCharacSlotIndexes(char, int) T 0x08652b06
#include "CUserOnlyOrig/decideLinkCharacSlotIndexes.inc"
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

#include "CUserOnlyOrig/DeleteCharac.inc"
}

// ---------------------------------------------------------------------------
// 18. CUser::DeleteCurrentCharacRentalItemInfo(int) T 0x08677050
#include "CUserOnlyOrig/DeleteCurrentCharacRentalItemInfo.inc"
}

// ---------------------------------------------------------------------------
// 19. CUser::deleteToBlackList(unsigned int) T 0x0867f040
#include "CUserOnlyOrig/deleteToBlackList.inc"
}

// ---------------------------------------------------------------------------
// 20. CUser::DelTradePunishType(int) T 0x086898c2
#include "CUserOnlyOrig/DelTradePunishType.inc"
}

// ---------------------------------------------------------------------------
// 21. CUser::discard_mouse_register() T 0x08680bd2
#include "CUserOnlyOrig/discard_mouse_register.inc"
}

// ---------------------------------------------------------------------------
// 22. CUser::DoItemLock(int, int) T 0x08646d4a
#include "CUserOnlyOrig/DoItemLock.inc"
}

// 23. CUser::DoItemUnlock(int, int) T 0x08646d82
#include "CUserOnlyOrig/DoItemUnlock.inc"
}

// 24. CUser::DoItemUnlockCancel(int, int) T 0x08646dba
#include "CUserOnlyOrig/DoItemUnlockCancel.inc"
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
#include "CUserOnlyOrig/QuestClearDeliveryMsg.inc"
}

// [b28d-22 N82] ORIG 0x086645a4：p167=CLEAR，gap=-1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x086645a4：末位实参套 P226 裸语句表达式
// `({ GetServerGroupToPvP(); })` ⇒ 4.4 把该值升格 %esi（原内联实参落 %ebx），
// 复现 ORIG「sg→%esi、cno→%ebx、accId→%eax」三实参 callee-saved 分配 ⇒ AE。
// 新范式（P226 扩展）：裸语句表达式可翻转多实参 callee-saved 的 ebx↔esi 选择。
#include "CUserOnlyOrig/eventDeliveryQuestClear.inc"
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
#include "CUserOnlyOrig/get_ingame_event_history.inc"
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
// [b28d-22 N82] ORIG 0x08682082：p167=CLEAR，gap=-6；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08682082：N82 封顶——JMP/SLOT/ALU 残差
// （p167 CLEAR，gap -6）。
#include "CUserOnlyOrig/getCompoundUpgrade.inc"
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
// [b28d-22 N82] ORIG 0x0868b106：p167=CLEAR，gap=5；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868b106：P30/P311 成员数组视图 + 常量索引
// （`m_counts[i+0x23ac8]`）复现 ORIG `add $0x23ac8,%edx; mov 0x8(%eax,%edx,4)` 两步寻址
// （旧 reinterpret_cast<int(&)[7]> 产 add 基址形）；视图元素与 m_addInfo 同型化（P248 档③）。
// 残余 this/idx 装载序 + lea-vs-add（P30 结构性伪影；76=76 条，指令数已对齐）。
#include "CUserOnlyOrig/getInDungeonItemSellCount.inc"
}

#include "CUserOnlyOrig/set_charac_guildkey_in_login.inc"
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
#include "CUserOnlyOrig/incPcRoomPlayTimeRewardCnt.inc"
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
// [b28d-22 N/P197④] ORIG 0x08672031 `cmp 0xc(%ebp),%eax; jge <mov$0 尾块>`（4.1 对
// 寄存器比较结果直跳）。4.4 对 `chance <= get_rand_int(100)` 恒物化 setge+test+je
// （探针 /tmp/p22_ge.cpp a..g 六形：setge/setl + 双 mov 岛，无直跳形）⇒ 封顶；
// 语义等价（chance<=rand ⇔ rand>=chance）。
// [b28d-solo-CUserOnlyOrig N 封顶] ORIG 0x08671fde：N（P197④ 镜像负例）——ORIG `cmp 0xc(%ebp),
// %eax; jge <mov$0 尾块>` 直跳；4.4 对 `chance <= rand` 与正极性 `rand >= chance` **同产**
// `setge+test`（本波 fresh 复验，34 条；反向 `chance > rand` 产 setl）⇒ 源形不可择，语义等价。
#include "CUserOnlyOrig/isSuccessCompound.inc"
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

// ORIG 0x08681160 T：安全保护/私摊忙碌/幽灵/交易/突袭/PVP频道 六重门
#include "CUserOnlyOrig/isEnableGiveGiftToNPC.inc"
}

// ============================ 交易金币累加 ==================================

// ORIG 0x0864658a T：累加并跨阈值（首次越过 10000000）向 log 队列投递 0xd0 记录；
// 尾部按当前角色存在与否增计日交易额。
#include "CUserOnlyOrig/IncrementTradeGold.inc"
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
#include "CUserOnlyOrig/LoadMileage.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0866ad92 CUser::lost_ext(int percent, int& actualLost, bool hardMode)
// 返回 unsigned char（movzbl 局部 ret，无 setne 规范化，P9）
// 注：hardMode 两分支中扣减调用（ORIG call 0x86967f0）传参按 ORIG 汇编
// （ae4d 载 -0x18=lvExpA）；Ghidra 反编译标注为 subtractCurCharacExp(loss)，
// 以 ORIG 常量为准——落地时两分支语义为「exp 高于本级下限全额扣除 / 否则
// 置回下限」的等价表达，与报告 Ghidra 文本一致。
#include "CUserOnlyOrig/lost_ext.inc"
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
// [b28d-22 N96+N18a] ORIG 0x08646941 尾 `xor $0x1,%eax; test %al,%al; leave; ret`
// 的 test 是 4.1 bool 归一 dead test（无分支消费者）。4.4 实测（/tmp/p22_unlock.cpp
// g1..g7，446h46 路由）：uint 形只出 `xor; leave; ret`（缺 test），bool 形反产
// test/sete/movzbl（P168a③）⇒ 无源形，封顶；语义等价（test 不改返回值）。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08646912：P168a③ 封顶——ORIG 尾 `xor $1,%eax;
// test %al,%al; leave; ret` 的 test 是 4.1 bool 归一 dead test（无分支消费者）；4.4 uint/bool
// 两形均只出 `xor; leave; ret`（g1..g7 探针）⇒ 语义等价，仅缺 1 条 dead test。
#include "CUserOnlyOrig/OnItemUnlockWaitTimeout.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x08681368 CUser::OnPunishUser() void  (circulation field 0x1c)
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08681368：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/OnPunishUser.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x08681312 CUser::OnRestrictTrade() void  (circulation field 0x1d)
#include "CUserOnlyOrig/OnRestrictTrade.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x086800c6 CUser::onSelectCharacter() void
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x086800c6：调用点/形态已对齐（check_tu_routed 口径
// AE，p167 CLEAR）。
// 掉落穿透(MCAP)衰减：+0x8e40c(=m_mcap.m_field1c) 初值 100，
// 0..9 格扫背包计数金币道具 -> 扣 40 与 10*n；
// 第 10..21 格(跳过 11)数剩余格 -> >8 再扣 20；pvpGrade>9 再扣 20。
#include "CUserOnlyOrig/onSelectCharacter.inc"
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
// [b28d-solo-CUserOnlyOrig NEAR] ORIG 0x0867ee28：P167 差集清零（迭代器族 begin/end/ne/de/ppEv
// + normal_iterator::C1Ev 默认构造 ⇒ P50「先声明后赋值」；旧索引形 EXTRA operator[]/size）
// + P214③ `char buf[1024]={0}` 复现 rep stos（66→98 条，gap -32→0）；残余槽基 -0x18 vs
// -0x10（N95/N82 帧基族）。
#include "CUserOnlyOrig/printBlackList.inc"
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
// [b28d-22 N82] ORIG 0x08675ac6：p167=M0/E1:_Z14G_CDataManagerv，gap=4；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08675ac6：N82 封顶——槽基差 4（-0x61 vs -0x65）
// + setcc（p167 CLEAR，gap 4）。
#include "CUserOnlyOrig/process_use_locked_multibox.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x08675d0c CUser::process_use_unlocked_multibox(
//   int keySlot, int boxSlot, vector<pair<Inven_Item,bool>>& dest) int
//   （参数名以 ORIG 装载序为准：GetInvenSlot(1,0xc)/check(.,0x10)/delete(1,0xc)）
#include "CUserOnlyOrig/process_use_unlocked_multibox.inc"
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
#include "CUserOnlyOrig/processReturnUser.inc"
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
// [b28d-22 N82] ORIG 0x0866ea92：p167=CLEAR，gap=4；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0866ea92：N82 封顶——帧/槽差（p167 CLEAR，gap 4）。
#include "CUserOnlyOrig/quest_reward.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x08647ffc CUser::Recv() uint  (报告 CUser/Recv.md)
#include "CUserOnlyOrig/Recv.inc"
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
// ORIG 0x0867efae CUser::registerToBlackList(unsigned int accId) bool
// [b28d-Main·检查点14] 返回类型 uint→bool（调用点 0x084d0e51 `call; xor $1; test %al; je`）；
// mangled 不含返回类型，符号名不变；定义尾 ORIG 为 mov $1/mov $0 两岛，与 bool 同形。
#include "CUserOnlyOrig/registerToBlackList.inc"
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
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0864f930：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/reset_tag_info_to_me.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0867f1a4 CUser::resetBlackList() void
#include "CUserOnlyOrig/resetBlackList.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0866259e CUser::ResetDailyMission() void
#include "CUserOnlyOrig/ResetDailyMission.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0866c196 CUser::resetGuildDBInfo() void
#include "CUserOnlyOrig/resetGuildDBInfo.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0867f720 CUser::resetHumanCertifyData() void
#include "CUserOnlyOrig/resetHumanCertifyData.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0866265a CUser::ResetMissionList() void
#include "CUserOnlyOrig/ResetMissionList.inc"
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
#include "CUserOnlyOrig/SaveBackupSkillInfo.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x086528b4 CUser::saveCharacLinkMessageFlag(bool flag) void
#include "CUserOnlyOrig/saveCharacLinkMessageFlag.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x086501c8 CUser::SaveInventoryOnly() uint
// [b28d-22 N82] ORIG 0x086501c8：p167=CLEAR，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [A06 2026-09-10 归零] 192/192 AE/identical（delta=0）。修法 = P259：删 `Stream* s`
// 具名局部，Acquire 结果直落 CStreamGuard ctor 实参（ORIG 0x086501c8 段
// `call Acquire; mov %eax,0x4(%esp)`，无 stream 槽；旧形多一对 store/load 并顶浅全帧）。
// 此项原判 N82 已作废。
#include "CUserOnlyOrig/SaveInventoryOnly.inc"
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
#include "CUserOnlyOrig/SaveMileage.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0868bf14 CUser::select_charac_set_visible_values(char value) void
#include "CUserOnlyOrig/select_charac_set_visible_values.inc"
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
// [b28d-22 N82] ORIG 0x0868d1ec：p167=CLEAR，gap=2；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868d1ec：N121/P63 封顶——三散布早退 4.4 各产
// jmp 中继岛（4 vs ORIG 2）；本轮合并守卫形实测反劣（90→94 条）⇒ 回退独立守卫。
#include "CUserOnlyOrig/send_broadcast_lottery_gold_message.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0868bf98 CUser::send_charac_visible_data() void
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868bf98：N115/N82 封顶——outgoing 区预留差致
// PacketGuard 槽基 -0x1c(ORIG) vs -0x20(4.4)（136=136 条，仅槽基平移 4）。
#include "CUserOnlyOrig/send_charac_visible_data.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0868e0ee CUser::Send_SoloPlay_Reject_NotiPack(
//     char const*, ENUM_SOLO_PLAY_REJECT_MODE, ENUM_SOLO_PLAY_REJECT_MSG)
#include "CUserOnlyOrig/Send_SoloPlay_Reject_NotiPack.inc"
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
#include "CUserOnlyOrig/sendCharacLinkMessage.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0868f0e8 CUser::sendCharacOption() void
#include "CUserOnlyOrig/sendCharacOption.inc"
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
#include "CUserOnlyOrig/SetMissionParameter.inc"
}

// ---------------------------------------------------------------------------
// 3. CUser::setPcRoomPlayTimeData @0x0868f808
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/setPcRoomPlayTimeData.inc"
}

// ---------------------------------------------------------------------------
// 4. CUser::SetPuUser @0x08692b58
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig IDENTICAL] ORIG 0x08692b58：**P211 档③新翻正位（本波）**——uchar 成员
// setter 的 store 序由「RHS 左值类型」决定：4.4 对 uchar 字段 + bool 右值恒 RHS-first，
// 把目标改 **bool 同址视角**（CUser.h:2350-2354 匿名 union `m_bPuUser796d8`，Main 代落）
// 即翻为 ORIG 的 LHS-first 5 条形（探针 /tmp/pu5.cpp u0：`mov %al,-0x4; mov 0x8,%eax;
// movzbl -0x4,%edx; mov %dl,0x796d8(%eax)`），逐条与 ORIG 全等；uchar 视角保留，
// Inter_LoadPuUserCheck 消费点零改动。
#include "CUserOnlyOrig/SetPuUser.inc"
}

// ---------------------------------------------------------------------------
// 5. CUser::SetPvpRecord @0x0864deea —— 返回恒 1(undefined4 mov $1)
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/SetPvpRecord.inc"
}

// ---------------------------------------------------------------------------
// 6. CUser::setQuestPoint @0x0868acfe
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/setQuestPoint.inc"
}

// ---------------------------------------------------------------------------
// 7. CUser::SetReceivedRequestType @0x082308cc W(报告缺失,objdump 直转)
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/SetReceivedRequestType.inc"
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
// [b28d-22 N82] ORIG 0x0867306e：p167=CLEAR，gap=-3；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0867306e：N82 封顶——帧/槽差（p167 CLEAR，gap -3）。
#include "CUserOnlyOrig/setRecipeEqiupmentOption.inc"
}
// ---------------------------------------------------------------------------
// 9. CUser::SetServerGroup @0x082300ac W(报告缺失,objdump 直转)
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/SetServerGroup.inc"
}

// ---- body_F.cpp 续篇 A(与主文件按序拼接后编译)----

// ---------------------------------------------------------------------------
// 10. CUser::SetSkill @0x0864db20
//     SIG_LOAD_SKILL 包内布局视图(+0x8 sp1 / +0xc bufA / +0x198 尺寸常量 /
//     +0x1a4 sp2 / +0x1a8 bufB / +0x340 tail1 / +0x344 tail2 / +0x348 cmd 200B)。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864db20：p167=CLEAR，gap=-6；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864db20：N82 封顶——push edi/esi callee-saved 差
// + 帧（p167 CLEAR）。
#include "CUserOnlyOrig/SetSkill.inc"
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
// [b28d-22 N82] ORIG 0x08654924：p167=M0/E3:_ZNKSt17_Rb_tree_iteratorISt4pairIKi18STSkillCommandDataEEneERKS4_;_ZSt9make_pairIRiR18STSkillCommandDataESt4pairINSt17__decay_and_stripIT_E6__typeENS4_IT0_E6__typeEEOS5_OS8_;_ZNSt4pairIKi18STSkillCommandDataEC1IiS1_EEOS_IT_T0_E，gap=26；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08654924：N82/N89 封顶——map/pair 模板实例差集
// （p167 M0/E3，gap 26）。
// [Lane6 2026-09-10 P172/P183 一试] 删具名指针 `e` 改宏展开表达式直取 → 实测**反劣**
// （200→233 vs ORIG 172）：4.4 对 `reinterpret_cast<uchar*>(&m_skillCommands[i])[k]`
// 恒展开完整 add 链（N4/N102 族；ORIG 的 `movzbl 0xb(%eax,%edx,4)` 单条 scaled 不可
// 复现）⇒ 已回滚为具名指针。memcpy 长度保留 ORIG 具名槽形（-0x18）。
// 附：`int size=0xc8` 局部试形亦反劣（200→204：4.4 每处回读槽 +store/load 各 1，
// 而立即数形零开销；ORIG 的「常量→槽→回读」两跳不可复现）⇒ 一并回滚为立即数。
#include "CUserOnlyOrig/setSkillCommandData.inc"
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
// [b28d-22 N82] ORIG 0x08690098：p167=M0/E0:，gap=-23；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08690098：P196 有符号界判 + P214① 正极性包裹
// + P30 显式元素取址（strncpy src）三修归零（见函数内注释）。
// [Lane3 spec 2026-09-10] 返回类型 int→bool（CUser.h:3753 已 bool 化）：ORIG 调用点
// 0x084c360b `call; xor $0x1,%eax; test %al,%al; je`（P292/T3）。mangled 不含返回类型
// ⇒ 符号名/ABI 零变化；定义随头（C++ 要求声明与定义返回类型一致）。
#include "CUserOnlyOrig/setTagCharacInfo.inc"
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
#include "CUserOnlyOrig/SetTradePunishOverThirtyDays.inc"
}

// ---------------------------------------------------------------------------
// 16. CUser::setUsedBreakAwayLuckPoint @0x086971ee W(报告缺失,objdump 直转)
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/setUsedBreakAwayLuckPoint.inc"
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
#include "CUserOnlyOrig/StartRecvEventWaiting.inc"
}

// ---------------------------------------------------------------------------
// 19. CUser::Update_ComboMission @0x0866280a
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/Update_ComboMission.inc"
}

// ---------------------------------------------------------------------------
// 20. CUser::update_ingame_event_history @0x0868d72c
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/update_ingame_event_history.inc"
}

// ---------------------------------------------------------------------------
// 21. CUser::update_record @0x08676768→0x08676876(map find/highscore/insert)
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08676876：p167=M0/E4:_ZNSt4pairIi14GameResultTypeED1Ev;_ZSt9make_pairIRiR14GameResultTypeESt4pairINSt17__decay_and_stripIT_E6__typeENS4_IT0_E6__typeEEOS5_OS8_;_ZNSt4pairIKi14GameResultTypeEC1IiS1_EEOS_IT_T0_E，gap=5；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08676876：N86/P135 封顶——p167 差集：MISSING
// getBattleRecordRefW×2 + `_Rb_tree_iterator::C1Ev` + `pair<const int,GameResultType>::C1`
// 右值版，EXTRA make_pair/`pair::D1`×2（4.1 临时槽+拷贝 vs 4.4 直构）。
// [b28d-PoolCUOO2 修成 2026-09-09] 按上述差集逐项翻形（RefW×3 重调 / iterator 默认构造
// 后赋值 / 具名 value_type 块作用域 D1 / 单出口 ret 槽 / 右值 pair 实参 / `GameResultType&`
// 具名引用物化）⇒ **p167 CLEAR**、gap 26→0、ndiff 92→32（残余=setcc/装载序 T5 族，
// N82 维持封顶）。
#include "CUserOnlyOrig/update_record.inc"
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
#include "CUserOnlyOrig/updateBlueMarbleEnterCount.inc"
}

// ---------------------------------------------------------------------------
// 25. CUser::UpdateBreakAwayDungeonClearCnt @0x086522ea
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x086522ea：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/UpdateBreakAwayDungeonClearCnt.inc"
}

// ---------------------------------------------------------------------------
// 26. CUser::UpdateBreakAwayLuckPoint @0x086521ec
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/UpdateBreakAwayLuckPoint.inc"
}

// ---------------------------------------------------------------------------
// 27. CUser::UpdateCharacLinkBonus @0x0865240c
//     实参写流序:(bonusState@+0x18 先物化) 0x14c/uid/state/exp/gold/bonusState
//     —— asm:local_10=param5 缓存;写入序 sig:uid,state(+0x14 参3),exp(参1),
//     gold(参2),bonusState。按 Ghidra operator<< 顺序照录。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0865240c：p167=CLEAR，gap=3；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0865240c：N82 封顶——槽号平移/callee-saved
// （p167 CLEAR）。
#include "CUserOnlyOrig/UpdateCharacLinkBonus.inc"
}

// ---------------------------------------------------------------------------
// 28. CUser::UpdateCharacNameInMercenary @0x084ec5e6 W(报告缺失,objdump 直转)
//     lea this+0x6ef6c → UserMercenaryInfoMgr::UpdateCharacName(uint,string const&)
//     (成员声明 NEEDS_DECL,ORIG _ZN20UserMercenaryInfoMgr16UpdateCharacNameEjRKSs)
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/UpdateCharacNameInMercenary.inc"
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

// [b28d-22 N82] ORIG 0x08676e3c：p167=M0/E4:_ZNK15CUserCharacInfo14getCurCharacNoEv;_ZNK9__gnu_cxx17__normal_iteratorIPKN20InstanceRentalSystem15DeletedRentItemESt6vectorIS2_SaIS2_EEEptEv;find_if 仿函数 ctor（旧匿名 namespace 形），gap=-7；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08676e3c：functor 归属已修正为全局作用域
// （`_ZN25FindRentalInfoByItemIndex...`，与 ORIG mangled 逐字一致）；残余 p167 差集为
// EXTRA `getCurCharacNo`/`normal_iterator::ptEv` 物化族，属 byte 层封顶差。
// [b28d-PoolCUOO2 真语义修复 2026-09-09] 旧 EXTRA 组实为**语义错**：ORIG 首 call
// getCurCharacR()（0x08676e4e）读 _Charac_info::m_characNo(+0x00) 后 `test/je`——
// characNo==0 整体跳过（旧重建无守卫会错误建 map 项）；循环 deref 用 (*del)（deEv）。
// 修后 **p167 CLEAR**、142 vs 139 gap 3、ndiff 125→115；本件修成连带使 TU identical
// 424→425、missing_in_game 265→264。
#include "CUserOnlyOrig/UpdateCurrentCharacRentalInfo.inc"
}

// ---------------------------------------------------------------------------
// 31. CUser::UpdateDimensionInout @0x08651fba
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08651fba：p167=CLEAR，gap=10；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08651fba：N82 封顶——帧差（147 vs 157）+ 槽序
// （p167 CLEAR，gap 10）。
#include "CUserOnlyOrig/UpdateDimensionInout.inc"
}

// ---------------------------------------------------------------------------
// 32. CUser::UpdateEquipslotFlag @0x0868aa54
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0868aa54：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/UpdateEquipslotFlag.inc"
}

// ---------------------------------------------------------------------------
// 33. CUser::UpdateMissionCondition @0x086625c8
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/UpdateMissionCondition.inc"
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
#include "CUserOnlyOrig/use_equipment.inc"
}

// ---------------------------------------------------------------------------
// 37. CUser::use_multibox_statistic @0x086767d2
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/use_multibox_statistic.inc"
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
#include "CUserOnlyOrig/UseCeraPoint.inc"
}

// ---------------------------------------------------------------------------
// 40. CUser::UseLimitItem @0x08688ae6
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08688ae6：p167=M0/E4:_ZNK9__gnu_cxx17__normal_iteratorIP14LimitItemUsageSt6vectorIS1_SaIS1_EEEdeEv;_ZN9__gnu_cxx17__normal_iteratorIP14LimitItemUsageSt6vectorIS1_SaIS1_EEEppEv，gap=5；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08688ae6：N82 封顶——帧/槽差（p167 CLEAR）。
#include "CUserOnlyOrig/UseLimitItem.inc"
}

// ---------------------------------------------------------------------------
// 41. CUser::UseMileage @0x08650a10
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/UseMileage.inc"
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
// [b28d-22 N73] ORIG 0x0868d198 `call isClearQuest; test %al,%al; je`（uchar 返回值
// 直判）。callee UserQuest::isClearQuest 权威型 unsigned char（ORIG 0x086ab920 直返
// 无归一，CUser.h:1268 b28c C28L9 定案）。4.4 对 uchar 返回值的任何 if 写法均物化
// （/tmp/p22_uchar.cpp a/b/c/d/e/g/h/i 八形：test+setne+test 或 cmpb 槽化）⇒ 跨版本
// bool/uchar ABI 差异，封顶；语义等价。
#include "CUserOnlyOrig/verifyDoingQuest.inc"
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
#include "CUserOnlyOrig/verifyStackableItemActionPlace.inc"
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
// [b28d-22 N82] ORIG 0x0866bce8：p167=CLEAR，gap=6；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0866bce8：N82 封顶——帧/槽差（p167 CLEAR，gap 6）。
// [A06 2026-09-10 必配对律消费侧获益] 3 处 `get_give_skill` 消费点之一（8665）在本件：
// D06 将 `CCharacter::get_give_skill` char→bool 落地后，我方 `bool got = …; if(!got)`
// 与该 bool callee **成对** ⇒ 复现 ORIG `call; mov %al,槽; movzbl; xor $1; test; je`
// （无 setne）⇒ 本件 247→**245** 条（gap −6→**−4**），源码零改动。
#include "CUserOnlyOrig/init_skill_tree.inc"
}

#include "GlobalData.h"        // GlobalData::s_systemTime_ / s_pSecuProtectionField 等
#include "CEnvironment.h"      // G_CEnvironment / Server_Envir 字段
#include "Cipher.h"            // G_Cipher
#include "CMailBoxHelper.h"    // WongWork::CMailBoxHelper::AssignMailBox / CMailBox::Init
#include "GameMasterCmd.h"     // CGameMasterCharacter（InitOnAccept static local）
#include "CPremiumLetheManager.h" // CPremiumLetheManager 完整类型（increase_status lethe 链）

unsigned int get_seed_from_date();   // ORIG T 0x086b1d25 _Z18get_seed_from_datev
// cSecuUserAction 类定义 + `Init` 外联定义已**前移至本 TU 文件头**（P327 T1 调序，
// 使 GI 键名 = `_GLOBAL__I__ZN15cSecuUserAction4InitEv` 与 ORIG 一致，见文件头注释）。
// ORIG 侧仅 sizeof=4 的状态对象；InitOnAccept 0x086493c8 +0x4a1 处 new。

// ORIG T 0x0827b402（外联定义发射 T 符号）。
#include "CUserOnlyOrig/IsSecurityStateOff.inc"
}

// ----------------------------------------------------------------------------
// CUser::InitOnAccept —— ORIG T 0x086493c8 _ZN5CUser12InitOnAcceptEi
// 报告 docs/class_func_reports/CUser/InitOnAccept.md 全文转写：
//   socket 挂接/标志清零 → PacketVerifier::Init → mailBox 分配&Init →
//   密钥握手包(Cipher 原始钥) → seed/随机数/stun 列表包 → Send/
//   StartRecvEventWaiting/setStdDropRate(100) → secu 系列子对象初始化 →
//   GM 调试角色指针(static local gameMasterCharacter, guard 版式与 ORIG 一致)。
// ----------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x086493c8：p167=M0/E2:_ZNKSt6vectorI5SK_IPSaIS0_EE4sizeEv;__cxa_guard_abort，gap=7；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x086493c8：p167 差集 MISSING
// `G_CEnvironment`/`CEnvironment::get_channel_name`/`strlen`/`vector<SK_IP>::operator[]`/
// `getHades` + EXTRA `size()`/`__cxa_guard_abort`；属依赖/重载差，需跨文件核对（本泳道不越界改）。
// [b28d-PoolCUOO2 复测 2026-09-09] MISSING 已全清（依赖族由跨波修复闭合）；残余仅
// EXTRA `__cxa_guard_abort`×1（static CGameMasterCharacter 构造 EH cleanup 岛，
// ORIG guard 版式无 abort 路径）——401 vs 413、ndiff 248→220。guard 发射由 ctor
// 异常性决定（他 TU 定义），本泳道不可控 ⇒ N 封顶。
#include "CUserOnlyOrig/InitOnAccept.inc"
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
// [b28d-22 N82] ORIG 0x086657fc：p167=M0/E5:_ZN15CUserCharacInfo23getCurCharacAddInfoRefWEv，gap=69；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x086657fc：N82 封顶——帧/槽差（p167 M0/E5，gap 69）。
// [b28d-BigCUserStat 2026-09-09] 判定表 if-else 链 → switch（P251/P261/P286/P324）：
// 4.4.7 对 switch(unsigned 局部) 复现 ORIG 4.1 的 `mov itemIdx; cmp $K,%eax; je/ja`
// 二分决策树（探针 /tmp/sw_probe.cpp 逐条同形；if-else 链恒退化 cmpl $K,mem+jne，
// 旧判词的「跨版本 lowering 不可翻」据此作废）。同时按 ORIG 汇编补 4 处体尾
// goto L08666990（0x08665d83 / 0x08665fce / 0x08666420 / 0x0866698c 实证；原链式
// else 会误落 L08666751 默认体 = 语义偏差）。itemIdx 用纯 register 存储类提示
// （P231 合法，非 asm 绑定）省 3 条 store/load。
// 读数：2757 vs ORIG 2703（gap -54，与改前持平未劣化）；structdiff 2068→1911、
// ndiff 2388→2287；TU 四数 427/862/0/59 不变；p167 CLEAR（418/418，C1↔C2/D1↔D2 镜像豁免）。
// 残余：mov +46 / jmp +15（尾部 switch(type) 段具名局部与跳转岛）、帧 0x20c vs 0x39c
// （N82/N126 族，P321 T5 不再动类型）。
// [A06 2026-09-10 必配对律消费侧获益 + 归因1分诊] 本件内 2 处 `get_give_skill` 消费点
// （9380/9462）在 D06 的 `CCharacter::get_give_skill` char→bool 落地并与我方 `bool got`
// 成对后：2757→**2753** 条（gap −54→**−50**），源码零改动、identical 净账 0。
// 另：本件 9 处 ORIG `setne` 经分诊（Main C05④ 四步）全为「call 结果 → 比较 → setne」
// = **归因 1（callee 返回类型差）**，非前端折叠；相关 callee 非本 TU，未单侧动（spec 已报 Main）。
#include "CUserOnlyOrig/increase_status.inc"
}

// W9 batch A: report-backed short accessors and container helpers.
#include "CUserOnlyOrig/complete_reward_list.inc"
}

#include "CUserOnlyOrig/IsWaitingStateCharac.inc"
}

#include "CUserOnlyOrig/GetUserSecretShop.inc"
}

#include "CUserOnlyOrig/getQuestPoint.inc"
}

// [b28d-solo-CUserOnlyOrig IDENTICAL] ORIG 0x082300d0：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/IsState.inc"
}

#include "CUserOnlyOrig/GetChangedGiftFatigueQuantity.inc"
}

#include "CUserOnlyOrig/GetReceivedRequestType.inc"
}

#include "CUserOnlyOrig/getPcRoomAuth.inc"
}

#include "CUserOnlyOrig/IsPCRoomUser.inc"
}

#include "CUserOnlyOrig/GetSlotIDX.inc"
}

#include "CUserOnlyOrig/SetAge.inc"
}

#include "CUserOnlyOrig/SetCurCharacUsedGiftFatigueQuantity.inc"
}

#include "CUserOnlyOrig/getAddQuestItemDropRateFromPremium.inc"
}

#include "CUserOnlyOrig/setCharacLinkBonusSetting.inc"
}

#include "CUserOnlyOrig/isCharacLinkBonusSetting.inc"
}

#include "CUserOnlyOrig/setBreakAwayRewardData.inc"
}

#include "CUserOnlyOrig/setAdvanceAltarIndex.inc"
}

#include "CUserOnlyOrig/getLinkCharacNo.inc"
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
    boost::shared_ptr<CBingo>* getBingoSystem();  // ORIG T 0x080c9fea（定义 BingoEventLink4.cpp）
};

#include "CUserOnlyOrig/initUserData.inc"
}

// ============================================================================
// CU_Fix-2 批次 —— TranslateLoginInfo 链接失败中的 CUser 域 16 符号
// （docs/class_func_reports/CUser/<Func>.md 全文 ## 汇编+## 反编译 C 已逐份读取）
// ============================================================================

#include "CUserOnlyOrig/SetBingoData.inc"
}

// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0867ecf0：P75 封顶——P319 整型加法+回 cast 已复现
// src 两段 `mov 0xc(%ebp),%eax; add $0x2,%eax`；残余为 4.4 先算 dest lea 致 src 中转 %edx
// （ORIG 先压 src 后 lea dest），属实参装载时刻跨版本差。
#include "CUserOnlyOrig/SetBirth.inc"
}

#include "CUserOnlyOrig/SetCharacMercenaryInfo.inc"
}

#include "CUserOnlyOrig/setCharacSlotCountInfo.inc"
}

#include "CUserOnlyOrig/getCharacSlotCountDefaultInfo.inc"
}

#include "CUserOnlyOrig/SetEventCreateDnfReward.inc"
}

#include "CUserOnlyOrig/SetGrowthCreatureEventInfo.inc"
}

// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08691bae：gap=1。修法（本轮）：memcpy 逐字段
// 改具名载荷视图直读直存（P235/P250，117→80 条）；视图 short 与目标 m_field10/12 同型
// 消 movzwl 中转（P211 档③）；`&&` 拆嵌套 if 复现双 setcc 段（P225 推广）。残余
// ORIG `movzbl field; xor $1,%eax; test %al,%al; je` vs 4.4 `cmp $1,%al; je`：
// 探针 pxor.cpp a1..a4 / pxor2.cpp a5..a8 实证——bool 字段+`!field` 才产 xor 形，
// 但字段 bool 化会使本函数写入侧（bool 源→bool 目标）由 RHS-first 翻 LHS-first
// （探针 setB vs setU），破坏已匹配的 `movzbl; mov %dl,disp` 形 ⇒ P168a④/N96+N18a 封顶。
#include "CUserOnlyOrig/SetGrowthWeaponEventInfo.inc"
}

// [b28d-22 N82] ORIG 0x08690be6：p167=M0/E0:，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08690be6：N/P11 封顶——ORIG 位域提取
// `movzbl; sar $0x4,%al`（8 位）；4.4 产 `movsbl %al,%eax`（显式 uchar 中转反多一条，5 vs 4）。
#include "CUserOnlyOrig/ValidateStrikerInfo.inc"
}

// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0868a9a4：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/sendReturnUserFirstLogin.inc"
}

#include "CUserOnlyOrig/set_acc_info.inc"
}

#include "CUserOnlyOrig/set_fatigue_count_db.inc"
}

// [b28d-22 N82] ORIG 0x0868d580：p167=M0/E3:_ZSt9make_pairIRiS0_ESt4pairINSt17__decay_and_stripIT_E6__typeENS2_IT0_E6__typeEEOS3_OS6_;_ZSt9make_pairIRiSt4pairIiiEES1_INSt17__decay_and_stripIT_E6__typeENS3_IT0_E6__typeEEOS4_OS7_;_ZNSt4pairIKiS_IiiEEC1IiS1_EEOS_IT_T0_E，gap=7；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868d664：N82 封顶——push esi/帧差（p167 CLEAR，gap 7）。
#include "CUserOnlyOrig/set_ingame_event_history.inc"
}

#include "CUserOnlyOrig/set_ingame_event_history_1.inc"
}

#include "CUserOnlyOrig/setCharacLevelUpGift_AccountOnce.inc"
}

#include "CUserOnlyOrig/initBingoData.inc"
}


// ---------------------------------------------------------------------------
// CUser::setBreakAwayRewardOrder @0x0831d4de（ORIG W sz0x11；报告缺失，
// nm 定址后 objdump 忠实转写：【推断】目标仅 4 字节 int 透传写 this+0x703b8，
// 该槽位在 CUser.h 处于 break-away 奖励域前、未建模字段区）
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/setBreakAwayRewardOrder.inc"
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
// [b28d-solo-CUserOnlyOrig IDENTICAL] ORIG 0x0868dd76：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/get_SoloPlay_msgType.inc"
}

// ---------------------------------------------------------------------------
// CUser::SendItemMakingSkillInfo() T 0x0868b00a sz0x39
// 报告：docs/class_func_reports/CUser/SendItemMakingSkillInfo.md
// GetCharacExpandDataR(this,10)（CUser.h:2715 声明，返回 CExpandEquipslot*，
// ORIG Ghidra 视图即 CItemMakingSkill* 槽）→ 非空则 CItemMakingSkill::
// Send_ItemMakingSkillList(this)。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/SendItemMakingSkillInfo.inc"
}

// ---------------------------------------------------------------------------
// CUser::sendBingoAddData() T 0x08691470 sz0x5b
// 报告：docs/class_func_reports/CUser/sendBingoAddData.md
// GetRepeatEvent(0x6a)->IsEventing(0)（vptr+0x34 槽实证，P5 直呼）；
// getBingoData()->m_isWin（+0x5 movzbl）非零才 sendDecorationObjectCnt(1)。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/sendBingoAddData.inc"
}

// ---------------------------------------------------------------------------
// CUser::SetGuildInfoProxy(STGuildProxy& proxy) T 0x0865cd0a sz0x48
// 报告：docs/class_func_reports/CUser/SetGuildInfoProxy.md
// ORIG：m_guildDB.m_fundLevel←proxy.m_level(+0x17)；memcpy(m_guildDB,&proxy,0x16)；
// m_guildDB.m_field95←proxy.m_powerSide(+0x18)。STGuildProxy 定义在
// CGuildServerProxy.h:39（0x16=名字区）。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0865cd0a：N102/P75 封顶——ORIG memcpy dest 为
// `mov this,%edx; add $0x8d020,%edx` 两段，4.4 恒折 `lea disp(%eax),%edx`（N102 七形全折，
// 唯一出 add 的裸偏移禁用）；src/this 装载时刻差属 P75。
#include "CUserOnlyOrig/SetGuildInfoProxy.inc"
}

// ---------------------------------------------------------------------------
// CUser::checkCharacLinkType(char linkPos, int slot) T 0x08652b90 sz0xfe
// 报告：docs/class_func_reports/CUser/checkCharacLinkType.md
// getCharacLinkType()==2：slot≤linkPos+1 或 linkPos 奇 → 拒；再验
// m_characList[linkPos+1].m_characLinkType==3。
// getCharacLinkType()==3：linkPos-1<0 或 linkPos 偶 → 拒；验 [-1]==2。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/checkCharacLinkType.inc"
}

// ---------------------------------------------------------------------------
// CUser::SendDungeonClearList() T 0x086781b4 sz0xcd
// 报告：docs/class_func_reports/CUser/SendDungeonClearList.md
// put_header(0,5)→put_short(getDungeonClearSize)→enumDungeonClear(this,
// fnEnumDungeonClear,&packet)→finalize(true)→Send。回调自由函数 ORIG 名
// fnEnumDungeonClear（_Z18fnEnumDungeonClearscPc 0x08678166；CUser.h 现声明
// fnEnumDungeonClears 为 19 字符错拼，mangled 不匹配——已报 Main 修正）。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/fnEnumDungeonClear.inc"

#include "CUserOnlyOrig/SendDungeonClearList.inc"
}

// ---------------------------------------------------------------------------
// CUser::SendEventBringUpInfo() T 0x08689968 sz0xba
// 报告：docs/class_func_reports/CUser/SendEventBringUpInfo.md
// getCurCharacNo()!=0 才发：put_header(0,0x196)→put_byte(
// getCurUseBringUpMaterialTotal)→finalize→Send。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/SendEventBringUpInfo.inc"
}

// ---------------------------------------------------------------------------
// CUser::SendTagCharacInfo() T 0x086903f8 sz0xf8
// 报告：docs/class_func_reports/CUser/SendTagCharacInfo.md
// getTagCharacR()!=0：byte@+0x14bc 非 1 则 W 侧置 1；clear→put_header(0,0x19e)
// →put_short(1)→MakeTagCharacInfo→finalize→Send。
// （已改善 ndiff 39→3：m_field14BC 判断改重复调用 getTagCharacR() 不缓存
// + ==0 早退形；残差为 `xor $0x1`（ORIG gcc4.4）vs `cmp $1;setne`（本工具链）
// 跨版本 ISel 单条，见报告）
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x086903f8：P248 档②（m_field14BC char→bool，Main
// 代落 CUser.h:719）+ 裸 `if(!field)` 内联形归零（原具名 uchar v + v^=1 落 -0x9 槽）。
#include "CUserOnlyOrig/SendTagCharacInfo.inc"
}

// ---------------------------------------------------------------------------
// ORIG 反汇编：getCurCharacR()==0 时 sete+test+jne 早退；Acquire 结果直接
// 作为 CStreamGuard 构造实参（无中间栈变量）。
#include "CUserOnlyOrig/requestCubePremiumInfoIfContracted.inc"
}
 // ---------------------------------------------------------------------------
// CUser::setCharacSlotLimit(unsigned char limit) W 0x0869755c sz0x1b
// 报告缺失（nm 0x0869755c 定址后 objdump 忠实转写）：
//   mov %al,-0x4; mov 0xc(%ebp)→eax? 实参经 -0x4 截断回读 movzbl 后写
//   this+0x8d245 = m_ceraUserInfo.m_field61（与 getCharacSlotLimit 读侧对称）。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/setCharacSlotLimit.inc"
}

// ---------------------------------------------------------------------------
// CUser::EnableCharacInfo(CACHE_SECTION section) W 0x081253f4 sz0x13
// 报告缺失（nm 0x081253f4 定址后 objdump 忠实转写）：
//   movb $1, 0x8d0fd(%edx,%eax,1) → this + section + 0x8d0fd
//   即 m_pad8d0fd[section] = 1（缓存段使能标志数组）。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/EnableCharacInfo.inc"
}

// ---------------------------------------------------------------------------
// CUser::SendRevengeDungeon() T 0x08656abc sz0x156
// 报告：docs/class_func_reports/CUser/SendRevengeDungeon.md
// GetRewardType()==1/2 写 type byte（其它 return）；remain=max(0,close-cur)；
// 10 字节 {type,1,remain:int,reserved:int} 经 put_binary(0x1ec 包)发送。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08656abc：N115/N82 封顶——全帧槽平移 4
// （ORIG -0x22/-0x18/-0x14 vs 4.4 -0x26/-0x1c/-0x18）；95 vs 97 条。
#include "CUserOnlyOrig/SendRevengeDungeon.inc"
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
    packet.put_binary(reinterpret_cast<char*>(&data), 10);   // ORIG E15InterfacePacketBuf10put_binaryEPci
    packet.finalize(true);
    Send(packet);
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
#include "CUserOnlyOrig/GetItemIdByAddInfo.inc"
#include "CUserOnlyOrig/setTagCharacInven.inc"
}

// ---------------------------------------------------------------------------
// CUser::SetInventory(SIG_LOAD_INVENTORY* sig) T 0x0864d160 sz0x35a
// 报告：docs/class_func_reports/CUser/SetInventory.md
// SIG_LOAD_INVENTORY 权威字段（SigTypes_C.h）填充；CInventory +0x1c/+0x55a
// 两个 memcpy 目标暂为布局偏移（字段化 owner=CInventory 域批次）。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864d160：p167=CLEAR，gap=11；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864d160：N82 封顶——帧/槽差（p167 CLEAR，gap 11）。
#include "CUserOnlyOrig/SetInventory.inc"
}

// ---------------------------------------------------------------------------
// CUser::SetLimitInfo(SIG_LIMIT_ITEM_INFO* info) T 0x086888f8 sz0x1ed
// 报告：docs/class_func_reports/CUser/SetLimitInfo.md
// 清空 m_limitItemUsage(+0x8cf68) 后遍历 CDataManager::GetLimitItemUsageInfo
// (m_info0.m_list)：sig 条目 itemId 匹配 LimitInfo.m_a → 填 usage{kind,m_b,m_a,
// entry.m_field0} + SetCreateTime(entry.m_createTime)；未匹配 → m_field19=0 +
// SetCurTime()。push_back 到 m_limitItemUsage。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x086888f8：p167=M0/E4:_ZN14LimitItemUsageD1Ev;_Unwind_Resume;_ZN9__gnu_cxx17__normal_iteratorIPN20stLimitItemUsageInfo9LimitInfoESt6vectorIS2_SaIS2_EEEppEv，gap=11；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x086888f8：P28 增量侧试形实测反劣（it++ 157 条 vs
// ++it 153，ORIG 142）⇒ 回退 ++it；p167 残余 ppEi×1 + LimitItemUsage D1×2/_Unwind_Resume
// （push_back EH landing，P216/N91 族）+ vector 模板 out-of-line（N89）；N82 封顶。
#include "CUserOnlyOrig/SetLimitInfo.inc"
}

// ---------------------------------------------------------------------------
// CUser::SelectCharac(int idx, int tagIdx) T 0x0864cd92 sz0x3cd
// 报告：docs/class_func_reports/CUser/SelectCharac.md
// 选择主角色/tag 角色：设 curCharac/set_parent 链、stamina 离线恢复
//（elapsed=min(600,difftime); min(stamina+elapsed*100/600,100)）、
// HistoryLog::WriteSelectCh、GameWorld::m_channelUserCount[job]++。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864cd92：p167=CLEAR，gap=16；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864cd92：N82 封顶——帧/槽差 + x87（p167 CLEAR，gap 16）。
#include "CUserOnlyOrig/SelectCharac.inc"
}

// ---------------------------------------------------------------------------
// CUser::set_MultimailMsgData(MSG_MULTI_MAILBOX_SEND* data) T 0x0868a4e8 sz0x32
// 报告：docs/class_func_reports/CUser/set_MultimailMsgData.md
// 0x8e3ec multimail flag=1 + memcpy(0x8e246, data, 0x1a6)。0x8e 区访问与
// InitOnAccept 既有写法一致（m_pad8e101 为 0x8e101 起块视图）。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/set_MultimailMsgData.inc"
}

// ---------------------------------------------------------------------------
// CUser::SendMail(bool certified) T 0x08689f12 sz0x3ca
// 报告：docs/class_func_reports/CUser/SendMail.md
// 0x8e101 MSG_MAILBOX_SEND 载荷（m_pad8e101 块视图）：name@+0x11/nameLen@+0xd/
// money@+0x2e/itemId@+0x39；multimail flag@0x8e3ec 分流；疲劳礼物金
// limit=(long long)(money*(0.7+(float)(rand*0.01)))（0x8cf3988=0.01、
// 0x8cf3990=0.7 实测）；SIG_FIND_FACTORY_HUB 0x24 块入 DB_Q。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08689f12：p167=CLEAR，gap=7；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08689f12：N82 封顶——帧/槽差 + setcc（p167 CLEAR，gap 7）。
#include "CUserOnlyOrig/SendMail.inc"
}

// ---------------------------------------------------------------------------
// CUser::sendDecorationObjectCnt(short count) T 0x08690d9a sz0x96
// 报告缺失（nm 0x08690d9a 定址后 objdump 忠实转写）：
// put_header(0,0x1ac)→put_short(count)→finalize(true)→Send。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/sendDecorationObjectCnt.inc"
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
// [b28d-22 N82] ORIG 0x0864a186：p167=M0/E1:memcpy，gap=-38；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864a186：修法（本轮）——删 invenBuf[0xcd0]/
// cargoBuf[0x4a] 缓冲，改在 info 内嵌子对象偏移（CInventory@0xda/CCargo@0xdaa）上直调
// ⇒ 帧 0x2200→0x14f0（ORIG 0x14fc）；memcpy(0x52) 改结构整体赋值复现 ORIG 内联 rep movsl
// （P83/N78）⇒ p167 CLEAR（15=15），130→172 条（ORIG 168）。残余槽基/初始化序差 4 条
// ⇒ N82/N78 封顶。
#include "CUserOnlyOrig/CreateCharac.inc"
}

// ---------------------------------------------------------------------------
// CUser::use_stackable(short slot, int space, int p3, int p4) T 0x0865e0ae sz0x8e5
// 报告：docs/class_func_reports/CUser/use_stackable.md（## 汇编+## 反编译 C 已完整阅读）
// 校验链（trade/find_item/is_stackable/CAlchemist/CoolTime）→use_item→
// BossTower/连续道具/CAlchemist 消费→LV70Buff 与疲劳药品表→结果包 0x2f。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/SendLV70Buff.inc"
#include "CUserOnlyOrig/use_stackable.inc"
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

// tagCharacVisibleFlag：权威类定义已入 CUser.h:530（L31CharacPad `_Charac_info` 成员化），
// 本 TU 删除局部重复声明（否则 redefinition）；成员函数定义见下（`m_flag = 0;`）。

// ---------------------------------------------------------------------------
// CUser::setCharacInfoDetail(int slot, unsigned int characNo, CHARAC_DATA* src)
// T 0x0864ac1a sz0xfe9 —— 报告：setCharacInfoDetail.md（## 汇编+## 反编译 C 全文）。
// slot/characNo 校验 → CHARAC_DATA 全字段搬运（成长型特化 7/8/0xc/0xd）→
// initCharacStat → 疲劳上限事件(2/0x57) → 疲劳电池错误分支 → LastPlayTick 等。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864ac1a：p167=M0/E1:_ZNSt6vectorI12_Charac_infoSaIS0_EEixEj，gap=26；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864ac1a：N82 封顶——帧/槽差（p167 M0/E1，gap 26）。
#include "CUserOnlyOrig/setCharacInfoDetail.inc"
}

// ---------------------------------------------------------------------------
// _Charac_info::Reset() T 0x086822b8 sz0x7d9
// 报告：docs/class_func_reports/_Charac_info/Reset.md（## 汇编+## 反编译 C 全文）。
// 全字段清零 + CommonTime::clear + _Additioal_info::clear + CInventory::reset +
// SkillSlot::clear_all_skills_both + CCargo::reset + 两 map clear +
// tagCharacVisibleFlag::reset + stAuraAvatarOption::reset。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/Reset.inc"
}

// ---------------------------------------------------------------------------
// CUser::SendMultiMail(bool certified) T 0x0868a2dc sz0x1d9
// 报告：docs/class_func_reports/CUser/SendMultiMail.md
// 0x8e246 MSG_MULTI_MAILBOX_SEND 块（m_pad8e101+0x145 视图）：name@+0x11、
// money@+0x2f、count@+0x33、条目 stride 0xb@+0x37（itemId 首字段）。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0868a2dc：p167=CLEAR，gap=-9；语义完成、byte 层帧槽/物化族封顶。
// [Lane6 2026-09-10 P183 一试] 删 `char* block` 缓存实测**反劣**（140→149 vs ORIG 128）：
// 4.4 对 `m_pad8e101 + 0x145` 恒发两段 `add $0x8e101,%eax; add $0x145,%eax`（N1 常量
// 折叠跨版本差），且每次重算抬高循环区寄存器压力 ⇒ 循环体形态整体错位；已回滚为具名
// 缓存（ORIG 单 add 无可择源形）。
#include "CUserOnlyOrig/SendMultiMail.inc"
}

// ---------------------------------------------------------------------------
// CUser::_onCreateCharacter(unsigned int characNo, char* name, char job)
// T 0x0864a432 sz0x178 —— 报告：docs/class_func_reports/CUser/_onCreateCharacter.md
// job==4 且 curSec<0x4936f351：create_item(3,item,1) 造 0x1d19 蛋 →
// 系统邮件（game_server_msg_56/57）→ _processUnlimitChallenge。
// ---------------------------------------------------------------------------
#include "CUserOnlyOrig/_onCreateCharacter.inc"
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
// [b28d-22 N82] ORIG ?：p167=M0/E28:_ZnwjPv;_ZdlPvS_，gap=211；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08491f86：N83 封顶——p167 EXTRA `_ZnwjPv`×20 +
// `_ZdlPvS_`×8（placement new 内联差异）。
#include "CUserOnlyOrig/SIG_LOAD_ETC.inc"
}

#include "CUserOnlyOrig/CharacterOption.inc"
}

// ORIG 0x08695ca4 _ZN19ExpBonusPointResultC1Ev/C2Ev：sub $0x18 后 call clear。
#include "CUserOnlyOrig/ExpBonusPointResult.inc"
}

// ORIG 0x08695cb8 _ZN19ExpBonusPointResult5clearEv：+0x00/+0x04 两 int 清零。
#include "CUserOnlyOrig/clear.inc"
}

#include "CUserOnlyOrig/tagCharacVisibleFlag.inc"
}

#include "CUserOnlyOrig/reset.inc"
}


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

// ---- _Charac_info ctor/dtor（LinkE2）----
// 收敛口径（2026-09-05 EmoC11，Main 裁决记封顶不批成员化）：ORIG W 0x08695cf8
// o=150 形 = 18 子对象按偏移升序隐式成员构造（每处 4 条 mov/add/mov/call T::C1）
// + 尾 call Reset；本重建 CUser.h（冻结）保留 pad 区、此处 placement new 补构，
// 4.4.7 对每处 placement operator new 发射 _ZnwjPv 实调用 + size 装载 + 判空 +
// _ZdlPvS_ EH 清理岛 ≈ +10 条 ⇒ n=304（we-bigger 154）。范式库 N83（SIG_LOAD_ETC
// ctor 49 处同族已豁免）×16；+0xecc/+0x1254 两真成员先构亦属声明序隐式形约束。
// 语义完成（18 ctor 与 Reset 全部真实对位 ORIG 符号），byte 层 N83 封顶，勿再耗轮次。
// [b28d-22 N82] ORIG ?：p167=M0/E1:_ZNSt3mapIi18STSkillCommandDataSt4lessIiESaISt4pairIKiS0_EEED1Ev，gap=-36；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08695cf8：N83 封顶——p167 EXTRA `_ZnwjPv`×16 +
// `_ZdlPvS_`×9（4.1 placement new 前端内联 0 call，4.4 恒 extern call+EH cleanup）。
#include "CUserOnlyOrig/_Charac_info.inc"
}

// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08697982：去 `char* p=this` 具名缓存（P0/P183）后
// 5 个成员析构调用形 `mov this; add off; mov (%esp); call` 与 ORIG 逐点一致、帧 0x20→0x10
// 对齐（59 条 vs ORIG 97）；残余为 ORIG EH 双份 dtor epilogue vs 4.4 merge（跨版本 EH 块
// 策略 N）+ vector D1 out-of-line 实例（N89）。
#include "CUserOnlyOrig/dtor__Charac_info.inc"
}
// ORIG 0x08699863：编译器生成的拷贝构造。平凡区（含 _Additioal_info@0x88、
// CCargo@0xdaa 等无堆成员子对象）逐字段拷贝；非平凡子对象调用拷贝构造。

// ORIG 0x0869da62：编译器生成的赋值。顺序：平凡区（+0x88 区 0x52 memcpy）→
// CInventory/SkillSlot 赋值 → +0xdaa 区 0x11 memcpy → 双 vector/双 map 赋值。

// ============================================================================
// CUser::giveCharacLinkBonusExp(unsigned int bonusExp)
// ORIG T _ZN5CUser21giveCharacLinkBonusExpEj @0x08652564 (0x34f)。
// 报告 docs/class_func_reports/CUser/giveCharacLinkBonusExp.md 全文（汇编+反编译 C）已读。
// 流程：记录 SP → calcHelpAbuseRatio → gain_exp_sp(reason=3) → 升级广播 basic info →
//       链接角色名 0x123 包 → SendNotiPacket(1,2,1) → send_skill_info。
// ============================================================================
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08652564：本波三修归零（228=228 逐条全等，p167 CLEAR，
// check_tu AE+1）——①`char linkName[0x1e]={0}` 替显式 memset（P214③/P312，消 EXTRA memset）；
// ②去 skillTreeIdx/skillR 具名局部改内联表达式（P0，ORIG 无槽、`mov %eax,%ebx` 跨 call 保活）；
// ③sfp/sp 声明序对调（P28，ORIG sfp 占深槽 -0x2c）+ 去 put_short 的 (short) cast（P200，
// ORIG 32 位减法无 cwtl）。仅余尾部对齐 nop（§6.1 豁免）。
#include "CUserOnlyOrig/giveCharacLinkBonusExp.inc"
}

// ============================================================================
// CUser::quest_action(int cmd, int questId, int triggerVal, int opt)
// ORIG T _ZN5CUser12quest_actionEiiii @0x0866da8a。
// 报告 docs/class_func_reports/CUser/quest_action.md 全文已读；草稿（本文件
// quest_action W7-BLOCKED 注释区）按报告全文转写，本定义为其真实落地。
// cmd：0x21 accept / 0x22 giveup / 0x23 trigger / 0x24 finish(reward)。
// ============================================================================
// [b28d-22 N82] ORIG 0x0866da8a：p167=M0/E12:_ZN9__gnu_cxx17__normal_iteratorIPKiSt6vectorIiSaIiEEEppEv;_ZNK9__gnu_cxx17__normal_iteratorIPKiSt6vectorIiSaIiEEEdeEv;_ZNSt20_List_const_iteratorIiEC1ERKSt14_List_iteratorIiE，gap=9；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0866da8a：N82 封顶——帧/槽差 + 迭代器族（p167 M0/E12，gap 9）。
#include "CUserOnlyOrig/quest_action.inc"
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
// [b28d-22 N82] ORIG 0x08649c6a：p167=CLEAR，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08649c6a：N82 封顶——帧/槽差（p167 CLEAR，gap 1）。
#include "CUserOnlyOrig/SetCharacInfo.inc"
}

// ============================================================================
// CUser::UseQuestItem(int itemIdx)  ORIG T 0x08668d1a (0x16e4)。
// 报告 docs/class_func_reports/CUser/UseQuestItem.md（2005 行，汇编+反编译 C
// 全文已读）。觉醒/转职道具链：state==3 门控→交易中拒绝(0x20/0x13)→
// GetInvenSlot(1,itemIdx) 取当前道具，ORIG 二叉判定树内联转录（2026-09-05
// CUserC8：grow/subType/keepSkill/elixir 四元组逐节点 cmp 序照抄）→ grow 边界与职业守卫
// （失败发 0x20/0x11 错误包）→ 经验钳制到等级区间→扫描槽 3..8 与 0x99..0xc8
// 收集可删道具→0x12 包→CSkillChanger/CCalcCharacStat 技能与属性重算→
// set_grow_type/_postCheckForceChangeGrowType/双 SendNotiPacket。
// ============================================================================
// [b28d-22 N82] ORIG 0x08668d1a：p167=CLEAR，gap=-493；语义完成、byte 层帧槽/物化族封顶。
// [b28d-BigUseQuestItem 复核 2026-09-09] **旧判 N89 作废**：本函数 p167 差集只有
// C1↔C2 / D1↔D2 同址别名，ORIG 1279 条内**无任何 std::/__gnu_cxx:: 模板 call**
// （N89 机判条件不成立）。gap -493 的真因 = 判定树/叶块被归纳表查替代：
// ①判定树+叶块 ≈790 条 vs 表查 ≈40 条；②curIdx/id/cnt 具名槽诱发 cmpl mem
// 71 处（ORIG 全寄存器 cmp）；③outsideAll bool 物化 9 条 vs ORIG 1 条 ja；
// ④doChangeGrow char（cmpb）vs ORIG bool（xor $1/test）。按 ORIG 叶块地址序
// 恢复 75-case switch（4.4.7 lowering 与 4.1 逐条同构，探针 /tmp/probe_switch.cpp）
// + 删具名中间量 + bool 化 + `= {0}` rep stos 后：our 1278 / ORIG 1279（gap -1），
// p167 CLEAR，TU 四数 427/862/0/59/251 零回退。残余 = 帧槽基址差（sub 0xb8c vs
// 0xb7c，N82/N126）、grow 界错误包内联 vs 远跳（块序）、经验钳制 setb/seta 物化（P225）。
#include "CUserOnlyOrig/UseQuestItem.inc"
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
// [b28d-22 N82] ORIG 0x0867218c：p167=M0/E14:memset;_ZNSt6vectorISt4pairIiiESaIS1_EE3endEv;_ZNSt6vectorISt4pairIiiESaIS1_EE5beginEv，gap=-2；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0867218c：N82 封顶——帧/槽差 + x87（p167 M0/E14，gap -2）。
// [b28d-PoolCUOO2 真语义修复 2026-09-09] 旧 E14 中三组是**真语义 bug**：①delList 非空
// 分支 ORIG 插入 `recipe+0x19c = recipeInfo.m_vec28`（const begin/end + insert<IPK>），
// 非「自我倍增」；②回包 size+逐项输出走 **recipeInfo.m_vecC 原始材料表**（const 迭代），
// 非倍增后的 resultMulList；③variation receiver 用值版 getCurCharacInvenR() 且
// GetCurCharacExpertJobType ORIG 调×2 不缓存（第二点 increaseItemCount）。另 loop3
// `it++`/ui 循环 `ui++`（ppEi 形）。修后 **p167 MISSING=0**（EXTRA memset×3=N78
// 4.1 rep-stos 阈值族、ptEv×1、x87 fucompp/fucomip=跨版本 lowering），949 vs 953
// gap -4；ndiff 对 >500 条函数失真（§6.2 三件套①②达标）⇒ 语义完成、byte 层封顶。
#include "CUserOnlyOrig/compound_item.inc"
}


// ---------------------------------------------------------------------------
// LinkF1 批次（2026-08-28）三小函数（报告 docs/class_func_reports/CUser/）。
// ---------------------------------------------------------------------------

// ORIG T _ZN5CUser10AddMileageEi14eCoinAddReason @0x08650954（报告 AddMileage.md）：
// LogHistory("Mileage %s,%d,%d,%d,%d","Add",amount,m_mileage,m_mileage+amount,reason)
// → m_mileage(+0x796d4) += amount → SaveMileage()。
#include "CUserOnlyOrig/AddMileage.inc"
}

// ORIG T _ZN5CUser27is_recipe_items_avatar_typeERKSt6vectorISt4pairIiiESaIS2_EE
// @0x0868c45e（报告 is_recipe_items_avatar_type.md）：逐项 find_item，任一
// CItem 虚槽 +0x10（IsAvatarItem）非零即返回 1；全部未命中返回 0。
#include "CUserOnlyOrig/is_recipe_items_avatar_type.inc"
}

// ORIG 反汇编对 get_state() 短路求值两次调用（==5 或 ==10 分支），不做缓存。
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0866d972：调用点/形态已对齐（本波修法见函数内注释）。
#include "CUserOnlyOrig/SendHasNoGoWithApcQuestInParty.inc"
}

// ---------------------------------------------------------------------------
// LinkF1 批次（2026-08-28）：CUser::process_recipe_avatar_items —— ORIG T
// _ZN5CUser26process_recipe_avatar_itemsEibhiPK14CStackableItem @0x0868c4f4。
// 报告 docs/class_func_reports/CUser/process_recipe_avatar_items.md 全文转写
// （草稿 /tmp/df_batches/linkf1/process_recipe_avatar_items.body.cpp + notes.md，
// 含 3 处 Ghidra 实参修正与 ORIG 立即数枚举 static_cast 标注）。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868c4f4：N82 封顶——帧/槽差（gap 41）。
// [b28d-PoolCUOO2 修成 2026-09-09] W7-BLOCKED 前提作废：ORIG **无** CInventory(
// InventoryMemory&) ctor、**无** CInventory::materials()/outputs()（nm 实证，Main 确认）；
// 真实形=直用 recipe->m_recipe174 成员（前波已如此落地）。本波清残余迭代器族：
// ①materials 循环删引用缓存、每读点重调 operator[]（ixEj 2→6/6，P78）；②删除三元组
// 循环=非 const iterator + `*it++`×3（deEv/ppEi IPi 各 3 + 同型 ne）——后置 ++ 结果被
// deref 消费，N113 折叠不适用；③发包三元组循环=const_iterator + `*it++`×3 + P326
// `const_iterator(updatedSlots.end())` 同型比较（转换 ctor 2/2）；④AddAvatarItem 重调
// InvenW（0x0868cc81）。⇒ **p167 CLEAR（117/117）**、739 vs 738 gap 1；ndiff 读数按
// 新纪律（>500 条不按 ndiff 记账）以三件套为准。
#include "CUserOnlyOrig/process_recipe_avatar_items.inc"
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

// [b28d-22 N82] ORIG 0x086cbc85：p167=M0/E1:_ZN9__gnu_cxxneIPKSt4pairIiiEPS2_St6vectorIS2_SaIS2_EEEEbRKNS_17__normal_iteratorIT_T1_EERKNS9_IT0_SB_EE，gap=-21；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x086cbc85：N82 封顶——帧/槽差 + 迭代器族（p167 M0/E1，gap -21）。
// [b28d-PoolCUOO2 P326 重试 2026-09-09] 旧形（`it != delList.end()`）：453 条，p167
// M0(转换ctor,同型ne)/E1(混型ne)，ndiff=452。新形（end 侧显式
// `const_iterator(delList.end())` 右值临时）：**p167 CLEAR**（45/45 calls 全等），
// 458 条（gap 21→16，逐轮与 ORIG 相同的转换 ctor+同型 ne 结构）；ndiff 读数 594 系
// difflib 错位窗口失真（§6.2：本函数级差集 ORIG-only 已无 call 族）。采新形。
#include "CUserOnlyOrig/deleteSpecificItems.inc"
}

