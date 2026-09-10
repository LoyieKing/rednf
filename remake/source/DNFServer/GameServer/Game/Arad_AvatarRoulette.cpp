// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_AvatarRoulette/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// Arad_AvatarRoulette — 原始工程 localjapan/Arad_AvatarRoulette.cpp 的复刻 TU。
// 收录 AvatarCoin 的「使用侧」方法（ORIG 参照 docs/class_func_reports/AvatarCoin/）：
//   Use（前批次）/ SaveToDB / SendSyncPacket / GetPackage / HistoryLog::AddLog
//   以及 CUser::_GetSaveInvenBase（SaveToDB 的内联依赖，ORIG 独立 T 符号）。
// 另含 AvatarRouletteServer::isAvatarPottery（0x817fb0a）。
// 注意：Use 调用 IsUsable 必须跨 TU（PLT 调用 → ORIG 帧 sub $0x18），
// 与 AvatarCoin.cpp 保持分 TU。

#include "AvatarCoin.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"               // m_avatarCoin 真实成员访问（_GetSaveInvenBase）
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "LogManager.h"
#include "PacketGuard.h"
#include "CDataManager.h"             // DataManagerInitDepends.h 前置依赖
#include "DataManagerInitDepends.h"   // AvatarRoulette 权威布局（成员声明）
#include <cstring>

// isAvatarPottery 为 AvatarRouletteServer 成员（ORIG T 0x0817fb0a，定义
// CtorProducers_1.cpp；本 TU 经 DataManagerInitDepends.h 类声明调用）。

// ---- AvatarRoulette::getAvatarList ORIG T 0x08911db0 ----
// 汇编形状（range 0x4d）：*isOk = 0；job<0 或 job>10 直接返回；
// 否则 out = m_avatars[job]（vector::operator=，PLT call）后 *isOk = 1。
// 地址计算 = this + job*0xc + 0x14（P30 两步寻址：*3 shl$2 后 +0x10 再 +0x4）。
#include "Arad_AvatarRoulette/getAvatarList.inc"
}

// ---- AvatarRoulette::getStikerList ORIG T 0x08911dfe ----
// 同 getAvatarList 结构；out = m_stikers[job]（+0x90 再 +0x8 → +0x98）。
#include "Arad_AvatarRoulette/getStikerList.inc"
}

#include "Arad_AvatarRoulette/Use.inc"
}

// ---- AvatarCoin 对象位于角色背包基址 +0x658（ORIG 全局约定） ----
// [b28c-lane17] 删除 coinOf static inline 助手（P49：-O0 下仍产真实 call 符号；
// ORIG 各调用点均为内联 `getCurCharacInvenW(); add $0x658` 直落 receiver）。

// ---- CUser::_GetSaveInvenBase ORIG @ 0x0864fd80 ----
// [b28d-solo-Arad_AvatarRoulette AE] 61=61 逐条对齐，p167 11/11 CLEAR。
// P211 档③字段侧同型化：ORIG 0x0864fda1 段为 LHS-first 直存
// `mov 0xc(%ebp),%edx; mov %eax,0x4(%edx)`；SIG_SAVE_INVENTORY::m_characNo 为 int 时
// 与 getCurCharacNo()(unsigned int) 异型 ⇒ 4.4 恒产 RHS-first+`mov %eax,%edx` 中转。
// 字段改 unsigned int（SigTypes_E.h:104，宽度/偏移/sizeof 0xad99 不变）后逐条全等；
// 源侧 static_cast<int> 形实测无效（P211 明证，勿再试）。
// 其余 store（+0x00/+0x09/+0x0d/+0x11/+0x15）本就与各自 callee 同型直存。
#include "Arad_AvatarRoulette/_GetSaveInvenBase.inc"
}

// ---- AvatarCoin::SaveToDB ORIG @ 0x081800d6 ----
#include "Arad_AvatarRoulette/SaveToDB.inc"
}

// ---- AvatarCoin::SendSyncPacket ORIG @ 0x0817ffe4 ----
// bool 返回：ORIG 调用点 0x0819f1cd `xor $0x1,%eax` 判据（Dispatchers 多处）。
#include "Arad_AvatarRoulette/SendSyncPacket.inc"
}

// ---- AvatarCoin::HistoryLog::AddLog ORIG @ 0x817ff9c ----
#include "Arad_AvatarRoulette/AddLog.inc"
}

// ---- AvatarCoin::GetPackage ORIG @ 0x0818028a ----
#include "Arad_AvatarRoulette/GetPackage.inc"
}

// ---- AvatarCoin::BuyProcess ORIG @ 0x081801d0 ----
// mangled _ZN10AvatarCoin10BuyProcessEP5CUserR10Inven_Item
// 陶罐（avatar pottery）商品购买：isAvatarPottery(item.m_addInfo) 为假直接
// 返 0；否则取 m_addInfo2（+0x07）为币值，加到背包基址 +0x658 的 AvatarCoin，
// SaveToDB 失败打 ORIG 同文日志（__PRETTY_FUNCTION__ 文本），随后
// HistoryLog::AddLog + SendSyncPacket，返 1。
#include "Arad_AvatarRoulette/BuyProcess.inc"
}

