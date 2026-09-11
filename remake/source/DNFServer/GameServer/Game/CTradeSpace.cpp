// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CTradeSpace/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// ============================================================================
// df_game_r 还原 —— CTradeSpace（交易空间，G2-4 批次）
// 参照 docs/class_func_reports/CTradeSpace.md + ORIG 弱符号反汇编。
// 布局（ORIG CTradeSpace::CTradeSpace 0x082a416c / SetIDX 0x082a41f4 推导）：
//   +0x00 Mutex（0x18）          +0x18 int m_index
//   +0x1c char m_bTrading        +0x1d char m_bLocked
//   +0x20 CUser* m_pTraders[2]   +0x28 int m_nState[2]
//   +0x30 TradeSlot m_slots[2][27]（每槽 0x48，每行 0x798）
//   +0xf60 int m_nMoney[2]        +0xf68 int m_fieldF68[2]（生物转移计数）
//   （总尺寸 0xf70）
// 本 TU 只做 CTradeSpace 自身（含 TradeSlot 构造/Reset），跨类调用以真实符号
// 声明（头文件已有者直接使用；缺失者用 asm-label extern）。
// ============================================================================

#include "CUser.h"
#include "CInventory.h"
#include "CDataManager.h"
#include "InterfacePacketBuf.h"
// 跨类符号：CUser/cUserHistoryLog(CUser.h)、CSecu_ProtectionField(CInventory.h)、
// Mutex(GameTypes.h)、CMailBoxHelper/G_CGameManager(CGameManager.h)、
// DB_AvatarChangeOwner(DB_AvatarChangeOwner.h)。
#include "CGameManager.h"
#include "DB_AvatarChangeOwner.h"
#include "CTradeSpace.h"   // 类/布局唯一声明点（CGameManager.h 亦 include）
#include "RDARScriptStringManager.h"   // RDARScriptStringManager/g_scriptStringManager_（CleanFAsm 新建）
#include "HistoryLog.h"            // HistoryLog::WriteTradeUp/Down（proceed_trade）
#include "CAuctionAveragePrice.h"  // CAuctionAveragePrice（proceed_trade 均价）
#include "CStreamGuard.h"          // CStreamGuard（零金币日志流）
#include "StreamPool.h"            // StreamPool::Acquire
#include "MsgQueueMgr.h"           // MsgQueueMgr::put
#include "GlobalData.h"            // s_stream_pool / s_msgq_mgr
#include "W6_HackPunishDeps.h"     // CAutoPunishRuleHackTypeMgr（checkTotalTradeRuleOver）
#include "SigTypes_E.h"            // SIG_SAVE_GOLD_ZERO_TRADE_LOG
#include "GameWorld.h"             // G_GameWorld/send_all（remove_item 广播）

extern Inven_Item g_emptySlot;   // ORIG 全局 0x943ddc0（Inven_Item.cpp 定义）

// DB_InsertCeraItemHistory（定义 DB_U3.cpp；本 TU 仅调用 makeRequest）
class DB_InsertCeraItemHistory
{
public:
    static int makeRequest(int itemIdx, unsigned int senderCharacNo,
                           unsigned int receiverCharacNo, char flag);
};

// ---- 交易状态枚举（ORIG ENUM_TRADESTATE；mangling 编码类型名 15ENUM_TRADESTATE）----
enum ENUM_TRADESTATE
{
    ENUM_TRADESTATE_0 = 0,  // 解除锁定
    ENUM_TRADESTATE_1 = 1,  // 锁定
    ENUM_TRADESTATE_2 = 2,
    ENUM_TRADESTATE_3 = 3,
    ENUM_TRADESTATE_4 = 4
};

#include "CTradeSpace/set_traders.inc"


#include "CTradeSpace/SendChangeTradeState.inc"


#include "CTradeSpace/get_rand_int.inc"



#include "CTradeSpace/TradeSlot.inc"


#include "CTradeSpace/Reset.inc"



#include "CTradeSpace/CTradeSpace.inc"

#include "CTradeSpace/dtor_CTradeSpace.inc"


// ---- 槽位辅助（P49：-O0 下 static inline 也产真实符号 → 宏）----
// 基于真实二维数组成员索引（m_slotsData[2][27]，行步长 0x798、槽步长 0x48）。
#define TradeSlotAt(slots, trader, idx) (&((slots)[trader][idx]))

#include "CTradeSpace/SetIDX.inc"


#include "CTradeSpace/lock.inc"


#include "CTradeSpace/unlock.inc"


#include "CTradeSpace/IsLocked.inc"


#include "CTradeSpace/_SetLock.inc"


#include "CTradeSpace/is_empty.inc"



#include "CTradeSpace/get_index.inc"


#include "CTradeSpace/reset.inc"


#include "CTradeSpace/cancel_trade_by_dis.inc"


#include "CTradeSpace/GetOppositeUser.inc"


#include "CTradeSpace/_CheckTrade4SameAvatar.inc"


#include "CTradeSpace/check_item_exist.inc"


#include "CTradeSpace/get_empty_itemslot.inc"

#include "CTradeSpace/check_trade_possibility.inc"


#include "CTradeSpace/_IsTradable.inc"


#include "CTradeSpace/_SaveAvatarItemOwnerChange.inc"


#include "CTradeSpace/add_item.inc"


#include "CTradeSpace/cancel_trade.inc"


#include "CTradeSpace/send_lose_item_for_china.inc"


// ============================================================================
// G4 交易域补齐：checkTrade / checkCancelTrade / change_trade_state /
// cancel_trade_for_china / proceed_trade / regist_item / remove_item
// （真实语义；跨类调用以 ORIG 符号为准；槽位访问统一走文件顶部 TradeSlotAt）
// ============================================================================

#include "CTradeSpace/checkTrade.inc"


#include "CTradeSpace/checkCancelTrade.inc"


#include "CTradeSpace/cancel_trade_for_china.inc"


#include "CTradeSpace/change_trade_state.inc"


#include "CTradeSpace/proceed_trade.inc"


#include "CTradeSpace/regist_item.inc"

#include "CTradeSpace/remove_item.inc"

