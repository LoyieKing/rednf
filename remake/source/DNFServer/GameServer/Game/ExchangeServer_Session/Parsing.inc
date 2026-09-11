// ---------------------------------------------------------------------------
// Parsing — ORIG 0x8490074 size 0x29a
// ---------------------------------------------------------------------------
// [b28d-68 修法] ORIG 0x08490074。
// ①控制流按 ORIG 重写为 do{...}while(parsingLength>5)（ORIG 0x084900b5 packetSize=0
//   后 jmp 循环体、0x0849020c `jg` 回跳）；memcpy 无条件执行（ORIG 0x084900df 直落，
//   非 else 分支）——删我方发明的 `m_recvOffset==0 && parsingLength<=5 → return 1`
//   与 `m_recvOffset==0 → packetSize=0` 两条分支（ORIG 无）；
// ②DispatchPacket 返回值存 int moved，`if (moved != 0)` 才 trace（ORIG 0x08490195
//   `mov %eax,-0xc` / `cmpl $0x0` / `je`），替换我方 bool dispatched 取反；
// ③logFormat 第三参改 __PRETTY_FUNCTION__（ORIG 0x8c79d00 串实证 =
//   "bool exchange_server::CSession::Parsing(int)"），0x18d 段补 recvLen 实参
//   （ORIG 7 参：0x14(%esp)=recvLen、0x18(%esp)=packetSize；原 "DispatchPacket()"
//   第三参为发明字符串）。
// 残差 N104/EH：4.4 对 try 内仅 nothrow memmove 的 EH 边整体消除（探针
// /tmp/d68probe/eh.o、eh2.o 三形实证：无 __cxa_begin_catch/_Unwind_Resume），
// 4.1 保留 ⇒ p167 的 5 项 EH MISSING 属跨版本结构性不可清。
// [M1S5 2026-09-11 本波两项，ndiff 195→188] ①**类型轴**：ORIG 0x08490090/0x08490093
// `mov %eax,-0x20(%ebp); mov %edx,-0x1c(%ebp)` 后 0x08490099..0x084900a2 两次
// **直传**（`mov %eax,0x4(%ecx); mov %edx,0x8(%ecx)`，无移位指令）⇒ `tick` 为
// **无符号**（有符号 `tick>>32` 必产 `sar $0x1f,%edx`，实证我方旧形含该条）
// ⇒ 改 `unsigned long long tick`；②P339 声明序：ORIG 槽 深→浅 =
// parsingLength(-0x24)/tick(-0x20,-0x1c)/packetSize(-0x14)/moved(-0xc)
// ⇒ 把 tick 提到 packetSize 之前（我方旧序 parsingLength/packetSize/moved/tick
// 使 tick 落最浅 -0x10/-0xc）。our 161→160。
// 残余（判 N，逐条 ORIG 证据）：③`lea (%edx,%eax,1)`(0x08490085) vs 4.4 `add`
//   满负荷位移形（P75 跨版本）；④0x08490219 `jle 尾块`(共享 `mov $1,%ebx` 出口
//   0x08490300) vs 4.4 `jg + mov $1,%eax; jmp`（P325 反向）；⑤do-while 尾
//   0x0849020c `jg` vs 4.4 `setg %al; test %al,%al; jne`（条件物化差）；
//   ⑥`(unsigned int)(tick>>32)` 4.4 产 `mov %edx,%eax; xor %edx,%edx; mov %eax,%edx`
//   三连规范形（4.1 无）；⑦N104 EH 段（上述）。
// [P8S6 2026-09-11] N104 推翻（EH 段可复现）：glibc `memmove` 声明带 `__THROW`(throw())
//   ⇒ 4.4 判 try 块不可抛、整段消除 catch 与 landing pad；ORIG 0x084902a4 起有完整
//   `__cxa_begin_catch/__cxa_end_catch/_Unwind_Resume` 链 + 串 @0x8c78224
//   `[PARSING EXCEPTION] memmove : parsinglength = %d`（我方源码本就有该 catch，
//   只是被删）。块作用域 asm 别名 GCC 拒绝（探针 /tmp/p8probe/eh2.cpp 实证），
//   故按 Dispatchers.cpp:4624 先例在本 include 位置声明**同符号**别名（无 exception
//   spec ⇒ 可抛）：调用点 reloc 仍为 memmove、参数传递逐条同 ORIG（探针 eh.o f1/f2
//   对照：f1 无 EH、f2 完整 EH+ebx 共享出口）。
extern "C" void* p8_memmove_eh(void*, const void*, unsigned int) __asm__("memmove");
bool CSession::Parsing(int recvLen)
{
    int parsingLength;
    unsigned long long tick;
    unsigned int packetSize;
    int moved;
    char header[HEADER_SIZE];

    parsingLength = (int)m_recvOffset + recvLen;

    tick = get_ms_tick();
    m_lastRecvTick = (unsigned int)tick;
    m_recvGapTick = (unsigned int)(tick >> 32);

    if (parsingLength <= 12)
    {
        return 1;
    }

    packetSize = 0;
    do
    {
        if (m_recvOffset != 0)
        {
            m_recvBuf -= m_recvOffset;
        }

        memcpy(header, (const void*)m_recvBuf, HEADER_SIZE);
        packetSize = *(unsigned int*)(header + 3);
        if (packetSize <= 0xc || packetSize > 0x17318)
        {
            LogManager::logFormat(1, "ExchangeServerNetwork.h",
                __PRETTY_FUNCTION__, 0x18d,
                "Recv Size[%d], Parsing Packet Size[%d] is Too Large",
                recvLen, (int)packetSize);
            return 0;
        }

        if (parsingLength < (int)packetSize)
        {
            break;
        }

        if (ES() != 0)
        {
            moved = ES()->DispatchPacket(this, (char*)m_recvBuf, (int)packetSize);
            if (moved != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x197, 0)(
                    "DispatchPacket() error line number(%d)", 407);
            }
        }

        parsingLength -= (int)packetSize;
        m_recvBuf += (int)packetSize;
        m_recvOffset = 0;

        if (parsingLength == 0)
        {
            m_recvBuf = m_recv;
            break;
        }
    } while (parsingLength > 5);

    if (parsingLength <= 0)
    {
        return 1;
    }
    if (parsingLength > 0x17318)
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            __PRETTY_FUNCTION__, 0x1b7,
            "[PARSING LENGTH EXCEPTION] parsinglength > MAX_SESSION_BUFFER_SIZE , memmove : parsinglength = %d",
            parsingLength);
        return 0;
    }

    try
    {
        p8_memmove_eh(m_recv, m_recvBuf, (unsigned int)parsingLength);
        m_recvOffset = parsingLength;
        m_recvBuf = m_recv + parsingLength;
    }
    catch (...)
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            __PRETTY_FUNCTION__, 0x1c1,
            "[PARSING EXCEPTION] memmove : parsinglength = %d", parsingLength);
        return 0;
    }

    return 1;
}