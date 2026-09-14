#ifndef DBMW_DNFPACKETDECODER_H_
#define DBMW_DNFPACKETDECODER_H_

#include "DBMWCommon.h"


class CPacketDecoder
{
public:
    CPacketDecoder();
    ~CPacketDecoder();
    void Process();
    void Attach(CApplication* app);
    void SetTCPQueue(TcpRecvQueue* q);
    void SetUdpQueue(UdpRecvQueue* q);
    void TcpProcess();
    void UdpProcess();
    bool MsgDecode(PacketHeader* header);
    UdpRecvQueue* m_udpQueue;    // +0
    CMutex* m_udpQLock;          // +4
    CMutex* m_udpBLock;          // +8
    TcpRecvQueue* m_tcpQueue;    // +0xc
    CMutex* m_tcpRecvQLock;      // +0x10
    CMutex* m_tcpRecvBLock;      // +0x14
    CServerHandler* m_serverHandler;  // +0x18
    // [2026-09-14 修复] 元素数必须是 **0x2800**，不是 0x2418。
    //   构造函数里是 `for (i = 0x3e8; i <= 0x27ff; i++) m_table[i] = 0;`
    //   （ORIG df_dbmw_r ctor 0x08090fbd `cmp [ebp-4],0x27ff` 与本地同型），
    //   因此数组至少要能索引到 0x27ff ⇒ 元素数 ≥ 0x2800。
    //   ORIG 实证：`CPacketDecoderInstance()::instance` 的符号大小 = **0xa01c**
    //   = 0x1c（前 7 个成员）+ 0x2800×4（表）= 恰好吻合；旧值 0x2418 让对象只有
    //   0x907c 字节 ⇒ 循环对 i = 0x2418..0x27ff 的写入**越界 0x3e8 项 = 4000 字节**。
    //   实测后果：我们二进制里 `CPacketDecoderInstance()::instance` 紧接着就是
    //   `CPacketTracerInstance()::instance` / `CSignalTranslatorInstance()::instance`
    //   （0x8356bbc → guard 0x8356c28 / instance 0x8356c40），越界清零把
    //   `m_signals[15]`（0x8356c7c）连同 guard 一起抹成 0 ⇒ SIGTERM 时
    //   `signal_handler` 对 `getSignal(15)` 的返回 NULL 解引用 ⇒ 关机即 SIGSEGV
    //   （ORIG 则干净退出 exit 1）。ORIG 的 .bss 排布不同，同样的越界落在别的
    //   数据上，所以这个缺陷只在我们的布局下显形。
    void (*m_table[0x2800])(PacketHeader*);  // +0x1c（有效索引 0..0x27ff）
};

#endif  // DBMW_DNFPACKETDECODER_H_
