// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CHandlePremium/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r —— WongWork::CHandlePremium 实现。
// 依据 docs/class_func_reports/WongWork__CHandlePremium/{handleSetUserPremium,
// handleNotifyPremiumInfo,handleSetUserPCRoom,handlePremiumEffect}.md + ORIG 反汇编。
// 权威声明见 CHandlePremium.h。
// ============================================================================

#include "CHandlePremium.h"

#include <ctime>

#include "CGameManager.h"   // CMailBoxHelper（ReqDBSendNewMail）+ ENUM_DUNGEON_MODE 守卫
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "GlobalData.h"
#include "TimerQueue.h"
#include "SPremiumInfo.h"
#include "CPremiumLetheManager.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"

namespace WongWork
{

// ---- handleSetUserPremium —— ORIG 0x086af7e8 (T) ----
// type==5 恒 AddPremium；start==0 或 end==0 则 RemovePremium；否则仅在
// s_systemTime_.getCurSec()<end 时 AddPremium。recalc 时 ReCalcAdvantage+RecalcAdditionalInfo。
#include "CHandlePremium/handleSetUserPremium.inc"
}

// ---- handleNotifyPremiumInfo —— ORIG 0x086af8b4 (T) ----
// 组 0x42 高级会员通知包：header(0,0x42)/short(2)/byte(type)/int(remainSec)。
#include "CHandlePremium/handleNotifyPremiumInfo.inc"
}

// ---- handleSetUserPCRoom —— ORIG 0x086b02fe (T) ----
// PC 房高级会员设置入口：isAffectedPremium+handleSetUserPremium 设置 + TimerQueue
// 定时 + handleNotifyPremiumInfo/handlePremiumEffect + 租赁物品包。
#include "CHandlePremium/handleSetUserPCRoom.inc"
}

// ---- handlePremiumEffect —— ORIG 0x086afc32 (T) ----
// 逐 premium 项处理：到期前定时、到期后发优势物品/金币、每日检查定时、
// 收尾 RecalcAdditionalInfo/SendNotiPacket + ProcPremiumFatigue/SendFatigue/SendCoin。
#include "CHandlePremium/handlePremiumEffect.inc"
}


// ---- handlePremiumTimeout —— ORIG T 0x086afa7c (handlePremiumTimeout.md) ----
// CheckPremiumTimeout 返回 0xc（PC 房类）→ setPremiumState(0xc,3)+通知；
// 其它非 0 → handleSetUserPremium(type,0,0,0,1) + RemoveRestrictedGoods +
// 角色在线时 ProcPremiumFatigue/SendFatigue + g_SPremiumInfo[type] 分支
// （kind==1 且 field64>0 → SetNeedCheckOverEquipItem+SendNotiPacket 组；
//  0x53/0x57 → 表情重算；0x21 → Lethe 标志）+ 通知。
#include "CHandlePremium/handlePremiumTimeout.inc"
}

}  // namespace WongWork