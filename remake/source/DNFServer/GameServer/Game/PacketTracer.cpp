// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PacketTracer/<Func>.inc（不参与编译、内部不含 #include）。
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
// `_GLOBAL__I__ZN13CPacketTracer9AddUdpLogEii`（ORIG 0x859954b）；AddUdpLog 已前移为首强符号。
#include <iostream>

#include "PacketTracer.h"
#include "GameTypes.h"
#include <cstdio>
#include <cstring>

// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x0859954b。
#include "PacketTracer/AddUdpLog.inc"
{
    // [b28d-solo-PacketTracer] 修复判词（DIFF→AE）：旧一维 `m_logs[idx + 10]` 恒被 4.4
    // 折成 `lea 0xa(%eax),%ecx`（单条），ORIG 0x0859926c 为两步
    // `mov -0x4(%ebp),%ecx; …; add $0xa,%ecx; mov %dl,0x8(%eax,%ecx,8)`。
    // 改二维具名视图 `m_logs[1][idx]`（PacketTracer.h，sizeof/偏移/mangled 零变化）后
    // 4.4 复现 ORIG 的两步索引形（探针 /tmp/b28d_s85/probe.cpp f15 与 ORIG 逐条同形；
    // 机制 = P291 具名数组成员视图 / P30 反例解）。旧「N81/P30 源形不可择」判词作废。
    int idx = m_udpCount % 10;

    m_logs[1][idx].m_flag = static_cast<char>(flag);
    m_logs[1][idx].m_value = value;
    ++m_udpCount;
}

#include "PacketTracer/CPacketTracer.inc"
{
    m_tcpCount = 0;
    m_udpCount = 0;
}

#include "PacketTracer/AddTcpLog.inc"
{
    int idx = m_tcpCount % 10;

    m_logs[0][idx].m_flag = static_cast<char>(flag);
    m_logs[0][idx].m_value = value;
    ++m_tcpCount;
}

CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}

#include "PacketTracer/AbsoluteTcpWriteLog.inc"
{
    char buf[0x800] = { '\0' };
    for (int i = 0; i <= 9; ++i)
    {
        std::sprintf(buf, "%s,%d/%d", buf,
                     static_cast<int>(static_cast<unsigned char>(m_logs[0][i].m_flag)),
                     m_logs[0][i].m_value);
    }
    --m_tcpCount;
    register unsigned int idx = m_tcpCount % 10;
    cMyTrace trace(__FUNCTION__, 0x41, 0);
    trace("[TRACE_TCP_PACKET] (idx:%d)%s", idx, buf);
}

#include "PacketTracer/AbsoluteUdpWriteLog.inc"
{
    // [b28d-solo-PacketTracer] 修复判词（DIFF→AE）：循环体 `m_logs[i + 10]` 的两步
    // `mov -0xc(%ebp),%edx; add $0xa,%edx; mov 0xc(%eax,%edx,8),%edx` 由二维具名视图
    // `m_logs[1][i]` 复现（旧一维形恒折 `lea 0xa(%eax),%ecx`；同 AddUdpLog，P291/P30 反例解）。
    // 其余段（%ebx 跨 cMyTrace ctor 保活）保持 ORIG 形。
    char buf[0x800] = { '\0' };
    for (int i = 0; i <= 9; ++i)
    {
        std::sprintf(buf, "%s,%d/%d", buf,
                     static_cast<int>(static_cast<unsigned char>(m_logs[1][i].m_flag)),
                     m_logs[1][i].m_value);
    }
    // [b28c-lane17] 判词（P0 合规，非 asm 绑定）：ORIG 0x0859949b 起把
    // `m_udpCount % 10` 商余以 mul 魔数算出后**留在 %ebx 跨 cMyTrace ctor 保活**，
    // ctor 后才 `mov %ebx,0x8(%esp)`。本工具链 4.4 -O0 下三形实测：
    //   A 具名局部 + 具名 trace 对象（现形）→ 与 ORIG 逐条等（仅 idx 落 %ebx 为
    //     编译器自由分配，`register` 提示在此处与 ORIG 同值，非 asm 绑定）；
    //   B `register` 提示去除 → 值槽化（-0x14 回读），+2 条且装载序反；
    //   C 完全内联（无局部，模运算直写 op() 实参位）→ 71 条但模运算落到
    //     ctor **之后**（%edx 直传），ORIG 是 ctor 之前算好留 %ebx 跨 ctor 保活；
    //   D P227 全表达式临时链 → 同 C 的装载时刻反置。
    //   ⇒ 本位点唯一保持 ORIG 求值时刻的是「具名局部 + 裸 register 提示」（A）。
    // 故保留 A 形（`register` 无 asm("reg") 绑定，P0 禁的是 `register T x asm("reg")`）。
    // 旧残余（`m_logs[i+10]` 两步 add vs 4.4 恒 `lea 0xa(%eax),%r`）已由二维具名视图
    // `m_logs[1][i]` 闭合，见函数头 [b28d-solo-PacketTracer] 判词（P30/N81 旧封顶判词作废）。
    --m_udpCount;
    register unsigned int idx = m_udpCount % 10;
    cMyTrace trace(__FUNCTION__, 0x5a, 0);
    trace("[TRACE_UDP_PACKET] (idx:%d)%s", idx, buf);
}
