// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CStreamBuffer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CStreamBuffer 实现（ORIG 0x086269be 域）
// 逐函数对应 docs/class_func_reports 缺失（无 CStreamBuffer 目录），按指南
// §3.2 以 nm 定位 + objdump 转写，报告路径：无（见头文件证据注释）。
// 非静态成员：AppendString/Insert 均带 this（参数槽 = this, 显式参数...）。
// AppendString 源码形态（声明 slen/n 后先赋 n 再赋 slen）为实测对齐 ORIG
// 指令序/栈槽（探针验证 gcc4.4 -m32 -O0 同形）。
// ============================================================================

#include "CStreamBuffer.h"

#include <algorithm>   // std::min<unsigned int>
#include <string.h>

#include "CStreamBuffer/CStreamBuffer.inc"
}

#include "CStreamBuffer/c_str.inc"
}

#include "CStreamBuffer/AppendString.inc"
}

CStreamBuffer& CStreamBuffer::operator<<(const char* v)   // 0x08626a4a "%s"
{
    Insert<const char*>("%s", v);
    return *this;
}

CStreamBuffer& CStreamBuffer::operator<<(int v)           // 0x08626a70 "%d"
{
    Insert<int>("%d", v);
    return *this;
}

CStreamBuffer& CStreamBuffer::operator<<(unsigned int v)  // 0x08626a96 "%u"
{
    Insert<unsigned int>("%u", v);
    return *this;
}

CStreamBuffer& CStreamBuffer::operator<<(float v)         // 0x08626abc "%f"
{
    Insert<float>("%f", v);
    return *this;
}
