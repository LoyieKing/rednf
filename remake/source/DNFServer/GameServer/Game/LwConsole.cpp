// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LwConsole/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// LwConsole.cpp —— LwConsole 类实现（仅 ORIG）
// 报告: docs/class_func_reports/LwConsole/LwConsole.md（仅 C1）
// isSourcePositionVisible/setSourcePositionVisible/log/D1/D0 报告缺失，
// 按 nm 地址 0x08ad3a06/0x08ad3a12/0x08ad3a2a/0x08ad44fa/0x08ad452a objdump 取证：
//   isSourcePositionVisible: movzbl 0x4(%eax) 直返（bool 直返，P32）
//   setSourcePositionVisible: mov %al 暂存+movzbl 回读+mov %dl store（bool 形参，P33）
//   log: movzbl 0x4; test %al; je 跳过 printf("%s(%d) %s: ",file,line,func)；
//        尾部无条件 puts(msg)；格式串 @0x8e32b14 = "%s(%d) %s: "
//   D1: vptr 复位 + LogWriter::~LogWriter + __in_chrg 检查；D0 = D1 + operator delete。
// 全部符号与 ORIG mangled 一致（nm 核验）。

#include "LwConsole.h"

#include <cstdio>

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit，使本 TU 发射
// `_GLOBAL__I__ZN9LogWriterC2Ev`（ORIG 0x8ad3fb7）；LogWriter::C1/C2 定义已是本 TU 首个
// 强外部符号（见下 [b28c-35] 段）。零语义/布局改动。
#include <iostream>

// [b28c-35] P210②：ORIG nm 实证 LogWriter::C1/C2 @0x08ad3990 与 LwConsole::C1
// @0x08ad39e2 同 TU 且定义在先 ⇒ 4.4 精确继承 outgoing 帧 `sub $0x4`；仅声明时
// 保守 `sub $0x18`。故 LogWriter::C1 的 out-of-line 空体定义从 LogManager.cpp
// 迁入本 TU（caller 之前）；符号全局仍唯一（探针 /tmp/l35_probe_frame.cpp）。
#include "LwConsole/LogWriter.inc"
{
}

#include "LwConsole/LwConsole.inc"
    : m_sourcePositionVisible(false)
{
}

#include "LwConsole/dtor_LwConsole.inc"
{
}

#include "LwConsole/isSourcePositionVisible.inc"
{
    return m_sourcePositionVisible;
}

#include "LwConsole/setSourcePositionVisible.inc"
{
    m_sourcePositionVisible = visible;
}

#include "LwConsole/log.inc"
{
    if (m_sourcePositionVisible)
        printf("%s(%d) %s: ", file, line, func);
    puts(msg);
}
