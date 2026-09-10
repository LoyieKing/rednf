// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_LoadQuest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_LoadQuest.cpp
// 目标：清零 /tmp/only_orig/DB_LoadQuest.txt 全部 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadQuest/<fn>.md（汇编为准）转写。
// SIG_LOAD_QUEST 权威布局见 SigTypes_C.h（pack(1)，size 0x7608）：
//   +0x000 m_characNo | +0x004 m_field04（错误标志）| +0x00c m_quest[20]
//   | +0x05c m_trigger[20] | +0x0ac m_urgentQuest | +0x0b0 m_cleared
//   | +0x75e0 m_authen
// ============================================================================

#include <string.h>

#include "DB_LoadQuest.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"         // logFormat
#include "AdvanceAltarZip.h"    // uncompress_zip

// ============================================================================
// DB_LoadQuest::DB_LoadQuest @0x0844d930（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D0 0x0844d97c deleting）由类定义自然产出同形符号。
// ============================================================================
#include "DB_LoadQuest/DB_LoadQuest.inc"
}

// ============================================================================
// ~DB_LoadQuest @0x0844d97c（D1/D2/D0）
// ============================================================================
#include "DB_LoadQuest/dtor_DB_LoadQuest.inc"
}

// ============================================================================
// DB_LoadQuest::dispatch @0x0840e236
// 基类 dispatch 通过 → BigStreamPool::Acquire("DBThread.cpp",0x29f9) →
// guard<<userIdx<<slot → GetInBuffer<SIG_LOAD_QUEST>（memset 0x7608、
// +0x4 清零）→ Stream::GetOutBuffer 首字（charac_no）拷入 →
// GetUserQuest 失败置 +0x4=1 → MsgQueueMgr::put(QUEUE_IDX 1)。返 1/0。
// ============================================================================
#include "DB_LoadQuest/dispatch.inc"
}

// ============================================================================
// DB_LoadQuest::GetUserQuest @0x0840e39a
// 列序：charac_no(0)/clear_quest(1)/play_1..play_20(2..39 交替 quest/trigger)/
// urgentQuestIndex(40)/quest_notify(41)。clear_quest blob 解压进 +0xb0；
// 40 个 int 依序 get_int 写 m_quest[i]/m_trigger[i]；+0xac urgent、
// +0x75e0 notify(0x28)。任一步失败 throw int（DBThread.cpp 行号），
// catch 后 logFormat 0x2a81 并返 0；成功返 1。
// ============================================================================
// [SweepDBLoadEtcMain 本波复核] 1030/1029（gap=1）、p167 CLEAR（156/156，仅 cMyTrace
// C1/C2 同址别名）、帧 `sub $0x8d00` 与 push 集全等；残余 3 处全部封顶：
//   ① memset size 经 `mov $0x8cac,%edx; mov %edx,0x8(%esp)` 中转（ORIG 0840e3e7）
//      —— 范式库「待解」条目；4.4 对字面量/cast/`'\0'`/sizeof 四形恒 `movl $imm,
//      0x8(%esp)`（探针 /tmp/memsetp.cpp f1..f4）⇒ N；
//   ② ORIG 0840e634 `nop`（if-join 对齐填充，§6.1 豁免）；
//   ③ 成功块 `mov $0x1,%ebx` 位置：ORIG 0840f47d 在 catch 块之后（`je` 直达尾块），
//      4.4 置于 try 末尾 `mov $0x1,%ebx; jmp`（0840f3d4 对应处）⇒ N104 EH 块相对序；
//   ④ 槽号恒定平移 4：ORIG db -0x18/col -0x14/buf -0x8cdc vs 我方 -0x1c/-0x18/
//      -0x8ce0（大数组+标量混合区，P331 声明序不可翻）⇒ N82/N95。
#include "DB_LoadQuest/GetUserQuest.inc"
}
