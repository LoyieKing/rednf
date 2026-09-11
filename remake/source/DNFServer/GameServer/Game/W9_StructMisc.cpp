// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W9_StructMisc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDungeon.h"        // RecoveryInfo（ORIG D1 0x088f9346）
#include "CItemList.h"       // stCeraShopBonusItemInfo_t（ORIG D1 0x089363da）
#include "STPacketLog.h"     // ORIG C1 0x08481100 / D1 0x084810ea / Compare 0x08481030
#include "DataManagerInitDepends.h"   // TOD_UserState 权威定义

#include "W9_StructMisc/CheckDailyScheduleTime.inc"
                            long lCurTime);

#include "W9_StructMisc/TOD_UserState.inc"



// ---- RecoveryInfo（ORIG W 0x088f9346：push ebp / mov esp,ebp / pop ebp / ret，
//      即空体析构；vector<RecoveryInfo> 元素销毁触发）----

// ---- stCeraShopBonusItemInfo_t（ORIG W 0x089363da：空体析构）----
// [Q1P342ScanA 2026-09-11 杠杆A·P342 落地·删本 TU 唯一发射点] 本 TU 原在此发
//   `T _ZN25stCeraShopBonusItemInfo_tD1Ev/D2Ev`。随 `CItemList.h` 删去
//   `~stCeraShopBonusItemInfo_t();` 声明，类型转 **trivial**（`_Destroy_aux` ILb0→ILb1，
//   与 ORIG 同形：`ILb1EE9__destroyIP25…` @0x085291cd / `…__normal_iteratorIP25…`
//   @0x08952b5c，全 ELF 无 ILb0 版），D1/D2 改由 `CCeraShopGoods.o` 的
//   `_ZN9__gnu_cxx13new_allocatorI25stCeraShopBonusItemInfo_tE7destroyEPS1_`
//   （ORIG @0x089363e0，紧邻 D1 @0x089363da）路径继续发射。trivial 类型不可再显式定义
//   析构 ⇒ 原 `W9_StructMisc/dtor_stCeraShopBonusItemInfo_t.inc` 已删除。
//   全局并集核账（5 个相关 TU）：LOST 16 项**全部是 ORIG 没有的**多余符号，REAL GAPS = 0。

#include "W9_StructMisc/STPacketLog.inc"
#include "W9_StructMisc/dtor_STPacketLog.inc"

#include "W9_StructMisc/Compare.inc"


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


#include "W9_StructMisc/ResetDaily.inc"


#include "W9_StructMisc/_reset.inc"


#include "W9_StructMisc/getData.inc"


#include "W9_StructMisc/loadData.inc"


#include "W9_StructMisc/_saveData.inc"

