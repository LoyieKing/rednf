// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W9_StructMisc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W9_StructMisc —— Wave9 杂项 struct 生命周期符号（worklist
// /tmp/only_orig/{RecoveryInfo,stCeraShopBonusItemInfo_t,STPacketLog,
// TOD_UserState}.txt）。全部为 ORIG 真实符号；声明在各权威头。
// ============================================================================

#include "CDungeon.h"        // RecoveryInfo（ORIG D1 0x088f9346）
#include "CItemList.h"       // stCeraShopBonusItemInfo_t（ORIG D1 0x089363da）
#include "STPacketLog.h"     // ORIG C1 0x08481100 / D1 0x084810ea / Compare 0x08481030
#include "DataManagerInitDepends.h"   // TOD_UserState 权威定义

// ORIG _Z22CheckDailyScheduleTimeill（T 0x0846c0a8，定义 GameUtils.cpp）——
// C++ linkage，误用 extern "C" 会产生 C 符号导致链接缺口。
#include "W9_StructMisc/CheckDailyScheduleTime.inc"
#include "W9_StructMisc/TOD_UserState.inc"
}


// ---- RecoveryInfo（ORIG W 0x088f9346：push ebp / mov esp,ebp / pop ebp / ret，
//      即空体析构；vector<RecoveryInfo> 元素销毁触发）----

// ---- stCeraShopBonusItemInfo_t（ORIG W 0x089363da：空体析构）----
#include "W9_StructMisc/dtor_stCeraShopBonusItemInfo_t.inc"
#include "W9_StructMisc/STPacketLog.inc"
#include "W9_StructMisc/dtor_STPacketLog.inc"
#include "W9_StructMisc/Compare.inc"
}

// ============================================================================
// TOD_UserState（ORIG T 0x08643460 / D1 0x086434ae / D0 0x086434de /
//   ResetDaily 0x08643624 / _saveData 0x08643642 / _reset 0x086437ec /
//   loadData 0x086434fc / getData 0x086435ca）
// ============================================================================
#include "CUser.h"                 // CUser::GetUID/get_charac_no/get_charac_level
#include "GlobalData.h"            // s_stream_pool/s_msgq_mgr/s_systemTime_/s_pTowerOfDespairMgr
#include "CEnvironment.h"          // G_CEnvironment()->m_serverEnvir.m_fatigueTime
#include "CStreamGuard.h"          // CStreamGuard/GetInBuffer
#include "MsgQueueMgr.h"


#include "W9_StructMisc/dtor_TOD_UserState.inc"
#include "W9_StructMisc/setEnterLayer.inc"
}

// ORIG T 0x08643624：enterCount=0 + CData::alter()
#include "W9_StructMisc/ResetDaily.inc"
}

// ORIG T 0x086437ec：+8/+0xc/+0x10 清零 → resetLayer(+0x14) → 清
// lastEnterLayer(+0x16)/lastEnterTime(+0x18)/lastClearTime(+0x1c)。
#include "W9_StructMisc/_reset.inc"
}

// ORIG T 0x086435ca const：
//   pData+0xe←this+0x16 / pData+0xc←this+0x14 / pData+4←this+0xc /
//   pData+8←this+0x10 / pData+0x10←this+0x18 / pData+0x14←this+0x1c
//   （ORIG 不回写 sig+0 ← this+8）。
#include "W9_StructMisc/getData.inc"
}

// ORIG T 0x086434fc：先虚调 _reset()，拷入 6 dword 数据区；
// 若当日跨天(按 serverEnvir.m_fatigueTime(+0x37c) 口径)再虚调 ResetDaily()；
// 有历史层且 lastEnterTime<0x4e275df1 时清层标记 + alter 脏标记。恒返 true。
// [b28d-23 N·N93] 槽/帧差（69/80）⇒ N。ORIG 0x086434fc
#include "W9_StructMisc/loadData.inc"
}

// ORIG T 0x08643642：等级低于 TowerOfDespairMgr 最小等级时直接返回 true；
// 否则经 StreamPool 取流组包：<<0x278、<<UID、GetInBuffer 填 0x18 记录、
// MsgQueueMgr::put(Q2)。异常安全由局部 CStreamGuard 析构承担。
// [b28d-23 N·N93] ⇒ N。ORIG 0x08643642
#include "W9_StructMisc/_saveData.inc"
}
