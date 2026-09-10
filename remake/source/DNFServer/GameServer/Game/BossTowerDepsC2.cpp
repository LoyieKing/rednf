// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BossTowerDepsC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// BossTowerDepsC2.cpp —— TowerOfDespair / pvp_assault 强制移动域依赖恢复
// （BossTowerDepsC2 批次，2026-08-24；独占文件，Main 统一登记 CMake）
//
// 逐份完整读取报告（## 汇编 + ## 反编译 C 全文）后转写：
//   docs/class_func_reports/TowerOfDespairMgr/CheckEnterable.md
//       ORIG T 0x086440ee _ZN17TowerOfDespairMgr14CheckEnterableEP5CUserPK8CDungeon
//   docs/class_func_reports/TOD_UserState/getEnterCount.md
//       ORIG T 0x08643872 _ZNK13TOD_UserState13getEnterCountEv
//   docs/class_func_reports/TOD_UserState/getLastClearTime.md
//       ORIG T 0x0864387e _ZNK13TOD_UserState16getLastClearTimeEv
//   docs/class_func_reports/pvp_assault__CAssaultMgr/ForcedMoveAtPowerWarPlace.md
//       ORIG T 0x082ed672 _ZN11pvp_assault11CAssaultMgr25ForcedMoveAtPowerWarPlaceEP5CUser
//   docs/class_func_reports/pvp_assault__CAssaultPlace/ForcedMoveAtPowerWarPlace.md
//       ORIG T 0x082ed6a6 _ZN11pvp_assault13CAssaultPlace25ForcedMoveAtPowerWarPlaceEP5CUser
//   docs/class_func_reports/pvp_assault__CForcedMoveCharacter/{README,ForcedMove}.md
//       ORIG T 0x082e567c _ZN11pvp_assault20CForcedMoveCharacter10ForcedMoveEv
//   （TowerOfDespairMgr::SendAPCInfo ORIG T 0x08644338 已由 TowerOfDespairMgr.cpp
//     按 SendAPCInfo.md 恢复——本 TU 不重复定义。）
//
// 调用方（此前 blocked）：
//   CPartyDepsA2.cpp GetDungeonIndex_isTOD —— tod->CheckEnterable(manager, target)
//   CParty.cpp:2736 walkout_member         —— mgr->ForcedMoveAtPowerWarPlace(target)
//   DisPatcherCore.cpp DisPatcher_LeaveParty::process（原 BLOCKED 注释即本链）
// ============================================================================

#include "CDataManager.h"            // DataManagerInitDepends.h 前置要求
#include "DataManagerInitDepends.h"  // TowerOfDespairMgr/TOD_Layer/TOD_UserState 权威布局

#include "CAssaultMgr.h"             // pvp_assault::CAssaultMgr 声明
#include "CGameManager.h"            // pvp_assault::CAssaultPlace 完整类型
#include "CUser.h"                   // CUser 方法集（含 CUserCharacInfo 继承面）
#include "CDungeon.h"                // CDungeon::m_entryRequiredItem（RequiredItem）
#include "CInventory.h"              // check_item_exist/GetInvenSlot/Inven_Item
#include "CParty.h"                  // CParty::get_member_count
#include "CPowerManager.h"           // CPowerManager::IsPowerWarEventOn
#include "GlobalData.h"              // s_systemTime_/s_power_manager
#include "CEnvironment.h"            // G_CEnvironment()->m_serverEnvir.m_fatigueTime
#include "GameWorld.h"               // move_area/send_to_area/G_GameWorld
#include "PacketGuard.h"

// ORIG _Z22CheckDailyScheduleTimeill T 0x0846c0a8；定义 GameUtils.cpp:88。
// 与 CBoosterGage.cpp:122 / CGameManager.cpp:114 同款局部声明口径（无权威头）。
#include "BossTowerDepsC2/CheckDailyScheduleTime.inc"

namespace pvp_assault
{

// ============================================================================
// pvp_assault::CForcedMoveCharacter —— 强制移动请求值对象（仅本 TU 使用，
// 类定义置于 .cpp 内避免跨 TU ODR 风险；后续 CExtraMatrix/CDynamicDisposition/
// CStaticDisposition::MoveUser(CForcedMoveCharacter*) 批次可迁至权威头）。
//
// 布局依据 ForcedMove.md 汇编读取偏移（this+0x08..+0x1c）；ORIG 唯一构造点
// （CAssaultPlace::ForcedMoveAtPowerWarPlace 栈上组装）不调用任何构造函数、
// 无 vtable 写入证据，故 +0x00..+0x07 未建模。
// ============================================================================
// pvp_assault::CForcedMoveCharacter 权威完整定义见 CAssaultMgr.h（本 TU 本地镜像已删，
// 避免与全域权威头 ODR 冲突）。以下 ForcedMove ORIG T 0x082e567c 报告全文转写。
// [b28d-L8] N82 | 块序 + 槽平移（ORIG 0x08xxxxxx）
#include "BossTowerDepsC2/ForcedMove.inc"
}

// ============================================================================
// CAssaultPlace::ForcedMoveAtPowerWarPlace —— ORIG T 0x082ed6a6（静态成员，
// asm 无 this 槽）。报告全文转写：IsPowerWarEventOn() 且村庄==7 才处理；
// getPowerSide 两次独立调用（ORIG 指令流如此）；非 1/2 时其余字段不写
// （保持未初始化栈值，与 ORIG 一致）。
// ============================================================================
// [b28d-L8][W6] P226：ORIG 短路 bool 无槽物化（%eax 直传 + test %al,%al; je 尾）
// ⇒ 用语句表达式包住整个 && 条件（内不含具名变量），消掉 int enter 落槽
// （旧形帧 0x48 vs ORIG 0x38）。ORIG 0x082ed6a6。
#include "BossTowerDepsC2/ForcedMoveAtPowerWarPlace.inc"
}

// ============================================================================
// CAssaultMgr::ForcedMoveAtPowerWarPlace —— ORIG T 0x082ed672（报告全文转写）：
//   try { CAassaultPlace::ForcedMoveAtPowerWarPlace(user); } catch(...) {}——
//   landing pad 取异常指针入局部后 __cxa_end_catch 吞掉。
// [b24F] DONE：catch 形参 void*→int（ORIG 解引用 (%eax),%eax = 值类型捕获）。
// ============================================================================
#include "BossTowerDepsC2/ForcedMoveAtPowerWarPlace_1.inc"
}

}  // namespace pvp_assault

// ============================================================================
// TOD_UserState::{getEnterCount,getLastClearTime} —— ORIG T 0x08643872/0x0864387e
// （两报告均为 2 条指令字段读；声明/字段见 DataManagerInitDepends.h）。
// ============================================================================
#include "BossTowerDepsC2/getEnterCount.inc"
}

#include "BossTowerDepsC2/getLastClearTime.inc"
}

// ============================================================================
// TowerOfDespairMgr::CheckEnterable —— ORIG T 0x086440ee（报告全文转写）：
// [b23S3C] N：帧重排+跨 call 寄存器缓存（N82/N18a 族），语义等价已核（详见 b22_s3_verdicts.tsv）
//   user 空 → 0x15；level < m_minLevel(+0x34e) → 0xe；
//   TOD_UserState(getCharacExpandData 0xd).getEnterCount()>0 时：
//     非 GM 且 !CheckDailyScheduleTime(env.m_fatigueTime(+0x37c),
//     lastClearTime, curSec) → 0xf5；
//     check_item_exist(requiredItem1.m_0)==-1 → 0xf5；
//     GetInvenSlot(1,slot).get_add_info()(+0x07) < requiredItem1.m_4 → 0x11；
//   party 空且未走上方分支 → 0x15；member_count>1 → 0xab；否则 0。
//   （env+0x37c = Server_Envir::m_fatigueTime，CEnvironment.h 权威布局。）
// ============================================================================
// [b28d-L8] N82 | 帧差 + 参数/字段装载序（ORIG 0x08xxxxxx）
#include "BossTowerDepsC2/CheckEnterable.inc"
}
