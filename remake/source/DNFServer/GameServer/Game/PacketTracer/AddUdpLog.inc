// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x0859954b。
void CPacketTracer::AddUdpLog(int flag, int value)
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