struct CONTINUOUS_ITEM_INFO;
struct SIG_LIMIT_ITEM_INFO;
class WarRoom;               // 仅指针引用（GetWarRoom 返回，与 GetParty 的 CParty 同法）
// [Main·检查点22] PvP_Room 前置声明（同 WarRoom 先例，仅指针返回）。
//   用于把 GetPVPRoom() 从 `void*` 改回真实返回类型，见下方成员声明处证据。
class PvP_Room;
struct MSG_REQ_TO_PEER;
struct stEquipCondition;
struct competition_reward_result;
struct SIG_LOAD_PREMIUM_LETHE;
struct SIG_INGAME_EVENT_HISTORY;
struct MSG_MULTI_MAILBOX_SEND;
struct SIG_LEVELUP_GIFT_ACCOUNT_ONCE;
struct SIG_GIVE_GROWTH_CREATURE_EVENT;
struct SIG_GROWTH_WEAPON_EVENT;
struct STGuildProxy;   // 仅指针引用（LoadCoolTimeItemInfo）；完整定义归冷却/连用物品域批次
#ifndef GAME_CUSER_H_
#define GAME_CUSER_H_

// ============================================================================
// df_game_r 还原 —— CUser（G1-3 批次，在线用户会话聚合根）
// 布局依据 ORIG 构造（0x0864e410）/ 析构（0x0864ef9c）逐子对象推导：
//   +0x000 CUserCharacInfo（基类，0xe0）      +0x0e0 CNetwork<4096,450000>
//   +0x6ef6c UserMercenaryInfoMgr             +0x6ef78 租赁 map
//   +0x6effc CAccountCargo                    +0x6fd64 CGameOption
//   +0x703c4 STBreakAwayRewards               +0x703db STCommand[50]
//   +0x705dc Client_Spec                      +0x711ec list<int>
//   +0x711f4 CPacketVerifier                  +0x7121c WongWork::CUserPremium
//   +0x71bac WongWork::CHackAnalyzer          +0x795e8 Secu_AccountHacking
//   +0x795fc CodeHackCheckStorage             +0x79664 string[2]
//   +0x79674 CharacMoneyLog[7]                +0x796e8 vector<_Charac_info>
//   +0x796f4 CCharacterView                   +0x79700 cUserHistoryLog
//   +0x79744 charac_expand::CDataMgr          +0x79794 InventoryMemory
//   +0x858d4 UserQuest                        +0x8cef4 vector<uint>
//   +0x8cf64 CDungeonGainedGold               +0x8cf68 vector<LimitItemUsage>
//   +0x8cf74 vector<_DBLogItemInfo>           +0x8cf80 list<int>
//   +0x8cf88 Mutex x2                         +0x8cfe4 set<uint>
//   +0x8d020 STGuildDBInfoOnly                +0x8d0dd STGuildAgitDBInfo
//   +0x8d0e4 vector<EventInfo>                +0x8d114 WongWork::CSecurityCard
//   +0x8d160 Sanicova::CPad                   +0x8d1b8 vector<stLookOuts> x2
//   +0x8d1d0 vector<tyIPCounters>             +0x8d1e4 CerashopAddRestrict::UserInfo
//   +0x8d24d STExpReward                      +0x8d264 APSystem::CActionPointManager
//   +0x8df60 advancealtar::CharacAdvanceAltarManager
//   +0x8e034 Character                        +0x8e080 string
//   +0x8e3f0 WongWork::CMCAPManager           +0x8e954 XNuclear::CHades
//   +0x8e980 Secu_GoldControl                 +0x8ea68 Secu_HackLogCheck
//   +0x8eaa4 map<short,int>                   +0x8eabc map<pair<char,char>,int>
//   +0x8eae4 PIReceiverManager                +0x8eaec PISenderManager
//   +0x8eb00 set<pair<uchar,ushort>>          +0x8eb50 map<uchar,short>
//   +0x8eb68 map<int,pair<int,int>>           +0x8eb80 map<int,bool>
//   +0x8ebc4 vector<uchar>                    +0x8ebdc BingoData
//   +0x8ebe4 GrowthWeaponEvent                +0x8ebf8 GrowthCreatureEvent
//   +0x8ec04 CharacLevelUpGiftAccountOnce     （总尺寸 0x8ec30）
// 子对象构造/析构大多为 ORIG weak（原版头文件 inline），本头以 inline 占位
// 定义（后续对应批次 TU 给出强定义后自动覆盖）。CAccountCargo / CDataMgr /
// CharacAdvanceAltarManager / PI(Sender|Receiver)Manager 在 ORIG 为强符号，
// 本头仍以 weak 占位，保证当前链接可用。
// ============================================================================

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <time.h>

#include "CUserCharacInfo.h"
#include "IData.h"          // Character 空基类（ORIG C1 0x0848683c 先调 IData C1）
#include "InstanceRentalSystem.h"
#include "SigTypes.h"        // SIG_LOAD_CHARAC/INVENTORY/TICKET + SIG_SAVE_PC_ROOM_RENT_ITEM 等完整布局
#include "CNetwork.h"
#include "CGameOption.h"
#include "CEnvironment.h"
#include "GameTypes.h"
#include "STEquipmentScript.h"
#include "HWSpec.h"         // Client_Spec::m_specs[255]（ORIG C1 0x0822b8b0 循环 0xff 次）
#include "GameEnums.h"      // ENUM_SKILL_TREE_KIND（master_new_skills 声明所需）
#include "advancealtar.h"   // advancealtar::_CharacAdvanceAltarDbData / AdvanceAltarData
                            // （叶子头，仅 vector/map + CUser 前置声明；无重入环）
#include "STExpReward.h"    // STExpReward 完整类型（叶子头；CDataManager 经重入环不可靠）
#include "CDungeonGainedGold.h"  // CDungeonGainedGold 完整类型（值成员 m_dungeonGainedGold@+0x8cf64；叶子头）
#include "Arad_DataManager.h"  // ARAD::ScriptInterface（CerashopAddRestrict::Manager 基类；叶子头无环）
#include "CodeHackChecksum.h"  // CodeHackCheckStorage 权威布局（唯一 owner；ORIG 仅 queue，0x28）
#include "HistoryLogEnums.h"  // cUserHistoryLog 域全局枚举（TaiwanLog 批次；叶子头）
#include "TaiwanQueries.h"  // Taiwan::GarenaAuthData 完整类型（m_garenaAuthData@+0x6ef92；叶子头无环已核）
#include "LevelupSupportEventManger.h"
// ---- Wave6-CUser：ORIG 全局枚举（mangled E28ENUM_... 前缀无 scope 实证）----
#ifndef DNF_ENUM_BREAK_AWAY_REWARD_ORDER_DEFINED
#define DNF_ENUM_BREAK_AWAY_REWARD_ORDER_DEFINED
// setBreakAwayRewardOrder(ORIG W 0x0831d4de) 仅透传 int 写 this+0x703b8；
// 成员名不可由二进制观察，以下按仓库具名约定取单值占位【枚举成员集推断】。
enum ENUM_BREAK_AWAY_REWARD_ORDER { ENUM_BREAK_AWAY_REWARD_ORDER_0 = 0 };
#endif
#ifndef DNF_ENUM_SOLO_PLAY_REJECT_DEFINED
#define DNF_ENUM_SOLO_PLAY_REJECT_DEFINED
// Send_SoloPlay_Reject_NotiPack(ORIG T 0x0868e0ee) 形参枚举（mangled 实证
// EPKc26ENUM_SOLO_PLAY_REJECT_MODE25ENUM_SOLO_PLAY_REJECT_MSG；调用点实证
// mode∈{0,1}、code∈{4,6}，成员名不可由二进制观察，按仓库约定取单值占位）。
enum ENUM_SOLO_PLAY_REJECT_MODE { ENUM_SOLO_PLAY_REJECT_MODE_0 = 0 };
enum ENUM_SOLO_PLAY_REJECT_MSG { ENUM_SOLO_PLAY_REJECT_MSG_4 = 4 };
#endif

#ifndef DNF_ENUM_CHARAC_LINK_BONUS_STATE_DEFINED
#define DNF_ENUM_CHARAC_LINK_BONUS_STATE_DEFINED
// UpdateCharacLinkBonus(ORIG T 0x0865240c) 将 bonusState cast uint 写流；
// 单值占位【枚举成员集推断】。
enum ENUM_CHARAC_LINK_BONUS_STATE { ENUM_CHARAC_LINK_BONUS_STATE_0 = 0 };
#endif

  // LevelupSupportEventManger::EventInfo 完整类型
                                        // （4 字节；替代旧 namespace 占位 m_pad[0x10]）
#include "STSkillCommandData.h"  // STSkillCommandData/STCommand 权威定义（原本头定义，抽出
                                 //   供 SigTypes_C.h SIG_LOAD_SKILL 数组成员构造）
#include "CCharacter.h"   // _Additioal_info 完整类型（CUserPremium::GetAdditionalInfo 返回值；
                          // 叶子头：string.h/utility/vector，无环）
// [L31CharacPad 2026-09-10 · Main 批准 R2] _Charac_info 余 13 处 pad/占位 → 真实子对象成员
//   （ORIG C1 0x08695cf8 的隐式成员构造点；两个叶子头无环，sizeof 由断言锁定）。
#include "STGrowthType.h"         // STGrowthType（_Charac_info +0x29/+0x2a；sizeof 1）
#include "STPowerWarCharacData.h" // STPowerWarCharacData（_Charac_info +0xe8d；sizeof 0xc）
#include "STGuildSkill.h" // STGuildSkill 完整类型（STGuildDBInfoOnly::m_skills[16]；
                          // 叶子头：pack(1) 5B，无环）
// [Lane37CUser 2026-09-10 解环] STGuildDBInfoOnly 迁至独立叶子头（SkillSlot.h 的
//   master_new_guild_skill 需要该完整类型，而 SkillSlot.h ⇄ CUser.h 成环）——
//   定义/成员/断言逐字节不变，见该头。
#include "STGuildDBInfoOnly.h"
// [Lane37CUser 2026-09-10 解环后反向 include] _Charac_info 的 +0xda/+0x892/+0xdaa
//   三处 pad 视图区改为真实成员 CInventory/SkillSlot/CCargo（布局零变化，
//   static_assert 锁 sizeof/offsetof）⇒ 需完整类型；两头的 CUser 引用均为
//   `CUser*` 指针（前向声明化）故无环。
#include "CInventory.h"     // CInventory（_Charac_info +0xda；sizeof 0x7b8）
#include "SkillSlot.h"      // SkillSlot（_Charac_info +0x892；sizeof 0x518）
#include "CMCAPManager.h" // WongWork::CMCAPManager 唯一声明点（自本文件收敛）
#include "CHackAnalyzer.h" // WongWork::CHackAnalyzer / Secu_AccountHacking /
                          // ENUM_HACKTYPE 权威声明点（2026-08-27 自本头迁出）
struct _Mastered_skill;     // 完整定义见 SkillSlot.h（避免循环包含）
struct MSG_MONSTER_DIE;     // 完整定义见 Secu_HackLogCheckByParty.h（cUserHistoryLog::KillMonster 引用）
struct RewardItemInfo;      // 完整定义见 Quest.h:102（getUserQuestRewardItem 返回 vector 视图）
class Quest;                // 完整定义见 Quest.h

struct stBuySkillInfo;  // forward decl（ORIG stub 传递引用；完整定义见 CEventCharacterHandler.cpp）

// ---- SIG_LOAD_* 前向声明（完整定义在 SigTypes.h；此处仅用于快照填充函数
//      与 UserQuest::get_quest 的声明，避免在 CUser.h 引入 SigTypes.h 包含序问题）----
struct SIG_LOAD_CHARAC;
struct SIG_LOAD_INVENTORY;
struct SIG_LOAD_QUEST;
struct SIG_LOAD_TICKET;
struct SIG_LOAD_DUNGEON_CLEAR;
struct SIG_REQ_BLACK_COUNT;
struct SIG_LOAD_ETC;
struct SIG_LOAD_SKILL;          // 完整定义见 SigTypes.h
struct SIG_COMBO_SKILL;         // 完整定义见 SigTypes.h
struct SIG_ATTENDANCE_EVENT;    // 完整定义见 SigTypes.h


// ---- 局部守卫枚举（值对齐 GameEnums.h；避免整头 include 引发的同名冲突） ----
#ifndef DNF_ENUM_DISCONN_SIG_DEFINED
#define DNF_ENUM_DISCONN_SIG_DEFINED
enum DISCONN_SIG
{
    DISCONN_SIG_0 = 0,
    DISCONN_SIG_5 = 5,
    DISCONN_SIG_0x30 = 0x30,
    DISCONN_SIG_0x31 = 0x31,
    DISCONN_SIG_0x32 = 0x32
};
#endif
#ifndef DNF_ENUM_QUEST_ENEMY_TYPE_DEFINED
#define DNF_ENUM_QUEST_ENEMY_TYPE_DEFINED
enum ENUM_QUEST_ENEMY_TYPE { ENUM_QUEST_ENEMY_TYPE_0 = 0 };
#endif


#ifndef DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
#define DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
enum ENUM_PREMIUM_TYPE { ENUM_PREMIUM_TYPE_0 = 0 };
#endif

// CACHE_SECTION：CUser 角色缓存段索引（ORIG 全局 enum，mangled 13CACHE_SECTION）。
// 实证值来自 DisPatcher_SelectCharac::RequestDB*（0x81c6ca0/0x81c7c1c/0x81c70c2/
// 0x81c7436/0x81c79f2/0x81c7836/0x81c7602 的 DisableCharacInfo 实参）与
// advancealtar::DBAndExchangeServerProc::requestLoadFromDBAndES @0x0813381d（push 0xa）。
#ifndef DNF_ENUM_CACHE_SECTION_DEFINED
#define DNF_ENUM_CACHE_SECTION_DEFINED
enum CACHE_SECTION
{
    CACHE_CHARAC = 0,           // RequestDBCharac
    CACHE_INVENTORY = 1,        // RequestDBInventory_forchina
    CACHE_SKILL = 2,            // RequestDBSkill（ORIG 0x81c6f08 push 0x2）
    CACHE_QUEST = 3,            // RequestDBQuest
    CACHE_DUNGEON_CLEAR = 5,    // RequestDBDungeonClear
    CACHE_BLACK_COUNT = 7,      // RequestBlackCount
    CACHE_KILL_MONSTER = 9,     // RequestDBKillMonsterInfo
    CACHE_ETC = 0xb,            // RequestDBETC
    CACHE_ADVANCE_ALTAR = 0xa   // requestLoadFromDBAndES（Prod_GlobalAdv 实证）
};
#endif

// ENUM_DUNGEON_MODE：权威定义在 GameEnums.h（CUser.h:58 include）。

// ORIG ctor 0x086b0826（W 同址 C1/C2）：movl 0→+0 / movb 0→+4 / movb 0→+5 /
// movl 0→+8（m_valuec 不清，P131）。响应读 +4 低字节(term)/+5 次字节(flag)/+8。
struct STPremiumItemData
{
    STPremiumItemData()
    {
        m_value0 = 0;
        m_c4 = 0;
        m_c5 = 0;
        m_value8 = 0;
    }

    // 2026-08-31：ORIG sizeof 12（map 节点 _Rb_tree_node 32 = 16+pair(4+12)；
    // SetPremiumItemData 0x86af70a 写 +0/+4/+5/+8）——原 m_valuec 多余，删。
    int m_value0;          // +0x00
    char m_c4;             // +0x04（term 低字节）
    char m_c5;             // +0x05（flag 次字节）
    char m_pad6[2];        // +0x06
    int m_value8;          // +0x08
};
// ---- ORIG 全局 g_map_premium_item_data（premium 商品数据表）----
// std::map<unsigned long, STPremiumItemData>。由 CUserPremium::SetPremiumItemData
// 填充（0x86af70a）；SpecialItemHandler::respond_special_item_premium（0x827d3dc）
// 经 find(itemIdx) 查询、STPremiumItemData 字段按 ORIG 从迭代器 second+4/8/9/0xc 读取
// （value4 低字节/次字节用作 char/byte，value8 为 int）。定义见 CUserPremium.cpp。
extern std::map<unsigned long, STPremiumItemData> g_map_premium_item_data;
// ---- 共享枚举（守卫；CInventory.h 同值，权威归属 CUser.h） ----
#ifndef DNF_ENUM_INVEN_TYPE_DEFINED
#define DNF_ENUM_INVEN_TYPE_DEFINED
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1,
    INVEN_TYPE_AVATAR = 2,
    INVEN_TYPE_CREATURE = 3,
    INVEN_TYPE_5 = 5,
    INVEN_TYPE_6 = 6
};
#endif

#ifndef DNF_ENUM_BADGE_TYPE_DEFINED
#define DNF_ENUM_BADGE_TYPE_DEFINED
enum ENUM_BADGE_TYPE
{
    ENUM_BADGE_TYPE_0 = 0
};
#endif

#ifndef DNF_ENUM_EITEMADDREASON_DEFINED
#define DNF_ENUM_EITEMADDREASON_DEFINED
enum eItemAddReason
{
    eItemAddReason_0 = 0,
    eItemAddReason_1 = 1,
    eItemAddReason_2 = 2,
    eItemAddReason_7 = 7,
    eItemAddReason_8 = 8,
    eItemAddReason_0xd = 0xd
};
#endif

#ifndef DNF_ENUM_EMONEYADDREASON_DEFINED
#define DNF_ENUM_EMONEYADDREASON_DEFINED
enum eMoneyAddReason
{
    eMoneyAddReason_0 = 0,
    eMoneyAddReason_4 = 4,
    eMoneyAddReason_7 = 7,
    eMoneyAddReason_0x1a = 0x1a
};
#endif

#ifndef DNF_ENUM_EITEMDELREASON_DEFINED
#define DNF_ENUM_EITEMDELREASON_DEFINED
enum eItemDelReason
{
    eItemDelReason_0 = 0,
    eItemDelReason_3 = 3
};
#endif

#ifndef DNF_ENUM_EMONEYSUBREASON_DEFINED
#define DNF_ENUM_EMONEYSUBREASON_DEFINED
enum eMoneySubReason
{
    eMoneySubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_ECOINSUBREASON_DEFINED
#define DNF_ENUM_ECOINSUBREASON_DEFINED
enum eCoinSubReason
{
    eCoinSubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_ECOINADDREASON_DEFINED
#define DNF_ENUM_ECOINADDREASON_DEFINED
enum eCoinAddReason
{
    eCoinAddReason_0 = 0
};
#endif

#ifndef DNF_ENUM_ELEVELUPREASON_DEFINED
#define DNF_ENUM_ELEVELUPREASON_DEFINED
enum eLevelUpReason
{
    eLevelUpReason_0 = 0  // 枚举值 ORIG 未还原，仅保证符号【推断】
};
#endif

#ifndef DNF_ENUM_ELEVELDOWNREASON_DEFINED
#define DNF_ENUM_ELEVELDOWNREASON_DEFINED
enum eLevelDownReason
{
    eLevelDownReason_0 = 0  // 枚举值 ORIG 未还原，仅保证符号【推断】
};
#endif

#ifndef DNF_GAME_ENUM_ERROR_DEFINED
#define DNF_GAME_ENUM_ERROR_DEFINED
enum ENUM_ERROR { ENUM_ERROR_NONE = 0 };
#endif

#ifndef DNF_ENUM_EAVATARITEMADDREASON_DEFINED
#define DNF_ENUM_EAVATARITEMADDREASON_DEFINED
enum eAvatarItemAddReason
{
    eAvatarItemAddReason_0 = 0,
    eAvatarItemAddReason_0x14 = 0x14
};
#endif

#ifndef DNF_ENUM_DBLOG_ITEM_TYPE_DEFINED
#define DNF_ENUM_DBLOG_ITEM_TYPE_DEFINED
enum ENUM_DBLOG_ITEM_TYPE
{
    ENUM_DBLOG_ITEM_TYPE_0 = 0
};
#endif

#ifndef DNF_ENUM_CMDPACKET_DEFINED
// 权威定义已移至 GameEnums.h（CParty.h 环路 include 时此处不可见）。
enum ENUM_CMDPACKET
{
    ENUM_CMDPACKET_0 = 0
};
#endif

#ifndef DNF_ENUM_NOTIPACKET_DEFINED
#define DNF_ENUM_NOTIPACKET_DEFINED
enum ENUM_NOTIPACKET { ENUM_NOTIPACKET_0 = 0 };
#endif
#ifndef DNF_ENUM_MONEY_FULL_REASON_DEFINED
#define DNF_ENUM_MONEY_FULL_REASON_DEFINED
enum ENUM_MONEY_FULL_REASON { ENUM_MONEY_FULL_REASON_0 = 0 };
#endif
#ifndef DNF_ENUM_USERINFO_DEFINED
#define DNF_ENUM_USERINFO_DEFINED
enum ENUM_USERINFO { ENUM_USERINFO_0 = 0 };
#endif
#ifndef DNF_ENUM_EQUIPSLOT_DEFINED
#define DNF_ENUM_EQUIPSLOT_DEFINED
enum ENUM_EQUIPSLOT { ENUM_EQUIPSLOT_0 = 0 };
#endif

#ifndef DNF_ENUM_PEER_REQUEST_TYPE_DEFINED
#define DNF_ENUM_PEER_REQUEST_TYPE_DEFINED
enum ENUM_PEER_REQUEST_TYPE
{
    ENUM_PEER_REQUEST_TYPE_0 = 0
};
#endif

enum ENUM_MESSAGE_TYPE
{
    ENUM_MESSAGE_TYPE_0 = 0
};

enum PAY_TYPE
{
    PAY_TYPE_0 = 0
};

#ifndef DNF_ENUM_EEXPADDREASON_DEFINED
#define DNF_ENUM_EEXPADDREASON_DEFINED
enum eExpAddReason { eExpAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_EWPADDREASON_DEFINED
#define DNF_ENUM_EWPADDREASON_DEFINED
enum eWPAddReason { eWPAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESKILLINITREASON_DEFINED
#define DNF_ENUM_ESKILLINITREASON_DEFINED
enum eSkillInitReason { eSkillInitReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESKILLADDREASON_DEFINED
#define DNF_ENUM_ESKILLADDREASON_DEFINED
enum eSkillAddReason { eSkillAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESKILLDELREASON_DEFINED
#define DNF_ENUM_ESKILLDELREASON_DEFINED
enum eSkillDelReason { eSkillDelReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESKILLSAVEREASON_DEFINED
#define DNF_ENUM_ESKILLSAVEREASON_DEFINED
enum eSkillSaveReason { eSkillSaveReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESKILLSPINFOSAVEREASON_DEFINED
#define DNF_ENUM_ESKILLSPINFOSAVEREASON_DEFINED
enum eSkillSPInfoSaveReason { eSkillSPInfoSaveReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESPADDREASON_DEFINED
#define DNF_ENUM_ESPADDREASON_DEFINED
enum eSPAddReason { eSPAddReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESPSUBREASON_DEFINED
#define DNF_ENUM_ESPSUBREASON_DEFINED
enum eSPSubReason { eSPSubReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ECHANGEGROWTYPEREASON_DEFINED
#define DNF_ENUM_ECHANGEGROWTYPEREASON_DEFINED
enum eChangeGrowTypeReason { eChangeGrowTypeReason_0 = 0 };
#endif

namespace WongWork
{
// ----（迁移）ENUM_HACKTYPE / ENUM_HACK_TYPE 定义已移至 CHackAnalyzer.h，
//      随 WongWork::CHackAnalyzer 声明整体收敛至唯一声明点（guide §4.4）----
class CDeathTower;   // 完整定义见 CDeathTower.h（getDeathTower 返回指针）
class CBossTower;    // 完整定义见 CBossTower.h（CUser::getBossTower 返回指针）
}

namespace advancealtar
{
class StageControl;  // 完整定义 CGameManager.h（CUser::getAdvanceAltar 返回指针）
}

enum ch_state
{
    ch_state0 = 0,
    ch_state_0 = 0,
    ch_state1 = 1,
    ch_state2 = 2,
    ch_state3 = 3,
    ch_state_3 = 3,
    ch_state4 = 4,
    ch_state_4 = 4,
    ch_state5 = 5,
    ch_state6 = 6,
    ch_state7 = 7,
    ch_state8 = 8,
    ch_state9 = 9,
    ch_state10 = 10,
    ch_state11 = 11
};

namespace UserSpace
{
enum T
{
    UserSpace_T_0 = 0
};
}

enum ENUM_SECURITY_REWARD_TYPE
{
    ENUM_SECURITY_REWARD_TYPE_0 = 0
};

struct SIG_CREATE_CHARAC;
struct SIG_SAVE_INVENTORY;   // 完整定义见 SigTypes.h:810（U3ShopDb _GetSaveInvenBase 前置）

// ---- 前向声明（真实定义属其它 TU） ----
class CInventory;
class CStackableItem;  // 完整定义见 CStackableItem.h（_processContinuousItem 等指针参数）
class CExpandEquipslot;
class PacketGuard;
class Store;
class CSecu_ProtectionField;
class CValueStatistic;
class CSlotBoundChecker;
class GameWorld;
class CGameManager;
class CGameMasterCharacter;
struct user_gold_info;   // 完整定义见 SigTypes.h
struct SIG_SAVE_INVENTORY;  // 完整定义见 SigTypes.h（U3ShopDb 前向声明）
enum ENUM_GM_ITEM_UPGRADE { E_GM_ITEM_UPGRADE_NONE = 0 };
// ORIG CUser::updateCharacViewVisibleData(_Charac_info const&, CHARAC_LOGIN_DATA&)
// （0x8651322 T）的可见数据宿主；布局按 UpdateCharacView(0x8651740)/
// updateCharacViewVisibleData 报告逐偏移具名（总长 0x90，pack(1)）。
#pragma pack(push, 1)
struct CHARAC_LOGIN_DATA
{
    int           m_characNo;        // +0x00（记录 +0x04）
    char          m_name[0x14];      // +0x04 有效名区（ORIG strncpy n=0x1e，
                                     //   尾部越界覆盖 +0x18 起头部字段后再次被覆写）
    unsigned char m_job;             // +0x18 ← _Charac_info+0x26
    unsigned char m_field19;         // +0x19 ← _Charac_info+0x29
    short         m_level;           // +0x1a ← _Charac_info+0x27
    unsigned char m_flags;           // +0x1c bit0=禁玩标记(info+0x1240)/bit4=info+0xe40 bit1
    unsigned char m_field1d;         // +0x1d ← _Charac_info+0x14b7
    int           m_field1e;         // +0x1e ← _Charac_info+0x14b8
    unsigned char m_renameOrViewState; // +0x22（当前角色=getRequestRenameState；否则=viewState）
    char          m_pad23[0x28 - 0x23];
    struct AvatarSlot                        // 外观槽数组元素（8 字节）
    {
        int   m_itemIndex;                   // +0x00 物品索引/GetClearAvatar 结果
        short m_expansionA;                  // +0x04 GetExpansionInfo(key)[0]
        short m_expansionB;                  // +0x06 GetExpansionInfo(key)[1]
    };
    AvatarSlot     m_avatarSlots[12];       // +0x28..0x87（memset(data+0x28,0,0x60)）
    long          m_lastPlayTime;            // +0x88 ← CommonTime(_Charac_info+0x39)
    int           m_field8c;                 // +0x8c ← _Charac_info+0xe5d
};
#pragma pack(pop)

typedef char charac_login_data_size_ok[(sizeof(CHARAC_LOGIN_DATA) == 0x90) ? 1 : -1];

// ---------------------------------------------------------------------------
// tagCharacVisibleFlag —— _Charac_info +0xe40 子对象（权威定义点，2026-09-10 L31CharacPad
// 自 CUserOnlyOrig.cpp 的 TU 局部声明提升；删局部声明由该 TU 属主同步）。
// 证据：ORIG C1 0x8695d5c `add $0xe40; mov (%esp); call 0x08695c60`；ctor/reset 体同为
//       `movb $0x0,(%eax)`（各 0xb 尺寸）⇒ 本波按**首字节视图**建模（sizeof 1，断言锁定），
//       0xe41..0xe8c 区间内标量字段保留在 _Charac_info。
// ---------------------------------------------------------------------------
class tagCharacVisibleFlag
{
public:
    tagCharacVisibleFlag();   // ORIG W _ZN20tagCharacVisibleFlagC1Ev @0x08695c60
    void reset();             // ORIG W _ZN20tagCharacVisibleFlag5resetEv @0x08695c6c
    unsigned char m_flag;     // +0x0（ctor/reset 写 0）
};

#pragma pack(push, 1)
struct _Charac_info
{
    // copy-ctor / operator= 不声明 → 编译器隐式生成（ORIG W 0x08699863 / 0x0869da62 即编译器逐字段版，
    // 声明却不定义会导致 vector<_Charac_info> 引用 undefined；隐式生成才 emit W 符号匹配 ORIG）。
    _Charac_info();                         // ORIG W 0x08695cf8（LinkClosureA-2 CreateCharac 消费）
    ~_Charac_info();                        // ORIG W 0x08697982
    void Reset();                           // ORIG T 0x086822b8（_Charac_info.cpp 归属域批次实现）
    unsigned int m_characNo; // +0x00（Inter_LoadBloodDungeonBestRecord 0x084d8345 与包 m_0(unsigned) 直折单装载）
    // ORIG m_name 30 字节（+0x04..+0x21）：拷贝构造 0x08699863 以 movzwl 0x20
    // 覆盖末 2 字节（7×int(0x04..0x1f)+short(0x20..0x21)）；CHackAnalyzer
    // strncmp(m_name, name, 0x1e)==0 亦为 0x1e=30。原 0x1d+pad21 系误标。
    char m_name[0x1e];      // +0x04..0x21
    unsigned char m_field22;          // +0x22 growType 规整位（setCharacInfoDetail 0x0864ac1a）
    unsigned char m_field23;          // +0x23 报告固定写 2
    unsigned char m_field24;          // +0x24 特例写 7
    unsigned char m_field25;          // +0x25 备份 data[0x22]
    char m_field26;         // +0x26（job）
    short m_level;          // +0x27（ORIG CalcurateUserMaxLevel 两处 movzwl+cwtl 符号扩展⇒signed；getCharacLevelByCharacNo 单 movzwl 系 ORIG 源码显式 (unsigned short) cast，非字段类型，CUserC8 反证回滚）
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695d00/0x8695d0e：两处 `mov 0x8(%ebp); add $0x29/
    //   $0x2a; mov (%esp); call STGrowthType::C1(0x8486bce)`（ctor 写 0xff）⇒ 1 字节子对象；
    //   消费点改 `.m_growthType` 字节视角（sizeof 1 由断言锁定）。
    STGrowthType m_field29;           // +0x29
    STGrowthType m_pad2a;             // +0x2a
    int m_field2b;                    // +0x2b ← CHARAC_DATA+0x28 (int)
    // 2026-09-04 §4.3 字段化（CUserOnlyOrig 批次，Main 授权 _Charac_info 区；
    // 全部宽度证据 docs/class_func_reports/_Charac_info/Reset.md objdump 0x086822b8）
    int m_field2f;                    // +0x2f ← Reset movl $0,0x2f（R+0x066）
    unsigned short m_field33;         // +0x33 ← CHARAC_DATA+0x2c (u16)
    unsigned short m_field35;         // +0x35 max-fatigue（GetRepeatEvent 键2缩放/键0x57 附加）
    unsigned short m_field37;         // +0x37 ← CHARAC_DATA+0x30 (u16)
    // [P344 · CUserWorker 2026-09-11 · Main 批准] ORIG 隐式 `_ZN12_Charac_infoC1ERKS_`
    //   （0x08699863）在该区是**单段 16B + 单段 4B int**：objdump 0x086998d0 段 4 组
    //   4B mov（ecx×3 + edx 收尾）后才 reload 做 0x49 的单 int（源先 `mov 0xc(%ebp),%eax;
    //   mov 0x49(%eax),%edx`）；旧 `char[0xc]+char[8]` 划分使 4.4 在 0x41 处提前释放
    //   源指针 ⇒ 拷貝形不同。布局逐字节不变（CommonTime 视图只用前 0xc 字节）。
    char m_delReqTime[0x10];          // +0x39..0x48（CommonTime 视图 +0x39，sizeof 0xc）
    int  m_pad49;                     // +0x49..0x4c
    unsigned short m_field4d;         // +0x4d ← CHARAC_DATA+0xb3 (u16)
    unsigned short m_field4f;         // +0x4f ← CHARAC_DATA+0x32 (u16，FATIGUE BATTERY 主载体)
    int m_field51;                    // +0x51 ← CHARAC_DATA+0xb5 (int)
    char m_tradeRestrictData[0x10];   // +0x55..0x64 ← memcpy(CHARAC_DATA+0xb9,0x10)
    char m_reliablePerson[0xc];       // +0x65..0x70（CReliablePerson reinterpret 视图前段；
                                      //   InitTradeGold 消费方 cast 视图不依赖数组尺寸）
    int m_schoolPoint;                // +0x71 ← CReliablePerson+0xc m_schoolPoint（Reset movl
                                      //   $0,0x71 R+0x389；calcCurCharacSchoolPoint/CUserCharacInfo
                                      //   m_reliablePerson.m_schoolPoint 同字段实证）
    unsigned char m_stamina;    // +0x75（疲劳值；SelectCharac 恢复/make_charac_stat 序列化）
    char m_pad76;                     // +0x76
    unsigned short m_field77;         // +0x77 ← CHARAC_DATA+0x5b (u16)
    unsigned short m_field79;         // +0x79 ← CHARAC_DATA+0x5d (u16)
    int m_field7b;                    // +0x7b ← CHARAC_DATA+0x4d (int，"CharLastPlayTick")
    int m_field7f;                    // +0x7f ← CHARAC_DATA+0x118 (int)
    char m_pad83;                     // +0x83 ← Reset movb $0,0x83（R+0x1e6；CharacListElemView
                                      //   m_forbidPlayFlag 同位）
    int m_field84;                    // +0x84 ← Reset movl $0,0x84（R+0x1f0；ElemView m_forbidPlayLeftSec 同位）
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695d1c `add $0x88; mov (%esp); call
    //   _Additioal_info::C1`（0x8348254 → clear：memset(this,0,0x52)）⇒ 0x88..0xd9 原 17 个
    //   CHARAC_DATA 镜像字段实为该子对象成员（权威名 m_field00/m_field04/m_field08/m_field0a/
    //   m_field0c/m_field0e/m_field10[4]/m_field18[17]/m_field3a/m_field3e/m_field40/m_field42/
    //   m_field46/m_field48/m_field4a/m_field4c/m_field4e；sizeof 0x52 断言锁定）。
    //   消费点改 `...m_additioalInfo.<权威名>`（原 `*(const _Additioal_info*)&...m_field88`
    //   视图 cast 一并有据消除）。
    _Additioal_info m_additioalInfo;  // +0x88..0xd9（0x52）
    // [Lane37CUser 2026-09-10 · Main 立项] pad 视图区 → 真实子对象成员（布局零变化）：
    //   探针实测 sizeof(CInventory)=0x7b8 / sizeof(SkillSlot)=0x518 / sizeof(CCargo)=0x14，
    //   0x7b8+0x518+0x14 == 0xce4 == 0xdbe-0xda 恰好拼满原 pad 区（/tmp/tu__ProbeCharacLayout.o
    //   改前基线：sizeof(_Charac_info)=0x14d1、offsetof 0xda/0xdbe 全等）。
    //   收益：编译器隐式 C1ERKS_/aSERKS_ 恢复对三子对象的拷贝构造/析构调用
    //   （ORIG W 0x08699820 / 0x0869da62；p167 miss=5/2，原为按字节浅拷贝容器 = 真 bug）。
    //   旧视图 cast（(char*)p+0xda 等）同址仍可用；CUserOnlyOrig.cpp 的 placement new/
    //   手工析构须同步删除（属主 lane 代落，见 2026-09-10 广播）。
    CInventory m_curCharacInven;      // +0xda..0x891（0x7b8；ORIG CInventory@+0xda）
    SkillSlot m_skillSlot;            // +0x892..0xda9（0x518；ORIG SkillSlot@+0x892）
    CCargo m_cargo;                   // +0xdaa..0xdbd（0x14；ORIG CCargo@+0xdaa）
    int m_guildId;                    // +0xdbe（GetGuildIdBeforeDeleteCharacter 读取）
    unsigned char m_fielddc2;         // +0xdc2 ← CHARAC_DATA[0x5a]
    // [P344 · CUserWorker 2026-09-11 · Main 批准] ORIG 同函数 0x08699b3d..0x08699bd5 段的
    //   拷贝单元划分（逐条 objdump）：short(0xdc3) 独立（movzwl+mov %dx）；0xdc5..0xddb
    //   为**单段 0x17**（5×4B mov 用 ecx + `movzwl/mov %cx` + `movzbl/mov %dl` 收尾，
    //   全程不 reload）；reload 后 0xddc..0xde8 为**单段 0xd**（3×4B ecx + movzbl/dl 收尾）。
    //   旧 char[0x26] 单段使 4.4 在 0xdd5/0xde4 提前释放源指针 ⇒ 拷贝形短 12 条。
    short m_fieldDC3;      // +0xdc3
    char  m_padDC5[0x17];  // +0xdc5..0xddb
    char  m_padDDC[0xd];   // +0xddc..0xde8
    int m_fieldDE9;                   // +0xde9 ← Reset movl $0（R+0x20a）
    short m_fieldDED;                 // +0xded ← Reset movw $0（R+0x217）
    int m_fieldDEF;                   // +0xdef ← Reset movl $0（R+0x223）
    char m_fieldDF3;                  // +0xdf3 ← Reset movb $0（R+0x230）
    int m_resetDwordDf4;              // +0xdf4 ← 常量清零（ORIG movl $0）
    int m_resetDwordDf8;              // +0xdf8 ← 常量清零
    short m_fieldDFC;                 // +0xdfc ← Reset movw $0（R+0x23a）
    int m_fieldDFE;                   // +0xdfe ← Reset movl $0（R+0x13d）
    int m_fieldE02;                   // +0xe02 ← Reset movl $0（R+0x116）
    int m_fieldE06;                   // +0xe06 ← Reset movl $0（R+0x14a）
    int m_fieldE0A;                   // +0xe0a ← Reset movl $0（R+0x157）
    char m_dataE0E[0x14];             // +0xe0e ← Reset memset(0,0x14)（R+0x164）
    int m_fielde22;                   // +0xe22 ← CHARAC_DATA+0x00（data[0x5f]!=0 才写）
    int m_fieldE26;                   // +0xe26 ← Reset movl $0（R+0x181）
    int m_fieldE2A;                   // +0xe2a ← Reset movl $0（R+0x18e）
    int m_fieldE2E;                   // +0xe2e ← Reset movl $0（R+0x1c5）
    char m_fieldE32;                  // +0xe32 ← Reset movb $0（R+0x1d2）
    int m_fieldE33;                   // +0xe33 ← Reset movl $0（R+0x1ab；非对齐 int 实证）
    int m_fieldE37;                   // +0xe37 ← Reset movl $0（R+0x1b8）
    unsigned char m_needCheckOverEquipItem;  // +0xe3b（SetNeedCheckOverEquipItem 0x0867ceae 写 1）
    int m_fieldE3C;                   // +0xe3c ← Reset movl $0（R+0x1fd）
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695d67 `add $0xe40; mov (%esp); call
    //   tagCharacVisibleFlag::C1`（0x8695c60 写首字节 0）⇒ 子对象成员（首字节视图 sizeof 1，
    //   断言锁定）；0xe41..0xe8c 标量保留。消费点 `...m_tagVisibleFlags` → `.m_flag`
    //   （getCharacInfoDetail 0x0864c7e6 movzbl 读→CHARAC_DATA+0x12e → m_flag 仍 unsigned char）。
    tagCharacVisibleFlag m_tagVisibleFlags;  // +0xe40
    int m_fieldE41;                   // +0xe41 ← Reset movl $0（R+0x246）
    int m_fielde45;                   // +0xe45..0xe5d 六连 int ← CHARAC_DATA+{0xed,0xd5,0xd9,0xdd,0xe1,0xe5}
    int m_fielde49;
    int m_fielde4d;
    int m_fielde51;
    int m_fielde55;
    int m_fielde59;
    int m_fielde5d;                   // +0xe5d ← +0xe9
    int m_fieldE61;                   // +0xe61 ← Reset movl $0（R+0x294）
    char m_fieldE65;                  // +0xe65 ← Reset movb $0（R+0x29d）
    char m_fieldE66;                  // +0xe66 ← Reset movb $0（R+0x2fd）
    char m_fieldE67;                  // +0xe67 ← Reset movb $0（R+0x30a）
    int m_fielde68;                   // +0xe68 ← +0xf1 (int)
    int m_fieldE6C;                   // +0xe6c ← Reset movl $0（R+0x317）
    unsigned char m_fielde70;         // +0xe70 ← CHARAC_DATA[0xf5]
    int m_fielde71;                   // +0xe71 ← CHARAC_DATA+0xf6 (int)
    int m_fieldE75;                   // +0xe75 ← Reset movl $0（R+0x331）
    int m_fielde79;                   // +0xe79 ← +0xfa
    int m_fielde7d;                   // +0xe7d ← +0x106
    int m_fielde81;                   // +0xe81 ← +0x10a
    int m_fieldE85;                   // +0xe85 ← Reset movl $0（R+0x358）
    int m_fieldE89;                   // +0xe89 ← Reset movl $0（R+0x365）
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695d6c `add $0xe8d; mov (%esp); call
    //   STPowerWarCharacData::C1`（0x86959dc：+0 word=0 / +2 byte=0 / +3 byte=1 / +4 int=0 /
    //   +8 int=0，sizeof 0xc 断言锁定）⇒ 真实子对象。
    STPowerWarCharacData m_powerWarData;  // +0xe8d..0xe98
    char m_fieldE99;                  // +0xe99 ← Reset movb $0（R+0x37f）
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695d87/0x8695d97：两处 `add $0xe9a/$0xea6; mov (%esp);
    //   call std::vector<ContinuousItemInfo>::vector()`（0x86980de）⇒ 两个真实 vector 成员
    //   （各 sizeof 0xc，偏移由断言锁定；编译器隐式构造/析构，dtor 顺序随声明逆序=偏移降序）。
    std::vector<ContinuousItemInfo> m_continuousItemListA;  // +0xe9a..0xea5
    std::vector<ContinuousItemInfo> m_continuousItemListB;  // +0xea6..0xeb1
    char m_fieldEB2;                  // +0xeb2 ← Reset movb $0（R+0x393）
    char m_padEB3;                    // +0xeb3（无证据）
    char m_fieldEB4;                  // +0xeb4 ← Reset movb $0（R+0x3a4）
    // [P344 · CUserWorker 2026-09-11 · Main 批准] ORIG 0x08699c9x：0xeb5/0xeb6 是两个
    //   **独立 char**（各自 reload 后 movzbl+mov %dl），非 2 字节数组（char[2] 会生成
    //   movzwl 合并单元）。
    char m_padEB5;                    // +0xeb5
    char m_padEB6;                    // +0xeb6
    char m_fieldEB7;                  // +0xeb7 ← Reset movb $0（R+0x3b1）
    char m_fieldEB8;                  // +0xeb8 ← Reset movb $0（R+0x39d）
    unsigned char m_demensionInout[6]; // +0xeb9 ← Reset memset(0,6)；CParty 0x0859f510 movzbl 零扩展=unsigned
    char m_fieldEBF;                  // +0xebf ← Reset movb $0（R+0x3db）
    unsigned char m_ultimateInout[3];  // +0xec0 ← Reset memset(0,3)；CParty 0x0859fa8f movzbl 零扩展=unsigned
    char m_fieldEC3;                  // +0xec3 ← Reset movb $0（R+0x405）
    int m_fieldEC4;                   // +0xec4 ← Reset movl $-1（R+0x40f）
    int m_fieldEC8;                   // +0xec8 ← Reset movl $0x17d78400（R+0x41c）
    std::map<int, stBloodBestRecord> m_bloodBestRecords;    // +0xecc（Reset 0x086822b8 map::clear 实证；sizeof 0x18，P40 口径）
    char m_fieldEE4;                  // +0xee4 ← Reset movb $0（R+0x436）
    float m_fieldEE5;                 // +0xee5（Reset 0x086822b8+0x443 `mov $0,%edx;mov %edx,mem` 两段物化=float 成员；镜像 CUserCharacInfo.h:450 已 float；CUOC7 probe p14）
    char m_fieldEE9;                  // +0xee9 ← Reset movb $0（R+0x45a）
    char m_fieldEEA;                  // +0xeea ← Reset movb $0（R+0x464）
    short m_fieldEEB;                 // +0xeeb ← Reset movw $0（R+0x46e）
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695db7/0x8695dc7：两处 `add $0xeed/$0x1086; mov (%esp);
    //   call SIG_LOAD_PREMIUM_LETHE::C1`（0x848ff7c：0xcc×_Mastered_skill 循环构造；sizeof
    //   0x199 断言锁定）⇒ 两个真实子对象。
    SIG_LOAD_PREMIUM_LETHE m_premiumLethe;   // +0xeed..0x1085
    SIG_LOAD_PREMIUM_LETHE m_premiumLethe2;  // +0x1086..0x121e
    unsigned short m_field121f;       // +0x121f ← CHARAC_DATA+0x10e (u16)
    int m_field1221;                  // +0x1221 ← +0x110 (int)
    int m_field1225;                  // +0x1225 ← +0x114 (int)
    short m_field1229;                // +0x1229 ← Reset movw $0（R+0x4de）
    char m_field122B;                 // +0x122b ← Reset movb $0（R+0x4ea）
    short m_field122C;                // +0x122c ← Reset movw $0（R+0x4f4）
    int m_field122E;                  // +0x122e ← Reset movl $0（R+0x500）
    short m_field1232;                // +0x1232 ← Reset movw $0（R+0x50d）
    short m_field1234;                // +0x1234 ← Reset movw $0（R+0x519）
    short m_field1236;                // +0x1236 ← Reset movw $0（R+0x525）
    char m_field1238;                 // +0x1238 ← Reset movb $0（R+0x531）
    unsigned char m_field1239;        // +0x1239 ← CHARAC_DATA[0x11e]（MakeTagCharacInfo 技能区选择）
    short m_field123A;                // +0x123a ← Reset movw $0（R+0x545）
    short m_field123C;                // +0x123c ← Reset movw $0（R+0x551）
    unsigned char m_field123e;        // +0x123e ← CHARAC_DATA[0x11f]
    unsigned char m_field123f;        // +0x123f ← CHARAC_DATA[0x120]
    char m_pad1240;
    bool m_field1241;                 // +0x1241 ← (CHARAC_DATA[0x121]!=0)
    char m_field1242;                 // +0x1242 ← Reset movb $0（R+0x571）
    char m_field1243;                 // +0x1243 ← Reset movb $0（R+0x57b）
    char m_field1244;                 // +0x1244 ← Reset movb $0（R+0x585）
    char m_field1245;                 // +0x1245 ← Reset movb $0xff（R+0x58f）
    unsigned char m_characLinkType;   // +0x1246（checkCharacLinkType 读）
    int m_field1247;                  // +0x1247 ← Reset movl $0（R+0x5a3）
    int m_field124B;                  // +0x124b ← Reset movl $0（R+0x5b0）
    unsigned char m_linkBonusSetting; // +0x124f（isCharacLinkBonusSetting movzbl 直返）
    int m_field1250;                  // +0x1250 ← Reset movl $0（R+0x5c7）
#pragma pack(pop)   // map 内部布局按默认 4 字节对齐：pack(1) 传染使 _Rb_tree_impl 变 header@+1/sizeof 0x15，ORIG 为 header@+4/0x18（NEW 二进制 _M_initialize 实证）
    std::map<int, STSkillCommandData> m_skillCommandsMap;   // +0x1254（Reset map::clear 实证；sizeof 0x18）
#pragma pack(push, 1)   // 恢复 _Charac_info 的 pack(1)
    unsigned short m_field126c;       // +0x126c ← CHARAC_DATA+0x122 (u16)
    unsigned char m_field126e;        // +0x126e ← CHARAC_DATA[0x124]
    int m_field126f;                  // +0x126f ← +0x125 (int)
    char m_field1273;                 // +0x1273 ← Reset movb $0（R+0x607）
    char m_field1274;                 // +0x1274 ← Reset movb $0（R+0x611）
    unsigned char m_field1275;        // +0x1275 ← CHARAC_DATA[0x129]
    unsigned char m_field1276;        // +0x1276 ← CHARAC_DATA[0x12a]
    unsigned char m_field1277;        // +0x1277 ← CHARAC_DATA[0x12b]
    unsigned char m_field1278;        // +0x1278 ← CHARAC_DATA[0x12c]
    char m_pad1279;
    unsigned char m_field127a;        // +0x127a ← CHARAC_DATA[0x12d]
    char m_field127B;                 // +0x127b ← Reset movb $0（R+0x667）
    int m_field127C;                  // +0x127c ← Reset movl $0（R+0x671）
    unsigned char m_field1280;        // +0x1280 ← CHARAC_DATA[0x12f]
    // stAuraAvatarOption 子对象（@0x1281，3×int）——证据 CUser/GetAuraAvatarOption.md
    // 0x0868ddc2 mov 0x1281/0x1285/0x1289(%eax) 三连 + get_aura_avatar_option_value
    // 写侧 cur+0x1281+idx*4（idx<3 边界）。Reset 的 reset() 视图 call 仍走 this+0x1281。
    union {                           // b28 BM1 代落（Main）：int[3] 数组视图复现 ORIG 单条 scaled 直存
        struct {                      // （probe p19.o a1 命中 0x0868e1xx；标量形 4 条不可翻）
            int m_auraValue0;         // +0x1281
            int m_auraValue1;         // +0x1285
            int m_auraValue2;         // +0x1289
        };
        int m_auraValues[3];
        // b28c-L5 申请（Main 代落）：具名结构视图，供 GetAuraAvatarOption 走 12 字节
        // POD 整体赋值（ORIG 0x0868ddc2 目标基址驻 %edx + 源 disp32 三连直读）。
        stAuraAvatarOption m_auraAvatarOption;  // +0x1281，sizeof 0xc，零布局影响
    };
    int m_field128D;                  // +0x128d ← Reset movl $0（R+0x6a5）
    int m_field1291;                  // +0x1291 ← Reset movl $0（R+0x698）
    int m_field1295;                  // +0x1295 ← Reset movl $0（R+0x6b2）
    int m_field1299;                  // +0x1299 ← Reset movl $0（R+0x6bf）
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695de7/0x8695df7：`call WpBonusPointResult::C1`
    //   （0x8695c78→clear）/`call ExpBonusPointResult::C1`（0x8695ca4→clear，各 +0/+4 清零）；
    //   两类型 sizeof 8（断言锁定）⇒ Reset 的 memset(8) 由 ctor 等价覆盖。
    WpBonusPointResult m_wpBonusPointResult;    // +0x129d..0x12a4
    ExpBonusPointResult m_expBonusPointResult;  // +0x12a5..0x12ac
    int m_field12ad;                  // +0x12ad ← +0x130 (int)
    int m_field12B1;                  // +0x12b1 ← Reset movl $0（R+0x719）
    short m_field12B5;                // +0x12b5 ← Reset movw $0（R+0x726）
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695e07 `add $0x12b7; mov (%esp); call
    //   CharacterOption::C1`（0x822abec：memset(this,0xff,0x200)；sizeof 0x200 断言锁定）
    //   ⇒ 真实子对象；字节视图消费点改 `.m_pad`。
    CharacterOption m_characterOption;   // +0x12b7..0x14b6
    unsigned char m_field14b7;        // +0x14b7（SelectCharac::process 回填 MSG_CHARAC_SELECT+0x12）
    int m_field14b8;                  // +0x14b8（SelectCharac::process 回填 MSG_CHARAC_SELECT+0xe）
    // [b28d-solo-CUserOnlyOrig 代落 P248档②] char→bool：ORIG SendTagCharacInfo
    // 0x0869043x `movzbl 0x14bc(%eax),%eax; xor $0x1,%eax; test %al,%al; je`
    // （字段 bool 直测）；char 裸 if(!f) 4.4 产 test/sete 双物化。消费点 5 处均 0/1。
    bool m_field14BC;                 // +0x14bc ← Reset movb $0（R+0x769）
    char m_field14BD;                 // +0x14bd ← Reset movb $0（R+0x773）
    int m_field14BE;                  // +0x14be ← Reset movl $0（R+0x77d）
    int m_field14C2;                  // +0x14c2 ← Reset movl $0（R+0x78a）
    int m_viewState;        // +0x14c6
    // [L31CharacPad 2026-09-10] ORIG C1 0x8695e17 `add $0x14ca; mov (%esp); call
    //   seriaRoom_AniDeco::C1`（0x822ee18：两 movw $0 ⇒ 两 short 成员 m_field0/m_field2；
    //   sizeof 4 断言锁定）⇒ 覆盖原 0x14ca/0x14cc 两标量（Reset 的 movw $0 即其成员写）。
    seriaRoom_AniDeco m_aniDeco;      // +0x14ca..0x14cd
    char m_field14CE;                 // +0x14ce ← Reset movb $0（R+0x7bc）
    char m_field14CF;                 // +0x14cf ← Reset movb $0（R+0x7c6）
    char m_field14D0;                 // +0x14d0 ← Reset movb $0（R+0x7d0）
};
#pragma pack(pop)
static_assert(sizeof(_Charac_info) == 0x14d1, "_Charac_info ORIG size");
// [Lane37CUser 2026-09-10] pad→真实子对象后的偏移锁定（改前探针基线，逐项必须全等）
static_assert(__builtin_offsetof(_Charac_info, m_curCharacInven) == 0xda,
              "_Charac_info CInventory @0xda");
static_assert(__builtin_offsetof(_Charac_info, m_skillSlot) == 0x892,
              "_Charac_info SkillSlot @0x892");
static_assert(__builtin_offsetof(_Charac_info, m_cargo) == 0xdaa,
              "_Charac_info CCargo @0xdaa");
static_assert(__builtin_offsetof(_Charac_info, m_guildId) == 0xdbe,
              "_Charac_info m_guildId @0xdbe");
// [L31CharacPad 2026-09-10 · Main 批准] 子对象尺寸 + 13 处成员化偏移锁定。
// 依据：ORIG `_ZN12_Charac_infoC1Ev` @0x08695cf8（sz 0x1f7）逐条 objdump 的 18 个
// 隐式成员构造点；任一处不符即回滚该处成员化（不得改断言迁就）。
static_assert(sizeof(STGrowthType) == 1, "STGrowthType 1B view");
static_assert(sizeof(_Additioal_info) == 0x52, "_Additioal_info 0x52");
static_assert(sizeof(tagCharacVisibleFlag) == 1, "tagCharacVisibleFlag 1B view");
static_assert(sizeof(STPowerWarCharacData) == 0xc, "STPowerWarCharacData 0xc");
static_assert(sizeof(SIG_LOAD_PREMIUM_LETHE) == 0x199, "SIG_LOAD_PREMIUM_LETHE 0x199");
static_assert(sizeof(WpBonusPointResult) == 8, "WpBonusPointResult 8");
static_assert(sizeof(ExpBonusPointResult) == 8, "ExpBonusPointResult 8");
static_assert(sizeof(CharacterOption) == 0x200, "CharacterOption 0x200");
static_assert(sizeof(seriaRoom_AniDeco) == 4, "seriaRoom_AniDeco 4");
static_assert(__builtin_offsetof(_Charac_info, m_field29) == 0x29, "STGrowthType @0x29");
static_assert(__builtin_offsetof(_Charac_info, m_pad2a) == 0x2a, "STGrowthType @0x2a");
static_assert(__builtin_offsetof(_Charac_info, m_additioalInfo) == 0x88, "_Additioal_info @0x88");
static_assert(__builtin_offsetof(_Charac_info, m_tagVisibleFlags) == 0xe40, "tagCharacVisibleFlag @0xe40");
static_assert(__builtin_offsetof(_Charac_info, m_powerWarData) == 0xe8d, "STPowerWarCharacData @0xe8d");
static_assert(__builtin_offsetof(_Charac_info, m_continuousItemListA) == 0xe9a, "vector @0xe9a");
static_assert(__builtin_offsetof(_Charac_info, m_continuousItemListB) == 0xea6, "vector @0xea6");
static_assert(__builtin_offsetof(_Charac_info, m_premiumLethe) == 0xeed, "SIG_LOAD_PREMIUM_LETHE @0xeed");
static_assert(__builtin_offsetof(_Charac_info, m_premiumLethe2) == 0x1086, "SIG_LOAD_PREMIUM_LETHE @0x1086");
static_assert(__builtin_offsetof(_Charac_info, m_wpBonusPointResult) == 0x129d, "WpBonusPointResult @0x129d");
static_assert(__builtin_offsetof(_Charac_info, m_expBonusPointResult) == 0x12a5, "ExpBonusPointResult @0x12a5");
static_assert(__builtin_offsetof(_Charac_info, m_characterOption) == 0x12b7, "CharacterOption @0x12b7");
static_assert(__builtin_offsetof(_Charac_info, m_aniDeco) == 0x14ca, "seriaRoom_AniDeco @0x14ca");
// [P344 · CUserWorker 2026-09-11] 拷贝单元划分反推出的字段边界锁定（ORIG 隐式 C1ERKS_）。
static_assert(__builtin_offsetof(_Charac_info, m_delReqTime) == 0x39, "m_delReqTime @0x39");
static_assert(__builtin_offsetof(_Charac_info, m_pad49) == 0x49, "m_pad49 @0x49");
static_assert(__builtin_offsetof(_Charac_info, m_field4d) == 0x4d, "m_field4d @0x4d");
static_assert(__builtin_offsetof(_Charac_info, m_fielddc2) == 0xdc2, "m_fielddc2 @0xdc2");
static_assert(__builtin_offsetof(_Charac_info, m_fieldDC3) == 0xdc3, "m_fieldDC3 @0xdc3");
static_assert(__builtin_offsetof(_Charac_info, m_padDC5) == 0xdc5, "m_padDC5 @0xdc5");
static_assert(__builtin_offsetof(_Charac_info, m_padDDC) == 0xddc, "m_padDDC @0xddc");
static_assert(__builtin_offsetof(_Charac_info, m_fieldDE9) == 0xde9, "m_fieldDE9 @0xde9");
static_assert(__builtin_offsetof(_Charac_info, m_padEB5) == 0xeb5, "m_padEB5 @0xeb5");
static_assert(__builtin_offsetof(_Charac_info, m_padEB6) == 0xeb6, "m_padEB6 @0xeb6");
static_assert(__builtin_offsetof(_Charac_info, m_fieldEB7) == 0xeb7, "m_fieldEB7 @0xeb7");


// ---- 子对象最小类（尺寸按构造偏移推导；方法签名按 ORIG nm 核对） ----
// RentalInfo 权威定义见 InstanceRentalSystem.h（§7 合并，原 namespace 块移除）。
#include "InstanceRentalSystem.h"
// ORIG GetLimitItemUsage @0x8688dc4 读取元素 +0xc(int)/+0x10(byte)/+0x15(int)/+0x19(int)，
// 序列化到 SIG_LOAD_ETC (i+0x263)*0x10。元素尺寸 ≥ 0x1d。
struct LimitItemUsage
{
    LimitItemUsage();   // ORIG W C1/C2 0x08695ef0
    // [Q2P342ScanB 2026-09-11 杠杆A·P342 落地] **不声明析构** ⇒ 类型 trivial。
    //   直证四条件：① 我方 CUserOnlyOrig.o 有 `_ZNSt12_Destroy_auxILb0EE9__destroyIP14LimitItemUsageEEvT_S4_`
    //   而 ORIG 同参**只有 ILb1 版**；② ORIG **全 ELF 仅 1 处** `call ~LimitItemUsage`，
    //   位于 `_ZN9__gnu_cxx13new_allocatorI14LimitItemUsageE7destroyEPS1_` 体内；
    //   ③ 我方 `CUserOnlyOrig.cpp.o` **有** 该 `new_allocator<LimitItemUsage>::7destroy` 实例
    //   ⇒ 删声明后隐式析构仍由该 odr-use 链发射（**且发射 TU 与 ORIG 一致**）；
    //   ④ ORIG D1/D2/D0 0x086a6480 绑定为 **W**（弱）＝ 隐式析构 odr-use 发射的特征，
    //   我方原为 W7_UserOnlyC.cpp 内的 **T**（强）⇒ 本改动同时修正绑定。
    //   影子 A/B：CUser 808/1717/5/68/588→809/1717/5/68/586、
    //   CUserOnlyOrig 495/975/1/44/285→497/975/1/44/284、W7_UserOnlyC 89/168/0/3/71→89/167/0/3/70
    //   （全局 identical +3 / ae −2 / missing −3；`_ZSt8_DestroyI14LimitItemUsageEvPT_` 消失＝ORIG 无此符号）。
    char             m_pad0[0xc];  // +0x00
    int              m_fieldc;     // +0x0c
    unsigned char    m_field10;    // +0x10
    // 双观：C1 0x08695ef0 构造期 4 条独立 movb（字节形），SetLimitInfo 0x08688a5c 区
    // 全宽 int 写 ⇒ union 保两形（P197① 同族）。
    union {
        int           m_field11;   // +0x11（int 观：SetLimitInfo/消费点）
        unsigned char m_field11b[4]; // 字节观：ctor 四 movb
    };
    int              m_field15;    // +0x15
    int              m_field19;    // +0x19
    // 2026-08-31：原 m_pad1d[3] 多余——ORIG 元素 sizeof 0x1d（new_allocator 29B），删。
    // ---- Wave6-CUser 补声明（ORIG T/W 符号实证；定义 owner=对应域批次）----
    bool isUsable();   // ORIG W 0x08695f68 _ZN14LimitItemUsage8isUsableEv
    bool useIt();      // ORIG W 0x08695f28 _ZN14LimitItemUsage5useItEv
    bool isAnotherDay(); // ORIG W 0x08688fba _ZN14LimitItemUsage12isAnotherDayEv
} __attribute__((packed));   // ORIG 元素内非对齐 int @+0x15/+0x19（GetLimitItemUsage 实证）

struct stLookOuts
{
    // 2026-08-31：ORIG C1（0x869380e）仅写 +0x0/+0x4——sizeof 8（影子测试
    // new_allocator 元素分配 8B）；原 m_pad8[8] 多余（误按 0x10 建模）——删。
    stLookOuts(unsigned int a, unsigned int b);   // ORIG W _ZN10stLookOutsC1Ejj 0x0869380e（定义=W8_CU_G.cpp）
    unsigned int m_a;                             // +0x00（ctor arg1）
    unsigned int m_b;                             // +0x04（ctor arg2）
};

struct tyIPCounters
{
    // 2026-08-31：同 stLookOuts——sizeof 8，原 m_pad8[8] 多余——删。
    tyIPCounters(unsigned int a, unsigned int b); // ORIG W _ZN12tyIPCountersC1Ejj 0x08693824（同上承载）
    unsigned int m_a;                             // +0x00
    unsigned int m_b;                             // +0x04
};

class UserMercenaryInfoMgr
{
public:
    UserMercenaryInfoMgr() {}
    ~UserMercenaryInfoMgr() {}

    int AddCharac(CHARAC_LOAD_MERCENARY& info);  // ORIG T 0x081b4300（返回 1=添加/0=已存在；非 const 引用：写 m_avatarBonus）
    void RemoveCharac(unsigned int characNo);
    void SetCharac(unsigned int characNo, const CHARAC_LOAD_MERCENARY& info);
    unsigned int FindIndexByCharacNo(unsigned int characNo) const;  // ORIG T 0x081b42ae
    bool CheckCharacIndex(unsigned char index) const;               // ORIG T 0x081b4262
    bool IsCompetition(unsigned int characNo) const;
    // [b28d-solo-CUser 代落 P168a/P292] int→bool：ORIG 0x081b4300 调用点
    // `call; xor $0x1,%eax; test %al,%al; jne`（bool callee）。位模式仍 0=命中/1=未命中，
    // 调用点 `!= 1` 与 `!f()` 在 0/1 值域等价。返回类型不入 mangled。
    bool CheckValidAppend(const CHARAC_LOAD_MERCENARY& info);                     // ORIG T 0x081b4198（0=命中/1=未命中）
    int CompetitionCharac(CUser* user, unsigned int slot, char modeA, char modeB);  // ORIG T 0x081b4536（0/1）
    bool IsWaitingStateCharac(unsigned int characNo);                                // ORIG T 0x081b45ce（非 const）
    void UpdateCharacName(unsigned int characNo, std::string const& name);           // ORIG T 0x081b462e
    // b28d-27：ORIG 0x081b44c0 尾部 `call CHARAC_LOAD_MERCENARY::Reward; leave; ret` 直传单字节
    // （无 movzbl 规范化，P9）⇒ 返回 bool；返回类型不入 mangled、零 ABI/布局影响。
    bool ReturnCharac(CUser* user, unsigned int characNo, competition_reward_result& reward);
    std::vector<CHARAC_LOAD_MERCENARY> m_characs;  // +0x00（ORIG C1/D1 = vector C1/D1）
};

class CAccountCargo
{
public:
    // ========================================================================
    // CAccountCargo（账号金库，G4 商城/交易域批次，ORIG 布局 0xd68）
    // 布局依据 C1 (0x8289794) / Clear (0x828986c) / SetDBData (0x8289816)
    // / AddMoney (0x828a742) / GetCapacity (0x822f012) 推导：
    //   +0x000 CUser* m_pUser        +0x004 Inven_Item m_slots[0x37]（0xd58）
    //   +0xd5c unsigned int m_money  +0xd60 unsigned int m_capacity
    //   +0xd64 char m_alter
    // ========================================================================
    CAccountCargo();

    // ---- 生命周期 / DB ----
    void Clear();
    void SetDBData(CUser* user, Inven_Item* items, unsigned int money,
                   unsigned int capacity);
    void ResetSlot(int slot);
    // ORIG 0x0828ad72/0x0828b0b2：静态成员（仅用第一实参 CUser*，无 this 访问）。
    static void CreateAccountCargo(CUser* user);
    static void UpgradeAccountCargo(CUser* user);
    static void DeleteAccountCargo(CUser* user);  // ORIG 0x0828b4ec this 不入栈（S23A_DB 取证；Main 代落，mangled 不变）
    static void SendNotifyRecipe(CUser* user, int slot, bool flag);  // ORIG 无 this（0x828b3d0 参数自 +8 起）

    // ---- 槽位 / 物品 ----
    Inven_Item GetSlot(int slot);
    Inven_Item* GetSlotRef(int slot);
    // [b28d-Main 代落·S282 P292档①] 三处 int→bool：ORIG 0x086600f9/0x08660127/
    // 0x086601d0 调用点均 `call; xor $0x1,%eax; test %al,%al; je`。返回类型不入 mangled。
    bool CheckInsertCondition(Inven_Item& item);
    int InsertItem(Inven_Item& item, int slot);
    // [b28d-Main 代落·S282 P292档①] int→bool：ORIG 0x08661e76/0x0866232b 调用点
    // `call; mov %al,槽; cmpb $0,槽; je`（bool 直存）。返回类型不入 mangled。
    bool DeleteItem(int slot, int count);
    bool MoveItem(int from, int to);
    int GetEmptySlot();
    bool CheckSlotEmpty(int slot);
    int GetSpecificItemSlot(int itemIdx);
    bool CheckStackLimit(int slot, int itemIdx, int count);
    bool CheckValidSlot(int slot) const;   // ORIG xor$1 直判链（NL3 实证，mangled 不变）
    int GetItemCount();

    // ---- 金钱 ----
    bool DepositMoney(unsigned int money);
    // ORIG caller Dispatcher_DepositMoney::dispatch_sig 0x0822508c `test %al,%al; je`
    // 直判 ⇒ bool 返回（int 下 4.4 产 setne 归一段；DispS18 证据/Main 代落 batch-18）
    bool WithdrawMoney(unsigned int money);
    // [b28d-solo-CAccountCargo 代落 P292/P168a] int→bool：ORIG 0x0828a1f0 段
    // `call <CheckMoneyLimit>; xor $0x1,%eax; test %al,%al; je`（bool callee 判据）；
    // int 声明产 `test %eax,%eax; sete %al`。定义在 CAccountCargo.cpp（返回 0/1），
    // 返回类型不入 mangled；消费点仅 CAccountCargo.cpp:396。
    bool CheckMoneyLimit(unsigned int money);
    void AddMoney(unsigned int money);
    void SubMoney(unsigned int money);
    unsigned int GetMoney();

    // ---- 容量 / 发包 ----
    int GetCapacity();
    void SetCapacity(unsigned int capacity);
    void SetStable();
    void SendNotifyMoney(ENUM_CMDPACKET cmd);
    bool SendItemList();   // b28d-5：ORIG send_itemspace 0x0865dcaf `call; mov %eax,%ebx`
                           // 直传（caller 亦 bool）⇒ bool
    void MakeItemPacket(PacketGuard* packet, int slot) const;
    char IsAlter();

    CUser* m_pUser;                    // +0x00
    Inven_Item m_slots[0x38];          // +0x04（0xd58 = 0x38 * 0x3d）
    unsigned int m_money;              // +0xd5c
    unsigned int m_capacity;           // +0xd60
    char m_alter;                      // +0xd64
    char m_padAfterAlter[3];           // +0xd65..0xd67（ORIG 全反汇编无任何直接访问；
                                       //   脏标记仅 m_alter@+0xd64，SetStable/IsAlter；tail pad 保留）
};

#pragma pack(push, 1)
class STBreakAwayRewards
{
public:
    STBreakAwayRewards();
    void clear();
    void operator=(const STBreakAwayRewards& other);

    unsigned int m_rewardItemId;   // +0x00（0x703c4；ORIG 0x08439204 单条装载 call CStreamGuard::lsEj）
    int m_rewardAItemCnt;    // +0x04
    int m_luckPoint;         // +0x08
    unsigned int m_fieldc;   // +0x0c（ORIG 0x08439277 单条装载 call CStreamGuard::lsEj）
    int m_itemOrder;         // +0x10
    bool m_field14;          // +0x14（ORIG 0x086973cc movzbl 读）
    bool m_field15;          // +0x15（ORIG setLinkCharacDisconnectFlag 0x086973dc this-先 bool 写形）
    bool m_field16;          // +0x16（ORIG 0x08697408 movzbl 读）
};
#pragma pack(pop)


class Client_Spec
{
public:
    // ORIG C1 W 0x0822b8b0：对 +0x04 起的 0xff 个 HWSpec（stride 0xc）逐个构造。
    // +0x00..+0x03 ORIG ctor 不触碰。
    Client_Spec() {}

    char m_pad0[4];         // +0x00..0x03（ctor 不触碰区）
    HWSpec m_specs[255];    // +0x04..0xbf7（ORIG ctor 0x0822b8b0 循环构造，0xff × 0xc = 0xbf4）
    // [b28d-Main·检查点14] 尾部 0x18 字节（原 m_birthDay..m_seedFromDate）**不属于**
    // Client_Spec：ORIG ctor 0x0822b8b0 循环计数 esi=0xfe→0（cmp $-1;setne）共
    // 0xff 次 HWSpec ⇒ sizeof = 4 + 0xff×0xc = 0xbf8；MSG_CLIENT_SPEC_STATISTIC
    // 内嵌对象尺寸独立实证 0xbf8（ORIG dispatch_template 0x082556c2 `sub esp,0xc20`
    // + `lea -0xc17(%ebp)` ⇒ 消息 0xc06 = 0xe + 0xbf8）。该 7 字段已迁至 CUser 本体
    // （+0x711d4..+0x711eb，绝对偏移不变，见 CUser 类 m_clientSpec 之后）。
};

// ORIG _header —— 网络包头（13 字节，0x8647a54 memcpy 实证）：
// pack(1) 代落（batch-25c Lane22 实证：CPacketVerifier::UpdatePacket ORIG 读 movzwl 0x1(%eax)=m_id，
// 无 pack 时自然对齐 m_id 落 +0x2、sizeof=16 ⇒ 全字段错位 bug 级）。
#pragma pack(push, 1)
struct _header
{
    unsigned char m_class;     // +0x00（ENUM_PACKETCLASS）
    unsigned short m_id;       // +0x01
    unsigned short m_size;     // +0x03（13..0x578）
    unsigned short m_field05;  // +0x05
    unsigned int m_checksum;   // +0x07
    unsigned short m_seq;      // +0x0b（client sequence）
};
#pragma pack(pop)
static_assert(sizeof(_header) == 13, "ORIG packet header is 13 bytes (0x8647a54 memcpy)");

// ORIG W 0x8696d5c/0x8696d74/0x8696dd8；sizeof 0x8。
class CPacketTimeChecker
{
public:
    CPacketTimeChecker() {}
    ~CPacketTimeChecker() {}

    void reset();              // ORIG W（count/last 清零）
    bool check();              // ORIG W（节流：窗口内超 count 返回 false）

    static int validCount_;    // D 0x093ace48 = 100（ORIG 成员名含尾下划线）
    static int validSecond_;   // D 0x093ace4c = 1

private:
    int m_count;               // +0x00
    long m_lastTime;           // +0x04
};

// ORIG ctor/dtor 0x8696fe0/0x8697002、Init 0x8696de4、UpdatePacket 0x8696e58、
// GetSequence 0x8696e48；sizeof 0x28（+0x20 为 CPacketTimeChecker）。
class CPacketVerifier
{
public:
    CPacketVerifier();
    ~CPacketVerifier() {}

    void Init() throw();       // ORIG W：全部字段清零 + checker.reset()（P216：b28 BL0 代落，消 C1Ev 0x8696fe0 侧 4.4 冗余 landing）
    int UpdatePacket(const _header& hdr);  // ORIG W：重复包检测，返回错误码
    int GetSequence();          // ORIG W 0x08696e48：读 +0x14（b28d-Main 代落 P159/SIGFIX：尾双段 `movzwl 0x14(%eax); movzwl %ax,%eax` ⇒ int 返回，D52 探针实证；推翻旧 N18a 判词）

    static int m_nVaridCount;        // D 0x093ace38 = 5
    static int m_nVaridSecond;       // D 0x093ace3c = 1
    static int m_nAuctionVaridCount; // D 0x093ace40 = 6
    static int m_nAuctionVaridSecond;// D 0x093ace44 = 1

private:
    int m_lastClass;           // +0x00
    int m_lastId;              // +0x04
    int m_lastChecksum;        // +0x08
    int m_sameCount;           // +0x0c
    long m_sameTime;           // +0x10
    unsigned short m_sequence; // +0x14
    int m_auctionCount;        // +0x18
    long m_auctionTime;        // +0x1c
    CPacketTimeChecker m_checker; // +0x20
};

namespace WongWork
{
struct SUserPremiumInfo
{
    int m_type;        // +0x00
    int m_start;       // +0x04
    int m_end;         // +0x08
    int m_flag;        // +0x0c
    int m_state;       // +0x10
};

enum ENUM_PREMIUM_STATE
{
    ENUM_PREMIUM_STATE_0 = 0,
    ENUM_PREMIUM_STATE_1 = 1,   // 生效中
    ENUM_PREMIUM_STATE_2 = 2,   // 等待开始
    ENUM_PREMIUM_STATE_3 = 3    // 已过期
};

class CUserPremium
{
public:
    // ORIG C1 W 0x08694b0e：_Additioal_info C1(+0x8bd) → _Additioal_info C1(+0x90f)
    // → std::map<int,int> C1(+0x964, m_returnItemMap 真实成员) → InitPremium()。
    // 三成员均真实类型,编译器按声明序自动构造即与 ORIG 同形;原三条 memset 冗余
    // (多 22 行 DIFF ndiff18),删。InitPremium 定义 CUserPremium.cpp:668（CUserC8）。
    CUserPremium() { InitPremium(); }
    ~CUserPremium() {}

    // ---- 基础访问器（ORIG W 弱符号，const）----
    unsigned short getOverSkillLevel() const;
    int GetAdvantageExpRate() const;
    // [b28d-Main 代落·S226 P211/P292] short→unsigned short：ORIG 0x0812cdb4
    // `movzwl 0x850(%eax),%eax`（零扩展）。返回类型不入 mangled。
    unsigned short GetAdvantageFatigueRate() const;
    int GetAdvPremiumCount() const;
    int GetTotalPremiumCount() const;   // ORIG W 0x0863bd5c：+0x848 + +0x84c
    int GetOverEquipableLevel(ENUM_EQUIPMENTTYPE type) const;
    int GetAdvantageCoin() const;           // 0x0864f8e0（推断实现，见 CUser.cpp）
    int GetGoldBonus(int level);            // ORIG W _ZN8WongWork12CUserPremium12GetGoldBonusEi 0x08694a64（非 const；定义 WongWorkMiscB_A.cpp）
    int GetAddQuestItemDropRate(int level); // ORIG W _ZN8WongWork12CUserPremium23GetAddQuestItemDropRateEi 0x08694a28
                                            //   level∉[0,0x69]→0，否则 g_SPremiumInfo[level].m_field48
    int GetAddExpBonus(int type) const;     // ORIG W _ZNK8WongWork12CUserPremium14GetAddExpBonusEi 0x085bf940
                                            //   type∈[0,0x69]→g_SPremiumInfo[type].m_field44，否则 0（定义 CUserPremium.cpp）
    bool isAffectedUnlimitFatigue() const;  // ORIG 存在（推断实现，见 CUser.cpp）
    _Additioal_info GetAdditionalInfo() const;  // ORIG W 0x8694a88：memcpy(this+0x90f, 0x52)
    unsigned short getCeraShopDiscountRate() const;  // ORIG W 0x08328100：movzwl +0x860（W6_CeraShopDeps 消费）
    // ORIG _ZN8WongWork12CUserPremium15LoadPremiumInfoEPKc（静态：ORIG 调用点
    // +0x2638 仅压 path 一参；CDataManager::init 调用，返回出错行码
    // 0x15c/0x1bc/0x1c4，0=成功；实现见 CUserPremium.cpp）
    static int LoadPremiumInfo(const char* path);
    // ---- 生命周期 / 状态操作（ORIG T）----
    void InitPremium();
    void AddPremium(int type, int start, int end, int flag);
    void RemovePremium(int type);
    bool startPremium(int type, bool force);   // ORIG 0x08634aa6 调用点 xor $0x1,%eax ⇒ bool 返回
    void ReCalcAdvantage();
    void setPremiumState(ENUM_PREMIUM_TYPE type, ENUM_PREMIUM_STATE state);
    bool _CheckApply(int type, tm t);
    void RecalcAdditionalInfo(CUser const* user);
    // ORIG 调用方仅压 (key,&data) 两参（LoadPremiumInfo 0x86aeca4 内），
    // 实为静态语义；经对象语法调用亦合法。
    // [b28d-Main·检查点17 P321-T3] int→bool：ORIG 调用点 0x086af61d/0x086af679
    //   `call; xor $0x1,%eax; test %al,%al; je`（无 setcc）⇒ callee bool；mangled 不变。
    static bool SetPremiumItemData(unsigned long key, const struct STPremiumItemData& data);

    // ---- 查询（ORIG T const）----
    SUserPremiumInfo* GetPremiumInfo(int type) const;
    SUserPremiumInfo CheckPremiumTimeout() const;
    // [b28d-Main·检查点22] 删 out-ref 重载：ORIG 只有按值 sret 版
    //   `_ZNK8WongWork12CUserPremium18GetPremiumInfoListEi`（0x086ade9c），
    //   两参版 `_...ERSt6vectorISsEi` ORIG 无 ⇒ 我方发明；全仓调用点
    //   （CHandlePremium.cpp:141 / Inter_LoadEtc_impl.cpp:709 / CDeathTower.cpp:969）
    //   均走按值版，两参版零调用者。定义同步从 CUserPremium.cpp 删除。
    // ORIG T 0x086ade9c _ZNK8WongWork12CUserPremium18GetPremiumInfoListEi（sret
    // 按值返回版；与上方 out-ref 重载并存。定义 W6_SecMiscDeps.cpp）。
    std::vector<SUserPremiumInfo> GetPremiumInfoList(int type) const;
    int GetReturnItemRate(int type) const;
    bool CheckPremium(int type) const;
    // ORIG _ZN8WongWork12CUserPremium19IsRestrictedPremiumEi：mangled 无 this
    //（调用点 cdecl 单参），故为 static（W6_ARAD.LoginSecurity 收口确认）。
    static bool IsRestrictedPremium(int type);
    bool IsNeedCheckEveryday();                // ORIG W 0x086b086c：返回 field862 字节
    std::vector<std::pair<int, int> > GetAdvantageItem(int type);
    // ORIG W 0x0830ed48：type∈[1,4] 且 (int)level∈[0,0x69] 时返回
    // g_SPremiumInfo[level].m_independentDropRates[type-1]，否则 0。
    unsigned int GetAddIndependentDropRate(int type, unsigned int level);

    SUserPremiumInfo m_entries[0x6a];   // +0x00..0x847（0x6a×0x14；InitPremium/AddPremium/GetPremiumInfo 以 this+slot*0x14 索引 5×int 条目）
    int m_normalCount;      // +0x848
    int m_restrictedCount;  // +0x84c
    unsigned short m_outFatigueRate; // +0x850（GetAdvantageFatigueRate 0x0812cdb4 movzwl）
    char m_pad852[2];       // +0x852..0x853
    int m_outPremiumCount;  // +0x854
    int m_outExpRate;       // +0x858
    bool m_overEquipFlag;   // +0x85c（overEquipFlag；isAffectedUnlimitFatigue movzbl 按 bool 读）
    char m_pad85d[1];       // +0x85d
    unsigned short m_overSkillLevel; // +0x85e（getOverSkillLevel 0x0860939e movzwl）
    unsigned short m_ceraDiscountRate;  // +0x860（getCeraShopDiscountRate movzwl 源；
                                        //       ReCalcAdvantage 取 def.m_field5a 较大者）
    bool m_field862;                    // +0x862（startPremium 依 def.m_applyDayMask 置位；ORIG IsNeedCheckEveryday movzbl 按 bool 读取）
    char m_pad863[1];       // +0x863
    int m_overEquipableLevel[0x16];  // +0x864..0x8bb（additionalInfo[0x58]，GetOverEquipableLevel 按 type 索引）
    char m_field8bc;        // +0x8bc（field8bc）
    _Additioal_info m_additionalBase;   // +0x8bd..0x90e（ORIG C1 0x08694b0e memset 0x52；RecalcAdditionalInfo/ReCalcAdvantage 读写此区）
    _Additioal_info m_additionalInfo;  // +0x90f（GetAdditionalInfo 0x8694a88 memcpy 源）
    char m_pad961[0x964 - 0x961];      // +0x961..0x963
    std::map<int, int> m_returnItemMap;  // +0x964..0x97b（ORIG 真实成员；C1/D1 自动构造/析构）
    void* m_mailBox;        // +0x97c
    char m_pad980[4];       // +0x980..0x983
    int m_lastPlayTime;     // +0x984
    int m_loginTime;        // +0x988（0x71ba4）
    unsigned char m_field98c;   // +0x98c
    char m_pad98d[3];       // +0x98d..0x990
};


class CMailBox;
class CMailBoxHelper;  // 完整声明见 CGameManager.h（含 CMailBox::CMail 依赖）

class CSecurityCard
{
public:
    CSecurityCard() {}
    ~CSecurityCard() {}

    int getCancelCnt();                       // ORIG W 0x0822ef4c：读 +0x48
    void resetCancelCnt();                    // ORIG W 0x086959a8：清 +0x48
    bool isActivate() const;    // ORIG W 0x0822ef0e：读 +0x2f
    bool isCertified() const;   // ORIG W 0x0822ef1a：读 +0x30
    unsigned int getFailCnt();  // ORIG W 0x0822ef26：读 +0x34
                                // [CUserLane 2026-09-11 · C-7 结构性正解] ORIG 0x08209eb2
                                //   调用点 `cmp $0x2,%eax; seta`（无符号）；同函数 CPad 侧
                                //   0x08209f0f `setg`（有符号）⇒ 返回 unsigned。A/B 实测：
                                //   与消费点 cast 形 codegen 逐符号全等（零劣化）。
    char getCertFlag();         // ORIG W 0x086959b8：读 +0x31（movzbl）
    int getRequestState();      // ORIG W 0x0822ef58：读 +0x44
    // ---- WongWorkA 批追加（worklist A 段 ORIG-only 符号）----
    enum ENUM_REQUEST_STATE
    {
        eRequestStateNone = 0,   // Dispatcher_SecurityCardAuthCancel/AuthRpy: setRequestState(0)
        eRequestStateAuth = 1    // CUser::sendBackSecurityAuthRequest: setRequestState(1)
    };
    void setRequestState(ENUM_REQUEST_STATE state);  // ORIG W 0x0822ef64：写 +0x44（_ZN…15setRequestStateENS0_18ENUM_REQUEST_STATEE）
    void generateQuestion(short& a, short& b, short& c,
                          short& d);          // ORIG T 0x085fddb2

    int getCertCnt();                 // ORIG W 0x0826528a：读 +0x38
    int incFailCnt();                 // ORIG W 0x082652ae：+0x34 自增并返回新值
    void resetFailCnt();              // ORIG W 0x082652c8：写 +0x34 = 0
    void setActivate(bool flag);      // ORIG W 0x084ebd82：写 +0x2f
    void setCertified(bool flag);     // ORIG W 0x08265296：写 +0x30
    void setCertFlag(char flag);      // ORIG W 0x084ebd9a：写 +0x31（字节）
    bool isCertFlagOn();              // ORIG W 0x0822ef72：读 +0x31 == 1
    int incCancelCnt();               // ORIG W 0x0822ef32：+0x48 自增并返回新值
    void init();                      // ORIG T 0x085fdd68：清 +0x2f/0x30/0x34/0x44/0x48/0x38/0x31/0x40
    void setSecurityCardInfo(char const* serial, bool activate, bool certified,
                             int certCnt, int certFlag, int cancelCnt,
                             char* answers);   // ORIG T 0x085fde9c
    // ORIG 0x085fdf6a _ZNK8WongWork13CSecurityCard11checkAnswerEhh：四问全 0
    // 返回 false；否则比较答案表 this+8[m_question3d*10+m_question3c] 等两元组。
    bool checkAnswer(unsigned char ansA, unsigned char ansB) const;   // 定义 W6_SecMiscDeps.cpp

    char m_pad[0x2f];             // +0x00..0x2e（未命名前段，保持 CSecurityCard 真实偏移）
    bool m_activate2f;              // +0x2f（isActivate；P33 bool 字段使 setter 生成 ORIG 形态）
    bool m_certified30;             // +0x30（isCertified；同上）
    char m_certFlag31;              // +0x31（getCertFlag/isCertFlagOn；ORIG setCertFlag
                                    //   0x084ebd9a this先+movzbl回读序，探针实证 char 字段唯一匹配）
    char m_pad32[2];                // +0x32..0x33
    int m_failCnt34;                // +0x34（getFailCnt/incFailCnt）
    int m_certCnt38;                // +0x38（setSecurityCardInfo 第6参写入；getCertCnt 读）
    char m_question3c[4];           // +0x3c..0x3f（generateQuestion 问题四元组）
    unsigned char m_pad40;          // +0x40（ORIG init 清零；WongWorkA 核证）
    char m_pad41to43[3];            // +0x41..0x43
    int m_requestState44;           // +0x44（setRequestState/getRequestState）
    int m_cancelCnt48;              // +0x48（getCancelCnt/incCancelCnt）
};

}


// CodeHackCheckStorage 唯一 owner 已收敛到 CodeHackChecksum.h：
// ORIG ctor 0x08575c22 仅构造 std::queue<CodeHackChecksumEx>（sizeof 0x28）；
// 原 +0x28..+0x64「具名字段」实为紧随校验队列的服务器/惩罚数据区（历史误并），
// 现以下方兄弟块承载（总大小/偏移与原视图一致：0x28+0x40=0x68）。
struct PostChecksumUserInfoBlock
{
    // 起始 = CUser+0x79624（m_codeHackCheck 后）；字段偏移为块内相对。
    int m_field28;                   // +0x00 → CUser+0x79624（getStdDropRate 旧视图）
    unsigned int m_tradePunishType;  // +0x04 → CUser+0x79628（DelTradePunishType 0x086898c2 dest=member 形需 uint）
    bool m_tradePunishOverThirtyDays; // +0x08 → CUser+0x7962c（SetTradePunishOverThirtyDays ORIG 0x08689850）
    bool m_field31;                  // +0x09（0x7962d tournament account）
    char m_pad32[2];                 // +0x0a..0x0b
    ENUM_SERVER_GROUP m_serverGroup; // +0x0c（SetServerGroup 0x082300ac this-先载 ⇒ 同型 enum）
    ENUM_SERVER_GROUP m_onlineServerGroup;  // +0x10（0x79634）
    unsigned int m_onlineCharacNo;   // +0x14（0x79638）
    int m_onlinePlayType;            // +0x18（0x7963c）
    bool m_field44;                  // +0x1c
    char m_pad45;                    // +0x1d
    short m_totalFatigueUsed;        // +0x1e
    int m_field48;                   // +0x20
    int m_field4c;                   // +0x24
    CGameMasterCharacter* m_gameMasterCharac;  // +0x28（0x7964c）
    unsigned int m_field54;          // +0x2c
    bool m_firstLogin;               // +0x30（0x79654）
    bool m_field59;                  // +0x31
    bool m_field5a;                  // +0x32
    char m_pad5b;                    // +0x33
    int m_field5c;                   // +0x34
    bool m_field60;                  // +0x38
    char m_pad61;                    // +0x39
    short m_field62;                 // +0x3a
    int m_field64;                   // +0x3c

};

class CCharacterView
{
public:
    CCharacterView() {}
    ~CCharacterView() {}

    void reset();
    void enableSaveCharacView();
    void disableSaveCharacView();
    bool isSaveCharacView();

    bool m_bSaveCharacView;  // +0x00
    char m_pad1[3];
};

namespace charac_expand
{
class CDataMgr
{
public:
    CDataMgr();
    ~CDataMgr();

    void reset();
    void ResetDailyMidnight();
    void ResetDaily();
    CExpandEquipslot* GetData(ENUM_CHARAC_EXPAND_TYPE type) const;
    CExpandEquipslot* GetDataR(ENUM_CHARAC_EXPAND_TYPE type) const;
    // ORIG T 0x0832acd0：20 个 CData* 槽（m_pad）isSave→saveData 全链；任一失败返回 0。
    char update(CUser* pUser);
    // ORIG T 0x0832ad30：`this[type*4]`（CData* 数组槽）isSave→saveData 链。
    char update(CUser* pUser, ENUM_CHARAC_EXPAND_TYPE type);

    CData* m_pad[0x14];  // +0x00（CData*[0x14] 槽数组；ResetDaily/update 遍历）
};
}

class InventoryMemory
{
public:
    // ORIG _ZN15InventoryMemoryC1Ev W 0x0822d3b8（0xb9 字节）：四段 Inven_Item 数组
    // 逐元素默认构造（312/105/242/152，stride 0x3d）后调 reset()。
    InventoryMemory() : m_items1(), m_items2(), m_items3(), m_items4() { reset(); }
    // ORIG _ZN15InventoryMemory5resetEv W 0x0822d472：四段 memset（见 CUser.cpp 实现）。
    void reset();

    Inven_Item m_items1[312];     // +0x0000（0x138 项 ×0x3d = 0x4a58）
    Inven_Item m_items2[105];     // +0x4a58（0x69 项 = 0x1905）
    Inven_Item m_items3[242];     // +0x635d（0xf2 项 = 0x39aa）
    Inven_Item m_items4[152];     // +0x9d07（0x98 项 = 0x2438）
    // [负结论 · CUserWorker 2026-09-11 · 实测已回滚] Q8OldNAgain.Q8S5 的 `m_tailC13f` 删除
    //   （sizeof 0xC140→0xC13F；证据：ORIG reset 四段 memset 和 = 0xC13F / C1Ev 只构造
    //   312/105/242/152 个 Inven_Item / CTradeSpace 相邻栈对象间隔 0xC13F）在 **CUser** TU
    //   实测**劣化**：四数 809/1719/5/66 → **809/1712/12/66**（ae −7、near 5→12，多件
    //   AE→NEAR）+ CUserOnlyOrig 497/977/1/42 → 497/976/2/42（ae −1、near +1）
    //   （同轮 CUserCharacInfo 251/390/1/4 → 251/391/1/3 与 WongWorkMiscB_A 92/145/0/1
    //   为改善）⇒ 按 Main「任一 TU 劣化即回滚」指令**恢复该成员与 0xc140 断言**。
    //   推断：栈上/成员侧 `InventoryMemory` 的 sizeof 变化使 CUser 系列 TU 的帧/拷贝 lowering
    //   偏离 ORIG（ORIG 侧 CUser+0x858d4 起点在两种 sizeof 下都自洽 ⇒ 该证据不足以定尺寸）。
    //   [Main 2026-09-11 裁决] **全局净账为负**：CUser 家族 −8 ae（+7 near）vs `WongWorkMiscB_A`
    //   +1 near ⇒ 不落地；Q8S5 的证据降级为「CUser 家族实测劣化」。判据 = 受影响 TU 全集的
    //   四数净账，不是单 TU 读数。
    unsigned char m_tailC13f;     // +0xc13f（ctor/reset 均未覆盖，忠实未初始化）
};                              // sizeof 0xc140

typedef char _assert_inventory_memory_size[(sizeof(InventoryMemory) == 0xc140) ? 1 : -1];

enum QUEST_CONDITION
{
    QUEST_CONDITION_0 = 0,
    QUEST_CONDITION_1 = 1,
    QUEST_CONDITION_2 = 2,
    QUEST_CONDITION_3 = 3,
    QUEST_CONDITION_4 = 4,
    QUEST_CONDITION_5 = 5
};

class _Quest_Authen_Data
{
public:
    _Quest_Authen_Data() throw();   // C++98 空异常规范：ORIG UserQuest C1 @0x8697b8a 两成员构造后 leave;ret 无 EH landing pad（无 throw() 时 4.4 对第二成员失败路径生 destroy pad，29 vs ORIG 13）；throw() 不进 mangled 零 ABI 险（CUserC8）
    void reset();

    short m_characNo;      // +0x00（set_authen_data cond0 v1）
    short m_characClass;   // +0x02（cond0 v2）
    short m_field4;        // +0x04（cond1；check_quest_type_condition case3 比较）
    char m_pad6[2];        // +0x06
    int m_field8;          // +0x08（cond2；subtype0 与 at2*1000 比较）
    char m_fieldc;         // +0x0c（cond3；subtype4 判非 0）
    char m_padd[3];        // +0x0d
    int m_field10;         // +0x10（cond4；subtype5 等级门槛）
    int m_field14;         // +0x14（cond5）
    int m_urgentQuest;     // +0x18（UserQuest+0x75f0；-1=无紧急任务）
    char m_authen[0x28];   // +0x1c（UserQuest+0x75f4..0x761b；SaveQuestNotify 10×int）
};  // sizeof 0x44（ORIG ctor/reset 显式 memset 0x48，尾部覆盖紧随的 m_field761c/d）

struct stSelectQuestParam;  // 完整定义见 Quest.h（引用形参仅需前向声明）

class CUser;  // 前置声明（UserQuest::m_user 指针成员）
#pragma pack(push, 1)   // ORIG sizeof 0x761d（setPcRoomAuth 写 CUser+0x8cef1 = m_quest+0x761d；
                        // Main 批准 2026-08-29：删臆造 m_field761d，尾部 bool 后无对齐）
class UserQuest
{
public:
    UserQuest() {}
    ~UserQuest() {}
    CUser* m_user;                    // +0x0000（权威布局见 UserQuest.cpp；缺失曾致
                                      //  其后全域 -4 偏移漂移，m_guildDB 落 +0x8d01c）
    bool isClearQuest(int questIdx) const;  // b28d-Main 终裁(P292)：ORIG 调用点 0x085a8320/0x086785da/0x0866e9a4 等 8+ 处 `call; xor $0x1,%eax; test %al,%al; je` ⇒ bool；整链(CQuestClear::isClearedQuest+STClearedQuest::m_cleared)同改 bool 后体仍直透
    bool IsDoingQuest(int questIdx) const;  // ORIG W 0x086acffe
                                            // _ZNK9UserQuest12IsDoingQuestEi（当前无定义）
    void set_parent(CUser* user);              // ORIG W _ZN10UserQuest10set_parentEP5CUser
    void reset();
    void resetClearQuest(int questIdx);  // ORIG _ZN9UserQuest15resetClearQuestEi（UserQuest.cpp 实现）
    void set_authen_data(QUEST_CONDITION cond, int v1, int v2);
    bool hasEpicQuest();                       // ORIG _ZN9UserQuest12hasEpicQuestEv（UserQuest.cpp 实现）
    int get_quest(SIG_LOAD_QUEST* sig) const;  // ORIG _ZNK9UserQuest9get_questEP14SIG_LOAD_QUEST（UserQuest.cpp 实现）
    bool set_quest(SIG_LOAD_QUEST const* sig);  // ORIG T @0x086ab95c；串定 bool（b28c SIGFIX，C28L9 代落）
    void get_quest_info(char* buf) const;  // ORIG T _ZNK9UserQuest14get_quest_infoEPc @0x086abba8（UserQuest.cpp:269/390 已有定义；set_pvp_grade 7165 消费）
    bool get_mail_quest_info(std::list<int>& questList,
                             stSelectQuestParam const& selectParam) const;
    // ORIG _ZNK9UserQuest19get_mail_quest_infoERSt4listIiSaIiEERK18stSelectQuestParam
    // 0x086abd7a（UserQuest.cpp 实现；_onLevelUp 调用点 test %al 实证返回 bool）
    int get_rescue_questmap(int mapIdx) const;  // ORIG T 0x086aca16 _ZNK9UserQuest19get_rescue_questmapEi（UserQuest.cpp 实现）
    int item_reward_quest(int mapIdx) const;                       // ORIG T 0x086ac954
    // ---- Wave6-CUser 补声明（ORIG T 实证；定义 owner=UserQuest.cpp 批）----
    int accept_quest(int questIdx, char* buf, int& out);        // T 0x086abf38 _ZN9UserQuest12accept_questEiPcRi
    bool check_cond_clear(int questIdx) const;                  // T 0x086acad0
    int finish_quest(int questIdx);                             // T 0x086ac854（ORIG 尾部 movzwl 0x75da 返 int）
    int giveup_quest(int questIdx);                             // T 0x086ac532（ORIG 尾部 mov $0x13 返 int）
    bool check_quest_condition(int questIdx);                   // T 0x086acf8a（UserQuest.cpp 实现）
    bool check_quest_type_condition(Quest* quest);              // T 0x086ace46（UserQuest.cpp 实现）
    bool check_quest_subtype_condition(Quest* quest);           // T 0x086acb52（UserQuest.cpp 实现）
    int set_trigger(int questIdx, char val, bool flag);         // T 0x086ac6ac
    int CheckDungeonEventStoryPause(int dungeonNo, int fieldNo, int mapNo) const;  // T 0x086ad054（UserQuest.cpp 定义）
    bool isDoingJobChangeQuest() const;                         // T 0x086ad48a（UserQuest.cpp 定义）
    void ResetUrgentQuestWaitingList();                  // ORIG T 0x086ad178


    WongWork::CQuestClear m_cleared;  // +0x0004（0x7531 = sizeof；含 +0x7530 save-flag）
    char m_pad7535[3];                // +0x7535（补齐至 m_quest 起点 0x7538）
    int m_quest[20];                  // +0x7538
    int m_trigger[20];                // +0x7588
    _Quest_Authen_Data m_authenData;  // +0x75d8（0x44；ctor 按 ORIG memset 0x48 覆盖尾部）
    bool m_field761c;                 // +0x761c（SetVillageAttackedReward 0x8cef0 实证）
};  // sizeof 0x761d（pack(1)；0x761d = CUser m_pcRoomAuth）
#pragma pack(pop)

#pragma pack(push, 1)  // 公会 DB 结构按 ORIG pack(1)（+0x9a int 非对齐；HEAD 原有，恢复）
// [Lane37CUser 2026-09-10 解环] STGuildDBInfoOnly 定义迁至独立叶子头
//   STGuildDBInfoOnly.h（本文件顶部 include；类型/成员/断言逐字节不变）。
//   本 pragma 区继续覆盖下方 STGuildAgitDBInfo 等（保持原 pack 语义）。

struct STGuildAgitDBInfo
{
    STGuildAgitDBInfo();   // ORIG C1 0x086940c6：清 byte+0（CUser.cpp 外联）

    char m_pad[1];   // +0x00
    bool m_todayGuildMember;  // +0x01（0x8d0de）
    char m_pad2[1];  // +0x02
    unsigned int m_usedCera;  // +0x03（0x8d0e0）
};
#pragma pack(pop)

namespace Sanicova
{
class CPad
{
public:
    CPad() {}
    ~CPad() {}

    bool isCertified() const;   // ORIG W 0x0822efe6：读 +0x25
    void setCertified(bool flag);     // ORIG W 0x0826530e：写 +0x25
    void setActivate(bool flag);      // ORIG W 0x084ebe08
    int getCancelCnt();               // ORIG W 0x0822efb4：读 +0x44
    int incCancelCnt();               // ORIG W 0x0822ef9a：+0x44 自增并返回新值
    void setCancelCnt(int count);     // ORIG W 0x084ebdfa：写 +0x44
    int getRequestState() const;      // ORIG W 0x0822efc0：读 +0x40（_ZNK…15getRequestStateEv）
    enum ENUM_REQUEST_STATE
    {
        eRequestStateNone = 0,   // Dispatcher_SecurityCardAuthCancel/AuthRpy: setRequestState(0)
        eRequestStateAuth = 1    // CUser::sendBackSecurityAuthRequest: setRequestState(1)
    };
    void setRequestState(ENUM_REQUEST_STATE state);  // ORIG W 0x0822efcc：写 +0x40（_ZN…15setRequestStateENS0_18ENUM_REQUEST_STATEE）
    bool isActivate() const;          // ORIG W 0x0822efda：读 +0x24
    int getFailCnt();                 // ORIG W 0x0822ef8e：读 +0x38
    char GetShakeData(int idx);       // ORIG T 0x08599936（定义 Sanicova.cpp:111）
    void setFailCnt(int cnt);         // ORIG W 0x0828712c：写 +0x38
    char getVersionInfo();            // ORIG W 0x084ebdee：读 +0x26
    void setVersionInfo(char v);      // ORIG W 0x084ebdd6：写 +0x26
    // 随机密钥/扰动（sendBackSecurityAuthRequest 依赖；报告 Sanicova__CPad/）
    bool isValidKeyGenTime();         // ORIG T 0x085998a6（对比 +0x30 与当前秒）
    void GenerateRandomKeyIdx();      // ORIG T 0x08599918（+0x2e = rand(5000)）
    void GenerateRandomShakeData();   // ORIG T 0x08599946（+0x34..= rand(200)+20）
    void GenerateRandomData();        // ORIG T 0x085998ea
    unsigned short GetSelectedCipherIdx();  // ORIG T 0x0859984c（min(+0x2e,0x1387)）
    int GetKeyIdx(int idx);           // ORIG T 0x08599872（读密钥管理器表）
    // ORIG _ZN8Sanicova4CPad12setCheckTimeEl W 0x084ebe20：写 +0x28（m_checkTime28）。
    void init();                      // ORIG T 0x0859957c _ZN8Sanicova4CPad4initEv
    void reset();                     // ORIG T 0x08599616 _ZN8Sanicova4CPad5resetEv（认证 180s 超时判定）
    bool isTimeAttackSeccess();       // ORIG T 0x0859979c _ZN8Sanicova4CPad19isTimeAttackSeccessEv

    char m_pad[0x24];               // +0x00..0x23（setPassword memset 区）
    bool m_activate24;             // +0x24（isActivate/setActivate/init 清零；ORIG movzbl 按 bool 直返）
    bool m_certified25;            // +0x25（isCertified/setCertified；ORIG movzbl 按 bool 直返）
    char m_version26;               // +0x26（setVersionInfo/getVersionInfo）
    char m_pad27[1];                // +0x27
    long m_checkTime28;             // +0x28（setCheckTime/reset 认证时间戳；4 字节到 0x2c）
    char m_pad2c[2];                // +0x2c..0x2d（ORIG 仅此 2 字节间隙；m_pad2a 系注释重叠误增，
                                    //   Main 批准 2026-08-29 收敛致 CPad 0x4c→0x48）
    unsigned short m_keyIdx2e;      // +0x2e（GenerateRandomKeyIdx/GetKeyIdx）
    int m_lastGenTime30;            // +0x30（isValidKeyGenTime/init 写当前秒）
    char m_shake34[4];              // +0x34..0x37（GenerateRandomShakeData）
    int m_failCnt38;                // +0x38（getFailCnt/setFailCnt/init 清零）
    bool m_advidePwd;               // +0x3c（是否建议改密；getAdvideChangePwd；ORIG 0x084ebdb2 P33 bool 截断形实证）
    bool m_changedPwd;              // +0x3d（是否已改密；setChangedPwd；ORIG 0x085999e8 同上）
    // ORIG _ZN8Sanicova4CPad12setCheckTimeEl W 0x084ebe20：写 +0x28（m_checkTime28）。
    void setCheckTime(long t);
    void setPassword(char* pwd);      // ORIG W 0x0828713a _ZN8Sanicova4CPad11setPasswordEPc（写 +0x00 0x24 串区）
    char m_pad3e[2];                // +0x3e..0x3f
    ENUM_REQUEST_STATE m_requestState40;  // +0x40（setRequestState/getRequestState/init；b26 NL13 同型化翻案：ORIG 0x0822efcc this-先 store 实证）
    int m_cancelCnt44;              // +0x44（getCancelCnt/incCancelCnt/setCancelCnt）
    bool getAdvideChangePwd();              // W 0x08265302：读 +0x3c（movzbl 直返=bool 直返形）
    void setAdvideChangePwd(bool flag);     // W 0x084ebdb2：写 +0x3c
    void setChangedPwd(bool flag);          // W 0x085999e8：写 +0x3d
    bool getChangedPwd();                   // W 0x084ebdca：读 +0x3d（movzbl 直返）
    void SendChagePwdReward(CUser* user);       // T 0x085997b8
    void SendChagePwdRewardMail(CUser* user);   // T 0x0859981e
    // ---- MouseRegister/SecurityCardAuthRpy 批次补齐（ORIG nm 核实）----
    int incFailCnt();                          // ORIG W 0x082652d8：+0x38 自增并返回新值
    void resetFailCnt();                       // ORIG W 0x082652f2：写 +0x38 = 0
    char* getPassword();                       // ORIG W 0x08265326：返回 m_pad 首址（非 const）
    bool checkAnswer(char* answer) const;      // ORIG T 0x08599712
};
}

namespace CerashopAddRestrict
{
// ---- 商城限购数据结构（G4 商城/交易批次） ----
// ORIG mangled 用嵌套枚举 RestrictType::T（NS_12RestrictType1TE，LoadScriptDaily/
// InfoDaily ctor 实证），不能是普通 enum。
struct RestrictType
{
    enum T
    {
        RestrictType_Account = 0,
        RestrictType_Charac = 1
    };
};

struct InfoDaily
{
    // ORIG W ctor 0x08abf826（C2 同址）：7 参按序写入 7 字段。
    InfoDaily(RestrictType::T type, unsigned int ipgNo, unsigned int field8,
              long start, long end, int limit, int field18)
        : m_type(type), m_ipgNo(ipgNo), m_field8(field8), m_start(start),
          m_end(end), m_limit(limit), m_field18(field18)
    {
    }
    RestrictType::T m_type;  // +0x00（b28c C28L4 代落：ORIG ctor 0x08abf826 该 store 为 LHS-先形，int←enum 异型反产 RHS-先，P211）
    unsigned int m_ipgNo; // +0x04
    unsigned int m_field8;   // +0x08（同上：int←unsigned int 异型反产 RHS-先）
    int m_start;          // +0x0c
    int m_end;            // +0x10
    int m_limit;          // +0x14
    int m_field18;        // +0x18
};

struct paramDaily
{
    unsigned int m_ipgNo;  // +0x00
    // [b28d-solo-CCeraShopAddRestrict P211档③ 代落] int→unsigned int：
    // ORIG InputBuyInfoDetail 0x080e11fb `mov (%eax),%eax; mov %eax,%edx; add mem,%edx;
    // mov this,%eax; mov %edx,(%eax)`（形参 count 为 unsigned，mangled ...jj 不可动）；
    // 字段 int 形多 1 条且目标寄存器错。sizeof/偏移/mangled 不变；消费点仅
    // CCeraShopAddRestrict.cpp + DB_CerashopAddRestrict.cpp（后者已显式 cast）。
    unsigned int m_count;  // +0x04
    int m_nextDate;        // +0x08
};

class DBInputData
{
public:
    // ORIG W 0x080e243c：+0/+4 清零。无用户 dtor（ORIG 无 D1 符号，
    // 调用点不生成析构/异常清理调用）。
    DBInputData() : m_result(0), m_pInfo(0) {}

    // [b28d-solo-CCeraShopAddRestrict P211档③ 代落] int→paramDaily*：
    // ORIG 0x080e1375 / 0x080e1404 调用点 `call InputBuyInfoDetail; mov 0x18(%ebp),%edx;
    // mov %eax,(%edx)` 为同型 LHS-first 直存；int 字段（callee 返回 paramDaily*）
    // 反产异型 RHS-first + 中转（InputBuyInfo 121 vs 123）。sizeof 0x8/偏移/mangled
    // 不变；DB_CerashopAddRestrict.cpp:85/102 的 reinterpret_cast 同型仍合法。
    paramDaily* m_result;  // +0x00（InputBuyInfo 写入 InputBuyInfoDetail 的 paramDaily* 结果）
    InfoDaily* m_pInfo;   // +0x04
    // b28c C28L4 代落（Main）：原 char m_pad[0x8]（sizeof 0x10，Lane2R 推定）删除。
    // 证据：①ORIG W C1/C2 0x080e243c 仅 2 条 zero store（+0/+4）；②DB_SaveCerashopAddRestrictType
    // ::makeRequest 全 0x310 字节内 12 处装载点最大 disp 仅到 +4（+8..0xf 无访问）；
    // ③ORIG 调用点 UpdateBuyableRestrictItem 0x080e0d14 data 实参为 `lea -0x10(%ebp)`，
    // 我方 sizeof 0x10 时漂至 -0x18。注：与 Lane2R 旧推定相反，以本条直接反汇编为准。
    // sizeof 0x10→0x8。
};

class ScriptLoader
{
public:
    // [b28d-solo-CCeraShopAddRestrict P299/CWG434 代落] 删用户默认 ctor：
    // ORIG Manager::C1 0x080e09bc 在调 ScriptLoader::C1 前对 m_loader(+0x08..+0x18)
    // 发 5 条 movl $0（value-init 前置清零）；用户提供的空 ctor 会抑制该清零。
    // 隐式 ctor 仍发射 W C1/C2（map 成员非平凡，体同 ORIG size 0x13）。
    // sizeof/mangled/全部偏移不变；全仓仅 CCeraShopAddRestrict.cpp 消费本类。
    ~ScriptLoader() {}

    int LoadScript();
    int LoadScriptDaily(RestrictType::T type);
    void ClearScript();
    InfoDaily* GetRestrictInfo(unsigned int ipgNo);
    bool FindIpgNo(unsigned int ipgNo);

    std::map<unsigned int, InfoDaily*> m_infoMap;   // +0x00
};

// ORIG C1 0x080e09bc：ScriptInterface C1 → ARAD::Singleton<Manager>::Singleton() → vptr → 清
// +0x8..0x18 → ScriptLoader(+4)。空基 EBO 不动 sizeof/vptr（S22A 证，Singleton 族第 3 例）。
class Manager : public ARAD::ScriptInterface, public ARAD::Singleton<CerashopAddRestrict::Manager>
{
public:
    Manager();
    ~Manager();

    virtual bool Load();
    virtual int Destroy();
    bool FindIpgNo(unsigned int ipgNo);
    // [L6Free61to75.S1 2026-09-11 · P292 返回类型链] int→bool（L6 父泳道已 R2 广播）：
    // ORIG 0x080e0cdc `call FindUserInfoDetail; jmp 尾`、0x080e0ce2 `call CheckBuyableProduct;
    // leave;ret` 两级直返**均无归一化**，且唯一消费点 0x08176d00
    // `call IsBuyableRestrictItem; xor $0x1,%eax; test %al,%al; je` 证 bool（P292）
    // ⇒ 同链三函数必须同为 bool（返回类型不入 mangled/零布局）。
    bool CheckBuyableProduct(CUser* user, unsigned int ipgNo,
                             unsigned int itemIdx, int type);
    // ORIG T 0x080e0d14 _ZN19CerashopAddRestrict7Manager25UpdateBuyableRestrictItemEP5CUserjj
    //（LinkE2.JpnExImpl 批次修正为 static：ORIG 调用点 0x817915c esp0=user 无 this 实证）。
    static void UpdateBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                          unsigned int itemIdx);
    // ORIG 0x080e0ce2 (T)：static（无 this；内部经 Singleton<Manager>::Get() ->
    // CheckBuyableProduct）。返回 CheckBuyableProduct 结果（1=可购买；调用方取反判断）。
    // [b28d-Main·检查点14 P292] int→bool：ORIG 调用点 `call;xor $1;test %al`；mangled 不变。
    static bool IsBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                      unsigned int itemIdx, int type);
    static void ClearBuyRestrictItem(CUser* user);
    static void LoadRequestBuyRestrictItem(CUser* user);
    static void SendSyncPacket(CUser* user);
    void makeSyncPacket(CUser* user);
    // ORIG T 0x080e0e68 _ZN19CerashopAddRestrict7Manager20GetItemListForPacketERSt6vectorIjSaIjEERSt3mapIjPNS_10paramDailyESt4lessIjESaISt4pairIKjS7_EEE
    //（Main 2026-08-28 授权单行声明；禁止改动本类其它成员/布局）
    void GetItemListForPacket(
        std::vector<unsigned int, std::allocator<unsigned int> >& out,
        std::map<unsigned int, paramDaily*,
                 std::less<unsigned int>,
                 std::allocator<std::pair<unsigned int const, paramDaily*> > >& m);
    bool InputBuyInfo(CUser* user, unsigned int ipgNo, unsigned int itemIdx,
                      DBInputData& out);  // b28c C28L4 代落（档①）：ORIG 调用点 0x080e0d14 `test %al,%al; je` 无 setne 归一 + 体 0x080e129a 出口仅 mov$0/$1；int callee 四形探针全产 setne（P168a 定论）
    static void LoadProcess(std::map<unsigned int, unsigned int>&,
                            std::map<unsigned int, class paramDaily*>&,
                            struct SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE*);  // ORIG T @0x080e142a

    ScriptLoader m_loader;   // +0x04
    // b28c C28L4 代落（Main）：原 +0x08..+0x18 五个标量系臆造，已删。硬证据：ORIG 单例
    // obj `_ZZN4ARAD9SingletonI...ManagerEE3GetEvE3obj` nm -S size=0x1c（vptr4+m_loader0x18）；
    // C1 0x080e09bc 的 5 条 zero-init 落在 map(+4) 的 _Rb_tree_impl 内部（P90/N77 前置清零形）。
    // sizeof(Manager) 0x30→0x1c。
};

// 用户侧限购数据（CUser +0x8d1e4，尺寸 0x69）
// 注意：map 类型须在 pack 外先行实例化（GCC 4.4 pack 会压缩模板内部布局），
// 使 pack(1) 仅压缩 UserInfo 自身成员对齐（无尾部填充，0x69）。
struct CerashopUserInfoMapPrelude {
    std::map<unsigned int, paramDaily*> preludeDaily;
    std::map<unsigned int, unsigned int> preludeUInt;
};
#pragma pack(push, 1)
class UserInfo
{
public:
    UserInfo() {}
    ~UserInfo() {}

    std::map<unsigned int, paramDaily*> m_accountDaily;  // +0x00
    std::map<unsigned int, paramDaily*> m_characDaily;   // +0x18
    std::map<unsigned int, unsigned int> m_field30;      // +0x30
    std::map<unsigned int, unsigned int> m_field48;      // +0x48
    unsigned char m_field60;                               // +0x60（setSlotEffectCount 0x08230880 uchar 参→uchar 字段同型直写无归一；CUserB probe7 s4 纠正）
    unsigned char m_field61;                             // +0x61
    bool m_field62;                                      // +0x62
    char m_pad63;                                        // +0x63
    int m_ontimeLastRecvIdx;                             // +0x64
    bool m_updateOntime;                                 // +0x68
};
#pragma pack(pop)
}

// STExpReward / advancealtar 类型：本头直接 include 权威叶子头
// （STExpReward.h / advancealtar.h）；CDataManager.h 经 PvPTypes→CUser.h 重入环，
// 不能作为本头类型可见性的可靠来源。

// ---- APSystem 块依赖叶子头（均为无环自包含头）----
#include "APSystemTypes.h"   // _SIG_LOAD_ACTION_POINT/_ActionPoint/CActionPointEx
#include "CUserProc.h"       // APSystem::_ActionGroupIndex 完整类型（按值形参）
#include "STItemScript.h"    // ENUM_EXPERT_JOB_TYPE（全局枚举，轻量头）

namespace APSystem
{
class CActionPointManager
{
public:
    // ORIG 符号 _ZN8APSystem19CActionPointManager10_gmWeekdayE：D 0x0937008c，size 0x8。
    // 写点 oldGmRoutine 0x08212798/0x08212859（byte 置 flag）与 +0x4 写 weekday int。
    struct GmWeekdayCtl
    {
        unsigned char m_gmOn;         // +0x00
        unsigned char m_pad1[3];
        int m_weekday;                // +0x04（SetTodayAction 的 DAT_09370090）
    };

    CActionPointManager() : m_flag00(0), m_prevApSum(0) {}
    ~CActionPointManager() {}

    void Reset();                              // ORIG W 0x08122af4（CActionPointManager.cpp）
    unsigned char* GetActionPoint() const;     // ORIG 0x8694bfc（W）：返回 this+0x08 地址
    void GM_Set(CUser& user, unsigned int point);      // ORIG T 0x08121b3e
    void GM_Reset();                           // ORIG T 0x08121d12
    void Set(_SIG_LOAD_ACTION_POINT& sig);     // ORIG T 0x08121b0c
    bool ClearAction(CUser& user, _ActionGroupIndex group, int itemIdx,
                     ENUM_PACKETCLASS packetClass);                    // ORIG T 0x08121dcc
    bool ClearActionOne(_ActionPoint& action, int itemIdx);            // ORIG T 0x08121f1c
    void SetTodayAction(int level, unsigned int characNo,
                        ENUM_EXPERT_JOB_TYPE jobType,
                        const std::map<unsigned short,
                                       std::vector<APSystem::CActionPointEx> >& table); // ORIG T 0x0812161c
    bool GetTodayRewardAndSendMail(CUser& user,
                                   _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward); // ORIG T 0x08122104

    // [b28d-Main 代落·S262 P248档①] uchar→bool：ORIG 0x08121f31
    // `movzbl 0x93ff07c,%eax; movzbl %al,%esi`（2 条 = bool 形参直传；
    // uchar 源在 4.4 恒 movzbl;test;setne;movzbl 4 条）。静态成员无类内偏移。
    static bool _IsGMOn;             // B 0x093ff07c（1 字节）
    static GmWeekdayCtl _gmWeekday;  // D 0x0937008c（8 字节）

    unsigned char m_flag00;                 // +0x00（GM_Set/GM_Reset/GetToday… 置 1）
    unsigned int m_prevApSum;               // +0x04（Set/ClearActionOne 前值暂存）
    _SIG_LOAD_ACTION_POINT m_sig;           // +0x08（嵌入整包；GetActionPoint 返回此地址）
};                                          // sizeof 0xcfc（0xcf9 尾对齐到 4）
typedef char _assert_action_point_manager_size[(sizeof(CActionPointManager) == 0xcfc) ? 1 : -1];
}


namespace XNuclear
{
// ORIG 枚举 XNuclear::ENUM_HADES_SECURITY：ORIG 内仅作为参数按 int 值透传，
// 成员常量集未还原（调用方传 1/0x15..0x26），此处仅承载 ABI 名称。
enum ENUM_HADES_SECURITY { EHS_UNSET = 0 };
enum ENUM_GOLD_CONTROL_REASON { EGCR_UNSET = 0 };
static const unsigned int PACKET_SEND_TIME = 0x1e;   // ORIG _ZN8XNuclearL16PACKET_SEND_TIMEE r 0x4 (=30)

class CHades
{
public:
    CHades() {}
    ~CHades() {}

    void Send_ReturnToVillage();  // ORIG 0x084b9386（纯空函数）
    bool needLogin();             // ORIG 0x084b9718
    void Send_Login();            // ORIG 0x084b9200
    void StdCateUpdate(unsigned short cate, unsigned short type);  // ORIG 0x084b938c
    void UseFatigue(unsigned int count);                           // ORIG 0x084b9552
    void ExpUp(unsigned int flag);                                 // ORIG T 0x084b953e（转调 needLogin(flag)）
    void GoldTracking(unsigned char type, unsigned int money, char* ip,
                      unsigned int param4, unsigned int fatigue);
    void Send_ChangeGold();      // ORIG T 0x084b93ee（DisPatcher_CheckConn::dispatch_sig 消费，
                                 //   DisPatcherWave1 落声明；定义由 Main 分派属主批→U 引用合规）
    void RunHadesAction(unsigned int, unsigned int, unsigned int, unsigned int, char*);  // ORIG T _ZN8XNuclear6CHades14RunHadesActionEjjjjPc @0x084b95bc
                                      // ORIG W 0x084b949a（GoldTracking.md；G6 接线）
    void GetIP(char* out);        // ORIG 0x084b9774（peer IP + PC房 IP 回退）
    void Send_UserStatus();       // ORIG 0x084b97fe
    void Init(CUser* user);       // ORIG T 0x084b91e8 _ZN8XNuclear6CHades4InitEP5CUser（m_user/user 挂接 + m_useFatigueCnt 清零）

    std::map<unsigned short, unsigned short> m_map00;  // +0x00（ORIG C1 0x8697bc6 构造 map<ushort,ushort>）
    char m_pad18[4];                 // +0x18
    unsigned int m_useFatigueCnt;    // +0x1c（UseFatigue 计数，ORIG jbe 无符号比较实证）
    CUser* m_user;                   // +0x20（Init(CUser*) 写入；各发包取 acc_id/UID/mac）
    int m_connSec;                   // +0x24（GetHadesSocketConnSec 缓存，needLogin 比较）
    bool m_secuReward;               // +0x28（0x8e97c）
    unsigned char m_secuType;        // +0x29（0x8e97d）
    unsigned char m_rewardType;      // +0x2a（0x8e97e）
    char m_secuGrade;                // +0x2b（0x8e97f）
    void ChangeSecurity(ENUM_HADES_SECURITY type, unsigned int value);   // ORIG T 0x084b95a8
    void Send_Logout();                                                  // ORIG T 0x084b92ea
    void Send_HackType();                                                // ORIG T 0x084b9322
    void AddHackTypeCnt(unsigned long, unsigned long, unsigned long, unsigned long); // ORIG T 0x084b9328
    void Send_ChangeSecurity(ENUM_HADES_SECURITY);                       // ORIG T 0x084b93f4
    void GoldPlus(unsigned int, ENUM_GOLD_CONTROL_REASON);               // ORIG T 0x084b93fa
    void GoldMinus(unsigned int, ENUM_GOLD_CONTROL_REASON);              // ORIG T 0x084b944a
};
}  // namespace XNuclear

class Secu_GoldControl
{
public:
    Secu_GoldControl();   // ORIG C1 0x08694f68：3×user_gold_info C1（CUser.cpp 外联）
    ~Secu_GoldControl() {}

    void Init(CUser* user);
    void SetInfo(user_gold_info& info);
    void SetMailCharName(const char* name);
    void resetRangeMoney(user_gold_info& cur, user_gold_info& copy);  // ORIG 0x08287bdc：cur 清零/减 0xffffffff，copy 收副本/-1
    void AddGold(unsigned int value, eMoneyAddReason reason);
    void SubGold(unsigned int value, eMoneySubReason reason);
    void AddMoney(unsigned int value);
    void SubMoney(unsigned int value);
    void AddImportMoney(unsigned int value);
    void SubImportMoney(unsigned int value);
    void AddAuctionMoney(unsigned int value);
    void SubAuctionMoney(unsigned int value);
    void MailComplete(bool flag);
    void TradeComplete(int subValue, int addValue);
    void MailGold(unsigned int value, bool flag);
    void MailSameMid();
    void SavetoDB(bool a, bool b, bool c);
    void CheckMoneyRange();
    void* GetGoldInfo();
    void CheckDate();
    void UseFatigue(int count);

    CUser* m_pUser;                                // +0x00
    // 2026-08-30 Fix8_E：ORIG 为 3×user_gold_info 成员（ctor 0x08694f68 仅
    // 构造 +0x04/+0x40/+0x7c 三子对象；AddMoney 等 64 位运算直读相邻字段，
    // 无组合 or 序列）。原展开 uint 字段已折叠为 user_gold_info。
    user_gold_info m_total;        // +0x04
    user_gold_info m_cur;          // +0x40
    user_gold_info m_backup;       // +0x7c
    int m_fieldb8;                                 // +0xb8
    int m_fieldbc;                                 // +0xbc
    unsigned int m_dateFlag;                       // +0xc0
    int m_mailGold;                                // +0xc4
    char m_mailCharName[0x1e];                     // +0xc8
    unsigned char m_fielde6;                       // +0xe6
};

// ---- Secu_MonsterCheck 基类（ORIG 报告 Secu_MonsterCheck/README.md；9 函数族）----
// 布局证据：SetUserPtr(0x0827abec) 写 +0x00；Reset(0x0827ac7a) 清 +0x04/+05/+06、
// 检查 +0x10/+0x1a/+0x26 的 word==1；SetMonsterInfo(0x0827abfa) 写 +0x28..+0x33。
// Secu_HackLogCheck C1(0x08695ad0) 先调 Secu_MonsterCheck C1（同 this）→ 基类布局。
class Secu_MonsterCheck
{
public:
    Secu_MonsterCheck();   // ORIG C1 0x08695ab0：清 +4/+5/+6（CUser.cpp 外联）
    ~Secu_MonsterCheck() {}

    void SetUserPtr(CUser* user);                    // ORIG T 0x0827abec
    void SetMonsterInfo(const MSG_MONSTER_DIE* msg); // ORIG T 0x0827abfa
    void Reset();                                    // ORIG T 0x0827ac7a
    void init223(const MSG_MONSTER_DIE* msg);        // ORIG T 0x0827b288（LinkClosureD 补声明）
    void init224(const MSG_MONSTER_DIE* msg);        // ORIG T 0x0827b2e4
    void init225(const MSG_MONSTER_DIE* msg);        // ORIG T 0x0827b334
    // ORIG T Check223 @0x0827ad98 / Check224 @0x0827af78 / Check225 @0x0827b100
    // （报告 docs/class_func_reports/Secu_MonsterCheck/；返回 1 字节）。
    // 定义待批次（LinkClosureC 上报 Main 收敛清单）；Secu_HackLogCheck::DieMob 消费。
    // b28d-36：ORIG DieMob 0x0827a84x/0x0827a88x/0x0827a8a4 为 `call Check22x; mov %al,0x38(%edx)`
    // LHS-first 无中转 ⇒ 返回类型与 `unsigned char m_flag22x` 同型（P211 档①）；返回类型不入 mangled。
    unsigned char Check223();
    unsigned char Check224();
    unsigned char Check225();

    CUser* m_user;              // +0x00
    bool m_active223;  // +0x04（init223 置 1 / Reset 清零；b28c C28L20A/lane12 代落：X2 族 bool 字段 + 裸 !field 直产 ORIG movzbl;xor$1;test;je，见 P219/P168a 补录）
    // ORIG init224/225 0x…：`movzbl 0x5; xor $1; test; je`（P22 ⇒ bool，uchar+!=1 出 cmp$1 反）；
    // A822W 取证，LC_Misc 条件形同步（!m_active 裸测）。
    bool m_active224;           // +0x05
    bool m_active225;           // +0x06
    unsigned short m_field08;   // +0x08 Check223 上次击杀数快照
    unsigned short m_field0a;   // +0x0a Check223 上次 x
    unsigned short m_field0c;   // +0x0c Check223 上次 y
    unsigned short m_field0e;   // +0x0e Check223 计数
    unsigned short m_field10;   // +0x10（Reset 检查 ==1）
    unsigned short m_field12;   // +0x12 Check224 上次击杀数快照
    unsigned int m_field14;     // +0x14 Check224 上次 tick（b28c C28L12 代落：ORIG init224 @0x0827b306 LHS-first 直存 unsigned 返回值；int 反产 edx 中转，P211 同型化）
    unsigned short m_field18;   // +0x18 Check224 计数
    unsigned short m_field1a;   // +0x1a（Reset 检查 ==1）
    unsigned short m_field1c;   // +0x1c Check225 上次击杀数快照
    unsigned int m_field20;     // +0x20 Check225 上次 tick（同上，init225 @0x0827b35x）
    unsigned short m_field24;   // +0x24 Check225 计数
    unsigned short m_field26;   // +0x26（Reset 检查 ==1）
    unsigned short m_curKillCount;   // +0x28 SetMonsterInfo 写（MSG_MONSTER_DIE+0xd）
    unsigned short m_curX;           // +0x2a（MSG_MONSTER_DIE+0xa24）
    unsigned short m_curY;           // +0x2c（MSG_MONSTER_DIE+0xa26）
    unsigned int m_setTick;          // +0x30（getCurTickCount）
    CUser* m_user34;            // +0x34（Secu_HackLogCheck::Init 写入）
};  // sizeof 0x38

class Secu_HackLogCheck : public Secu_MonsterCheck
{
public:
    Secu_HackLogCheck();   // ORIG C1 0x08695ad0：基类 C1 + 置 +0x38/39/3a=1（CUser.cpp 外联）
    ~Secu_HackLogCheck() {}

    void Init(CUser* user);                 // ORIG T 0x0827a7d0（blocked：定义待补）
    void DieMob(const MSG_MONSTER_DIE* msg);// ORIG T 0x0827a81c（blocked：定义待补）
    void MoveMap();                         // ORIG T 0x0827a7f4；定义见 UserCoreDepsB2.cpp

    unsigned char m_flag223;    // +0x38（MoveMap 置 1 / DieMob 存 Check223 结果）
    unsigned char m_flag224;    // +0x39
    unsigned char m_flag225;    // +0x3a
    // b28c C28L27：三字段曾按「档②」bool 化，fresh 实测**劣化**（DieMob 41/46→41/49：bool 字段
    // 接收 char callee 返回值需 `test %al; setne %dl` 归一到 0/1），故回退。P248 档②适用前提
    // = 「该字段自身被裸测」；纯存储汇聚点（char callee → 字段）bool 化反引入归一化段。
};  // sizeof 0x3b → 与原 m_pad[0x3c] 对齐一致
class PIReceiverManager
{
public:
    PIReceiverManager();
    virtual ~PIReceiverManager();   // ORIG 虚析构（vtable _ZTV17PIReceiverManager@0x8af3490 仅 D1/D0 槽）

    bool CheckHashReceived(unsigned char* data, unsigned int len,
                           unsigned int key, bool bNewKey);  // ORIG T 0x0808bf7e

    char m_pad[4];  // +0x04（+0x00..0x03 为 vptr；+0x04 接收 hash key 字节）
};

class PISenderManager
{
public:
    PISenderManager();
    virtual ~PISenderManager();   // ORIG 虚析构（vtable _ZTV15PISenderManager@0x8af34a0 仅 D1/D0 槽）
    unsigned int MakeHashForSending(unsigned char* data, unsigned int len,
                                    bool bNewKey);  // ORIG 0x0808be6e（U3ShopDb）

    char m_pad[4];       // +0x04..0x07（+0x00..0x03 为 vptr；+0x04 为发送 hash key 字节）
    bool m_nonClientFlag;            // +0x08（0x8eaf4 GetNonClientFlag；ORIG SetNonClientFlag 0x08275694 bool 写形）
    char m_pad9[3];      // +0x09..0x0b
    int m_nonClientRandInt;          // +0x0c（0x8eaf8 GetNonClientRandInt）
    unsigned char m_field10;  // +0x10
    char m_pad11[3];   // +0x11..0x14
};

struct BingoData
{
    BingoData();   // ORIG C1 0x08697128：清 4 字段（CUser.cpp 外联）

    void clear();

    unsigned int m_eventId;   // +0x00（updateBingoData ORIG 0x86914cc 整型读实证）
    char m_pad4;              // +0x04
    bool m_isWin;             // +0x05（movzbl 0x5(%eax)）
    bool m_needSave;          // +0x06（movzbl 0x6(%eax)）
    char m_pad7;              // +0x07
};

struct GrowthWeaponEvent
{
    GrowthWeaponEvent();   // ORIG C1 0x08697068：清 6 字段（CUser.cpp 外联）

    // ---- 具名化（CU_Fix-2：SetGrowthWeaponEventInfo SIG_GROWTH_WEAPON_EVENT 载荷搬运，
    //      偏移出自该报告逐字段汇编；语义为推断，sizeof 恒 0x14 不变）----
    bool m_eventFlag;                  // +0x00（ORIG SetGrowthWeaponEventInfo 0x08691c48
                                       //   `movzbl 0x8ebe4(%eax),%eax; xor $0x1; test %al,%al; je`
                                       //   = bool 字段 + `!field`（P292/P168a）；1 字节，布局不变）
    char m_pad1[3];                    // +0x01..0x03
    unsigned int m_field4;             // +0x04
    unsigned int m_field8;             // +0x08
    unsigned int m_fieldc;             // +0x0c
    short m_field10;                   // +0x10
    short m_field12;                   // +0x12
};

struct GrowthCreatureEvent
{
    // 真实布局 stride 0x2c（GiveGrowCreatureEvent 写入/比较侧 ORIG 实证；
    // vector 字节数不变 → sizeof(CUser)/sizeof(GrowthCreatureEvent) 不变）。
    struct eventCharacInfo
    {
        unsigned short m_serverGroup;   // +0x00
        int m_characNo;                 // +0x04
        int m_job;                      // +0x08
        char m_name[30];                // +0x0c..0x29

        eventCharacInfo() : m_serverGroup(0), m_characNo(0), m_job(0)
        {
            memset(m_name, 0, 0x1e);   // ORIG C1 0x0817ac4c：memset(+0xc,0,0x1e)
        }
    };
    static_assert(sizeof(eventCharacInfo) == 0x2c, "GrowthCreatureEvent::eventCharacInfo size");

    GrowthCreatureEvent();
    ~GrowthCreatureEvent() {}

    std::vector<eventCharacInfo> m_vec;  // +0x00（0xc）
};

struct CharacLevelUpGiftAccountOnce
{
    CharacLevelUpGiftAccountOnce();
    ~CharacLevelUpGiftAccountOnce() {}

    std::multimap<int, int> m_map;  // +0x00（0x18）
    unsigned int m_field18;         // +0x18（0x8ec1c）
    char m_pad1c[4];                // +0x1c..0x20
};

class PlayInfo
{
public:
    PlayInfo();   // ORIG C1 0x084866d4：call PlayInfo::Reset()（CUser.cpp 外联）


    void Reset();
    void operator+=(const PlayInfo& other);  // ORIG W 0x0866e8（CDungeonSuccessNode::UpdateCharacter）；尾无 this-reload⇒void（同 1871 Character 先例）

    int m_f00;          // +0x00
    int m_f04;          // +0x04
    int m_f08;          // +0x08
    int m_f0c;          // +0x0c
    int m_f10;          // +0x10
    int m_f14;          // +0x14
    int m_f18;          // +0x18
    int m_f1c;          // +0x1c
    int m_f20;          // +0x20
    int m_f24;          // +0x24
    int m_f28;          // +0x28（CUser::gain_exp_sp 经验累计；ORIG this+0x8e05c）
    char m_f2c;         // +0x2c
    char m_pad2d[3];
    int m_f30;          // +0x30
    int m_f34;          // +0x34（CUser::gain_exp_sp reason==1/2 分段累计；ORIG this+0x8e068）
    char m_f38;         // +0x38
    char m_pad39[3];    // +0x39..0x3b
    int m_f3c;          // +0x3c（Party::Reset 清零 / CDungeonSuccessNode::UpdateCharacter 自增）
};
static_assert(sizeof(PlayInfo) == 0x40, "PlayInfo size");

class Character : public IData, public PlayInfo
{
public:
    Character();   // ORIG C1 0x0848683c：IData C1 + PlayInfo C1 + Character::Reset()（CUser.cpp 外联）
    ~Character() {}

    void Reset();                                  // ORIG W 0x08486818（PlayInfo::Reset + m_f3c=0）
    void operator+=(const Character& other); // ORIG W 0x084867ee 尾 store 后 leave;ret 无 return *this（0x8486815）；mangled pLE 不含返回类型零 ABI 险；唯一调用 CDungeonNode.cpp:404 不消费（CUserC8）
    // 布局修正（Layout_D 2026-08-30）：ORIG 中 +0x40/+0x44/+0x48 是 CUser 直属字段
    // （0x8e074 reset 清零 byte / 0x8e078 rating / 0x8e07c schoolNo），非 Character 成员；
    // 已移回 CUser（见 CUser 类 m_field8e074/m_rating/m_schoolNo）。ORIG Character 大小 0x40
    // （Character::Reset 只写 0x3c）。
};
static_assert(sizeof(Character) == 0x40, "Character size");

// ---- 用户行为日志门面（CUser +0x79700，0x44 字节） ----
#ifndef DNF_ENUM_ETRADEENDREASON_DEFINED
#define DNF_ENUM_ETRADEENDREASON_DEFINED
enum eTradeEndReason { eTradeEndReason_0 = 0 };
#endif
#ifndef DNF_ENUM_EAVATARITEMDELREASON_DEFINED
#define DNF_ENUM_EAVATARITEMDELREASON_DEFINED
enum eAvatarItemDelReason { eAvatarItemDelReason_0 = 0 };
#endif
#ifndef DNF_ENUM_ESEALITEMREASON_DEFINED
#define DNF_ENUM_ESEALITEMREASON_DEFINED
enum eSealItemReason { eSealItemReason_0 = 0, eSealItemReason_1 = 1 };
#endif
#ifndef DNF_ENUM_ABNORMAL_USER_LOG_TYPE_DEFINED
#define DNF_ENUM_ABNORMAL_USER_LOG_TYPE_DEFINED
// ORIG cUserHistoryLog::AbnormalUser 首参（mangled 27ENUM_ABNORMAL_USER_LOG_TYPE）。
// df_game_r 无 DWARF，成员名不可恢复（rodata 相邻串含 ANTIBOT/DPROTO，映射不可确证）；
// 占位名遵循项目 _N 约定。调用点实测取值：0（PacketDispatcher::dispatch Antibot 分支）、
// 1/2/4（Dispatcher_Antibot_DPCallBack）、3/4（DP 检测路径）。
enum ENUM_ABNORMAL_USER_LOG_TYPE
{
    ENUM_ABNORMAL_USER_LOG_TYPE_0 = 0,
    ENUM_ABNORMAL_USER_LOG_TYPE_1 = 1,
    ENUM_ABNORMAL_USER_LOG_TYPE_2 = 2,
    ENUM_ABNORMAL_USER_LOG_TYPE_3 = 3,
    ENUM_ABNORMAL_USER_LOG_TYPE_4 = 4
};
#endif
struct MSG_MAILBOX_SEND;
class cUserHistoryLog
{
public:
    cUserHistoryLog();               // ORIG 0x8695fe4：清零 m_user（C1/C2 同址）
    // [b28d-Main 代落·S318 P317] 删用户空析构：ORIG nm 仅 W C1/C2（0x08695fe4），
    // **无 D1/D2** ⇒ 类型 trivial（D1/D2 不再发射，与 ORIG 符号集一致）。

    void SetUser(CUser* user);
    void AvatarItemAdd(int itemIdx, int avatarUid, const char* agency,
                       eAvatarItemAddReason reason);
    void ItemAdd(INVEN_TYPE invenType, int reason1, int reason2,
                 const Inven_Item& item, eItemAddReason reason);
    void ItemDel(INVEN_TYPE invenType, int count, int param2,
                 const Inven_Item& item, eItemDelReason reason);
    void BloodRoundClear(int round, unsigned int clearTime, unsigned int playTime,
                         const char* dungeonName, const char* memberNames);  // ORIG T 0x08684af0
    void MoneyAdd(int money, int add, eMoneyAddReason reason);
    void MoneySub(int money, int sub, eMoneySubReason reason);
    void MoneyAddTrade(int money, int add, eMoneyAddReason reason);  // ORIG 0x8683ae0
    void MoneySubTrade(int money, int sub, eMoneySubReason reason);  // ORIG 0x8683b38
    void CoinSub(int coin, int sub, eCoinSubReason reason);
    void EventCoinSub(int coin, int sub, eCoinSubReason reason);
    void PayCoinSub(int coin, int sub, eCoinSubReason reason);
    void CoinAdd(int coin, int amount, eCoinAddReason reason);           // ORIG 0x8683b90
    void EventCoinAdd(int coin, int amount, eCoinAddReason reason);      // ORIG 0x8683c58
    void PayCoinAdd(int coin, int amount, eCoinAddReason reason);        // ORIG 0x8683bf4
    void ChangeItemLimitCount(int a, int b, int itemIdx, int d,
                              int e);                                    // ORIG 0x8686d54
    void CreatePrivateStore(int zoneA, int zoneB, char const* name);   // ORIG 0x868455a
    void RemovePrivateStore(int zoneA, int zoneB, char const* name);   // ORIG 0x8684586
    // ORIG 0x868433e：私商上架/回收条目历史（"Item+,%d,..."，含 0x15 起唯一标识串）。
    void PrivateStoreItemAdd(INVEN_TYPE invenType, Inven_Item const& item,
                             int addNew, int addOld, eItemAddReason reason,
                             char const* name1, char const* name2);
    // ORIG 0x868412a：私商下架/售出条目历史（"Item-,%d,..."）。
    void WPSub(int cur, int sub, eWPSubReason reason);   // ORIG T 0x8682cc8
    void PrivateStoreItemDel(INVEN_TYPE invenType, Inven_Item const& item,
                             int delNew, int delOld, eItemDelReason reason,
                             char const* name1, char const* name2);
    void pvpMissionClearReward(int expPoint, int exp, int missionKind,
                               int missionIndex);  // ORIG 0x8686a4e
    void CharacDie(char const* name, unsigned short unkD, unsigned short unkF);  // ORIG T 0x08684dfa（DisPatcher_DieCharacInDungeon::process 使用；DisP_GroupG）
    void EnterDungeon(const char* dungeonName, int level);  // ORIG 0x8684a16
    void EnterDungeon(const char* dungeonName, int unk, const char* memberNames, int state);  // ORIG 0x86849a2
    void EnterDungeon(int dungeonIdx, int a, int b, const char* memberNames,
                      char flag, int state);  // ORIG T 0x0868491a _ZN15cUserHistoryLog12EnterDungeonEiiiPKcci
    void LeaveDungeon(const char* dungeonName, int level);
    void LeaveDungeon(const char* dungeonName, int unk, const char* memberNames, int state);
    void LeaveDungeon(int dungeonIdx, int unk, const char* memberNames, int state);
    void DungeonClearInfo(int isLast, long playTimeSec);  // ORIG 0x8684ac4
    void DungeonClearInfo(int isLast, long playTimeSec, int idx, int mode2,
                          int val2, ENUM_DUNGEON_MODE dungeonMode,
                          const char* name, int val3);     // ORIG 0x8684a6e
    void CreatureItemAdd(INVEN_TYPE invenType, int itemIdx, int count, int addInfo,
                         int type, eItemAddReason reason);
    void InitSkill(int treeKind, int level, int sp, int sfp,
                   eSkillInitReason reason);
    // ORIG 0x08686188：记录 "SkillSPInfo,%d,%d,%d,%d"（reason, remainSP, totalSP, treeKind）。
    void SkillSPInfoSave(CUser* user, eSkillSPInfoSaveReason reason);
    // ORIG 0x08685dce：Lethe 保存技能日志 "SaveLethe,…"。
    void SkillSave(const _Mastered_skill* skills, unsigned int a, int b,
                   eSkillSaveReason reason);
    // ORIG 0x86858a0 / 0x8685838：Taiwan 计费成功/失败日志（BuyAuctionItem 等）。
    void BillingSuccess(ENUM_BILLING_TYPE type, unsigned long long tranNo, int amount);
    void BillingFail(ENUM_BILLING_TYPE type, unsigned long long tranNo, int amount,
                     int code, char* reason);
    void SkillAdd(int job, int treeKind, int skillIdx, int count,
                  eSkillAddReason reason);
    void SkillDel(int job, int treeKind, int skillIdx, int count,
                  eSkillDelReason reason);
    void SPAdd(int treeKind, int remainSP, int sp, eSPAddReason reason);
    void SPSub(int treeKind, int remainSP, int sp, eSPSubReason reason);
    void SFPAdd(int treeKind, int remainSFP, int sfp, eSPAddReason reason);
    void SFPSub(int treeKind, int remainSFP, int sfp, eSPSubReason reason);
    void FPAdd(int a, int b, int c, eFPAddReason reason);
    void FPSub(int a, int b, int c, eFPSubReason reason);
    // ORIG _ZN15cUserHistoryLog5WPAddEii12eWPAddReason @0x08682c96：记录 "WP+,%d,%d,%d"。
    void WPAdd(int winPoint, int add, eWPAddReason reason);
    // ORIG _ZN15cUserHistoryLog5QPAddEiii @0x0868888c：记录 "QP+,%d,%d,%d"（before, questIdx, add）。
    void QPAdd(int before, int questIdx, int add);

    void AvatarItemDel(int itemIdx, int avatarUid, const char* agency,
                       eAvatarItemDelReason reason);
    void CreatureItemDel(INVEN_TYPE invenType, int itemIdx, int count,
                         int addInfo, int type, eItemDelReason reason);
    void ChangeGrowType(int a, int b, int c, int d, int e,
                        eChangeGrowTypeReason reason);
    void ClearUsedQP(int before, int after, int initCount, int unused);
    void DeleteInvaildItem(const std::string& str, int a, int b);
    void ItemDelCargo(int a, int b, int c, eItemDelReason reason);
    // ORIG 直调依赖（exchange_interspace 金库/仓库域），声明先行（定义由后续批次补）：
    void ItemAddCargo(int itemIdx, int count, int slotType);   // ORIG T 0x08683cee _ZN15cUserHistoryLog12ItemAddCargoEiii
    void AccountCargoAddItem(int itemIdx, int count, int slotType,
                             eItemAddReason reason);           // ORIG T 0x086868b4 _ZN15cUserHistoryLog19AccountCargoAddItemEiii14eItemAddReason
    void AccountCargoDelItem(int itemIdx, int count, int slotType,
                             eItemDelReason reason);           // ORIG T 0x086868ee _ZN15cUserHistoryLog19AccountCargoDelItemEiii14eItemDelReason
    void LevelDown(int level, eLevelDownReason reason);   // ORIG _ZN15cUserHistoryLog9LevelDownEi16eLevelDownReason
    void LevelUp(int level, eLevelUpReason reason);       // ORIG _ZN15cUserHistoryLog7LevelUpEi14eLevelUpReason
    void Logout(const char* a, const char* b, int c, int d, int e,
                unsigned char f);                          // ORIG _ZN15cUserHistoryLog6LogoutEPKcS1_iiih
    void MoveArea(int a, int b, int c, int d, int e);
    void RequestCleanPad(unsigned int a, char b);   // ORIG T 0x086853e6
    void SendMail(MSG_MAILBOX_SEND* mail, unsigned int a);
    // ORIG T 0x08685452 / 0x08685654：拍卖/拍卖点历史日志（LogHistory 分发）。
    void Auction(SIG_AUCTION_LOG_MESSAGE_AG* msg);
    void AuctionPoint(SIG_AUCTION_LOG_MESSAGE_PG* msg);
    void SetTrader(const char* accName, const char* characName);
    void TradeBegin();
    void TradeEnd(eTradeEndReason reason, int a, int b, int c, int d);
    void TradeItemAddFail(int a, int b);
    void AchievementComplete(int idx);
    void apply_Item(int a, int b, eApplyItemReason reason);  // ORIG _ZN15cUserHistoryLog10apply_ItemEii16eApplyItemReason
    void RedeemItemAdd(int a, int b);   // ORIG 0x8686990
    void pvpMissionAdd(int a, int b, int c, int d);
    void pvpMissionDel(int a, int b);
    void RecvMail(unsigned int mailIdx, const char* senderName,
                  const char* accName);   // ORIG 0x086848a8
    // ORIG _ZN15cUserHistoryLog16UseSelectBoosterEjiiRSt6vectorI10Inven_ItemSaIS1_EE
    // ORIG 0x08686b52：把 vector 中各 item 的 m_addInfo 拼成逗号串，经 CUser::LogHistory
    // 记录 "UseSelectBooster,%u,%d,%d,%s"。
    void UseSelectBooster(unsigned int a, int b, int c,
                          std::vector<Inven_Item>& items);
    // ORIG _ZN15cUserHistoryLog23Enchant3rdChronicleItemEbiiii @0x08686fdc：
    //   记录三代强化日志（ProcUpgrade(MSG_ENCHANT) 调用；经 m_user->LogHistory）。
    void Enchant3rdChronicleItem(bool isEnchant, int itemIdx, int materialIdx,
                                 int equipType, int count);
    // ORIG 0x8684ca0：记录强化日志 "Upgrade+,%d,%d,%d" / "Upgrade-,%d,%d,%d"。
    void UpgradeItem(int itemIdx, int from, int to, bool isUpgrade);
    // ORIG 0x8684d0a：记录分离强化日志（同样经 CUser::LogHistory）。
    void SeparateUpgradeItem(int itemIdx, int from, int to, bool isUpgrade);
    // ORIG _ZN15cUserHistoryLog15ExpandInventoryEiii @0x08683cbc：
    //   记录背包扩容日志 "ExpandInventory+,%d,%d,%d"（cap, cap+value, 0）。
    void ExpandInventory(int cap, int newCap, int a);
    // ORIG _ZN15cUserHistoryLog9seal_ItemEis15eSealItemReason @0x08686c36：
    //   m_user 非空时 LogHistory("SealItem ,%d,%d,%d", itemIdx, count, reason)。
    void seal_Item(int itemIdx, short count, eSealItemReason reason);

    // ORIG _ZN15cUserHistoryLog9GiveupPvPE23ENUM_PVP_HISTORY_REASON @0x0868646c：
    //   记录 "PVP,%d" 放弃对战日志（GiveupPanalty 调用：PowerWar 时 7，否则 3）。
    void GiveupPvP(ENUM_PVP_HISTORY_REASON reason);
    // ORIG _ZN15cUserHistoryLog8MapClearEtii @0x08684b22：副本通关物品日志
    //   （CBossStage::handleMoveMap 逐成员调用）。
    void MapClear(unsigned short itemIdx, int playSec, int a);
    // ORIG _ZN15cUserHistoryLog11KillMonsterEijjttRK15MSG_MONSTER_DIEcbii13eExpAddReason
    //   @0x086851ac：击杀怪物日志（CBossStage::handleKillMonster 调用）。
    void KillMonster(int mobIndex, unsigned int killValue1, unsigned int killValue2,
                     unsigned short a, unsigned short b, const MSG_MONSTER_DIE& msg,
                     char roleType, bool isBoss, int curExp, int exp,
                     eExpAddReason reason);
    // ORIG _ZN15cUserHistoryLog12AbnormalUserE27ENUM_ABNORMAL_USER_LOG_TYPEtii
    //   @0x08686df4：m_user->LogHistory("AbnormalUser,%u,%hu,%d,%d", type, a2, a3, a4)
    //   （无 m_user 空检查，与 ORIG 一致；定义在 UserPacketCounter.cpp）。
    void AbnormalUser(ENUM_ABNORMAL_USER_LOG_TYPE type, unsigned short value,
                      int a3, int a4);
    // ORIG _ZN15cUserHistoryLog12RandomOptionERK10Inven_ItemN18randomOptionReason1TE
    //   @0x08682d3a：m_user->LogHistory("RandomOptionItem,%d,%02X×14,%d",
    //   item.m_addInfo, item.m_random.m_random 14 字节逐字节, reason)
    //   （无 m_user 空检查，与 ORIG 一致；定义在 DisPatcherCore.cpp）。
    void RandomOption(const Inven_Item& item, randomOptionReason::T reason);
    // ORIG _ZN15cUserHistoryLog11GuildAddExpEjPKcji @0x08685bb8：公会加成经验
    // 历史（WorkPerFiveMin 调用；经 m_user->LogHistory）。
    void GuildAddExp(unsigned int guildKey, const char* guildDBInfo,
                     unsigned int characNo, int exp);
    // ---- Wave5 仅ORIG 清零批（TaiwanLog）：以下 54 声明按 ORIG mangled 签名 ----
    enum eRidableType { eRidableType_0 = 0 };  // ORIG N15cUserHistoryLog12eRidableTypeE（值推断，透传 %d）

    void Login(const char* ip, const char* param2, int a, int b,
               unsigned char loginType, int c);                      // ORIG 0x08683d86
    void QPSub(int before, int questIdx, int sub);                   // ORIG 0x0868885a
    void DiePvP(ENUM_PVP_HISTORY_REASON reason, unsigned int a, unsigned int b,
                int c, int d, int e, int f, const char* name);       // ORIG 0x08686490
    void EndPvP(ENUM_PVP_HISTORY_REASON reason, bool flag, unsigned int v);  // ORIG 0x0868642e
    void StartPvP(ENUM_PVP_HISTORY_REASON reason, const char* n1, const char* n2,
                  const char* n3, const char* n4, const char* n5, const char* n6,
                  const char* n7, const char* n8);                   // ORIG 0x086863b6
    void ExpAdd(int exp, int sp, eExpAddReason reason, int reasonIdx);  // ORIG 0x08684bd0
    void UseDye(int a, int b, int c, const std::string& dyeName, int d);  // ORIG 0x08686cba
    void CharacAdd(int idx);                                         // ORIG 0x08682a92
    void CharacDel(int idx);                                         // ORIG 0x08682ae0
    void GuildJoin(unsigned int guildKey, const char* guildName, unsigned int characNo,
                   unsigned short a, char b);                        // ORIG 0x08685a48
    void GuildOut(unsigned int guildKey, const char* guildName, unsigned int characNo,
                  char a, unsigned short b);                         // ORIG 0x08685b00
    void GuildCreate(unsigned int guildKey, const char* guildName, unsigned int characNo);  // ORIG 0x08685978
    void GuildChangeGrade(unsigned int guildKey, const char* guildName, unsigned int characNo,
                          unsigned int grade, unsigned char oldGrade, unsigned char newGrade);  // ORIG 0x08685c4e
    void useEmblem(unsigned long a, unsigned long b, unsigned int c,
                   const char* d, unsigned int e);                   // ORIG 0x08685a08
    void StartRidable(eRidableType type, int a);                     // ORIG 0x08686928
    void EndRidable(eRidableType type, int a);                       // ORIG 0x0868695c
    void EnchantItem(int a, int b, int c, bool up,
                     const char* name1, const char* name2);          // ORIG 0x08684d74
    void GuildCargo(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a);  // ORIG 0x086864e6
    void SecuService(ENUM_SECU_SERVICE_TYPE type, ENUM_SECU_SERVICE_ACTION action);  // ORIG 0x0868479c
    unsigned short getItemXPos();                                    // ORIG W 0x08696000
    unsigned short getItemYPos();                                    // ORIG W 0x0869600c
    void CashInvenAdd(eCashInvenAddReason reason, unsigned int a, unsigned int b,
                      unsigned long c);                              // ORIG 0x0868683a
    void CashInvenDel(eCashInvenDelReason reason, unsigned int a, unsigned int b,
                      unsigned long c, unsigned int d);              // ORIG 0x08686874
    void HackDetected(unsigned int a, unsigned int b, unsigned int c,
                      unsigned int d, unsigned int e, unsigned int f);  // ORIG 0x08685320
    void IPCounterLog(int a, int b, int c, int d, int e, char* ip);  // ORIG 0x08685d0c
    void SetItemXYPos(unsigned short x, unsigned short y);           // ORIG W 0x0822f084
    void QuestComplete(int questIdx);                                // ORIG 0x0868514e
    void RedeemItemDel(int a, int b);                                // ORIG 0x086869bc
    void BuyCashShopItem(unsigned int itemIdx, unsigned long uid, unsigned int count,
                         const char* buyer, char flag, CeraShopPaymentType::T payType,
                         unsigned long long price);                  // ORIG 0x08686ea0
    void HadesHistoryLog(unsigned int a, unsigned int b, unsigned int c, unsigned int d);  // ORIG 0x08685418
    void MercenaryReturn(unsigned int mercNo, int a, int b, int c, int d, int e);  // ORIG 0x08686b0a
    void AntibotPacketLog(AntibotPacketLogType::T type);             // ORIG 0x08686dd0
    void FullIPCounterLog(int a, int b, int c, int d, int e, char* ip);  // ORIG 0x08685d54
    void GuildCargoResult(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a, int b);  // ORIG 0x08686512
    void GuildChangeGradeLogPack();  /* 占位防误用：见下方真名声明 */
    void ItemLockUsingLog(unsigned char lockType, int a, int b, int c, int d);   // ORIG 0x08689808
    void GuildCargoHistory(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a);       // ORIG 0x08686544
    void GuildCargoPopItem(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a, int b,
                           int c, int d, int e);                     // ORIG 0x086866ea
    void GuildCargoMoveItem(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a, int b);// ORIG 0x086867ce
    void GuildCargoPushItem(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a, int b,
                            int c, int d, int e, int f, unsigned char g);  // ORIG 0x08686638
    void ItemMakingSkillAdd(ENUM_ITEM_MAKING_TYPE type, int a, int b, short c); // ORIG 0x08686a88
    void MercenaryCompetition(unsigned int a, char b, char c);        // ORIG 0x08686aca
    void IPCounterLog_InsertIp(int a, int b, char* ip);               // ORIG 0x08685d9c
    void SendBuffertoClientLog(int kind, short a, unsigned char b, bool c);      // ORIG 0x08686e3a
    void UseAvatarOptionChange(int a, int b, int c, int d);           // ORIG 0x08686d0e
    void GuildCargoItemRecovery(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a, int b,
                                int c, int d, int e, unsigned char f);  // ORIG 0x08686694
    void GuildCargoCheckPushItem(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a, int b,
                                 int c, int d, int e, int f);         // ORIG 0x0868659c
    void GuildCargoHistoryResult(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a); // ORIG 0x08686570
    void SendMailForCeraShopGift(unsigned int mailIdx, char flag, unsigned int a,
                                 unsigned int b);                     // ORIG 0x086848da
    void GuildCargoMoveItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a,
                                  int b, int c);                      // ORIG 0x08686800
    void GuildCargoPushItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a,
                                  int b, int c, int d, int e, int f); // ORIG 0x08686732
    void GuildCargoPopItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a,
                                 int b, int c, int d, int e, int f);  // ORIG 0x08686780
    void SelectCubeForPremiumService(int a);                          // ORIG 0x08686da0
    void GuildCargoCheckPushItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                       int a, int b, int c, int d, int e, int f);  // ORIG 0x086865ea
    void CeraPoint(ENUM_CERAPOINT_LOG_TYPE type, unsigned int amount);// ORIG 0x0868593c

    // ---- 布局（ORIG 尺寸 0x44：+0x22/+0x04 字符串 + 尾部对齐） ----
    CUser* m_user;                   // +0x00（SetUser 写入 / ctor 清零）
    char m_traderAccount[0x1e];      // +0x04（SetTrader param2 / strncpy 0x1e）
    char m_traderCharac[0x1e];       // +0x22（SetTrader param1 / strncpy 0x1e，首字节当标志）
    unsigned short m_itemX;          // +0x40（SetItemXYPos 写入 / getItemXPos 读取）
    unsigned short m_itemY;          // +0x42（SetItemXYPos 写入 / getItemYPos 读取）
};

class CUser : public CUserCharacInfo
{
public:
    CUser();
    void setInitQuestPoint();  // ORIG 0x868adfc T（CQuestShop::clearQP 调用）
    void clearInitQuestPoint();   // ORIG 0x868ad88
    void clearQuestPiece();       // ORIG 0x868afd0
    void incInitQuestPoint(int val);  // ORIG 0x868adc2
    void incQuestPiece(int val, int piece, bool send);  // ORIG 0x868af66
    void incQuestPoint(int qp, int questIdx, bool send);  // ORIG T 0x868ac66
    // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c：
    //   winPoint != 0 时累加 PvpResultType::m_winPoint 并记 WP+ 日志。
    void gainWinPoint(int winPoint, eWPAddReason reason);
    void giveCharacLinkBonusExp(unsigned int bonusExp);  // ORIG T _ZN5CUser21giveCharacLinkBonusExpEj @0x08652564
    // ORIG _ZN5CUser12AddMoneyPlusEi @0x0868f266 / _ZN5CUser13AddMoneyMinusEi
    // @0x0868f310：write_log_gain_money/use_money（CInventory 域）的底层加减。
    void AddMoneyPlus(int value);
    void AddMoneyMinus(int value);
    void AddSecuRewardItem(unsigned char rewardType);  // ORIG T 0x08688328（ORIG 即空操作）
    // ORIG _ZN5CUser11UseWinPointEi12eWPSubReason @0x0864fcc6。
    int UseWinPoint(int winPoint, eWPSubReason reason);
    // ORIG _ZN5CUser18NextCharacMoneyLogEi @0x868f1c0 /
    // _ZN5CUser21exceptedStateMoneyLogEv @0x868f3ba（AddMoneyPlus/Minus 依赖）。
    void NextCharacMoneyLog(int now);
    bool exceptedStateMoneyLog();
    ~CUser();

    enum eSendTarget
    {
        eSendTarget_0 = 0,
        eSendTarget_1 = 1
    };


    // ORIG：gain_exp_sp/_check_level_up mangled `S0_13eExpAddReason` 中 `S0_` 是
    // 第二参数 `Ri`(int&) 的 substitution（ABI 复用），`13eExpAddReason` 为全局枚举
    // （GameEnums.h），非嵌套类型。类内以 typedef 暴露同名，既有 `CUser::eExpAddReason`
    // 调用点（CParty/CDeathTower 等）继续编译，mangling 与 ORIG 一致。
    typedef ::eExpAddReason eExpAddReason;
    // ---- 嵌套结构（vector<_DBLogItemInfo> 元素 / 金钱日志） ----
    struct _DBLogItemInfo
    {
        // 2026-08-31：原 m_pad0c[4] 多余——ORIG 元素 sizeof 12（new_allocator 12B）。
        unsigned int m_itemIdx;    // +0x00
        unsigned int m_type;       // +0x04
        ENUM_DBLOG_ITEM_TYPE m_value;  // +0x08
    };

    class CharacMoneyLog
    {
    public:
        CharacMoneyLog() { reset(); }
        void reset()
        {
            m_money[0] = 0;
            m_money[1] = 0;
            m_money[2] = 0;
        }
        // [b28d-Main 代落·S318 P235] 加 unsigned 同址视角（+0x00，sizeof 0xc 不变）：
        // `+=` 段用 m_moneyU[0]（unsigned 字段 + int 形参 ⇒ ORIG 的 RHS-first 三段），
        // `[2]=now` 段仍走 int 视角（同型 LHS-first）。b28 BM1 的「整数组 unsigned 化净负」不适用本双视角。
        union {
            int m_money[3];              // +0x00（0xc 字节，保持既有消费面）
            unsigned int m_moneyU[3];    // +0x00（P235 双视角）
        };
    };

    // ---- 成员布局（ORIG 构造 0x864e410 逐子对象推导，总尺寸 0x8ec30） ----
    CNetwork<4096, 450000> m_network;                       // +0x0e0
    char m_field6ef68;                                      // +0x6ef68（reset=0；CNetwork 后独立状态字节）
    char m_field6ef69;                                      // +0x6ef69（reset=0xff）
    char m_pad6ef6a[2];                                     // +0x6ef6a
    UserMercenaryInfoMgr m_mercenaryMgr;                    // +0x6ef6c
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >
        m_rentalInfo;                                       // +0x6ef78
    bool m_rentalFlag1;                                     // +0x6ef90（GetPcRoomRentInfo 0x8676a10 xor$1 取反形⇒ORIG 即 if(!flag)；SetSave 0x82a677d bool=bool 直落）
    bool m_rentalFlag2;                                     // +0x6ef91
    Taiwan::GarenaAuthData m_garenaAuthData;                // +0x6ef92（sizeof 0x68；证据 CUser/prepareDisconnect.md
                                                            //   0x6ef92=getTimeStamp 对象、CUser/InitOnAccept.md memset(this+0x6ef92,0,0x68)）
    bool m_rentalFlag3;                                     // +0x6effa
    char m_pad6effb;                                        // +0x6effb
    CAccountCargo m_accountCargo;                           // +0x6effc
    CGameOption m_gameOption;                               // +0x6fd64
    int m_multiboxLotteryFailCnt;                           // +0x703bc（inc_multiboxLotteryItemFailCnt）
    bool m_breakAwayRewardData;                             // +0x703c0（setBreakAwayRewardData 0x0831d4xx this-先 movzbl 直落 byte=bool 形；CUOB probe14/27）
    bool m_usedBreakAwayLuckPoint;                          // +0x703c1（setUsedBreakAwayLuckPoint ORIG W 0x086971ee）
    char m_pad703c0b[2];                                    // +0x703c2..0x703c3
    STBreakAwayRewards m_breakAway;                         // +0x703c4
    STSkillCommandData::STCommand m_skillCommands[50];      // +0x703db
    short m_slotIDX;                                        // +0x704a4（GetSlotIDX movzwl 直返）
    short m_field704a6;                                     // +0x704a6（SetIncreID 0x82a6859 this-先需 short 字段×short 形参；ushort 反致 arg-先回退，GetUID cwtl 被 fold 无增益，故保 short）
    unsigned short m_field704a8;                            // +0x704a8
    char m_pad704aa[2];                                     // +0x704aa..0x704ab
    unsigned int m_accId;                              // +0x704ac（set_acc_info 0x08649c02 this-先=成员×uint 形参全等直落；int 反产 arg-先，CUOC7 技巧5）
    char m_accName[0x128];                                  // +0x704b0..0x705d7
    int m_field705d8;                                       // +0x705d8
    Client_Spec m_clientSpec;                               // +0x705dc（sizeof 0xbf8）
    // [b28d-Main·检查点14] 原误置于 Client_Spec 尾部的 7 字段——ORIG 属 CUser 本体：
    //   绝对偏移不变（+0x711d4..+0x711eb），故所有 `m_clientSpec.X` 消费点改为 `m_X`，
    //   codegen（this+绝对偏移）逐条不变。证据：Client_Spec ctor 0x0822b8b0 只构造
    //   0xff 个 HWSpec（4+0xbf4=0xbf8），且 CUser 布局注释「+0x705dc Client_Spec /
    //   +0x711ec list<int>」在 0xbf8 下自洽（0x705dc+0xbf8=0x711d4）。
    unsigned char m_birthDay;                               // +0x711d4（IsOverBirthDay movzbl；uchar 实证）
    unsigned char m_birthMonth;                             // +0x711d5（IsOverBirthDay movzbl）
    char m_pad711d6[2];                                     // +0x711d6..0x711d7
    bool m_gmUserFlag;                                      // +0x711d8（isGMUser movzbl 直返⇒bool）
    char m_pad711d9[3];                                     // +0x711d9..0x711db
    ENUM_GM_ITEM_UPGRADE m_gmUpgradeMode;                   // +0x711dc（SetGMUpgradeMode enum×enum LHS-first）
    unsigned int m_age;                                     // +0x711e0（SetAge this-先载 ⇒ uint）
    bool m_progLogout;                                      // +0x711e4（IsProgLogout movzbl 直返⇒bool）
    char m_pad711e5[3];                                     // +0x711e5..0x711e7
    int m_seedFromDate;                                     // +0x711e8
    std::list<int> m_list711ec;                             // +0x711ec
    CPacketVerifier m_packetVerifier;                       // +0x711f4
    WongWork::CUserPremium m_premium;                       // +0x7121c
    WongWork::CHackAnalyzer m_hackAnalyzer;                 // +0x71bac
    Secu_AccountHacking m_accHacking;                       // +0x795e8
    CodeHackCheckStorage m_codeHackCheck;                   // +0x795fc
    PostChecksumUserInfoBlock m_postChecksumData;           // +0x79624（0x40；tradePunish/serverGroup/gameMasterCharac 数据区，CodeHackCheckStorage 后继块）
    std::string m_str79664[2];                              // +0x79664（isDuplicationMessage ORIG 0x8688814 双槽
                                                            //   比较/写回；~CUser.md 逆序 dtor 0x7966c..0x79664 区间实证）
    unsigned char m_dupMsgSlotIdx;                          // +0x7966c（isDuplicationMessage ORIG 0x8688814 movzbl 读取并 ^1 轮转写回实证）
    char m_pad7966d[3];                                     // +0x7966d..0x7966f（ORIG ctor 0x865836d 清零；无访问证据保留 pad）
    int m_field79670;                                       // +0x79670
    CharacMoneyLog m_moneyLogs[7];                          // +0x79674
    int m_moneyLogIdx;                                      // +0x796c8（货币日志游标；
                                                            //   NextCharacMoneyLog 推进/钳 6）
    // GM 调试指令写入（Dispatcher_New_Gmdebug_Command/oldGmRoutine.md ORIG 0x82122c8/
    // 0x8212326）："bossdifficult" → byte（1..4 校验后写入）、"bossmap" → int。
    // CBossStage::RandomConsistMap/handleMoveMap 与 CBossTower 取走即清零。
    char m_gmBossDifficulty;                                // +0x796cc
    char m_pad796cd[3];                                     // +0x796cd..0x796cf
    int m_gmBossMapIndex;                                   // +0x796d0
    int m_mileage;                                          // +0x796d4 (GetMileage/SetMileage)
    // [b28d-Main 代落·S320 P235/P311] 加同址 bool 视角（union 1B/align 1，sizeof/偏移/mangled 零变化；
    // uchar 名保留）：ORIG SetPuUser 0x08692b58 的 `mov %al,-0x4; mov 0x8,%eax; movzbl -0x4,%edx;
    // mov %dl,0x796d8(%eax)` 10 条形需 bool 成员直访问（`*(bool*)&field=` 视图多一条 add $0x796d8）。
    union {
        unsigned char m_field796d8;                         // +0x796d8（Inter_LoadPuUserCheck 0x084e7fe2 uchar 读）
        bool m_bPuUser796d8;
    };
    char m_field796d9;                                      // +0x796d9（WorkPerFiveMin 公会经验上报负载字节，
                                                            //   oldGmRoutine 0x0820c478 置 1；与 ORIG 一致保留）
    unsigned char m_seedBackup796da[6];                     // +0x796da..0x796df（无 ORIG 具名访问；CUser::backupSeed
                                                            //   0x859506f 读 *(int*)(this+0x796e0)——即本块 +6 与
                                                            //   m_checkSumSeed 相邻视图，保持 6 字节 pad）
    unsigned int m_checkSumSeed;                            // +0x796e0（b28d-Main P211档③：ORIG 0x0864e0dc/0x0864e139 两段装载 ⇒ unsigned）
    unsigned int m_backupSeed;                              // +0x796e4（b28d-Main 试验：与 m_checkSumSeed 同型化）
    std::vector<_Charac_info> m_characList;                 // +0x796e8
    CCharacterView m_characterView;                         // +0x796f4
    int m_field796f8;                                       // +0x796f8（HistoryLog::WriteLevelUp 文件句柄）
    int m_winPointBuffer;                                   // +0x796fc（add_pvp_result 累加缓冲，ORIG 0x865c678）
    cUserHistoryLog m_historyLog;                           // +0x79700
    charac_expand::CDataMgr m_characExpand;                 // +0x79744
    InventoryMemory m_inventoryMemory;                      // +0x79794
    UserQuest m_quest;                                      // +0x858d4
    bool m_pcRoomAuth;                                      // +0x8cef1（ORIG setPcRoomAuth 0x084ec834 bool 写形）
    char m_pad8cef2[2];                                     // +0x8cef2..0x8cef3
    std::vector<unsigned int> m_vec8cef4;                   // +0x8cef4
    int m_field8cf00;                                       // +0x8cf00
    int m_playingDungeonIdx;                                       // +0x8cf04
    int m_lastPacketID;                                     // +0x8cf08
    // [CUserLane 2026-09-11 · P211 档③] char → unsigned char：+0x8cf0c/0x8cf0d 为
    //   send/recv 计数字节，写入点 CUser::Send ORIG 0x0864861e `mov 0x8(%ebp),%edx;
    //   mov %al,0x8cf0c(%edx)` 要求与 PacketBuf::at（unsigned char）同型 ⇒ LHS-first。
    unsigned char m_pad8cf0c[4];                            // +0x8cf0c..0x8cf0f
    int m_antibotKey;                                       // +0x8cf10
    unsigned int m_clientVersion;                           // +0x8cf14
    char m_pad8cf18[0x40];                                  // +0x8cf18..0x8cf57
    int m_field8cf58;                                       // +0x8cf58
    char m_pad8cf5c[8];                                     // +0x8cf5c..0x8cf63
    CDungeonGainedGold m_dungeonGainedGold;                 // +0x8cf64
    std::vector<LimitItemUsage> m_limitItemUsage;           // +0x8cf68
    std::vector<_DBLogItemInfo> m_dbLogItems;               // +0x8cf74
    std::list<int> m_list8cf80;                             // +0x8cf80
    Mutex m_mutex1;                                         // +0x8cf88
    Mutex m_mutex2;                                         // +0x8cfa0
    int m_field8cfb8;                                       // +0x8cfb8
    int m_field8cfbc;                                       // +0x8cfbc
    int m_field8cfc0;                                       // +0x8cfc0
    ch_state m_chState;                                  // +0x8cfc4（set_state 0x0867edb9 this-先直存=成员×ch_state 形参全等；int 反产 arg-先，CUserC7 技巧5；enum 4B 同宽）
    short m_field8cfc8;                                     // +0x8cfc8
    char m_pad8cfca[2];                                     // +0x8cfca..0x8cfcb
    UserSpace::T m_field8cfcc;                              // +0x8cfcc
    int m_field8cfd0;                                       // +0x8cfd0
    int m_field8cfd4;                                       // +0x8cfd4
    int m_field8cfd8;                                       // +0x8cfd8
    char m_field8cfdc;                                      // +0x8cfdc（OnDisconnect 清零的状态字节）
    char m_pad8cfdc[3];                                     // +0x8cfdd..0x8cfdf
    int m_guildWarPoint;                                    // +0x8cfe0
    std::set<unsigned int> m_set8cfe4;                      // +0x8cfe4
    unsigned short m_posX;                                 // +0x8cffc
    unsigned short m_posY;                                 // +0x8cffe
    char m_direction;                                      // +0x8d000
    char m_pad8d001;                                        // +0x8d001
    short m_tradeSpaceIdx;                                     // +0x8d002
    short m_field8d004;                                     // +0x8d004
    short m_pvpRoomIdx;                                     // +0x8d006
    short m_field8d008;                                     // +0x8d008
    char m_pad8d00a[2];                                     // +0x8d00a..0x8d00b
    short m_field8d00c;                                     // +0x8d00c
    short m_field8d00e;                                     // +0x8d00e
    short m_bossTowerIdx;                                     // +0x8d010
    short m_advanceAltarIdx;                                     // +0x8d012
    unsigned int m_regDate;                                 // +0x8d014
    int m_regTime;                                          // +0x8d018
    unsigned int m_cera;                                    // +0x8d01c
    STGuildDBInfoOnly m_guildDB;                            // +0x8d020
    STGuildAgitDBInfo m_guildAgitDB;                        // +0x8d0dd
    std::vector<LevelupSupportEventManger::EventInfo>
        m_levelupEvents;                                    // +0x8d0e4
    bool m_field8d0f0;                                      // +0x8d0f0（unlock4DataLoad 0x864a9dd xor$1 唯 bool 产，CUserB probe14/8）
    bool m_field8d0f1;                                      // +0x8d0f1（DeleteCharac 锁，isLockedDeleteCharac 0x864aa78 movzbl 直返）
    bool m_field8d0f2;                                      // +0x8d0f2（CreateCharac 锁，isLockedCreateCharac 0x864aae6 movzbl 直返）
    char m_pad8d0f3;                                        // +0x8d0f3
    int m_field8d0f4;                                       // +0x8d0f4
    int m_field8d0f8;                                       // +0x8d0f8
    bool m_field8d0fc;                                      // +0x8d0fc
    char m_pad8d0fd[0xf];                                   // +0x8d0fd..0x8d10b
    int m_tradeCount;                                       // +0x8d10c（incTradeCount 自增；UpdateLogout 随包上报、log_out 清零）
    int m_field8d110;                                       // +0x8d110
    WongWork::CSecurityCard m_securityCard;                 // +0x8d114
    Sanicova::CPad m_cpad;                                  // +0x8d160
    // CUser::._379（0x10 字节；ORIG 名 _ZN5CUser5._3795resetEv）
    class _379
    {
    public:
        // §8 判据（Bridge21B）：asm 别名必须保留——ORIG 类名首字符为 '.'
        // （nm 086454d8 `_ZN5CUser5._3795resetEv`），GCC 无法声明 `class ._379`，
        // 自然 mangled `5_379` 缺点不匹配 ⇒ 编译器 mangling 必要性，非 §4.5 违例。
        void reset() asm("_ZN5CUser5._3795resetEv");
        bool m_field0;     // +0x00
        bool m_field1;     // +0x01
        char m_pad2[2];    // +0x02..0x03
        unsigned long long m_hackTradeGold; // +0x04..0x0b（setTotalHackUserTradeGold 0x084ec952 ull 成员×ull 形参=ORIG this→%ecx 先于双 value load；ll 反产 value-先；32位 ABI 4对齐偏移不变）
        unsigned int m_hackTradeCnt; // +0x0c（getTotalHackUserTradeCnt）
    } m_379;                                            // +0x8d1a8
    std::vector<stLookOuts> m_lookOuts1;                    // +0x8d1b8
    std::vector<stLookOuts> m_lookOuts2;                    // +0x8d1c4
    std::vector<tyIPCounters> m_ipCounters;                 // +0x8d1d0
    int m_field8d1dc;                                       // +0x8d1dc
    int m_field8d1e0;                                       // +0x8d1e0
    CerashopAddRestrict::UserInfo m_ceraUserInfo;           // +0x8d1e4
    // +0x8d24d 起 0x17 字节：ORIG 为 CUser 平铺字段区（STExpReward 类型仅 4 字节
    // 用于 vector/解析；此处字段独立——2026-08-31 影子测试实测分离）。
    // ORIG 该平铺区（m_reward@{0x8d24d} 非对齐 int）为 pack(1)。逐 int 成员加
    // packed 属性避免 4 字节对齐 pad，使 m_reward 落 0x8d24d、m_localIp 落 0x8d254。
    STExpReward m_reward __attribute__((packed));           // +0x8d24d（get_growth_power_exp_reward sret 结构返回 ORIG ret $0x4；4B 同宽 packed 落位不变，CUOC7 probe g5/g6）
    char m_field4;                                          // +0x8d251
    char m_field5;                                          // +0x8d252
    bool m_powerUp;                                         // +0x8d253（ORIG SetPowerUp 0x08595078 bool 写形）
    int m_localIp __attribute__((packed));                  // +0x8d254
    int m_publicIp __attribute__((packed));                 // +0x8d258
    int m_padf __attribute__((packed));                     // +0x8d25c（int 视图）
    bool m_hangameUser;                                     // +0x8d260
    char m_pad261[0x8d264 - 0x8d261];                       // +0x8d261..+0x8d263
    APSystem::CActionPointManager m_actionPoint;            // +0x8d264
    advancealtar::CharacAdvanceAltarManager m_advanceAltar; // +0x8df60
    Character m_character;                                  // +0x8e034（ORIG 大小 0x40 → 0x8e074）
    // 布局修正（Layout_D 2026-08-30）：0x8e074/0x8e078/0x8e07c 为 CUser 直属字段
    // （ORIG reset movb $0x0,0x8e074 / GetRating/SetRating / GetSchoolNo/SetSchoolNo），
    // 自 Character 移回（见 Character 类注释）。
    char m_field8e074;                                      // +0x8e074（reset 清零状态字节）
    char m_pad8e075[3];                                     // +0x8e075..0x8e077
    float m_rating;                                         // +0x8e078（GetRating/SetRating）
    unsigned int m_schoolNo;                                // +0x8e07c（GetSchoolNo/SetSchoolNo）
    std::string m_str8e080;                                 // +0x8e080
    short m_dailyBadge[3];                                  // +0x8e084..0x8e089
    unsigned char m_tutorialSkipable;                       // +0x8e08a
    char m_pad8e08b;                                        // +0x8e08b
    int m_fatigueGain;                                       // +0x8e08c（FatigueUp 写入）
    int m_debugCommand;                                     // +0x8e090（GetDebugCommand/SetDebugCommand）
    unsigned int m_field8e094;                              // +0x8e094
    unsigned char m_childrensDayEventState;                 // +0x8e098（==0xff 判定）
    char m_pad8e099[3];                                     // +0x8e099..0x8e09b
    int m_childrensDaySlots[10];                            // +0x8e09c..0x8e0c3（ClearChildrensDayEvent 置 -1 表 A）
    int m_childrensDayPeriods[10];                          // +0x8e0c4..0x8e0eb（ClearChildrensDayEvent 置 -1 表 B）
    union { char m_capchaCertify[0xa]; unsigned short m_capchaCertifyW[5]; };  // +0x8e0ec（b28d-Main P235：W 视角复现 ORIG 单条 disp movzwl）                              // +0x8e0ec（reqHumanCertify
                                                            //   memcpy 验证码头 0xa 字节）
    char m_pad8e0f6[2];                                     // +0x8e0f6..0x8e0f7
    int m_humanCertifyTimerKey;                             // +0x8e0f8
    int m_humanCertifyErrorCnt;                             // +0x8e0fc
    bool m_humanCertified;                                  // +0x8e100（setHumanCertified/isHumanCertified）
    char m_pad8e101[0x2ec];                                 // +0x8e101..0x8e3ec
    bool m_field8e3ed;                                      // +0x8e3ed
    WongWork::CMCAPManager m_mcap;                          // +0x8e3f0
    XNuclear::CHades m_hades;                               // +0x8e954
    Secu_GoldControl m_goldControl;                         // +0x8e980
    Secu_HackLogCheck m_hackLogCheck;                       // +0x8ea68
    std::map<short, int> m_mapEtc;                          // +0x8eaa4（GetETC 0x08697474 / SetETC 0x080f9968；占 0x8ea68+sizeof(Secu_HackLogCheck 后空隙至 +0x8eabc）
    std::map<std::pair<char, char>, int> m_map8eabc;        // +0x8eabc
    char m_pad8ead4[4];                                     // +0x8ead4..0x8ead7
    bool m_field8ead8;                                      // +0x8ead8（ORIG SetBuyingGold 0x08692a08 bool 写形）
    char m_pad8ead9[3];                                     // +0x8ead9..0x8eadb
    unsigned int m_ceraPoint;                               // +0x8eadc（GetCeraPoint/SetCeraPoint）
    bool m_field8eae0;                                      // +0x8eae0
    char m_pad8eae1[3];                                     // +0x8eae1..0x8eae3
    PIReceiverManager m_piReceiver;                         // +0x8eae4
    PISenderManager m_piSender;                             // +0x8eaec
    std::set<std::pair<unsigned char, unsigned short> >
        m_set8eb00;                                         // +0x8eb00
    unsigned char m_field8eb18;                             // +0x8eb18
    char m_pad8eb19[3];
    int m_field8eb1c;                                       // +0x8eb1c
    bool m_deleteEventDungeonInvitation;                                      // +0x8eb20
    char m_pad8eb21[3];                                     // 0x8eb21..0x8eb23
    int m_itemSellCount;                                    // +0x8eb24
    char m_sellCountData[0x1c];                             // +0x8eb28..0x8eb43
    bool m_field8eb44;                                      // +0x8eb44
    char m_pad8eb45[3];
    int m_field8eb48;                                       // +0x8eb48
    char m_serverFatigueValue;                                      // +0x8eb4c
    char m_serverFatigueDbType;                                      // +0x8eb4d
    char m_pad8eb4e[2];                                     // 0x8eb4e..0x8eb4f
    std::map<unsigned char, short> m_map8eb50;              // +0x8eb50
    std::map<int, std::pair<int, int> > m_map8eb68;         // +0x8eb68
    std::map<int, bool> m_map8eb80;                         // +0x8eb80
    bool m_field8eb98;                                      // +0x8eb98
    bool m_field8eb99;                                      // +0x8eb99
    char m_pad8eb9a[2];                                     // 0x8eb9a..0x8eb9b
    unsigned int m_pcRoomPlayStartTime;                     // +0x8eb9c（setPcRoomPlayStartTime）
    unsigned int m_totalPcRoomPlayTime;                     // +0x8eba0（setPcRoomPlayTimeData 0x0868f808 this-先载 ⇒ uint）
    unsigned char m_pcRoomPlayTimeRewardCnt;                // +0x8eba4（getPcRoomPlayTimeRewardCnt）
    char m_pad8eba5[3];                                     // +0x8eba5..0x8eba7
    unsigned int m_field8eba8;                              // +0x8eba8（ORIG Inc 0x08110e1e copy-add 形需 uint）
    unsigned int m_field8ebac;                              // +0x8ebac（ORIG Inc 0x08110e3a copy-add 形需 uint）
    bool m_field8ebb0;                                      // +0x8ebb0（SetChangedGiftFatigueQuantity 0x8110e7a this-先 bool=bool 直落）
    unsigned char m_informNoticeFlags[16];                  // +0x8ebb1..0x8ebc0（bit 索引访问）
    bool m_saveInformNoticeFlag;                                // +0x8ebc1
    char m_pad8ebc2[2];                                     // 0x8ebc2..0x8ebc3
    std::vector<unsigned char> m_vec8ebc4;                  // +0x8ebc4
    int m_field8ebd0;                                       // +0x8ebd0
    int m_field8ebd4;                                       // +0x8ebd4
    int m_field8ebd8;                                       // +0x8ebd8
    BingoData m_bingo;                                      // +0x8ebdc
    GrowthWeaponEvent m_growthWeapon;                       // +0x8ebe4
    GrowthCreatureEvent m_growthCreature;                   // +0x8ebf8
    CharacLevelUpGiftAccountOnce m_levelUpGift;             // +0x8ec04
    int m_field8ec24;                                       // +0x8ec24
    char m_receivedRequestType;                             // +0x8ec28（SetReceivedRequestType 0x082308cc this-先仅 char 成员可产，probe36 决定性；Get movzbl 非排他）
    bool m_field8ec29;                                      // +0x8ec29
    char m_pad8ec2a[2];                                     // 0x8ec2a..0x8ec2b
    int m_field8ec2c;                                       // +0x8ec2c
    bool m_eventCreateDnfReward;                            // +0x8ec30（SetEventCreateDnfReward 0x08693198 bool=bool 唯一产此序）
    bool m_field8ec31;                                      // +0x8ec31（[b28d-Main 代落·S336 P211] char→bool：ORIG 0x0816158d 段 `movzbl 0x4(%eax),%edx; mov %dl,0x8ec31(%eax)` 单段直落；异型 RHS(bool) 会多 1 条 eax→edx 中转。1 字节/偏移/后续字段不变）
    char m_field8ec32;                                      // +0x8ec32
    char m_pad8ec33[1];                                     // +0x8ec33（ORIG CUser = 0x8ec34）

    // ---- 构造 / 重置 ----
    void reset();
    void ResetCurCharac();
    void ResetDailyData();
    void resetDailyData();
    void ResetDailyCharacExpandDataMidnight();
    void ResetDailyCharacExpandData();
    void resetGuildDBInfo();
    void resetBlackList();
    void resetInformNoticeFlag();
    void enableSaveInformNoticeFlag();
    void disableSaveInformNoticeFlag();
    void resetHumanCertifyData();
    void reset_ingame_event_history();
    void reset_ingame_event_history_update();
    void reset_pvp_masterid_walkingout_me();
    void reset_server_fatigue_data();
    short get_server_fatigue_count(unsigned char type);  // 0x0818d3fe：m_map8eb50 查找
    void set_server_fatigue_count(short v);              // ORIG T 0x0868d32a：m_map8eb50
                                                         //   按当日 get_today_time_day 累加/插入
    int   get_fatigue_count_db(int type);                // 0x0818d4c4：m_serverFatigueDbType 匹配则 m_serverFatigueValue
    void resetNPCRelationShipDailyData();
    void ResetUsedCoinInDungeon();
    void ClearItemSellCount();
    void ResetDBLogItem();
    void DBUpdateDBLogItem() const;
    void resetMoneyLog();
    void set_guildwar_point_per_pvpplay(int point);
    int get_guildwar_point_per_pvpplay();
    XNuclear::CHades* getHades();   // ORIG 0x08230800：返回 this+0x8e954（内嵌 CHades 成员，CHades 在 namespace XNuclear）
    // [Main·检查点22 P211/T1] 返回 void*→**PvP_Room\***。直证：ORIG 消费点
    //   `*out = GetPVPRoom()`（CFairMatchRooms::_CheckMovableRoom）是 **LHS-first
    //   直存** `mov 0x14(%ebp),%edx; mov %eax,(%edx)`；4.4 只在两侧**同型**时出该形，
    //   返回 void* 而目标字段是 PvP_Room* ⇒ 异型 ⇒ 恒退化成 RHS-first 多 2 条
    //   （P211 档①）。mangled 不含返回类型 ⇒ 符号账不变。
    //   定义体本即 `return G_CGameManager()->GetPvp(...)`（其返回类型就是 PvP_Room*）。
    PvP_Room* GetPVPRoom();
    void UpdateRating(float rating);   // ORIG W 0x08568e9c _ZN5CUser12UpdateRatingEf（Elo 个人分回写；定义归 CUser 域）
    void* GetSecretShopData();
    void* GetPICSMap();
    bool IsPermissionPrivateStore();   // b28 BL0 代落：ORIG 体内 return isAffectedPremium(8)（callee bool）+消费点 0x85cacc0 test%al;je 直判（P22）；mangled 不变
    bool IsEquipAvatar();
    bool IsHavePremiumAdvantage() const;
    void RecoverFatigue(int value);
    void LogHistory(const char* fmt, ...);
    int get_aura_avatar_option_value(int idx);
    bool isDuplicationMessage(const std::string& msg);
    void DimensionInoutUpdate(bool flag1, bool flag2);
    void SetTradeSpace(int idx);
    bool CheckFatigue();
    void gainCurCharacFatigue(int value);   // ORIG 0x0864f7fe
    bool CheckLimitMiniumAge(int age, int& param);   // ORIG T 0x0864fc3e
                                                     //   _ZN5CUser19CheckLimitMiniumAgeEiRi（PartyUserLink 批次定义）
    short has_within_Mission() const;   // ORIG 尾 call getIndex_byKind;leave;ret 无归一；5 调用点 test %ax,%ax(16位)⇒short
    bool acceptable_within_mission() const;
    bool isCompetitionMercenary() const;
    int CalcMercenaryAvatarBonus() const;      // ORIG T 0x0868ea68 _ZNK5CUser24CalcMercenaryAvatarBonusEv
    void setAvatarTypeBonus(char bonus);       // ORIG T 0x0868ebaa _ZN5CUser18setAvatarTypeBonusEc
    void* getBlueMarble();
    void checkLogOutCorrectly();   // ORIG 无返回值指令（void）
    bool find_pvp_masterid_walkingout_me(unsigned int id);   // ORIG caller IsQuickJoinablePVPRoom 0x085d6c30 xor$1;test%al;je⇒bool（PvPRC7 技巧1）
    void insert_pvp_masterid_walkingout_me(unsigned int id);
    void update_old_pvp_point();
    bool update_pvp_rank(const PvpResultType& result);
    void saveTaxMoneyForUpperMember(int money);
    int gainExpAsUpperMember(int exp);
    int gainPowerWarRewardExp(int exp);
    void add_guild_point_item();
    void add_inventory_item(unsigned int itemId);
    bool AddDungeonClear(int dungeonIdx, int clearCount);
    void SendConditionEventInfo();
    void ResetCurCharacUsedGiftFatigueQuantity();
    void SetChangedGiftFatigueQuantity(bool flag);
    void resetTotalPcRoomPlayTime();                    // ORIG（CUser.cpp 已有定义）
    void ResetCurCharacUsedFatigueQuantity();           // ORIG（CUser.cpp 已有定义）
    void IncCurCharacUsedFatigueQuantity(unsigned int n);       // ORIG W 0x08110e1e：m_field8eba8 += n
    void IncCurCharacUsedGiftFatigueQuantity(unsigned int n);   // ORIG W 0x08110e3a：m_field8ebac += n
    void ResetDailyQuest();
    void ResetTrainingQuest();
    void SetPvpIndex(short idx);
    void SetSaveRentalInfoToExchange(bool flag);
    void DeleteRentalItemInfo(int idx);
    bool is_fighting();   // ORIG 0x867ed68 尾三分支 mov $0/$1,%eax 直返；caller CCreature::Equip 0x8337330 test %al;je 无 setne⇒bool
    bool CheckMoney(int money);   // ORIG 0x085c7c0a 调用点 xor $0x1,%eax ⇒ bool
    int getDungeonIdxAfterClear();
    bool isAffectedPremium(ENUM_PREMIUM_TYPE type) const;
    void set_before_area(int area);

    const char* getWebAddress();
    WongWork::CSecurityCard* getSecurityCard();
    CGameOption* GetGameOptionRef();
    Sanicova::CPad* getPad();

    // ---- 登录 / 初始化 / 发包 ----
    int InitOnAccept(int param);
    void set_acc_info(unsigned int accId, const char* accName);

    // ---- 批次 A 访问器（ORIG 反汇编逐偏移补全；0x85a932a 族） ----
    void questAutoClear(int questIdx);
    void _onCompoundItem(int itemIdx);
    void _updateNPCRelationship();
    void processNPCGiftOnLevelUp();
    char* GetUserName();
    char* GetUserEMail();
    const char* GetSsnString();
    // ORIG getSex 体 `mov $0xffffffff,%eax`（int 宽三态 -1/0/1）+ 调用侧 `test %eax,%eax`
    // 全宽判（ADeepX3 证据 0x…checkCondition 两处 sete/cmp）⇒ 返回 int；
    // mangled 不含返回型零 ABI 险，全消费点 -1/0/1 比较源兼容（operator+= 判例族）。
    int getSex();
    void* GetCSHashSet();
    char* GetGuildName();
    unsigned char GetGuildLevel();
    void* GetHackCheck();
    void* getBingoData();
    void* GetGuildDBInfo();
    char* getGarenaAuthData();
    void* GetPISenderManager();
    void _GetSaveInvenBase(SIG_SAVE_INVENTORY* out);  // ORIG 0x0864fd80（U3ShopDb）

    // ---- 租借物品序列化链（SaveInventory 直接依赖；m_rentalInfo@+0x6ef78）----
    void GetInstanceRentalInfo(
        std::map<unsigned int,
                 std::vector<InstanceRentalSystem::RentalInfo> >& out, int param);  // ORIG 0x08676dba
    bool getCurrentCharacRentalInfo(
        std::vector<InstanceRentalSystem::RentalInfo>& out, int param);            // ORIG 0x086773ea
    bool GetCurrentCharacRentalInfo(
        std::vector<InstanceRentalSystem::RentalInfo>& out, int param);            // ORIG T 0x08677166（尾调小写版）
    void setCurrentCharacRentalInfo(
        std::vector<InstanceRentalSystem::RentalInfo>& in, int param);             // ORIG 0x08677516
    bool removeRentalItemInfoFromInvenBuf(SIG_SAVE_INVENTORY* sig, int param);     // ORIG 0x08677584；串定 bool（P174，b28c SIGFIX）
    void* GetPIReceiverManager();
    void* getGrowthWeaponEventdata();
    void* get_ingame_event_history();
    void* get_ingame_event_history_update();
    void* getGrowthCreatureEventdata();
    void* get_server_fatigue_day_data();
    void* getCharacLevelUpGift_AccountOnce();
    void* GetMailBox();
    int GetMileage();
    void SetMileage(int mileage);      // ORIG T 0x086509c6：LogHistory + m_mileage 写
    int GetPCRoomNo();
    unsigned int GetCeraPoint();
    int getAntibotKey();
    int GetManageLevel();
    int GetManagePoint();
    void SetManageLevel(int level);                     // ORIG T 0x0868fe2c
    void CalcurateManagePoint();                        // ORIG T 0x0868ffa0
    void SetManagePoint(int point);                     // ORIG T 0x0868fe72
    bool CalcurateManageLevel();                        // ORIG E 0x08690026 _ZN5CUser20CalcurateManageLevelEv
    void SendManageLevelUpNoti();                       // ORIG T 0x08690b58 _ZN5CUser21SendManageLevelUpNotiEv
    void SetUserMaxLevel(int level);                    // ORIG T 0x0868fec8 _ZN5CUser14SetUserMaxLevelEi
    void CalcurateUserMaxLevel();                       // ORIG T 0x0868ff04 _ZN5CUser21CalcurateUserMaxLevelEv
    void SetMercenaryInfoPacket(PacketGuard& packet);   // ORIG T 0x0868e704 _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard
    int GetUserMaxLevel();
    void onDungeonClear(bool flag);
    int GetDebugCommand();
    int GetTradePunishType();
    int getLastLotteryTime();
    int GetNonClientRandInt();
    bool GetNonClientFlag();   // ORIG _NS_NonClient_CheckResponse 0x08274ee4 调用点 xor$1;test;je⇒bool（m_nonClientFlag 已 bool）
    int GetLastLoginCharacNo();
    int GetLastLoginChannelNo();
    bool IsValidLastLoginCharac();
    // ORIG W 0x081348ce _ZN5CUser17DisableCharacInfoE13CACHE_SECTION（定义：advancealtar_DB.cpp，
    // Prod_GlobalAdv 批次；体=m_byte[0x8d0fd+section]=0）
    void DisableCharacInfo(CACHE_SECTION section);
    // ORIG T 0x08692d2a _ZN5CUser20LoadCharacTypeHotKeyEii（0x171 字节；唯一调用方
    // DisPatcher_SelectCharac::RequestDBCharac。依赖 GetCharacHotKeyType/
    // CheckLoadHotKeyCashedCharacType/Send(PacketGuard&)/DB_LoadCharacHotKeyOption——
    // 定义缺口已记录于 /tmp/df_batches/Prod_DispatcherHelpers.md，由 CUser 域批次认领）
    void LoadCharacTypeHotKey(int cashedCharacJob, int characJob);
    int GetCharacHotKeyType(int characJob);    // ORIG T 0x08692ef2 _ZN5CUser19GetCharacHotKeyTypeEi（调用点 mov %eax 全宽取值；语义 characJob==10）
    bool CheckLoadHotKeyCashedCharacType(int cashedCharacJob,
                                         int characJob);  // ORIG T 0x08692e9c _ZN5CUser31CheckLoadHotKeyCashedCharacTypeEii
    int getRecipeProbability();
    int get_local_ip_address();
    unsigned int GetGuildCargoCapacity();
    int GetGuildPowerWarPoint();
    long GetResumeChecksumTime();
    CGameMasterCharacter* GetGameMasterCharacter();
    int getBreakAwayAccureCera();
    int getBreakAwayRewardOrder();
    int get_multiboxLotteryItemFailCnt();
    int GetCurCharacUsedFatigueQuantity();
    bool isHumanCertified();
    bool isLoadingHackGold();   // b28d-Main(P292)：ORIG 0x0867fbe0 调用点 `call; xor $1; test %al; je` ⇒ bool（字段 m_loadingHackGold 已 bool，体直透）
    bool isSaveInformNoticeFlag();
    int CheckMaxLuckyLevel();
    void OnRecvEvent();
    unsigned char GetBuyingGold();   // ORIG movzbl 直返
    void SetBuyingGold(bool flag);
    unsigned char GetFirstLogin();   // ORIG movzbl 直返
    void SetProgLogout();
    bool isRestingUser();            // ORIG movzbl 直返；调用侧 0x8288dd5 test%al 直测（P73/P168a①）
    void InitReceivedRequestType();
    bool isBreakAwayDungeonClear();
    bool isUsedBreakAwayLuckPoint();
    unsigned char isJoinedSchoolPointEvent();   // ORIG movzbl 直返
    void DisableSaveMemberBonusFatigue();
    unsigned char IsEnableSaveMemberBonusFatigue();   // ORIG movzbl 直返
    void SetProperLevelDungeonUser();
    void ClearProperLevelDungeonUser();
    bool IsProperLevelDungeonUser();
    void ProcProperLevelDungeonClear();   // ORIG 0x0868f5fa = GiveMeBoxReward（声明供
    void GiveMeBoxReward();               //   ORIG 0x08693758 = DBInsertGiveBoxEventReward::makeRequest
                                          //   TournamentDungeonClear 调用；定义待 CUser 域批次）
    bool isCharacLinkMessageFlag();   // ORIG callee 0x086973cc movzbl 直返+caller 0x08652a69 test %al 直判；
                                      //   成员 m_field14 已 bool⇒返回 bool 两侧全对（uchar 反产 caller setne 岛）
    bool isLinkCharacDisconnectFlag();   // b28d-Main 代落(P292)：ORIG 0x08652c9a 调用点 `call; test %al,%al; je` 无 setcc ⇒ bool
    void setLinkCharacDisconnectFlag(bool flag);   // ORIG 0x086973dc
    unsigned char getDisconnectLinkCharacSlotIdx();
    // b28d-CUser P248 档①/P292：ORIG T 0x08697598 体 `movzbl 0x8d24c; ret`（无 setne）
    //   + UpdateLogout 0x08655998 调用点 `call; test %al,%al; je` ⇒ bool
    //   （m_updateOntime 本已 bool，字段/mangled 零影响）。
    bool is_update_ontime_last_recv_idx();
    void reset_update_ontime_last_recv_idx();
    bool CheckLoadRentalInfoFromExchange();   // ORIG 消费点 0x084c45cd `call; test %al,%al; je` 无 setne（P292 T3）
    unsigned char GetPuUser();
    void SetCeraPoint(unsigned int point);
    void SetDebugCommand(int cmd);
    void decre_check_count();
    void setLastLotteryTime(unsigned long t);
    void SetNonClientRandInt(int v);
    void recipeForceProbability(int v);
    void setBreakAwayAccureCera(int v);
    void setCharacAntibotSerialNum(int v);
    void set_multiboxLotteryItemFailCnt(int v);
    void resetPlayExpAdd();
    void resetCleanpadFailCnt();
    bool isCleanPadVerifyLimit();
    void onUpdateHackCount(WongWork::ENUM_HACKTYPE type, int count,
                           unsigned int a, unsigned int b);
    int get_server_fatigue_day_size();
    void IncreaseUsedCoinCount();
    void setLoadHackGold(bool flag);
    void SetNonClientFlag(bool flag);
    void setHumanCertified(bool flag);
    void incTradeCount();
    float GetRating();
    void backupSeed();
    void ResetCharacExpandData();
    bool IsExistGuildAgit();
    short GetDailyBadge(ENUM_BADGE_TYPE type);
    int get_charac_count();
    bool isTradePunishOverThirtyDays();
    void set_area(int area);
    int get_area(bool param);
    void onUpdateHackCount(WongWork::ENUM_HACKTYPE type, unsigned int a,
                           unsigned int b, unsigned int c);  // ORIG T 0x867fd0a
    void set_state(ch_state state);
    int get_state();
    void setLastPacketID(int id);
    int getLastPacketID();
    void setPlayingDungeonIndex(int index);
    int getPlayingDungeonIndex();
    void setMoveSpace(UserSpace::T space);
    UserSpace::T getMoveSpace() const;
    int CheckMoveTown(int vill);
    void setBeforeAreaTime(long t);
    long getBeforeAreaTime() const;
    void setUseCraneStart(bool flag);
    bool isUseCraneStart() const;   // [P354/P292 判据] ORIG 调用点 `call; xor $0x1,%eax; test %al,%al; je` ⇒ bool（原 unsigned char 为误标）
    void setForceDropFlag(bool flag);
    unsigned char getForceDropFlag();
    void setDungeonClearHackCheck(bool flag);
    unsigned char isDungeonClearHackCheck();
    void setAllowLocationGeoState(bool flag);
    unsigned char isAllowLocationGeoState();
    void setComboSkillSet(bool flag);
    unsigned char isComboSkillSet();
    bool lock4DataLoad();
    bool unlock4DataLoad();
    bool isLocked4DataLoad();   // 调用点 test %al;je / ! 形 xor$1⇒bool
    bool lockCreateCharac();
    bool unlockCreateCharac();
    bool isLockedCreateCharac();   // 同上
    bool lockDeleteCharac();
    bool unlockDeleteCharac();
    bool isLockedDeleteCharac();   // 同上
    void setAntibotKey(int key);
    void setClientVersion(unsigned int version);
    unsigned int getClientVersion();

    bool Send(PacketGuard& packet);  // ORIG 串 "bool CUser::Send(PacketGuard&)"（P174）；调用点 0x082e6c2b xor$1/test%al
    // ORIG T 0x08690776（报告 CUser/MakeTagCharacInfo.md）：向包写入 tag 角色
    // 完整信息（名字/职业/技能/装备列表/求购单），成功返回 1。定义待 CUser 域批次。
    bool MakeTagCharacInfo(PacketGuard& packet);
    bool isTradePunishType(int type);                  // ORIG T 0x0868991c（ORIG 函数体恒产 0/1，调用方 test %al——原源码为 bool；SecuPF 批次）
    void sendBackSecurityAuthRequest();                // ORIG T 0x08680706（定义待还原）
    bool isSecurityServiceUsed();   // ORIG T 0x0868866e（定义待 CUser 域还原；SecuPF 批次）
    void SendAradEventItemLog(unsigned long itemIndex, int count);  // ORIG 0x08692b84
    void eventDeliveryBuyItem(int deliveryType, const Inven_Item& item);  // ORIG T 0x0866463c
    bool SendPacket(eSendTarget target, PacketGuard& packet);  // b28d-5：ORIG SendCmd* 族尾
                                                              // `call SendPacket; mov %eax,%ebx`
                                                              // 直传 + SendCmdPacket 串证 bool
    bool SendCmdPacket(eSendTarget target, ENUM_CMDPACKET cmd, int param);  // ORIG 串 bool
    bool SendNotiPacket(eSendTarget target, ENUM_NOTIPACKET cmd, int param);  // ORIG 串 bool
    int SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot);   // ORIG T 0x867c2d8 返回 int
    void AddMileage(int mileage, eCoinAddReason reason);                      // ORIG T 0x08650954（W6_ARAD_ItemBonus 补）
    bool SendUpdateItemList(eSendTarget target, ENUM_ITEMSPACE space, int slot);   // [b28d-Main 代落·S338 P292] int→bool（ORIG 调用点 0x081971c2 段 `call; xor $0x1,%eax; test %al,%al; je` 无 setne）
    // b28d-5：ORIG rodata 串 "bool CUser::SendCmdPacket(...)" + 本族体均 `return Send(packet)`
    // （Send 为 bool）⇒ 本族返回 bool；改后定义体免 movzbl 中转。返回类型不入 mangled。
    bool SendCmdOkPacket(ENUM_CMDPACKET cmd);
    bool SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error);
    bool SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error, PAY_TYPE pay);
    bool SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error,
                            PacketGuard& packet);
    bool SendCmdErrorPacket2Byte(ENUM_CMDPACKET cmd, unsigned short error);
    // ORIG T 0x08680a96：拍卖操作前置门。pay==1(cera)/==0(普通) 代理未运行报 0x8c；
    // 状态<3 或无当前角色返回 0x8151；Secu(0x35) 失败报错返回 0；通过返回 -1。
    int AuctionPreCheck(ENUM_CMDPACKET cmd, PAY_TYPE pay);
    bool SendCmdErrorPacketJpn(ENUM_CMDPACKET cmd, unsigned short error);
    // b28d-5：ORIG 3 参版 0x0867be6a `call 4参版; mov %eax,%ebx; …; mov %ebx,%eax`（返回直传）。
    bool SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                unsigned char error);
    bool SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                unsigned char error, PacketGuard& packet);
    bool AntibotSend(PacketBuf& packet);  // ORIG 串 "bool CUser::AntibotSend(PacketBuf&)"（本函数体 cMyTrace 串亦为 bool）
    void SendZeroExpPacket();
    void SendFatigue();
    void SendCoin();                 // ORIG 0x08656808
    bool SaveMoneyCoin();            // ORIG 0x086504d8
    void SendEventMailAlaramTooltip();  // ORIG 0x08690e36
    void giveLotteryCountItem(int itemNo, bool match);  // ORIG T 0x869154a
    void SendMaxEquipLevel();
    void SendOpenflag();
    void SendMoenyLog();
    void SendRevengeDungeon();
    bool SendEffectItemInfo(unsigned int itemIdx);   // b28d-Main(P292)：ORIG 0x086591e9 调用点 `call; xor $1; test %al; je` ⇒ bool
    // ORIG T 0x0868a782：升级罐产出统计。vec 长度==2 时按 Inven_Item+2 的 addInfo
    // 定位罐子（itemIdx==0x28be7b 为罐子），另一元素为产出道具，组
    // Packet_Event_UpgradeJar_Output_Statistic_GTS（0x106e/0x12）经 statistic proxy 下发。
    void SendEventJarOutputItemStatistics(
        const std::vector<Inven_Item>& items);
    // ORIG T 0x0865e994：查当前角色 cooltime 列表（GetCurCharacCoolTimeItemListR）
    // 中是否存在 itemIdx==param 且 expireTime > curSec 的条目。
    bool CheckCoolTimeItem(unsigned long itemIdx) const;
    // ORIG T 0x0865ea62：连续效果/冷却道具处理主链。param_2 非空时依次
    // _processCoolTimeItem→_processEffectItem（各自读 CStackableItem 选项），
    // 最后 _processSpeicalContinuousTimeItem。
    void _processContinuousItem(unsigned int itemIdx, CStackableItem* item);
    // 链簇内部 helper（ORIG T，见各 .md）：
    void _processCoolTimeItem(CStackableItem* item, unsigned int itemIdx,
                              unsigned int curSec);   // ORIG 0x0865eb0c
    void _processEffectItem(CStackableItem* item, unsigned int itemIdx,
                            unsigned int curSec);     // ORIG 0x0865ec0e
    void _processSpeicalContinuousTimeItem(unsigned int itemIdx); // ORIG 0x08659b60
    void UpdateCoolTimeItemInfo(unsigned int expireTime); // ORIG 0x08658b2a
    void UpdateEffectItemInfo(unsigned int expireTime);   // ORIG 0x08658d90
    // b28d-5：ORIG 0x0867c8da 尾 `mov %ebx,%eax`（保存 Send 返回值跨 ~PacketGuard）⇒ bool。
    bool SendMoneyFullReason(ENUM_MONEY_FULL_REASON reason, unsigned long a,
                             unsigned long b);
    void SendProperDungeonClearCount();
    void SendDungeonClearList();
    void SendTagCharacInfo();
    void SendCashData(bool param);
    void SendRestrictedGoods(bool param, int itemIdx);
    bool IsRestrictedGoods(int itemIdx);                                 // ORIG 0x864d878
    void SetRestrictedGoods(int itemIdx);                                // ORIG 0x864d84e
    bool DoGetGoldItem(int itemIdx, std::vector<std::pair<int, int> >& out,
                       bool bReset, ENUM_ERROR& err);                    // ORIG 0x86928c2
    bool DoGetCeraPointItem(unsigned int money, bool bReset,
                            ENUM_ERROR& err, unsigned long itemIdx);     // ORIG 0x8692a34
    void send_skill_info();
    void _UpdateUserInfo4Guild();                         // ORIG T 0x0867cd20
    void send_equip(int slot);
    bool send_itemspace(int space);  // ORIG 0x0865db6c 调用点(GmCmds_B 0x084ab115/DebugCmd 0x085903a9)均 xor$1;test%al 直测形(P22；S23C-R5 取证，Main 代落)
    // ---- OnlyOrig_CreatureJob 批依赖（声明在此，定义归 OnlyOrig_CUser 域生产；
    //      EJMaterial/EJEnchanter 调用）----
    Inven_Item* GetCurCharacInventoryRef(unsigned short area, unsigned short slot);               // ORIG _ZN5CUser24GetCurCharacInventoryRefEtt T 0x08680f2e
    const Inven_Item* GetCurCharacInventoryRef(unsigned short area, unsigned short slot) const;   // ORIG _ZNK5CUser24GetCurCharacInventoryRefEtt T 0x08680c8a
    void CalculateExpertJobMaterials(std::vector<std::pair<int, int> >& materials);               // ORIG _ZN5CUser27CalculateExpertJobMaterialsERSt6vectorISt4pairIiiESaIS2_EE T 0x0868ba58
    // ---- StubS3A_deps 批（item_lock 委托；定义 Game/StubS3A_deps.cpp）----
    int IsAbleItemLock(int space, int slot) const;                    // ORIG T 0x08646a9c
    int IsAbleItemUnlock(int space, int slot, int& outSec) const;     // ORIG T 0x08646bfe
    int IsAbleItemUnlockCancel(int space, int slot) const;            // ORIG T 0x08646cae
    void sendCharacOption();
    void send_pvp_record();
    void sendEventInfo();
    void sendNPCRelationShipFavor();
    void send_aura_avatar_option();
    void sendCharacQp();
    void sendCharacQuestPiece();
    void send_MissionList();
    void send_clear_quest_list();
    void processReturnUserQuestAutoClear();
    void send_RedeemInfo();
    void sendBingoData();
    void sendBingoAddData();
    void sendComboSkillList();
    void sendCharacLinkMessage();
    void sendBlueMarbleEnterCount();
    void sendNotiPcRoomPlayTime();
    void sendReturnUserFirstLogin();
    bool send_ontime_reward_start_notify();   // 串定 bool（b28c SIGFIX）
    bool sendSecuRewardItem(Inven_Item* item, ENUM_SECURITY_REWARD_TYPE type,
                            int* param);
    // [b28d-W8] void→bool：ORIG 0x08688548 `mov $0x0,%eax` / 0x0868855d
    // `mov $0x1,%eax` 双返回值（P159 尾形判定），无其它调用点。
    void sendInformNoticeFlag();
    void sendDecorationObjectCnt(short count);

    // ---- 角色选择 / 登录数据 ----
    // [K7CUserOnlyOrig 2026-09-11 · P174 串定] ORIG .rodata 实串 `bool CUser::SelectCharac(int, int)`
    //   （我方旧串 `int ...` 在 ORIG 内不存在）⇒ bool。返回类型不入 mangled，符号/布局零影响；
    //   消费点 InterDispatcher.cpp:1908 忽略返回值、CUserOnlyOrig 定义侧同刻改。
    bool SelectCharac(int slot, int param);
    int CreateCharac(SIG_CREATE_CHARAC* data);
    void DeleteCharac(unsigned int characNo);   // ORIG Ghidra: return; (void)
    void GiveItemsReturnUser();         // ORIG T 0x08692000
    void GiveEquipItemsReturnUser();    // ORIG T 0x08692140
    void ChangeCharacSlot(unsigned int a, unsigned int b);   // ORIG 0x08651bb9 单条跳尾、全函数无 mov $0,%eax（P159）
    bool checkSelectCharac(int slot);   // [b28d-Main·P292] int→bool（ORIG 调用点 0x081c6b50 `call; xor $0x1,%eax`）
    int getCharacSlotCountDefaultInfo();  // ORIG _ZN5CUser29getCharacSlotCountDefaultInfoEv W 0x0868b4fc
    void ReturnToSelectCharacList(bool param);
    unsigned char LogoutCachedCharacter(unsigned char param);   // ORIG 0x0864cd85 `mov $0x1,%eax`（尾显式 return 1）
    void SetCharacInfo(int slot, char* data);
    // [b28d-solo-DisPatcher_DebugCommand_cmds 代落 P248档①] int→bool：ORIG
    // 0x0858f496 `call SetCharacLevel; xor $1,%eax; test %al,%al; je`（!=1 物化，
    // bool 唯一解）；定义 CUser.cpp:6419 返回 0/1，消费点 Dispatchers.cpp:2946
    // `int result = ...` 保持 0/1 语义。返回类型不入 mangled。
    bool SetCharacLevel(int level);
    // 同上：ORIG 0x0858eb1c `call SetSkillLevel; test %al,%al; je`（无 setne/xor）；
    // 定义 W7_UserOnlyC.cpp:464 返回 0/1。
    bool SetSkillLevel(int skillIdx, int level);
    void onSelectCharacter();
    void select_charac_set_visible_values(char value);
    void UpdateCharacView();
    void UpdateData();
    // ORIG _ZN5CUser40UpdateCurCharacVisibleToCharacViewMemoryEv T 0x08651628
    //   （ReturnToSelectCharacList 调用；定义 CUser.cpp）。
    void UpdateCurCharacVisibleToCharacViewMemory();
    void UpdateLogout(bool param);
    void OnDisconnect();
    bool Recv();   // ORIG 尾部 mov $0/$1,%eax 实证 bool 返回（0x08648057/805e）
    bool Parsing(int param);

    // ---- 基础访问器（GameStubs 桩替换） ----
    void DecreaseGuildPowerWarPoint(unsigned int point);
    int get_charac_no(int type);
    int get_charac_idx(unsigned int characNo) const;
    int get_character_job(unsigned int characNo) const;
    unsigned int GetUID() const;
    unsigned int get_acc_id() const;
    const char* get_acc_name() const;
    bool CheckItemLock(int a, int b) const;
    int GetAccountLastPlayTime();
    ENUM_SERVER_GROUP GetServerGroup() const;
    int GetServerGroupToPvP() const;   // ORIG T 0x0864a97c（定义 CUserDepsB.cpp）
    // ORIG W 0x084ed008（弱符号⇒头内 inline 定义；proceed_trade 0x853087a 调用点
    // add $0x8e980 直取成员，消费侧按 ORIG 逐点改成员访问，TraC9 请求/Main 代落）
    Secu_GoldControl* GetGoldControl() { return &m_goldControl; }
    CDungeonGainedGold* getDungeonGainedGold();
    WongWork::CHackAnalyzer* getHackAnalyzer();
    CExpandEquipslot* GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE type) const;
    // [b28d-Main·检查点18] 删除 `ENUM_CHARAC_EXPAND_TYPE&` 重载（ORIG 仅按值版 0x0828b5de）。
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const;
    void AddDBLogItem(unsigned int itemIdx, unsigned int type,
                      ENUM_DBLOG_ITEM_TYPE value);
    int addAvatarItem(unsigned long itemIdx, char param2, char param3,
                      const char* ipgAgency, eAvatarItemAddReason reason);
    bool getGmQuestFlag();
    int GetCurExpertJobLevel(int exp);
    WongWork::CUserPremium* GetPremiumInfo() const;
    // ORIG W 0x0830edaa _ZN5CUser36getAddIndependentDropRateFromPremiumE17ENUM_PREMIUM_TYPEj
    // 转发 m_premium（+0x7121c）的独立掉落率查询；clear_reward 掉率链消费。
    unsigned int getAddIndependentDropRateFromPremium(ENUM_PREMIUM_TYPE type, unsigned int level);
    UserQuest* getCurCharacQuestR() const;
    UserQuest* getCurCharacQuestRefR() const;   // ORIG W 0x082604ec _ZNK5CUser21getCurCharacQuestRefREv
                                                // （clear_reward 任务物品链消费；定义待任务域批次）
    UserQuest* getCurCharacQuestW();
    bool gain_exp_sp(int exp, int& sp, int& sfp, eExpAddReason reason,
                    int param, bool flag);   // ORIG 0x0866a3fe _ZN5CUser11gain_exp_spEiRiS0_13eExpAddReasonib：嵌套 S0_=CUsereExpAddReason（2137 行枚举），bool 返回
    void makeGuildSkillMessage(const char* skillName, int count);
    // ---- 高频簇 producers（本批次还原；签名均与 ORIG mangled 一致）----
    // [b28d-Main·检查点17 P245/P88] static：ORIG 调用点 0x08664ec0..ce 仅压 2 实参
    //   (q, user) 无 this；static 不入 mangling ⇒ 符号名/mangled/sizeof/偏移零变化。
    static std::vector<RewardItemInfo>* getUserQuestRewardItem(
        const Quest* quest, const CUser* other);           // ORIG _ZN5CUser22getUserQuestRewardItemEPK5QuestPKS_ T 0x0866e914（LinkClosureA 落 LinkClosureA_User.cpp）
    void make_charac_stat(PacketGuard& packet);                       // ORIG T 0x8659faa _ZN5CUser16make_charac_statER11PacketGuard
    void Remove_Mission(short kind);                                  // ORIG T 0x866273c _ZN5CUser14Remove_MissionEs
    void incPlayExpAdd(int v);                                        // ORIG W 0x869729a
        // （_check_level_up 完整声明见其它区段；截断残句删除）
    void gain_sp(int v);                                              // ORIG T 0x866a9a0
    void gain_sfp(int v);                                             // ORIG T 0x866aad2
    void history_log_sp(int before, int delta, eSPAddReason reason);  // ORIG T 0x866ac0e
    void history_log_sfp(int before, int delta, eSPAddReason reason); // ORIG T 0x866acd0
    bool VerifyPresentAvengerTitle();                                 // ORIG T 0x868b552
    // [b28d-W8] char→bool：ORIG 调用点 0x0866a774 `call; xor $0x1,%eax; test %al,%al;
    // je`（P292/P248档① 直证 callee bool）；本体尾 `mov $0/%1,%eax`。
    void decide_growth_power_reward_system();                         // ORIG T 0x868d780
    void AddCurCharacMercenaryInfo();                                 // ORIG T 0x868e596
    void _update_charac_stat(bool add);                               // ORIG T 0x86646c8
    void UpdateTutorialSkipable();                                    // ORIG W 0x8697318
    void _onLevelUp();                                                // ORIG T 0x866311a（_check_level_up 升级尾调；定义待还原）
    // ---- LinkMisc4 批次补声明（ORIG 符号 nm 核实；实现见 UserPartyLink4.cpp）----
    void RewardTutorial(unsigned int flag);              // ORIG T 0x086475d8 _ZN5CUser14RewardTutorialEj
    void adjust_charac_stat();                           // ORIG T 0x08664766 _ZN5CUser18adjust_charac_statEv
    void deleteGuildExpBook();                           // ORIG T 0x0867e898 _ZN5CUser18deleteGuildExpBookEv
    void calc_lev_stat(_Additioal_info& info);           // ORIG T 0x08664ae8 _ZN5CUser13calc_lev_statER15_Additioal_info（定义待还原，adjust_charac_stat 直调）
    // ---- _onLevelUp 的被调（声明先行，定义由其它批次还原）----
    void _processLevelReward(unsigned int level, unsigned int characNo,
                             char job);                              // ORIG T 0x868745e
    void _postQuestTriggerItem(const Quest* quest);                  // ORIG T 0x086689ee
    void processLevelUpEventReward(bool flag);                       // ORIG T 0x08663cc0
    void processLevelUpEvent();                                      // ORIG T 0x0869115a
    // ORIG 0x8679cee：公会强化公告（name/upgrade 标记/是否增幅）。
    // 尾 epilogue 无 %eax 写（结果仅 %ebx 内部 goto）+4 调用点均表达式语句⇒void
    //（CUserC8 证据/Main 代落，mangled EPKciib 无返回类型零 ABI 风险）
    void makeGuildItemUpgradMessage(const char* itemName, int from,
                                    int to, bool isAmplify);
    // ORIG T 0x0867a242：分离强化公告（Ctor2_ContB 定义于 CItemUpgrade_Separate.cpp）。
    void makeGuildItemSeparateUpgradMessage(const char* itemName, int oldLevel,
                                            int newLevel, bool success, int upgrade);
    void set_grow_type(unsigned char firstGrow, unsigned char secondGrow,
                       char* name, eChangeGrowTypeReason reason);
    bool is_clear_stealingSkillMission() const;
    void givePvPSkillTree(int a, bool b, int c);
    bool make_basic_info(char* buf, char type);            // ORIG 0x0865a44e；串定 bool（b28c SIGFIX）
    unsigned short get_unique_id() const;   // ORIG 单 movzwl（返回 unsigned short，读 m_field704a8）
    int get_pvp_WinningRate_relateMission() const;  // ORIG 0x86626e0
    CParty* GetParty();   // 体即 return G_CGameManager()->GetParty(idx)（CParty* 透传，
                          // void* 系历史规避写法；调用点 cast 变冗余 no-op，用户令修正）
    int GetUserPosInParty();      // 0x08688638
    WarRoom* GetWarRoom();   // 体即 G_CGameManager()->GetWarRoom(idx) 透传（与 GetParty 同法修正）
    void set_growth_power_exp_reward(int reward);
    STExpReward get_growth_power_exp_reward();   // ORIG sret 结构返回（仅填 m_reward dword）
    unsigned short getCurCharacTotalFatigue() const;
    unsigned short getCurCharacTotalMaxFatigue() const;
    int serviceOfShusia();    // ORIG T 0x865798a（休沙疲劳恢复服务：扣费+满疲劳；定义见 DisPatcherCore.cpp）

    // ---- G1-4 访问器/索引族（ORIG 逐符号转录） ----
    bool CheckInTrade();
    int isEnableAvatarSocketAction();     // ORIG _ZN5CUser26isEnableAvatarSocketActionEv 0x086810a8
    bool CheckInParty();
    bool checkInBlueMarble();
    bool CheckInWarRoom();
    bool checkInDeathTower();
    void setDeathTowerIndex(short idx);
    short getDeathTowerIndex();
    WongWork::CDeathTower* getDeathTower();          // ORIG 0x086552a4
    int Add_RedeemInfo(const Inven_Item& item, int count, bool isRedeem);  // ORIG 0x086472c0
    int getCurCharacInvenCheckSum() const;   // ORIG T 0x0864e0b6（blocked：定义待补；
                                             //  DisPatcher_FinishLoad::process 比对
                                             //  MSG_FINISH_LOADING.m_0d）
    int getCurCharacSkillCheckSum() const;   // ORIG T 0x0864e114（blocked：定义待补；
                                             //  比对 MSG_FINISH_LOADING.m_11）
    // ---- 以下四符号为 DisPatcher_{DeleteItem,MoveItem,SelectDungeon}::process 的
    // ORIG 直调依赖，声明先行（定义由后续物品域批次补，勿在此重复实现）----
    int delete_item(char itemSpace, int position, unsigned long itemIdx,
                    int count, eItemDelReason reason);              // ORIG T 0x0866285c _ZN5CUser11delete_itemEcimi14eItemDelReason
    // [Q3PrettSweepA 2026-09-11] 第 3/7 参拼写对齐 ORIG typedef（同底层 unsigned long，
    // mangled `_ZN5CUser19exchange_interspaceEiimiiimPi` 不变）；ORIG prett 直证 =
    // `int CUser::exchange_interspace(int, int, itemIndexOnlyServer_t, int, int, int,
    // itemIndexOnlyServer_t, int*)` size 0x6c。typedef 见 GameTypes.h:60。
    int exchange_interspace(int fromSpace, int fromPos, itemIndexOnlyServer_t count,
                            int toSpace, int toPos, int field1b,
                            itemIndexOnlyServer_t itemIdx, int* outSlot);   // ORIG T 0x0865f7b0 _ZN5CUser19exchange_interspaceEiimiiimPi
    int exchange_interspace_expand(int fromSpace, int fromPos, unsigned long count,
                                   int toSpace, int toPos, int field1b,
                                   unsigned long itemIdx, int* outSlot,
                                   char expanda, char expandb);     // ORIG T 0x0865f614 _ZN5CUser26exchange_interspace_expandEiimiiimPicc
    int change_equip(int space, short slot, int dstSlot);           // ORIG T 0x0865eed2 _ZN5CUser12change_equipEisi（定义由后续物品域批次补）
    int change_equip_expand(int space, int slot, int dstSpace,
                            int dstSlot, int* outSlot);             // ORIG T 0x0865f382 _ZN5CUser19change_equip_expandEiiiiPi
    void SendNotiPacketMessage(const char* msg, ENUM_MESSAGE_TYPE type);  // ORIG T 0x086886ce _ZN5CUser21SendNotiPacketMessageEPKc17ENUM_MESSAGE_TYPE
    advancealtar::StageControl* getAdvanceAltar();  // ORIG T 0x08655262：m_advanceAltarIdx<0→0 否则 mgr->getAdvanceAltar(idx)（定义 BossTowerFunc.cpp）
    void SetDeleteEventDungeonInvitation(bool flag);                // ORIG W 0x08230844（写 m_deleteEventDungeonInvitation；定义宿主 DisPatcherCore.cpp 尾部）
    bool checkInBossTower();
    short getBossTowerIndex();
    WongWork::CBossTower* getBossTower();  // ORIG T 0x08655220：m_bossTowerIdx<0→0 否则 mgr->getBossTower(idx)（定义 BossTowerFunc.cpp）
    void setBossTowerIndex(short index);
    bool checkInAdvanceAltar() const;
    short getAdvanceAltarIndex() const;
    short getBlueMarbleIndex();   // ORIG checkOutBlueMarble 调用点 cwtl（符号扩展）⇒ short
    void setBlueMarbleIndex(short index);
    short GetPartyIndex();
    bool CheckInPvp();
    short GetPvpIndex();
    short GetWarRoomIndex();
    int GetTradeSpace();
    void SetPartyIndex(short index);
    void SetWarRoomIndex(short index);
    void SetSlotIDX(short idx);
    short GetIncreID() const;
    void SetIncreID(short id);
    void set_unique_id(unsigned short id);
    void set_position(unsigned short x, unsigned short y, char z);
    unsigned short get_posX();   // ORIG 0x0813492c：单 movzwl mem,%eax（返回 unsigned short，非 int）
    unsigned short get_posY();   // ORIG 0x0813493c 同形
    char get_direction();
    int GetUserState();
    int GetAge();
    bool IsOverBirthDay();
    bool IsProgLogout();   // ORIG 调用点 test %al,%al; je 无 setne ⇒ bool（GWorld check_user_var / Inter_LoadDungeonClear 证据）
    int GetSeedFromDate();
    void SetGMUpgradeMode(ENUM_GM_ITEM_UPGRADE mode);
    bool isGMUser();
    bool IsGameMasterMode() const;
    // [b28d-Main·检查点14] S402 P159/P174：ORIG 0x086796dc 两路径尾均 `mov $0x1,%eax`
    //   （+0x6d 恒返 1）⇒ 真实返回 bool；mangled 不含返回类型，符号名不变。
    bool SetGameMasterMode(bool flag);
    bool GetEventCreateDnfReward();  // b28d-Main 代落(P292)：ORIG 0x08160cba 调用点 `call; xor $0x1,%eax; test %al,%al; je` ⇒ bool；字段 +0x8ec30 已 bool，体保持 movzbl 直返
    void SetPowerUp(bool flag);
    int SetETC(short key, int value);

    // ---- G1-4 CodeHackCheckStorage / 返回用户 / 预登录数据 ----
    void setTournamentAccount(bool flag);
    unsigned char isTournamentAccount();
    void setJoinedSchoolPointEvent(bool flag);
    void setOnlinePreliminaryServerGroup(ENUM_SERVER_GROUP group);
    void setOnlinePreliminaryCharacNo(unsigned int characNo);
    void setOnlinePreliminaryPlayType(int playType);
    int getOnlinePreliminaryPlayType();
    unsigned int getOnlinePreliminaryCharacNo();
    void SetPCRoomNo(unsigned int no);
    void SetFirstLogin();
    void SetReturnUser();
    void ResetReturnUser();
    bool IsReturnUser();   // ORIG 调用点 test %al,%al; je 直判 ⇒ bool（uchar 必物化 setne）
    void SetReturnUserFirstUserLogin(bool flag);
    bool IsReturnUserFirstLogin();   // 同上：ORIG 直判 bool 形态
    int GetReturnUserExpireTime();
    void SetReturnUserExpireTime(int time);
    void EnableSaveMemberBonusFatigue();
    void incAccountMemberBonusFatigue(short value);
    void resetAccountMemberBonusFatigue();
    short getAccountMemberBonusFatigue();
    void resetAccountUsedFatigue();
    void incChattingMessageCount(int count);
    void setChattingMessageCount(int count);
    int getChattingMessageCount();
    void SetGameMasterCharacter(CGameMasterCharacter* pCharac);
    int getStdDropRate();
    Secu_AccountHacking* getAcccountHackInfo();
    void setHackUserTimer();
    void setHackUserFlag();
    bool isHackUser() const;        // P292/T3 返回 bool：ORIG 消费点 0x081be54c `call; test %al,%al; je`（无 setcc）；mangled 不含非模板返回类型
    bool isHackUserTimer() const;   // ORIG bool（P22：调用侧 xor $1 取反实证；P32 bool 字段直返 movzbl）
    // ---- 阶段A语义对齐补声明（ORIG 符号 nm 核实；实现由 CUser.cpp/后续批次提供） ----
    bool CacheCharacterMemory();                    // 0x0864c85a T；串定 bool（b28c SIGFIX）

    // ---- 角色内存快照填充（CacheCharacterMemory 依赖；ORIG 符号，定义由 producer 提供）----
    void GetRentInfo(SIG_LOAD_INVENTORY& sig);                      // 0x086769b4
    bool GetCharacInfo(SIG_LOAD_CHARAC* sig) const;                 // 0x0864bc04
    bool GetInventoryInfo(SIG_LOAD_INVENTORY* sig);                 // 0x0864d4ba（P159：ORIG FAIL 岛 mov$0/成功尾 mov$1 ⇒ 非 void；mangled 无返回类型 ABI 零变）
    void GetTicket(SIG_LOAD_TICKET* sig) const;                     // 0x0867846e
    // ---- 角色内存快照填充 helper（CacheCharacterMemory 链依赖；ORIG 符号）----
    int GetPcRoomRentInfo(SIG_SAVE_PC_ROOM_RENT_ITEM* sig);         // 0x086769ea
    int GetDimensionRentInfo(SIG_SAVE_DIMENSION_RENT_ITEM* sig);    // 0x08676bec
    bool getCharacInfoDetail(int idx, unsigned int characNo,
                             CHARAC_DATA* out) const;               // 0x0864bdba（b28c C28L5 代落：全出口仅 mov$1/mov$0 P159 + 消费点 0x0864bdf2 xor$1/test%al P168a①）
    int getTagCharacInfo(int idx, unsigned int characNo,
                         TAG_CHARAC_DATA* out) const;               // 0x086904f0
    bool getTagCharacInventory(TAG_CHARAC_INVEN_DATA* out) const;   // 0x0869067c
    bool GetInventory(SIG_LOAD_INVENTORY* sig);                     // 0x0864d646（ORIG 尾 mov$1 直返；调用点 xor$1/test/jne = bool callee 直判形；mangled 无返回类型 ABI 零变）
    bool removeRentalItemInfoFromInvenLoadBuf(SIG_LOAD_INVENTORY* sig,
                                             int param);            // 0x0867776c
    bool GetDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig) const;        // 0x0864de58
    int GetBlackListCount(SIG_REQ_BLACK_COUNT* sig) const;          // 0x0867f1d4
    int GetEtcInfo(SIG_LOAD_ETC* sig) const;                        // 0x0867794c
    void GetRecord(SIG_LOAD_ETC* sig) const;                        // 0x08677dd8
    void GetLimitItemUsage(SIG_LOAD_ETC* sig) const;                // 0x08688dc4
    bool GetSkill(SIG_LOAD_SKILL* skill, SIG_COMBO_SKILL* combo) const; // 0x0864dcf2
    void GetAuraAvatarOption(stAuraAvatarOption& option) const;     // 0x0868ddc2
    void GetCurCharacOption(CharacterOption& option) const;         // 0x0868f03c
    void GetCharacCashAttendanceInfo(SIG_ATTENDANCE_EVENT& info) const; // 0x086910a6
    unsigned int getTotalPcRoomPlayTime() const;                    // 0x08110e10 W：b28d-Main 代落·S350(P302/P248档①)——ORIG 0x081108c4 `cmp %edx,%eax; setb`（call 结果留 %eax 无中转）⇒ unsigned int
    unsigned int getPcRoomPlayTimeRewardCnt() const;               // 0x08110dec W：b28d-Main 代落(P248档①)——ORIG 0x08110a38 `cmp %edx,%eax; setae`（无符号比较，call 结果留 %eax 无中转）⇒ unsigned int
    int GetCurCharacUsedGiftFatigueQuantity() const;                // 0x08110e6c W
    bool getFlagPresentAvengerTitle() const;                        // 0x08697578 W
    // [b28d-W8] 返回类型 uchar→bool：ORIG 调用点 0x0868b68b
    // `call; xor $0x1,%eax; test %al,%al; je`（P292/P248档① 直证 callee bool）；
    // 字段 m_ceraUserInfo.m_field62 本就是 bool，本体 movzbl;ret 不变。
    STSkillCommandData::STCommand* get_skill_command_buf();               // ORIG _ZN5CUser21get_skill_command_bufEv 0x0865459a（写 m_skillCommands 并返回）
    STSkillCommandData::STCommand* get_skill_command_buf(STSkillCommandData::STCommand* out) const; // 0x08654768
    void SendLogoutToPCRoomServer();                // 0x0868170c T
    void SendLoginToPCRoomServer();                 // 0x0868152c T（TimerCheckPaidPCRoomTerm 调用）
    void SetTradePunishType(int type);              // 0x08689890 T（TimerSetPunishUser/TimerSetTradePunishUser 调用）
    int recover_stamina();                          // 0x0865784e T（TimerStamina 调用；b28 BM1 P159 代落：ORIG 体尾 movzbl 双段非 void，mangled 不变）
    void WorkPerFiveMin();                          // 0x08652f0c T（TimerScheduleFiveMinPerUser 调用）
    void WorkPerTenMin();                           // 0x08653196 T（TimerScheduleTenMinPerUser 调用）
    bool delete_ontime_reward_list();               // 0x0868c3bc T；串定 bool（b28c SIGFIX）
    bool HasCurCharacRentalItem(int type) const;    // ORIG _ZNK5CUser22HasCurCharacRentalItemEi 0x86772d2
    bool findSkillCommandData(int skillId, STSkillCommandData& out);          // 0x08654b94 T
    void RefillItemByScript(std::vector<std::pair<int, int> >& list);         // 0x086890be T
    bool reqHumanCertify();                         // 0x867f26e T
    void GenerateCleanPadVeriData(unsigned short& outA, unsigned char& outB,
                                  unsigned short& outC);   // ORIG T 0x08649ac4
    void setStdDropRate(int rate);                  // 0x828715c W
    void makeGuildLevelUpMessage(int level);        // 0x8679754 T
    void updateCharacViewVisibleData(const _Charac_info& info,
                                     struct CHARAC_LOGIN_DATA& data);          // 0x8651322 T
    bool checkRentedItem(unsigned int characNo, int itemIndex, int param);     // ORIG T 0x08677188
    void _deleteSpecificItemInven(INVEN_TYPE type,
                                  const std::vector<std::pair<int, int> >& list,
                                  std::vector<std::pair<int, int> >& out,
                                  int begin, int end);                // 0x867d0be T
    void _deleteSpecificItemCargo(const std::vector<std::pair<int, int> >& list,
                                  std::vector<std::pair<int, int> >& out);      // 0x867d3ee T
    void _deleteSpecificItemAccountCargo(const std::vector<std::pair<int, int> >& list,
                                         std::vector<std::pair<int, int> >& out); // 0x867d8a4 T
    void _deleteSpecificItemTitieBook(const std::vector<std::pair<int, int> >& list); // 0x867db66 T
    void _deleteSpecificRedeemItem(const std::vector<std::pair<int, int> >& list);    // 0x867d69c T
    int _isMatchedItem(int itemIdx, const std::vector<std::pair<int, int> >& list);   // 0x867cf0c T
    bool IsGuildMaster();                           // ORIG W 0x08230172
    bool IsSubGuildMaster();                        // ORIG T 0x0866c414
    bool IsCompleteGuildFund();   // ORIG T 0x0866c1ca _ZN5CUser19IsCompleteGuildFundEv
                                  // 定义见 CUserDepsA.cpp（m_guildDB.m_fundLevel/m_fundMoney 已字段化）
    // [b28d-solo-CUser 代落] void→int：ORIG 0x0864870e 尾 `mov $1,%ebx; … mov %ebx,%eax; ret`
    // （恒返回 1；调用点忽略返回值）。返回类型不入 mangled。
    int SendClientCrashDownLogToLogDB(int reason);  // ORIG T 0x0864870e
    void _SaveCharacData();                         // ORIG T 0x08650aa2
    // GetCurCharacAssaultFatigueCount/Inc/Set 三方法由基类 CUserCharacInfo
    // 提供（ORIG 仅存在 _ZN16CUserCharacInfo* 弱符号，无 CUser:: 版本）。

    // ---- G1-4 签到 / 突破奖励 / 商城 / 日常 ----
    int getBreakAwayRewardItemId();
    void setBreakAwayRewardItemId(int id);
    int getBreakAwayRewardAItemCnt();
    void setBreakAwayRewardAItemCnt(int cnt);
    int getUserBreakAwayRewardLuckPoint();
    void setUserBreakAwayRewardLuckPoint(int point);
    int getBreakAwayRewardItemOrder();
    STBreakAwayRewards* getBreakAwayRewards();
    void setBreakAwayRewards(const STBreakAwayRewards& rewards);
    int getBreakAwayQuestTime();
    void setBreakAwayQuestTime(unsigned int time);
    int getBreakAwayDungeonClearCnt();
    void setBreakAwayDungeonClearCnt(int cnt);
    void subBreakAwayDungeonClearCnt();
    void setBreakAwayDungeonClear(bool flag);
    unsigned char isBreakAwayRewardData();
    void setGmQuestFlag(bool flag);
    void setWebAddress(char* addr);
    // [b28d-CPrivateStoreMgr] 返回类型判据（ORIG 0x085cbd38 + 探针 f15/f16）：unsigned int。
    // [SweepCGuild2 判据㉓ 符号绑定] ORIG `_ZN5CUser8GetCeraEv` = **W** 0x080fdf7a
    //   size 0xe（头内 inline 定义；对照同族 GetCeraPoint 为 T 0x08692af6 外联）
    //   ⇒ 改类内定义复现绑定。CUser.cpp:5716 有调用 ⇒ 本 TU 仍发射 W 副本，四数不变。
    unsigned int GetCera() { return m_cera; }
    void SetCera(unsigned int cera);
    unsigned int UseCera(unsigned int cera);
    void setRegDate(unsigned int date);
    void setRegTime(long time);
    int getRegTime() const;
    void resetUsedCera();
    void addUsedCera(unsigned int cera);
    unsigned int getUsedCera();
    void SetVillageAttackedReward(bool flag);
    void setPcRoomAuth(bool flag);
    void fillUserInfo4Antibot();
    char* getUserInfo4Antibot();
    void SetAccountLastPlayTime(long time);
    void SetLoginTime(long time);
    long GetLoginTime();
    void SetLogInOutState(unsigned char state);
    unsigned char GetLogInOutState();
    int get_public_ip_address();
    void set_public_ip_address(int ip);
    void set_local_ip_address(int ip);
    void setHangameUser(bool flag);
    bool isHangameUser();   // ORIG LogHistory @0x867c8fe call 后 test %al;je 无 setne ⇒ bool 返回（gcc4.4.7 仅 bool 产此形态）
    bool IsPowerUp();  // b28c C28L3 代落：ORIG 0x0863be64 movzbl 直返 + 调用点 test%al 免归一（P211/P32）
    void SetRating(float rating);
    void SetSchoolNo(unsigned int no);
    unsigned int GetSchoolNo();
    void SetSchoolName(const char* name);
    void SetDailyBadge(short* badge);
    void SetUserDetailInfo(char* a, char* b, char* c);
    unsigned char GetTutorialSkipable();
    void SetTutorialSkipable(unsigned char flag);
    unsigned int getHumanCertifyTimerKey();   // 2026-09-11 H3CUser（Main 核准）：ORIG 0x086371cd 调用点 `mov 0x10(%ebp),%edx; cmp %edx,%eax; seta`（无符号比较 2 条形）⇒ unsigned 返回；返回类型不进 mangled
    void setHumanCertifyErrorCnt(int cnt);
    bool isSendMailCertified();  // b28d-Main(P292)：ORIG 0x086371a4 调用点 `call; test %al,%al; je` ⇒ bool（字段 m_field8e3ed 已 bool，体直透）
    void setRestingUserRestrict(bool flag);
    void setPunishTradeAlert(short alert);
    short getPunishTradeAlert();
    void resetDungeonClearHackCheck();
    bool IsRecvEvent();  // b28d-Main(P292)：ORIG 0x086369b8 调用点 `call; xor $1; test %al; je` ⇒ bool（字段 m_recvEventWaiting 已 bool）
    int GetAutoClientRate();
    bool isARSUserKick();   // ORIG 0x08638c1a test %al;je 直判 ⇒ bool
    void setSecuReward(bool flag);
    unsigned char NeedSecuReward();
    void setSecuType(unsigned char type);
    unsigned char getSecuType();
    unsigned char getRewardType();
    void setRewardType(unsigned char type);
    void setSecuGrade(char grade);
    void setFlagPresentAvengerTitle(bool flag);
    void incCharacSlotLimit(unsigned char count);
    // ORIG 0x08230860 (W)：movzbl +0x8d245（m_ceraUserInfo.m_field61）
    unsigned char getCharacSlotLimit() const;
    // ORIG 0x08230870 (W)：movzbl +0x8d244（m_ceraUserInfo.m_field60）
    unsigned char getSlotEffectCount() const;
    // ORIG 0x08230880 (W)：写 +0x8d244（m_ceraUserInfo.m_field60）
    void setSlotEffectCount(unsigned char count);
    // ORIG 0x08676826 (T)：组 Packet_Randombox_statistic（header 0x17b6/0xc）发统计服务器
    static void SendRandomBoxStatistic(int boxKind, ENUM_RANDOMBOX_ACTION_KIND actionKind);   // ORIG 0x8676826 静态 ABI（报告证实无 this 实参）
    // ORIG 0x0868f6bc (T)：find_item(itemIdx)→verifyStackableItemProcess
    bool verifyStackableItemActionPlace(StackableAction::Type type, unsigned int itemIdx);   // [b28d-Main·P292] int→bool
    bool verifyStackableItemActionPlace(StackableAction::Type type, const CItem* item);  // ORIG W 0x0868f69a（[b28d-Main·P292] int→bool）
    // ORIG 0x0868f60e (T)：is_stackable→verifyStackableAction→verifyUsablePlace→verifyUsableServerType
    bool verifyStackableItemProcess(StackableAction::Type type, const CItem* item);   // [b28d-Main·P292] int→bool（ORIG 0x0868f60e 调用点 `call; xor $0x1,%eax`）
    void set_ontime_last_recv_idx(int idx);
    int get_ontime_last_recv_idx() const;
    void set_update_ontime_last_recv_idx();
    bool InsertOnTimeEventReward(unsigned int itemNo, unsigned int itemCount);  // ORIG T 0x08681c30；串定 bool（b28c SIGFIX）
    void setCheckPickUpRandomOptionItem(bool flag);
    unsigned char getCheckPickUpRandomOptionItem();
    void set_cera_item_order_no(unsigned int no);
    unsigned int get_cera_item_order_no();
    void setPcRoomPlayStartTime(unsigned int time);
    unsigned char IsDeleteEventDungeonInvitation() const;
    void EventDungeonClearReward(std::vector<std::pair<int, int> > const& items);
    // ORIG W 0x08689576 _ZN5CParty…（实为 _ZN5CUser23EventDungeonClearRewardERKSt6vectorISt4pairIiiESaIS2_EE；
    // clear_reward 事件地下城奖励链消费；定义待事件域批次）
    unsigned char IsUsedCoinInDungeon() const;
    void ResetCoinCount();
    int GetUsedCoinCount();
    void set_trick_area(int area);
    int get_before_area();
    // ---- G1-4 账号货物 / 锁 / 其余访问器 ----
    CAccountCargo* GetAccountCargo();
    bool IsExistAccountCargo();
    void DeleteCargo();
    void lock();
    void unlock();
    int getBackupSeed();
    char getPowerSide();
    char* getClientHWSpec();
    void SetGuildDBInfo(STGuildDBInfoOnly& info);
    void ChangeGuildName(char* name);
    void SetGuildAgitFlag(bool flag);
    void SetGuildAgitDBInfo(STGuildAgitDBInfo& info);
    void SetGuildCargoCapacity(unsigned int capacity);
    void SetTodayGuildMember(bool flag);
    unsigned char IsTodayGuildMember();
    void SetLastLoginCharacNo(int characNo);
    void SetLastLoginChannelNo(int channelNo);
    void SetValidLastLoginCharac(bool flag);
    void SetReportMannerlessUserCount(int count);
    int GetChecksumAnswerCount();
    void IncChecksumAnswerCount();
    int GetCheckConnInterval();
    void SetResumeChecksumTime(int time);
    void incre_check_count();      // ORIG 无返回值指令（void）
    short get_connect_count();
    void setTotalHackUserTradeGold(unsigned long long gold);
    void setTotalHackUserTradeCnt(unsigned int cnt);
    void SetRevivalTime(long time);
    long GetRevivalTime();
    void SetUseAPCPotionTime(long time);
    long GetUseAPCPotionTime();
    // ORIG W 0x0863be94（弱符号⇒头内 inline；RecoverFatigue 0x57ca4/0x57cd3 实证
    // 调用点全内联 this+0x7121c 无 call，CUserD17 请求/Main 代落 batch-17）
    WongWork::CUserPremium* GetPremiumInfoW() { return &m_premium; }
    // ORIG T 0x0864d90a：从 m_list711ec（+0x711ec）删除 state。
    void RemoveRestrictedGoods(int state);
    // ORIG T 0x0867ceae：全部角色 m_needCheckOverEquipItem（+0xe3b）置 1。
    void SetNeedCheckOverEquipItem();

    // ---- G1-4 角色列表 / 视图 ----
    void SetLoadRentalInfoFromExchange(bool flag);              // ORIG W 0x0822fc06（写 m_rentalFlag2 +0x6ef91）
    void RemoveCharacMercenaryInfo(unsigned int characNo);      // ORIG T 0x0868e672（转调 m_mercenaryMgr.RemoveCharac）
    int GetGuildIdBeforeDeleteCharacter(int slot);              // ORIG T 0x0864a956（读 m_characList[slot].m_guildId）
    const char* get_charac_name(int idx) const;
    unsigned char GetCharacJob(int idx) const;   // ORIG caller checkPvpEnterCharacter 0x081c6c22 `cmp $0x9,%al` 字节比较⇒uchar 返回（int 声明致调用点 cmpl %eax）
    unsigned short getCharacLevelByCharacNo(int characNo) const;
    // ORIG caller Inter_ReqChangeCharacName 0x84d205b `mov %ax,-0x1a(%ebp); cmpw $0x0,-0x1a; jg`
    // ⇒ 返回 HImode（int 声明致 movzwl%ax 往返残差；CUserC8 证据/Main 代落 batch-13）
    int GetSpecialLevelCharacCnt(int type);   // ORIG 0x08656636 T（CFatigueBatteryHandle 充电链使用）
    const char* get_charac_name_by_charac_no(int characNo) const;
    bool setCharacName(int characNo, const char* name);   // ORIG W 0x084ecc7c：命中返回 true
    void SetCharacViewState(int state);
    void SetCharacViewState(int characNo, int state);
    int GetCharacViewState(int idx) const;
    bool SetCharacViewStateByCharacNo(int characNo, int state);

    // ---- GameStubs 迁移（ORIG 逐符号，强符号 T） ----
    void DisConnSig(DISCONN_SIG sig, bool flag, int param);                    // 0x086489f4
    void master_new_skill(stBuySkillInfo& info, bool flag);                    // 0x0866afa2
    bool master_new_skills(_Mastered_skill* skills,                          // 0x0866b53e
                           ENUM_SKILL_TREE_KIND kind);
    // [b28d-solo-DisPatcher_DebugCommand_cmds 代落 P248档①] int→bool：ORIG
    // 0x0858ef58 `call ChangeGrowType_GM; test %al,%al; je`（无 setne）；定义
    // CUser.cpp:6658 返回 0/1，消费点 Arad:362/369 忽略值。返回类型不入 mangled。
    bool ChangeGrowType_GM(int a, int b);                                      // 0x0867b048
    int AddItem(int itemIdx, int count, eItemAddReason reason,                 // 0x0867b6d4
                ENUM_ITEMSPACE& space, int slot);
    void ReCalcChattingEmoticon();                                             // 0x08689a22
    void ResetItemByScript(std::vector<std::pair<int, int> >& list);           // 0x08689010
    void SendChattingEmoticon();                                               // 0x08689b90
    void deleteDailyStackableItem(std::vector<std::pair<int, int> >& list,     // 0x0867e734
                                  int a, int b);
    void deleteSpecificItem(const std::vector<std::pair<int, int> >& list,     // 0x0867dba0
                            std::vector<std::pair<int, int> >& out);
    void doLinkCharacDisconnect();                                             // 0x08652c8e
    void giveup_panalty();                                                     // 0x086786be
    void log_out();                                                            // 0x08658910
    void prepareDisconnect();                                                  // 0x086487ea
    void CheckQuestMonster(int a, int b, ENUM_QUEST_ENEMY_TYPE type);          // 0x0866cb04
    int CheckQuestMap(int dungeonIdx);                                          // ORIG T 0x0866cac6
    void FatigueUp(int v);                                                     // 0x08655c60
    void add_guild_pvp_result(int v);                                          // 0x0865c936
    void add_pvp_play_info(unsigned int a, unsigned int b);                    // 0x0865d986
    void add_pvp_result(bool flag, unsigned int* out);                         // 0x0865c678
    int gainGuildSkillExp(int exp);                                           // 0x0864fb3a
    // [b28d-solo-CUser 代落 P9] bool→unsigned char：ORIG 0x0864fac4 尾
    // `movzbl (%eax),%eax` 直返字节（无 bool 归一 setne）。消费点均按条件/char 使用。
    unsigned char IsGuildSkillLearn(int skillIdx);                             // 0x0864fac4
    bool update_pvp_point(int v);                                                // 0x0865cfd8；串定 bool（b28c SIGFIX）
    void RecoverCoin(unsigned int v);                                          // 0x08657f10
    void RewardItem2DeleteInvalidItem(const std::string& name,                 // 0x086931c4
                                      const std::vector<std::pair<int, int> >& list);
    void AddDailyItem();                                                       // 0x08656caa
    void SendOneADayItemShopIndex(                                             // 0x0868aaea
        const std::vector<std::pair<int, int> >& list);
    void UpdateAuraAvatarOption(int a, int b);                                 // 0x0868de0a
    // [b28d-solo-GlobalData 代落] char→int：ORIG 两调用点（Teleport::check_rule
    // 0x08283d17 `call; mov %eax,-0xc`、Dispatchers 0x…`call; mov %eax,-0x28`）
    // 均存 raw %eax 无 movsbl ⇒ callee 返回 int（char 形 4.4 恒插 movsbl）。
    // 返回类型不入 mangled。
    int is_equip_aura_avatar(char slot, int& out);                             // 0x0868dff8
    void processDelDailyItem();                                                // 0x0867e092
    void reqHumanCertify4ClearMap(bool flag);                                  // 0x086802b8
    void ProcPremiumFatigue();                                                 // 0x0867cbe4
    bool IsPremiumUser() const;                                                // 0x0867cd20
    bool SaveInventory();                                                        // 0x0864fe52；串定 bool（b28c SIGFIX）

    // ---- OnlyOrig 批次C（35 个 ORIG-only，逐符号见 /tmp/df_batches/OnlyOrig_CUser.md）----
    unsigned short GetSlotIDX() const;                                       // 0x084ecd42 W
    unsigned char GetReceivedRequestType() const;                            // 0x082308e8 W
    bool getPcRoomAuth() const;                                             // 0x0863be54 W：ORIG 调用点 xor $0x1,%eax ⇒ bool
    bool IsState(ch_state state) const;                                      // 0x082300d0 W
    bool IsPCRoomUser() const;                                               // 0x084ecb00 W（ORIG 恒 false）
    bool IsOKChildrensDayEventCondition();                                   // 0x08328220 W
    bool IsWaitingStateCharac(unsigned int characNo);                        // 0x0822fbe6 W
    int getTotalHackUserTradeCnt() const;                                    // 0x08532078 区 W
    long long getTotalHackUserTradeGold() const;                             // 0x08532078 区 W
    void incTotalHackUserTradeCnt(unsigned int n);                           // 0x086972ee W
    void incTotalHackUserTradeGold(unsigned long long gold);                 // 0x086972b6 W
    int inc_multiboxLotteryItemFailCnt();                                    // 0x080fdf56 W
    void incPcRoomPlayTimeRewardCnt();                                       // 0x08110dd0 W
    int getLinkCharacNo(char index);                                         // 0x0869736e W
    unsigned char isCharacLinkBonusSetting(char index);   // b28 BL1 探针定夺：uchar 返回单段 movzbl=ORIG 0x084ecd52 尾全等（int 返回 4.4 恒两段；mangled 不变）
    int GetPowerWarPartyVictory();                                           // 0x0868856c T（ORIG 定义尾自带 movzwl 扩展，调用点直存）
    int getQuestPoint();                                                     // 0x0868ad52 T
    int getReturnUserLevelKey(int level, int mode);                          // 0x0869230a T
    CParty* GetUserListByParty(std::vector<CUser*>& out);                    // 0x0868056a T
    void* GetUserSecretShop();                                               // 0x086885d4 T
    bool isBlackUser(unsigned int accId);                                    // 0x0867f104 T
    void handleInformNoticeFlag(unsigned int bitIndex);                      // 0x0868f956 T
    void loadInformNoticeFlag(char* src);                                    // 0x0868f900 T
    void getInformNoticeFlagIndex(std::vector<unsigned char>& out);          // 0x0868fa8a T
    LimitItemUsage* GetItemLimitInfo(int itemIdx);                           // 0x08688d26 T
    bool IsLimitItemUsable(int itemIdx);                                     // 0x08688c70 T
    bool is_recipe_items_avatar_type(const std::vector<std::pair<int,int> >& items);
                                                                             // 0x0868c45e T
    bool isSuccessCompound(int chance);                                      // 0x08671fde T
    bool IsOldLotteryItem(unsigned int itemIdx);                             // 0x08673342 T
    bool IsAddableGuildFund(int amount);                                     // 0x0866c21e T
    bool LoadCoolTimeItemInfo(CONTINUOUS_ITEM_INFO* info);                   // 0x08658ff6 T
    bool LoadEffectItemInfo(CONTINUOUS_ITEM_INFO* info);                     // ORIG _ZN5CUser18LoadEffectItemInfoEP20CONTINUOUS_ITEM_INFO T 0x086590f0
    int GetItemSerialChecker(unsigned short serialIdx, unsigned short serialVal,
                             int monsterIdx);                                // 0x0823066a W
    int GetItemTimeChecker(unsigned int nowSec, unsigned short val1,
                           unsigned short val2, unsigned int rangeSec);      // 0x082302ac W
    int isEnableGiveGiftToNPC();                                             // 0x08681160 T
    void IncrementTradeGold(unsigned int gold, int mode);                     // 0x0864658a T
    void _CalcItemVectorMultiply(std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >&, int);
    bool _check_level_up(int, int&, int&, eExpAddReason, int);   // ORIG _ZN5CUser15_check_level_upEiRiS0_13eExpAddReasoni（全局枚举参数实证）
    bool _CheckCompoundSkill(const CStackableItem*, bool);
    static bool _fnStatQuestClear(unsigned int, char*);   // ORIG T 0x08664e8e（data=[this,rewards*,ids*]，enumQuestClear 回调）
    CItem* _GetRecipeItem(long unsigned int, bool, int&);
    bool _IsCompoundResult2MoreEquip(const std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >&);
    void _onChangeGrowType(unsigned char, unsigned char, eChangeGrowTypeReason);
    void _onCreateCharacter(unsigned int, char*, char);
    void _onQuestClear(int, int);
    bool _ProcCompoundCore(const std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >&, std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >&, std::vector<int, std::allocator<int> >&, int, bool, PacketGuard*, bool&, short unsigned int, stEquipCondition, CStackableItem*, Inven_Item&);
    void _processCharacterDayEvent(unsigned int, unsigned int);
    void _processFatigueBuffEvent(unsigned int, unsigned int);
    void _processUnlimitChallenge(unsigned int, char);
    void _saveConditionEventInfo(unsigned int, short int, short int);
    void _saveProperDungeonClearCount(unsigned int, short int);
    void _updateSaveOption();
    static void add_matched_list(std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >&, int, int);   // ORIG 0x08403a?? 栈证 list@0x8 无 this（CUOB：静态成员形，形参槽下移 4）
    void AddDailyBadge(ENUM_BADGE_TYPE);
    void AddTicket(int, bool);
    bool alonePlay_Mode();
    bool alonePlay_Mode(const MSG_REQ_TO_PEER&);
    void quest_basic_reward(Quest* q, int& a, int& b, int& c, int& d,
                            bool flag);                       // 0x0869235a T（定义本批 body_B）
        // ---- Competition / Mercenary ----
        int Competition_Mercenary_Charac(unsigned int slot, char modeA, char modeB);   // 0x0868e6c0 T（透传 CompetitionCharac 0/1）
        // ---- Reward list / compound ----
        void complete_reward_list(std::vector<std::pair<int,int> >& needList,
                                  std::vector<std::pair<int,int> >& gotList);           // 0x08664dce T
        int compound_item(int itemIdx, bool useSkillCheck, unsigned short count, int subCmd); // 0x0867218c T
        void compoundGuildMessage(Inven_Item const& item);          // 0x0869304e T
        void compoundSystemMessage(Inven_Item const& item);         // 0x08692f62 T
        // ---- Quest reward math ----
        int compute_level_penalty(int levelDelta);                  // 0x0866e3a8 T
        int compute_quest_basic_reward_exp(Quest* quest);           // 0x0866e3e2 T
        int compute_quest_basic_reward_gold(Quest* quest);          // 0x0866e5e6 T
        // ---- Item helpers ----
        void copyItemOption(Inven_Item& dst, Inven_Item& src);      // 0x08671eb2 T
        void count_specific_items(std::vector<int>& ids,
                                  std::vector<std::pair<int,int> >& out); // 0x0866514a T
        // ---- Character lifecycle ----


        void DeleteCurrentCharacRentalItemInfo(int key);            // 0x08677050 T
        // ---- Skill command ----
        int customizingSkillCommand(STSkillCommandData& data);      // 0x08654f74 T
        bool defaultSkillCommand();                                 // 0x086550ee T

        // [b28d] 返回 int→bool：ORIG 调用点 0x081bf7d1 `call; test %al,%al; je`
        // （无 setne 归一化，P292/P248 档① ⇒ callee bool；mangled 不含返回类型）。
        bool findSkillCommandData(int skillId, STSkillCommandData& out, bool flag);  // 0x08654e6a T 重载！
        // ---- EXP / SP ----

        // ---- Hack money ----
        void gainMoneyFromHackUser(unsigned int who,
                                   unsigned long long gold,
                                   unsigned int cnt,
                                   unsigned long long totalGold);   // 0x0867fbba T
        // ---- Event history / misc getters ----
        void get_ingame_event_history(int key, std::pair<int,int>& out); // 0x0868d50c T（带参版！报告 get_ingame_event_history.md 为无参 Ev 版本）
        int get_pvp_exp_of_week();                                  // 0x08230100 W
        int get_skill_count();                                      // 0x0866af86 T
        int get_SoloPlay_mode();                                    // 0x0868dd44 T
        int get_SoloPlay_msgType(MSG_REQ_TO_PEER const& msg);       // 0x0868dd76 T
        int getInDungeonItemSellCount(int invenType, int slotIdx);  // 0x0868b106 T
        unsigned char getInformNoticeFlag(int idx);                 // 0x0868fa76 T：ORIG 单 movzbl（返回 unsigned char）
        int getCharacAntibotSerialNum() const;                      // 0x0823089c W（读 m_field8ec24）
        // ---- Premium ----
        int getAddExpRateFromPremium(ENUM_PREMIUM_TYPE type);           // 0x085bfd76 W
        int getAddQuestItemDropRateFromPremium(ENUM_PREMIUM_TYPE type); // 0x0869733e W
        unsigned char GetChangedGiftFatigueQuantity() const;        // 0x086975ee W（读 m_field8ebb0）
        // ---- Childrens Day ----
        int GetChildrensDayEventTargetItemPeriod(int idx);          // 0x08328262 W
        int GetChildrensDayEventTargetItemSlot(int idx);            // 0x08328240 W
        // ---- Compound upgrade roll ----
        int getCompoundUpgrade(CStackableItem const* item,
                               stEquipCondition cond) const;        // 0x08682082 T
        // ---- Inventory ref / cache ----
        void EnableCharacInfo(CACHE_SECTION section);               // 0x081253f4 W
        void GetDailyBadge(short* out /*实为 ENUM_BADGE_TYPE 变体共用*/); // 0x8568e0c EPs（无返回值语义）；另有 0x8568e48 E15ENUM_BADGE_TYPE
        int GetETC(short key);                                      // 0x08697474 W
        // ---- Black list / trade punish ----
        bool deleteToBlackList(unsigned int id);                    // 0x0867f040 T（[b28d-Main·P321-T3] 调用点 xor $1）
        void DelTradePunishType(int mask);                          // 0x086898c2 T
        // ---- Mouse register pad ----
        void discard_mouse_register();                              // 0x08680bd2 T
        // ---- Item lock ----
        void DoItemLock(int a, int b);                              // 0x08646d4a T
        void DoItemUnlock(int a, int b);                            // 0x08646d82 T
        void DoItemUnlockCancel(int a, int b);                      // 0x08646dba T
        // ---- Link message ----
        void decideCharacLinkMessage();                             // 0x08652a58 T
        void decideLinkCharacSlotIndexes(char idx, int maxIdx);     // 0x08652b06 T
        // ---- Event delivery ----
        void eventDeliveryQuestClear(int a, int b,
                                     std::vector<std::pair<int,int> >& items,
                                     int d, int e, int f);          // 0x086645a4 T
        // ---- Hash ----
    void getHashCode(unsigned char* digestOut); void LoadMileage();
    unsigned char lost_ext(int percent, int& actualLost, bool hardMode);
    int lottery_item(int slot);
    int multibox_lottery_item(int boxSlot, int keySlot);
    unsigned int OnItemUnlockWaitTimeout(int expandIdx, int unused);
    void OnPunishUser();
    void OnRestrictTrade();
    void printBlackList();
    int process_recipe_avatar_items(int mainSlot, bool param2, unsigned short count, int cmdHeader, CStackableItem const* recipe);
    int process_use_locked_multibox(int boxSlot, int keySlot, std::vector<std::pair<Inven_Item, bool> >& dest);
    int process_use_unlocked_multibox(int boxSlot, int keySlot, std::vector<std::pair<Inven_Item, bool> >& dest);
    void processAwakenReward(char stage);
    void processFatigueCharac(int usedMin);
    void processGrowTypeEvent(unsigned char p1, unsigned char p2, eChangeGrowTypeReason reason);
    void processReturnUser();
    void quest_action(int cmd, int questId, int triggerVal, int opt);

    int quest_reward(int questId, int selectIdx, int repeatN);
    bool Redeem_Item(int redeemId);  // b28c C28L13 SIGFIX 代落：ORIG 串 "bool CUser::Redeem_Item(int)"
    // [b28d-Main·检查点14] S403 P292 档①：ORIG 调用点 0x084d0e51
    //   `call; xor $0x1,%eax; test %al,%al; je` ⇒ callee bool（int 会产 test/sete）。
    //   另两调用点 0x084d0b81/0x084d0d64 不用返回值。定义 0x0867efae 尾 mov $1/$0。
    bool registerToBlackList(unsigned int accId);
    bool reqSendMailCertify();   // [b28d-Main·P292] unsigned int→bool（ORIG 调用点 `call; xor $0x1,%eax`）
    void requestCubePremiumInfoIfContracted();
    void reset_tag_info_to_me(unsigned int tagAccId);
    void ResetMissionList();
    bool Return_Mercenary_Charac(unsigned int mercUid, competition_reward_result& reward);   // ORIG 调用点用 %al 返回值（实现 void 转发无 return，UB 复刻）
    void rewardExp(int expBase, int& bonusA, int& bonusB, int& spBefore, int& spAfter, ::eExpAddReason reason, bool triggerGrowth);   // ORIG _ZN5CUser9rewardExpEiRiS0_S0_S0_13eExpAddReasonb（全局 13 前缀，非嵌套）
    void SaveBackupSkillInfo(SIG_LOAD_PREMIUM_LETHE* src, ENUM_SKILL_TREE_KIND kind);
    void saveCharacLinkMessageFlag(bool flag);
    // [K7CUserOnlyOrig 2026-09-11 · P174 串定] ORIG 实串 `bool CUser::SaveInventoryOnly()`
    //   ⇒ bool；消费点 CUser.cpp:7478 忽略返回值（语句调用）。
    bool SaveInventoryOnly();
    unsigned int SaveMercenaryData();
    void SaveMileage();
    	                             // select_charac_set_visible_values 已在头(2397)跳过
    unsigned int SellItemTimeChecker(unsigned int nowSec, unsigned int interval);
    void send_broadcast_lottery_gold_message(unsigned long itemIdx, int count);
    void send_charac_visible_data();
    void Send_SoloPlay_Reject_NotiPack(const char* msg, ENUM_SOLO_PLAY_REJECT_MODE mode,
                                       ENUM_SOLO_PLAY_REJECT_MSG code);
    void SendCeraShopGiftStatisticLog(unsigned int arg1, unsigned int arg2, unsigned long val64, unsigned int arg4);
    bool SendCoolTimeItemInfo(unsigned int nowMs);   // b28d-Main(P292)：ORIG 0x086590da 同形 ⇒ bool
        // ---- Send 族（ORIG 地址已核 nm）----
    void SendEventBringUpInfo(); void SendHasNoGoWithApcQuestInParty(Quest const* quest);
                                                           // ORIG T 0x0866d972 _ZN5CUser30SendHasNoGoWithApcQuestInPartyEPK5Quest
        void SendItemMakingSkillInfo();                    // ORIG T 0x0868b00a
        void SendLimitItemUsageNotiInfo(int a, int b, int c);   // ORIG T 0x08688f02
        void SendMail(bool certified);                     // ORIG T 0x08689f12（bool 重载；与既有 SendMail(MSG_MAILBOX_SEND*,unsigned) 并存）
        void SendMultiMail(bool certified);                // ORIG T 0x0868a2dc
        void SendSecretShopItemList();                     // ORIG T 0x0868860a
        void SendUnlimitChallengePacket();                 // ORIG T 0x08686f58
        // ---- set/Set 族 ----
    void set_charac_guildkey_in_login(unsigned int guildKey, unsigned int characNo);
                                                           // ORIG T 0x0866c04e
        void set_fatigue_count_db(int type, int value);    // ORIG T 0x0868d4a2
        // set_growth_power_exp_reward(int) / reset_ingame_event_history 等已存在。
    void set_ingame_event_history(int eventType, int v1, int v2); void set_ingame_event_history(SIG_INGAME_EVENT_HISTORY* sig); void set_MailMsgData(MSG_MAILBOX_SEND* msg); void set_MultimailMsgData(MSG_MULTI_MAILBOX_SEND* msg); void set_skill_slot(char* data1, char* data2, int size); void SetAccountCargoDBData(Inven_Item* items, unsigned int money, unsigned int capacity);
                                                           // ORIG T 0x0867d86e
        void setAdvanceAltarIndex(short idx);              // ORIG W 0x0813490e
        void SetAge(unsigned int age);                     // ORIG W 0x082300be
        void SetBingoData(SIG_LOGIN_DATA& sig);            // ORIG T 0x0869126e
        void SetBirth(char const* birth);                  // ORIG T 0x0867ecf0
        void setBreakAwayRewardData(bool flag);            // ORIG W 0x0831d50c
        // SetCharacInfo(int,char*) 已存在（0x2393 行）。
    bool setCharacInfoDetail(int slot, unsigned int characNo, CHARAC_DATA* src);
                                                           // ORIG T 0x0864ac1a _ZN5CUser19setCharacInfoDetailEijP11CHARAC_DATA
    void setCharacLevelUpGift_AccountOnce(SIG_LEVELUP_GIFT_ACCOUNT_ONCE& sig);
                                                           // ORIG T 0x08691dca
    void setCharacLinkBonusSetting(int slot, bool flag); void setCharacLinkMessageFlag(bool flag); void SetCharacMercenaryInfo(int count, CHARAC_LOAD_MERCENARY* infos);
                                                           // ORIG T 0x0868e544
    void setCharacSlotCountInfo(unsigned char effectCnt, unsigned char limit);
                                                           // ORIG T 0x0868b480
    void setCharacSlotLimit(unsigned char limit); void SetChildrensDayEventValue(int slot, int v1, int v2); void SetCurCharacUsedGiftFatigueQuantity(unsigned int qty); void SetEquipment(int slot, void* invenData, int size, void* outData);
                                                           // ORIG T 0x0865da4e
    void SetEventCreateDnfReward(bool flag); void SetGrowthCreatureEventInfo(const SIG_GIVE_GROWTH_CREATURE_EVENT& sig);
                                                           // ORIG T 0x08691cde
    void SetGrowthWeaponEventInfo(const SIG_GROWTH_WEAPON_EVENT& sig);
                                                           // ORIG T 0x08691bae
    void SetGuildInfoProxy(STGuildProxy& proxy); void setHackUser(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned short type, unsigned int param6);
                                                           // ORIG T 0x0867f748
        bool SetInventory(SIG_LOAD_INVENTORY* sig);        // ORIG T 0x0864d160（[b28d-Main·P321-T3] 调用点 xor $1 ⇒ bool）
        // SendDungeonClearList/SendEffectItemInfo/sendInformNoticeFlag/
        // sendReturnUserFirstLogin/SendRevengeDungeon/sendSecuRewardItem/
        // SendTagCharacInfo/set_acc_info/GetParty 已存在于头。
        // ---- 全局自由函数（与 ORIG 同名同址：_Z18fnEnumDungeonClearscPc T 0x08678166）----
    bool fnEnumDungeonClear(short dungeonIdx, char diff, char* userData);
    public:
        // ---- setters / 记录 ----
        void SetLimitInfo(SIG_LIMIT_ITEM_INFO* info);                       // _ZN5CUser12SetLimitInfoEP19SIG_LIMIT_ITEM_INFO @0x086888f8
        void SetMissionParameter(int kind, int value);                      // @0x08662558
        void setPcRoomPlayTimeData(unsigned int startTime, unsigned int total, unsigned char rewardCnt);  // @0x0868f808
        void SetPuUser(bool flag);                                          // @0x08692b58
        // [b28d-solo-Inter_LoadEtc_impl 代落 P292/P248档①] int→bool：ORIG 调用点
        // Inter_LoadEtc::SetPvpRecord 0x084c2983 `call; xor $0x1,%eax; test %al,%al; je`
        // （bool callee 唯一形）；定义 CUserOnlyOrig.cpp:6762 恒返回 1。
        // 返回类型不入 mangled（_ZN5CUser12SetPvpRecordER13PvpResultType 不变）。
        bool SetPvpRecord(PvpResultType& record);                           // @0x0864deea(ORIG 恒返 1)
        void setQuestPoint(int qp);                                         // @0x0868acfe
        void SetReceivedRequestType(char type);                             // @0x082308cc W
        void setRecipeEqiupmentOption(Inven_Item& outItem, stEquipCondition cond, int matUpgrade,
                                      const CStackableItem* stackable, int invenSpace);   // @0x0867306e
        void SetServerGroup(ENUM_SERVER_GROUP group);                       // @0x082300ac W
        bool SetSkill(SIG_LOAD_SKILL* skill, SIG_COMBO_SKILL* combo);       // @0x0864db20(Ghidra undefined4→bool,setne 无规范化风险,调用侧 test %al)
        void setSkillCommandData(char* data);                               // @0x08654924

        // P292/T3 返回 bool：ORIG 消费点 0x084c360b `call; xor $0x1,%eax; test %al,%al; je`
        // （无 setcc）；mangled 不含非模板返回类型（定义侧 CUserOnlyOrig.cpp:7267 同步）。
        bool setTagCharacInfo(int idx, unsigned int characNo, TAG_CHARAC_DATA* data);   // @0x08690098
        // [K7CUserOnlyOrig 2026-09-11 · P174 串定] ORIG 实串 `bool CUser::setTagCharacInven(TAG_CHARAC_INVEN_DATA*)`
        //   ⇒ bool；消费点 InterDispatcher.cpp:2177 用 `!` 取反（bool 语义），:18448 忽略。
        bool setTagCharacInven(TAG_CHARAC_INVEN_DATA* data);                // @0x08690226
        void SetTradePunishOverThirtyDays(bool flag);                       // @0x08689850
        void setUsedBreakAwayLuckPoint(bool flag);                          // @0x086971ee W(isUsedBreakAwayLuckPoint 头已有)
        // ---- 嵌套类 st_monstorkill（ORIG CUser::st_monstorkill::Check @0x0823049c
        //      W size 0x1cd；更新方向按反汇编：+0 旧值>新值才写（保 min）、
        //      +2 旧值<新值才写（保 max），超窗重置时 min/max 同置）----
        class st_monstorkill
        {
        public:
            st_monstorkill() {}
            bool Check(unsigned int nowTick, unsigned short nx, unsigned short ny,
                       unsigned short xLim, unsigned short yLim,
                       unsigned int windowLimit);   // ORIG W 0x0823049c size 0x1cd

        private:
            unsigned short m_minX;      // +0x00 检测窗 minX
            unsigned short m_maxX;      // +0x02 检测窗 maxX
            unsigned short m_minY;      // +0x04 检测窗 minY
            unsigned short m_maxY;      // +0x06 检测窗 maxY
            unsigned int   m_lastTick;  // +0x08 上次检查 tick
            int            m_count;     // +0x0c 窗口内检查次数
            short          m_deltaX;    // +0x10 = abs((short)(maxX-minX))
            short          m_deltaY;    // +0x12 = abs((short)(maxY-minY))
        };  // sizeof 0x14（W8_CU_E TU 内 static_assert 锁定）



    // ===================== Wave6-CUser（OnlyOrig 269 批）新增声明 =====================
    // 签名与 ORIG mangled 一致；定义落 CUserOnlyOrig.cpp（本批新 TU）。
    void apply_elixir_item(_Additioal_info& ai, std::pair<int,int>& pr);   // ORIG T 0x08665274
    void calc_quest_stat(_Additioal_info& ai);                             // ORIG T 0x08664c50
    bool CalLevelUpItemCheck(unsigned long itemId);                        // ORIG T 0x08689d06
    bool CalLevelUpItemState(char& state, int& needExp, int fromLevel, int toLevel); // ORIG T 0x08689d74
    int ChangeGrowType_Item(int newFirst, short slotNo);                   // ORIG T 0x0867b196
    int check_insert_lottery_reward(const CItem* opener,
                                    std::vector<std::pair<Inven_Item,bool> >& rewards,
                                    std::vector<std::pair<Inven_Item,bool> >& leftover);   // ORIG T 0x08676296
    int check_locked_multibox_error(const CItem* opener, int openedCnt, bool doubleMode);  // ORIG T 0x08675fd2
    int check_multibox_error(int requiredEmptySlots);                      // ORIG T 0x08675edc
    int check_unlocked_multibox_error(const CItem* stack, int addInfo);    // ORIG T 0x086761e0
    bool checkCharacLinkType(char linkPos, int slot);                      // ORIG T 0x08652b90
    bool checkCodeChecksum(const CodeHackChecksum& ck, char* dumpOut);     // ORIG T 0x0867ebba
    bool checkCodeChecksumRange(const CodeHackChecksumEx& ckEx, const CodeHackCheckRangeEx& rangeEx); // ORIG T 0x0867ea5c
    bool CheckDeleteCharacTime(int slot);                                  // ORIG T 0x0864a830
    int CheckDungeonClear(int questId);                                    // ORIG T 0x0866d934 尾 call item_reward_quest;leave;ret 直返 int（item_reward_quest 返 m_quest[i] 全 int，非 bool）
    unsigned int CheckGuildLevelUp(unsigned int& priceOut);                // ORIG T 0x0866c27e
    bool checkHumanCertify(int len, unsigned short* answer);               // ORIG T 0x0867f4c8
    bool checkInformNoticeFlag(unsigned int bitIdx, unsigned int byteIdx); // ORIG T 0x0868f9e8
    bool CheckItemLockSlotSpace(int spaceType, int spaceIdx) const;        // ORIG W 0x086469f0
    bool CheckPossibleRequestRenameState(int slot) const;                  // ORIG W 0x0822ff62
    bool CheckTicket(int ticketIdx);                                       // ORIG T 0x086785f4
    void ClearChildrensDayEvent();                                         // ORIG W 0x0832815a
    void ClearMission(short missionId);                                    // ORIG T 0x08662606
    void ClearMissionKind(short kind);                                     // ORIG T 0x086626a0
    bool ClearSkills();                                                    // ORIG T 0x0867b4c4
    void collectPcRoomPlayTime();                                          // ORIG T 0x0868f7ba
    void ResetDailyMission();                                              // ORIG T 0x0866263c
    void setBreakAwayRewardOrder(ENUM_BREAK_AWAY_REWARD_ORDER order);      // ORIG W 0x0831d4de
    void StartRecvEventWaiting();                                          // ORIG T 0x0867f1fe
    void Update_ComboMission(int comboIdx, short count);                   // ORIG T 0x0866280a
    void update_ingame_event_history(int eventKey);                        // ORIG T 0x0868d72c
    int update_record(GameResultType& result);                             // ORIG T 0x08676876
    void initBingoData();                                                  // ORIG T 0x086912be
    void updateBingoData();                                                // ORIG T 0x086914cc
    void UpdateBloodRecord();                                              // ORIG T 0x08651de6
    void updateBlueMarbleEnterCount();                                     // ORIG T 0x0869106c
    void UpdateBreakAwayDungeonClearCnt();                                 // ORIG T 0x086522ea
    void UpdateBreakAwayLuckPoint();                                       // ORIG T 0x086521ec
    void UpdateCharacLinkBonus(unsigned int exp, unsigned int gold, unsigned int state,
                               ENUM_CHARAC_LINK_BONUS_STATE bonusState);   // ORIG T 0x0865240c
    void UpdateCharacNameInMercenary(unsigned int characNo, const std::string& name); // ORIG W 0x084ec5e6
    void updateComboSkill();                                               // ORIG T 0x08692396
    void initTPSkill();                                                    // ORIG T 0x0868fdbc _ZN5CUser11initTPSkillEv
    void initAllSkill();                                                   // ORIG T 0x0868fc46 _ZN5CUser12initAllSkillEv
    void UpdateCurrentCharacRentalInfo(const InstanceRentalSystem::RentOutput& output,
                                       int machineKey, bool clearFirst);   // ORIG T 0x08676e3c
    void UpdateDimensionInout();                                           // ORIG T 0x08651fba
    // ORIG T 0x0868aa54：getCurEquipslotFlag 后直 ret 无 movzbl 规范化（P9）⇒ 返回 unsigned
    // char；0x13/0x2 分支值域安全，唯一调用点 int=... 提升不判别负值（Dispatchers 20315）。
    unsigned char UpdateEquipslotFlag(char flag);                          // ORIG T 0x0868aa54
    void UpdateMissionCondition(std::vector<short>& kinds, std::vector<int>& values); // ORIG T 0x086625c8
    void updatePcRoomPlayTime();                                           // ORIG T 0x0868f726
    void use_equipment(int slot);                                          // ORIG T 0x0865ede0
    void use_multibox_statistic(unsigned int itemIdx);                     // ORIG T 0x086767d2
    bool use_stackable(short slotNo, int space, int param3, int param4);   // ORIG T 0x0865e0ae
    unsigned int UseCeraPoint(unsigned int amount);                        // ORIG T 0x08692b16
    unsigned char UseLimitItem(int itemIdx, bool sendNoti);                // ORIG T 0x08688ae6
    void UseMileage(int cost);                                             // ORIG T 0x08650a10
    void UseQuestItem(int itemIdx);                                        // ORIG T 0x08668d1a
    void ValidateStrikerInfo();                                            // ORIG T 0x08690be6
    bool VerifyCleanPadVeriData(unsigned int value);                       // ORIG T 0x08649b44
    void verifyDoingQuest();                                               // ORIG T 0x0868d144

    // ---- W7-BigThree（increase_status / init_skill_tree / InitOnAccept 专项）----
    void init_skill_tree(ENUM_SKILL_TREE_KIND kind);                      // ORIG T 0x0866bce8 _ZN5CUser15init_skill_treeE20ENUM_SKILL_TREE_KIND
    void increase_status(short slotNo);                                   // ORIG T 0x086657fc _ZN5CUser15increase_statusEs
};

#endif  // GAME_CUSER_H_
