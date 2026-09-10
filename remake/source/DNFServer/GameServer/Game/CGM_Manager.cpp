// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CGM_Manager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "MySQL.h"          // MySQL::{set_query,exec,get_n_rows,fetch,get_int}
#include "GameTypes.h"      // cMyTrace / rwLock / writeGuard
#include "CGM_Manager.h"
#include <stdio.h>          // NumberToString（W8LT_DBTask TU 同款局部声明）

char* NumberToString(unsigned int value, int radix);   // ORIG T 0x081090b4

// ORIG C1/C2 0x082990b4：movb $0,(+0)；movl $0,(+4)（函数体赋值形态，无 ecx 缓存）。
#include "CGM_Manager/CGMRecord.inc"
}

// ORIG D1/D2 0x082990ca：空析构（用户定义，生成 T 符号）。
#include "CGM_Manager/dtor_CGMRecord.inc"
}

// ORIG 符号: _ZN11CGM_Manager4IsGmEj @ 0x829948c
// readGuard(m_lock)；find 结果与 end() 用 operator== 比较，相等（命中）返回 true。
// [B24D判词 N] 语义正确：readGuard+find，it==end() 返回 true 否则 false（ORIG
//   operator==）；diff=mov $0x0/mov $0x1 分支汇聚顺序互换（GCC4.1 vs 4.4），
//   55/55 纯 lowering，语义等价。
#include "CGM_Manager/IsGm.inc"
}

// ORIG 符号: _ZN11CGM_Manager10TurnGmModeEj @ 0x829917c
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 翻转 +0 模式字节并返回新值。
// [b28d-solo-CGM_Manager 判词 AE] ORIG 81 = our 81（compare_tu AE 口径）。旧 B24D
//   「N」与 b28c-13「N18a」判词作废：ORIG 0x0829923e `movzbl (%eax),%ebx` 直落
//   callee-saved 可由「register bool 局部 + return」复现（P231 纯存储类提示，
//   非绑定形；详见函数体内注释与 /tmp/b28d_s78_probe1.cpp f_regbool）。
#include "CGM_Manager/TurnGmMode.inc"
}

// ORIG 符号: _ZN11CGM_Manager16GetCurrentGmModeEj @ 0x8299276
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 读 +0 模式字节。
// [b28d-solo-CGM_Manager 判词 AE] ORIG 60 = our 60（compare_tu AE 口径）。旧 B24D
//   「N」与 b28c-13「N18a」判词作废：同 TurnGmMode，register bool 局部即直落 %ebx。
#include "CGM_Manager/GetCurrentGmMode.inc"
}

// ORIG 符号: _ZN11CGM_Manager10GetGmLevelEj @ 0x082993d8
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 读 +0x04。
#include "CGM_Manager/GetGmLevel.inc"
}

// ORIG 符号: _ZN11CGM_Manager10SetGmLevelEji @ 0x0829932a
// writeGuard(m_lock)；find 命中后经 operator[] 写 +0x04（m_level）。无返回值。
#include "CGM_Manager/SetGmLevel.inc"
}

// ORIG 符号: _ZN11CGM_Manager16LoadGmDataFromDbEj @ 0x08299740
// set_query("seLect level from gm_manifest where m_id = %s", NumberToString(accId,0))
// → exec(true) 失败：cMyTrace(__PRETTY_FUNCTION__, 0x15a2, 0) 记
// "Failed to get GM list from DB." 返 false；get_n_rows()==0 返 false；
// writeGuard(m_lock) 内 fetch() 一次，get_int(0, m_gmMap[accId].m_level)，
// 等级为 0：cMyTrace(__PRETTY_FUNCTION__, 0x15b4, 0) 记
// "GM: %s exists in GM manifest but doen't have proper level. check DB" 返 false；
// 否则返回 exec 结果。
// [b28d-solo-CGM_Manager 判词 AE] ORIG 123 = our 123（compare_tu AE 口径）。旧
//   b28d-37「N104/N82 封顶」作废，复现三钥匙：①guard 用显式作用域块——块内
//   `return false;`（失败臂析构前 `mov $0,%ebx; mov $0,%esi`）、块外 `return ok;`
//   （成功路径析构后才 `movzbl -0xd,%ebx`），4.4 即发 ORIG 的 %esi 选择器形；
//   ②t2 vararg 用全表达式临时链 `cMyTrace(...)(fmt, NumberToString(...))`（P227：
//   实参在 ctor 前求值并跨 ctor 保活 %ebx；具名两语句形实参序反）；③两把
//   cMyTrace 槽 -0x30/-0x20 与 guard -0x34 随①②归位。注：game_func_report 在
//   .o 阶段仍报 DIFF（未解析重定位标签显示差），以 compare_tu_game_orig.py 为准。
#include "CGM_Manager/LoadGmDataFromDb.inc"
}
