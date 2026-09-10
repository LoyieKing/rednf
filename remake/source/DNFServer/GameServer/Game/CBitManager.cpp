// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBitManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11CBitManagerC2Ev`（ORIG 0x831c3af）。
#include <iostream>

#include "WindowsPackFiles.h"

#include <cstdlib>
#include <cstring>

// ===========================================================================
// CBitManager
// ===========================================================================
#include "CBitManager/CBitManager.inc"
    : m_data(0), m_buffer(0), m_size(0), m_field0c(0)
{
}                                                // 0x0831c09c

#include "CBitManager/dtor_CBitManager.inc"
{
}                                                // 0x0831c0c8 空析构

#include "CBitManager/SetAll.inc"
{
    // ORIG 0x0831c0d4/0x0831c0db：i 声明置 0 + for 头再置 0（两条 movl $0 同
    // 槽）——原码即 `int i = 0; for (i = 0; ...)` 形，忠实保留。
    int i = 0;
    for (i = 0; i < m_size; ++i)
    {
        if (value == 0)
        {
            buf[i] = 0x00;
        }
        else
        {
            buf[i] = 0xff;
        }
    }
}

#include "CBitManager/ReadData.inc"
{
    m_data = data;
    m_size = size;
    m_buffer = static_cast<unsigned char*>(malloc(size));
    memset(m_buffer, 0, size);
}

#include "CBitManager/GetBitFromLSB.inc"
{
    // ORIG 0x0831c160：栈上 8 字节掩码表**逐元素赋值**（非初始化列表——P60
    // 列表形变 .rodata 模板+rep movs，ORIG 为 8 条 movb 立即数）；下标 bits-1；
    // -0x10 dead int 零初始化槽（P31）；ShiftRight 第 3 实参 = mov (%eax) =
    // this+0 即 m_data（此前 m_buffer 系误转，0x0831c1b8 实证）。
    unsigned char table[7];
    table[0] = 0x01;
    table[1] = 0x03;
    table[2] = 0x07;
    table[3] = 0x0f;
    table[4] = 0x1f;
    table[5] = 0x3f;
    table[6] = 0x7f;
    unsigned char ret = 0;
    int unused = 0;
    (void)unused;
    unsigned char mask = table[bits - 1];
    ret = m_data[m_size - 1] & mask;
    ShiftRight(bits, m_data);
    return ret;
}

// [N5A 2026-09-11] 旧「变长移位计数掩码 N 类」判词**已推翻**：3 处 `and $0x1f`
// 全部来自我方显式 `bits & 0x1f`（4.4 对 `%cl` 变长移位**不**插掩码）。
// 改写为 ORIG 形「int i=0; for(i=m_size-1;i>=0;--i) + buf[i] | (buf[i-1]<<(8-bits))」
// 后探针（/tmp/r5/n5/N5A_BattleField/sr_probe.cpp 变体 A）逐条全等 ORIG 0x0831c1d6
// （含 push %edi、槽 -0x10、not/shr 尾测）⇒ ORIG 57 / 我方 57。
#include "CBitManager/ShiftRight.inc"
{
    int i = 0;
    for (i = m_size - 1; i >= 0; --i)
    {
        buf[i] = static_cast<unsigned char>(buf[i] >> bits);
        if (i != 0)
        {
            buf[i] = static_cast<unsigned char>(buf[i] | (buf[i - 1] << (8 - bits)));
        }
    }
}

#include "CBitManager/ShiftLeft.inc"
{
    // P191 配套：ORIG 0x0831c26f/276 双 `movl $0x0,-0x10` = `int i=0;`+`for(i=0;..)`
    // 双初始化；移位掩码实测两侧均无 and（旧 N 标注掩码差已失效，仅剩此条）。
    int i = 0;
    for (i = 0; i < m_size; ++i)
    {
        buf[i] = static_cast<unsigned char>(buf[i] << bits);
        if (i != m_size - 1)
        {
            buf[i] = static_cast<unsigned char>(
                buf[i] | (buf[i + 1] >> (8 - bits)));
        }
    }
}

#include "CBitManager/GetBit.inc"
{
    // ORIG 0x0831c2fc：int 循环槽先置 0（-0x8）、acc/ret 置 0（-0x2/-0x1），
    // 栈掩码表逐元素赋值，(int)b<=bitHi setle 形循环。
    int b = 0;
    unsigned char acc = 0;
    unsigned char ret = 0;
    unsigned char masks[8];
    masks[0] = 0x01;
    masks[1] = 0x02;
    masks[2] = 0x04;
    masks[3] = 0x08;
    masks[4] = 0x10;
    masks[5] = 0x20;
    masks[6] = 0x40;
    masks[7] = 0x80;
    for (b = bitLo; b <= bitHi; ++b)
    {
        acc = static_cast<unsigned char>(acc | masks[b]);
    }
    ret = static_cast<unsigned char>(m_data[idx] & acc);
    return ret;
}
