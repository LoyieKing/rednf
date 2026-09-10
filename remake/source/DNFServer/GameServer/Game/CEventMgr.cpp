// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// WongWork::CEventMgr 实现 —— 严格转写 ORIG df_game_r：
//   _IsEventing            0x0848ef0c（docs/class_func_reports/WongWork__CEventMgr/_IsEventing.md）
//   GetPremiumServiceBonus 0x0848f078（同目录 GetPremiumServiceBonus.md）
//   in_time                0x086b1d86（ORIG objdump，全局自由函数）
// ============================================================================
#include "CEventMgr.h"

#include <string.h>
#include <time.h>

#include "GlobalData.h" // GlobalData::s_systemTime_（CSystemTime::getCurSec 0x080cbc9e）

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit，使本 TU 发射
// `_GLOBAL__I__ZN8WongWork9CEventMgr11_IsEventingEiiiiiiiiNS0_10EVENT_TYPEE`（ORIG 0x848f1e4）；
// _IsEventing 定义随之前移为本 TU 首个强外部符号（= GI 键名来源）。
// in_time 仅给前置声明（纯 extern 声明不参与键名，P327）。零语义/布局改动。
#include <iostream>

#include "CEventMgr/in_time.inc"

namespace WongWork
{

// ---- ORIG @0x0848ef0c ------------------------------------------------------
// begin.tm = {year-1900, mon-1, mday, hour}；end 同构；
// in_time 通过后：EVENT_WEEKEND 时再要求 localtime_r(now).tm_wday ∈ {0,6}。
#include "CEventMgr/_IsEventing.inc"
}

} // namespace WongWork（_IsEventing 前移段结束）

// ---- ORIG @0x086b1d86 _Z7in_timeP2tmS0_ ------------------------------------
// t1 = mktime(begin); t2 = mktime(end); now = time(NULL);
// return now >= t1 && now < t2;
#include "CEventMgr/in_time_1.inc"
}

namespace WongWork
{

// ---- ORIG @0x0848f078 ------------------------------------------------------
// GetPremiumServiceBonus: _IsEventing(2006,1,2,0, 2006,1,11,0, EVENT_PLAIN) ? 7 : 0
#include "CEventMgr/GetPremiumServiceBonus.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x0848f140 _ZN8WongWork9CEventMgr22IsChildrensDayEventingEv：
// 逐指令转写 —— return _IsEventing(0x7d6,5,5,6, 0x7d6,5,8,6, EVENT_PLAIN);
// （ORIG 栈 movl 常量序：(esp)=0x7d6,(4)=5,(8)=5,(c)=6,(10)=0x7d6,
//   (14)=5,(18)=8,(1c)=6,(20)=EVENT_TYPE(0)）
// ---------------------------------------------------------------------------
#include "CEventMgr/IsChildrensDayEventing.inc"
}

} // namespace WongWork
