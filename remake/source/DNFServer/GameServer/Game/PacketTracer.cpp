#include "PacketTracer.h"
#include "GameTypes.h"
#include <cstdio>
#include <cstring>

CPacketTracer::CPacketTracer()
{
    m_tcpCount = 0;
    m_udpCount = 0;
}

void CPacketTracer::AddTcpLog(int flag, int value)
{
    int idx = m_tcpCount % 10;

    m_logs[idx].m_flag = static_cast<char>(flag);
    m_logs[idx].m_value = value;
    ++m_tcpCount;
}

void CPacketTracer::AddUdpLog(int flag, int value)
{
    // [b28c-lane17] 判词：34=34、除两处 `m_logs[idx + 10]` 的地址折叠形外逐条
    // 同形。ORIG 4.1 = `mov idx,%r; ...; add $0xa,%r`（两步），4.4 恒
    // `lea 0xa(%eax),%r`（单条）；实测 `10+idx`/`(unsigned)`/指针算术/具名 off
    // 局部五形均不产两步形 ⇒ N81/P30（scaled 基址+常数折叠源形不可择）。
    int idx = m_udpCount % 10;

    m_logs[idx + 10].m_flag = static_cast<char>(flag);
    m_logs[idx + 10].m_value = value;
    ++m_udpCount;
}

CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}

void CPacketTracer::AbsoluteTcpWriteLog()
{
    char buf[0x800] = { '\0' };
    for (int i = 0; i <= 9; ++i)
    {
        std::sprintf(buf, "%s,%d/%d", buf,
                     static_cast<int>(static_cast<unsigned char>(m_logs[i].m_flag)),
                     m_logs[i].m_value);
    }
    --m_tcpCount;
    register unsigned int idx = m_tcpCount % 10;
    cMyTrace trace(__FUNCTION__, 0x41, 0);
    trace("[TRACE_TCP_PACKET] (idx:%d)%s", idx, buf);
}

void CPacketTracer::AbsoluteUdpWriteLog()
{
    char buf[0x800] = { '\0' };
    for (int i = 0; i <= 9; ++i)
    {
        std::sprintf(buf, "%s,%d/%d", buf,
                     static_cast<int>(static_cast<unsigned char>(m_logs[i + 10].m_flag)),
                     m_logs[i + 10].m_value);
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
    // 唯一残余 = `m_logs[i+10]` 的 `mov idx,%r; add $0xa,%r` 两步 vs 4.4 恒
    // `lea 0xa(%eax),%r` 折叠（范式库 P30/N81：scaled 基址+常数折形源形不可择）。
    --m_udpCount;
    register unsigned int idx = m_udpCount % 10;
    cMyTrace trace(__FUNCTION__, 0x5a, 0);
    trace("[TRACE_UDP_PACKET] (idx:%d)%s", idx, buf);
}
