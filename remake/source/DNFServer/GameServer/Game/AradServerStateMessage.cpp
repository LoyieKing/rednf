// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AradServerStateMessage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ AradServerStateMessage 实现（批3：从 GameStubs.cpp 迁移）。
#include "AradServerStateMessage.h"
#include <cstring>


#include "AradServerStateMessage/AradServerStateMessage.inc"
{
    // ORIG ctor @0x8186652：3×movl $0(+0/+4/+8) + movw $0(+0) + movl $0(+2)
    // —— 等价清空全对象 12B（含发送/拷贝覆盖的 m_pad06，ARAD.cpp:616
    // put_binary(&msg,0xc) 整 12B 发送）。旧注释误读为仅清 +0..+5，实为全零。
    // [b28c-30] 4.4 对 memset 恒发 call（N78）⇒ 以 m_words[3] 双视角写三条 dword
    // store 复现 ORIG 前三段；随后真实字段复清保 movw/movl 尾形。
    m_words[0] = 0;
    m_words[1] = 0;
    m_words[2] = 0;
    m_active = 0;
    m_endSec = 0;
}