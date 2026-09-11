void CPacketTracer::AbsoluteUdpWriteLog()
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