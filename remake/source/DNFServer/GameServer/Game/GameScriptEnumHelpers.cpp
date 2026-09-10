// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GameScriptEnumHelpers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// GameScriptEnumHelpers.cpp — 字符串→枚举映射自由函数。
// 逐函数依据 ORIG df_game_r objdump 反汇编还原（地址见各函数注释）；
// getChangeStatusType / getNpcRoleByString 按 ORIG 结构还原为「函数级静态
// map<char const*,int,less_TCHAR> + SpinLock 双检 + 贯穿式 insert 建表链 +
// switch(码) 派发」原形（键/码/返回值逐条转写自反汇编）。
// 注意：getCharacterJobToEnumValue(0x89bbc0f) 由 CDataManager.cpp 提供，不在此重复。

#include "GameScriptEnumHelpers.h"
#include "DNFLexWrapper.h"   // ScanStr(std::string*)
#include "CEnvironment.h"    // less_TCHAR / SpinLock（ORIG __lookup 静态表依赖）

#include <map>
#include <cstring>

// getChangeStatusType / getNpcRoleByString 的函数级静态查找表类型：
// ORIG 实证 std::map<char const*, int, less_TCHAR>（键按串内容比较）。
typedef std::map<const char*, int, less_TCHAR> EnumLookupMap;

// [b28 P210] 调用者定义移到 callee 之前：4.4 对「TU 内已见定义的 callee」按其实
// 帧精确继承 outgoing（塌缩 sub $0x4）；ORIG 0x0808ec67 帧 0x18 为 callee 未见时
// 保守形。前置定义后恢复保守大帧。
// setCodePageByLocal — ORIG @ 0x808ec64：setCodePage(950 /*0x3b6*/)。
#include "GameScriptEnumHelpers/setCodePageByLocal.inc"
}

// ---------------------------------------------------------------------------
// getStateToEnumValue — ORIG _Z19getStateToEnumValueRSs @ 0x89bbc05（10 字节）：
// 函数体仅 mov $-1,%eax; ret —— 所有输入恒返回 -1。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getStateToEnumValue.inc"
}
// ---------------------------------------------------------------------------
// getActiveStatusToEnumValue — ORIG @ 0x88c29a2，20 个 strcmp 分支 → 0..19，默认 -1。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getActiveStatusToEnumValue.inc"
}

// ---------------------------------------------------------------------------
// getTargetToEnumValue — ORIG @ 0x88c2c34，10 个分支 → 0..9，
// 全部未命中时 ORIG 尾部 mov $0xa,%eax：默认返回 10。
// ---------------------------------------------------------------------------
// ORIG @ 0x88c2c34：10 个 if 链（s == 字面量），非查表循环。
#include "GameScriptEnumHelpers/getTargetToEnumValue.inc"
}

// ---------------------------------------------------------------------------
// getAttackConditionToEnumValue — ORIG @ 0x89bcb6d，4 个分支，默认 4。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getAttackConditionToEnumValue.inc"
}

// ---------------------------------------------------------------------------
// getCategoryToEnumValue — ORIG @ 0x89bcbf2，11 个分支（无 5），默认 0x2c(44)。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getCategoryToEnumValue.inc"
}

// ---------------------------------------------------------------------------
// getElementToEnumValue — ORIG @ 0x89bca42，默认 -1。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getElementToEnumValue.inc"
}

// ---------------------------------------------------------------------------
// getGradeToEnumValue — ORIG @ 0x89bcd5b，4 个分支，默认 4。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getGradeToEnumValue.inc"
}

// ---------------------------------------------------------------------------
// getObjectTypeToEnumValue — ORIG @ 0x89bbd99，默认 -1。
// （33=passive 对象、17=active 对象、529=monster、273=character，ORIG 立即数）
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getObjectTypeToEnumValue.inc"
}

// ---------------------------------------------------------------------------
// getChangeStatusType — ORIG @ 0x8892b0d。
// ORIG 结构（/tmp/getChangeStatusType.md 汇编+Ghidra C 双段实证）：函数级静态
// __lookup = map<char const*, int, less_TCHAR>（__cxa_guard 惰性构造）+
// __init_lock(SpinLock) + __inited 双检；未建表时持锁以 idx=0 进入 switch，
// 自 case 0（与 case 0x294 合并）起逐 case **贯穿** insert(value_type(键,码))
// 共 53 条，落到表尾释放锁、置 __inited 并跳回 guard 重新 find 派发；建表后
// find 命中按 case 直接 return change-status 枚举，未命中/未知码 return 0x4d。
// 「merge」case（0x29c/0x29f/0x2ae/0x2b1/0x2b4/0x2b7/0x2ce/0x2d1）无独立返回值，
// 命中后落入下一 case 的 return（ORIG 汇编 je 直落下一块，逐条转写）。
// 【batch-9 收敛】case 体改 `if (!__inited){insert} else return N;` xor-bool 岛形
// （ORIG 每块 movzbl+xor$1+test，0x08892e53 起）+ 外层翻为 if(__inited){find}
// else{enter 双检}：ours 1294 → 1383 == ORIG 1383（残余仅 .o relocation 显示，
// relink 后按 AE 口径核销）。
// ---------------------------------------------------------------------------
// [b28d-lane16 DIFF→AE] ORIG 08892b0d：p167 CLEAR；本轮把 guard 回跳的 `continue`
// 改 `goto retry`（标签置于 static 三件套之后、`if (__inited)` 之前）⇒ 复现 ORIG
// 0x088944e5 `jmp +0xc` 直跳，消 4.4 的 2 条 jmp 中转跳板（P164 循环旋转族）。
#include "GameScriptEnumHelpers/getChangeStatusType.inc"
}

// ---------------------------------------------------------------------------
// getNpcRoleByString — ORIG @ 0x89fecee（按 std::string 值传参）。
// 【batch-9 收敛】case 体改 `if (!__inited){insert} else {result=N;break;}`
// xor-bool 岛形（ORIG 每块 movzbl+xor$1+test）+ 外层 if(__inited){find}
// else{enter 双检}：ours 1364 → 1475（ORIG 1476；唯一实质残差 = lock 区
// 2 条 4.1 块对齐 nop，负例表空 else nop;jmp 岛族；relocation 显示差待
// relink 核销）。
// ORIG 结构（/tmp/getNpcRoleByString.md 汇编+Ghidra C 双段实证）：函数级静态
// __lookup = map<char const*, int, less_TCHAR>（__cxa_guard 惰性构造）+
// __init_lock(SpinLock) + __inited 双检；未建表时持锁以 idx=0 进入 switch，
// 自 case 0（与 case 0x706 合并）起逐 case **贯穿** insert(value_type(键,码))
// 共 56 条，表尾释放锁、置 __inited 并跳回 guard 重新 find 派发；建表后命中
// case 写 result 并 break 返回角色 0..55，未命中保持入口初值 -1。
// ---------------------------------------------------------------------------
// [b28d-lane16 DIFF→AE] ORIG 089fecee：`goto retry` 直跳形消 4.4 jmp 中转跳板后 AE（同 getModuleType 修法；旧「ours 1364 → 1475，残差 = lock 区
#include "GameScriptEnumHelpers/getNpcRoleByString.inc"
}

// ---------------------------------------------------------------------------
// getNpcMoodFromString — ORIG @ 0x89faf46。
// strncmp(name, X, 10)：good→2、normal→1、bad→0、其余→3。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getNpcMoodFromString.inc"
}

// ---------------------------------------------------------------------------
// scanModuleType — ORIG @ 0x88c0b9a。
// { std::string s; ScanStr(&s); return getModuleType(s.c_str()); }
// ---------------------------------------------------------------------------
// [b28d-lane16 DIFF→AE] ORIG 088c0bfc：p167 CLEAR；本轮 `continue`→`goto retry` 直跳形
// 复现 ORIG 0x088c141a `jmp +0xc`，消 2 条 jmp 跳板（P164 循环旋转族）⇒ AE。
#include "GameScriptEnumHelpers/getModuleType.inc"
}

#include "GameScriptEnumHelpers/scanModuleType.inc"
}

// ---------------------------------------------------------------------------
// setCodePage — ORIG @ 0x8adea1a：全局 codepage 变量置换，返回旧值。
// （Library/Include/Core/UnicodeConvert.cpp 有同名语义实现，但 game 目标未编入，
//   故按 ORIG 在此提供唯一定义。）
// ---------------------------------------------------------------------------
// [b28d-Main R3 迁移·检查点13] gCodePage/getCodePage/setCodePage 已迁入 StringConvert.cpp
// （ORIG 0x09501e90/0x08adea10/0x08adea1a 归属 TU）⇒ 消本 TU 副本，修复码页语义分叉。

// setCodePage 定义已迁入 StringConvert.cpp（声明见 GameScriptEnumHelpers.h）。
