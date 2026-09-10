// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StrikerScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// StrikerScript.cpp — StrikerScript 查询/导入补充（独占 TU，任务 OrigStriker）
//
// 覆盖符号（ORIG df_game_r nm 核实；报告 ##汇编 + ##反编译 C 全文已读，
// 另经 objdump 逐指令核对）：
//   importComboList()                          T 0x08a9e79c
//   importStrikerLevelPenalty()                T 0x08a9e628
//   getPrecedeSkillIndex(short,short,int)      T 0x08a9ea96
//   getComboIndex(short,short,int)             T 0x08a9e97a
//   getAddCommand(short,int)                   T 0x08a9eb3a
//   isCommonSkill(short,int)                   T 0x08a9ebee
//
// 依赖：getStrikerSkillVector（定义 LC_Misc.cpp，ORIG T 0x08a9e7e0）、
//   loadRDARScriptListFile（定义 STScriptFileList.cpp，
//   _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
//   T 0x088bbd77）、ScanInt（DNFLexWrapper.h）。
//
// 不含（他人已落盘，查重后跳过）：
//   StrikerScript::{ctor,dtor,getInsance,clear,create,destory}  CtorProducers_2.cpp
//   StrikerScript::importScript                                ScriptLeafA2.cpp
//   StrikerScript::{importSkillDataUpVector,importSkillAttackPenalty,
//       importSkillCoolTimePenalty,importParentLevelPenlaty,readSkillPenalty}
//                                                              StrikerWorldDropB2.cpp
//   StrikerScript::{getStrikerSkillVector,checkSkillIndex}     LC_Misc.cpp
// ============================================================================

#include <map>
#include <vector>

#include "StrikerScript.h"
#include "DNFLexWrapper.h"        // ScanInt(bool*)

class STScriptFileList;
// ORIG T 0x088bbd77
// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 定义 STScriptFileList.cpp。本 TU 按 ORIG 调用点语义以 bool 视图声明
// （importComboList 的 ORIG 汇编为 xor $1 直接取反，P22：被调函数按 _Bool
// 使用；返回类型不参与 mangling，链接到 STScriptFileList.cpp 的 int 定义，
// 其返回恒为 0/1，ABI 兼容）。
#include "StrikerScript/loadRDARScriptListFile.inc"
#include "StrikerScript/importComboList.inc"
}

// ORIG T 0x08a9e628：两个 ScanInt((bool*)0) 依次写入 +0x64/+0x68。
#include "StrikerScript/importStrikerLevelPenalty.inc"
}

// ORIG T 0x08a9ea96：getStrikerSkillVector(first,second)；空指针返回 0；
// 循环 operator[] 命中 m_field00==skillIndex 返回 m_field08（+8），未命中 0。
// 循环变量 unsigned（Ghidra uint local_10；ORIG seta 无符号判续）；
// vec 先置 0 再赋值（ORIG call 前 movl $0,-0x10 实证）。
#include "StrikerScript/getPrecedeSkillIndex.inc"
}

// ORIG T 0x08a9e97a：外层 i=0..1（setle 有符号判续）；i==0 用
// getStrikerSkillVector(first,second)，否则 (first,0)；空返回 0；
// 迭代器遍历（begin/end/operator->/后置 it++）命中 m_field00==skillIndex
// 返回 m_field04（+4）。
#include "StrikerScript/getComboIndex.inc"
}

// ORIG T 0x08a9eb3a：遍历 m_macroCommands(+0x94)，短路双条件
// （first==m_field00 且 command==m_field04，各一次 operator[]），
// 命中返回 m_type(+8)；未命中 0。循环变量 unsigned（Ghidra uint；
// ORIG 循环头 seta 无符号判续）。比较左侧 first 先求值（ORIG 循环体
// 首条 movswl first→ebx 跨 op[] 调用缓存）。
#include "StrikerScript/getAddCommand.inc"
}

// ORIG T 0x08a9ebee：getStrikerSkillVector(first,0)；空指针返回 0；
// 迭代器遍历命中 m_field00==skillIndex 返回 1，否则 0（无 setne 规范化 → int）。
#include "StrikerScript/isCommonSkill.inc"
}
