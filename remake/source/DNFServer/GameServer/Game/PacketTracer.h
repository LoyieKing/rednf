#ifndef DNF_GAME_PACKETTRACER_H_
#define DNF_GAME_PACKETTRACER_H_

// df_game_r CPacketTracer 还原（TCP/UDP 包追踪环形日志）。
// 布局 0xa8：实例 _ZZ21CPacketTracerInstancevE8instance @ 0x0940b5a0，
// 下一 BSS 符号 @ 0x0940b648（nm 核实）→ sizeof = 0xa8。
// ORIG：C1 0x08260628（仅清零两计数器，记录区不初始化）、
//       AddTcpLog 0x085992c8（写 logs[count%10]）、
//       AddUdpLog 0x0859926c（写 logs[count%10+10]）、
//       AbsoluteTcpWriteLog 0x0859931a / AbsoluteUdpWriteLog 0x0859940e（CryptoUtils1 批实装）。

class CPacketTracer
{
public:
    CPacketTracer();                     // ORIG C1/C2 0x08260628
    void AddTcpLog(int flag, int value); // ORIG 0x085992c8
    void AddUdpLog(int flag, int value); // ORIG 0x0859926c
    void AbsoluteTcpWriteLog();          // ORIG T 0x0859931a
    void AbsoluteUdpWriteLog();          // ORIG T 0x0859940e

private:
    struct LogEntry
    {
        char m_flag; // +0x00（ORIG 存 %dl/%cl 低字节）
        unsigned int m_value; // +0x04（int→unsigned 异型化：N76 store 载入序可翻，NL7 判例；偏移/sizeof 不变）
    };               // 步长 8

    unsigned int m_tcpCount; // +0x00（ORIG %10 用无符号魔数除法）
    unsigned int m_udpCount; // +0x04
    // +0x08：二维具名视图 [0][0..9]=TCP、[1][0..9]=UDP（ORIG 0x0859926c/0x0859940e 实证
    // 4.4 对 `m_logs[1][idx]` 发 `mov idx,%r; add $0xa,%r; mov d(%base,%r,8)` 两步索引形；
    // 一维 `m_logs[idx+10]` 恒被折成 `lea 0xa(%r)` 单条 ⇒ P30/N81 反例解 = P291 具名数组视图）。
    // sizeof/偏移不变：2*10*8 = 20*8 = 0xa0，m_logs 起点仍 +0x08。
    LogEntry m_logs[2][10];

    static_assert(sizeof(LogEntry) == 8, "CPacketTracer::LogEntry size");
};

static_assert(sizeof(CPacketTracer) == 0xa8, "CPacketTracer size");

CPacketTracer* CPacketTracerInstance(); // ORIG W 0x08260640（guard 单例）

#endif // DNF_GAME_PACKETTRACER_H_
