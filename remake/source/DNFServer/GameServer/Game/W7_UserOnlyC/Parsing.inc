bool CUser::Parsing(int received)
{
    _header hdr;
    // P0/P183：ORIG 无 net 引用局部（每处 `mov 0x8(%ebp),%eax; mov 0xf8(%eax),…`
    // this 表达式直读）⇒ 全部改 m_network 具名成员访问。

    int remain = static_cast<int>(m_network.m_recvOffset) + received;
    // ORIG 骨架（CUserOnlyOrig.cpp:4524-4529）：tick/zero/headerLen 三具名局部；
    // `m_lastRecvTick=tick; m_recvGapTick=zero;` 由 4.1 合并 8 字节写。
    unsigned int tick =
        static_cast<unsigned int>(GlobalData::s_systemTime_.getCurTickCount());
    unsigned int zero = 0;
    (void)zero;
    // [N1B 2026-09-11 探针] ORIG 槽序（地址递增）：remain、tick、zero、size、
    // headerLen、死槽、verdict ⇒ 4.4 声明序=地址递减，size 须声明在 zero 之后、
    // headerLen 之前（原在循环内声明 ⇒ 得最高槽 -0x24，与 ORIG -0x2c 不符；
    // 提前到 tick 之前 ⇒ 得 -0x34，亦不符）。
    unsigned int size;
    // ORIG 4.1 把相邻两 int 字段合并为单次 8 字节写（`mov eax,0xe4; mov edx,0xe8`）；
    // CNetwork.h 已备 u64 整写视图 m_lastRecvGapPair（P206 先例），4.4 逐条重载
    // this 的问题由此消解。
    m_network.m_lastRecvGapPair = static_cast<unsigned long long>(tick);
    int headerLen = 0xd;                  // ORIG -0x28（照抄；memcpy 用立即数）
    (void)headerLen;

    if (remain <= 0xc)
    {
        m_network.m_recvOffset += received;                 // +0xf8
        m_network.m_recvBuf += received;                    // +0x10c
        // [R16a04 P227] ORIG 于 cMyTrace::C1 之前求值全部 vararg（ctor 后压栈）⇒
        //   原码是构造即调用的临时链；具名对象把 C1 提前到求值之前。
        cMyTrace(__PRETTY_FUNCTION__, 0x69d, 0)
            ("(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d]",
             reinterpret_cast<unsigned int>(m_network.m_recvBuf),
          // [N1B 2026-09-11 负结论] `&m_network.m_recv[0]` 形已实测（ORIG +0x8c
          // `lea 0x110(%eax),%edi` = 地址直算；我方 add+mov 两段）⇒ n1_v 归一后
          // 残差**逐字节不变**，回滚。
             reinterpret_cast<unsigned int>(m_network.m_recv), m_network.m_recvOffset,
             received);
        m_network.m_bBufOffsetTrace = 1;                    // bufoffset 标记 +0x109
        return true;
    }

    // [M2S1] ORIG 0x08647ee1/0x08647ee9：尾条件为 `cmpl $0xc,-0x3c; jg +0x10a`
    // （直接回跳循环头，无 setcc 物化）+ 入口 `jmp +0x10b` ⇒ 原码是
    // `for (;;) { ...; if (!(remain > 0xc)) break; }`；旧 do-while/while 形在
    // 4.4.7 恒产 `setg %al; test %al,%al`（探针 /tmp/m2probe_dw.o g1/g2 vs g3）。
    // [N1B 2026-09-11] 探针：ORIG +0x101 `movl $0x0,-0x24(%ebp); jmp +0x10b`
    //（循环入口前一个清零槽，函数其余处不再读）⇒ 试以具名局部复现。
    int zeroPad = 0;
    (void)zeroPad;
    for (;;)
    {
        if (m_network.m_recvOffset != 0)
            m_network.m_recvBuf -= m_network.m_recvOffset;

        if (m_network.m_bBufOffsetTrace != 0)
        {
            // [R16a04 P227] 同 0x69d 处（ORIG ctor 前求值）。
            cMyTrace(__PRETTY_FUNCTION__, 0x6b2, 0)
                ("[BUFOFFSET TRACE](offset:%x - buf:%x) = remainlen:%d, Recv Size[%d]",
                 reinterpret_cast<unsigned int>(m_network.m_recvBuf),
                 reinterpret_cast<unsigned int>(m_network.m_recv),
                 m_network.m_recvOffset,
                 received);
        }

        std::memcpy(&hdr, m_network.m_recvBuf, 13);

        // ORIG 0x08647a59 `mov -0x76(%ebp),%eax; mov %eax,-0x2c(%ebp)`：m_size 以
        // 32 位整写入具名 unsigned size 局部（P171 照抄 ORIG 的 32 位读形）。
        size = *reinterpret_cast<const unsigned int*>(&hdr.m_size);

        if (size < 13 || size > 0x578)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x6ce,
                "Recv Size[%d], Parsing Packet Size[%d], recvbuf_offset[%d] is Too Large",
                received, static_cast<int>(size),
                reinterpret_cast<int>(m_network.m_recvBuf));
            return false;
        }

        if (remain < size)
            break;

        // [P-G7b] ORIG 0x08… 段 `call UpdatePacket; mov %eax,-0x20(%ebp);
        // cmpl $0x0,-0x20(%ebp); setne %al; test %al,%al; je`（ORIG-only setcc）
        // ⇒ 原码是赋值内嵌条件（两句式恒直折 je，无物化）。
        int verdict = 0;
        if ((verdict = m_packetVerifier.UpdatePacket(hdr)) != 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x6f4,
                "[Packet Varifier Error %d][classification %d][packet id %d][ServerSequence: %d][ClientSequence: %d](m_id: %s)",
                verdict, static_cast<int>(hdr.m_class),
                static_cast<int>(hdr.m_id),
                static_cast<int>(m_packetVerifier.GetSequence()),
                static_cast<int>(hdr.m_seq), NumberToString(get_acc_id(), 0));
            if (verdict != 4)
                return false;
        }

        if (verdict == 0)
        {
            _NS_PI_2ND_RecvHash_CS(m_network.m_recvBuf, this,
                                   size);

            if (hdr.m_id != 0x25c)
            {
                unsigned char decrypted[0x578];
                int outLen = 0x578;
                G_Cipher()->Decrypt(
                    static_cast<int>(hdr.m_id),
                    reinterpret_cast<const unsigned char*>(
                        m_network.m_recvBuf + 13),
                    size - 13, decrypted, &outLen);
                if (outLen > 0)
                    std::memcpy(m_network.m_recvBuf + 13, decrypted, outLen);

                unsigned int crc = Encryption::crc32N(
                    reinterpret_cast<void*>(m_network.m_recvBuf + 11),
                    size - 11);
                _NS_PI_2ND_MakeChecksumTo1Byte2(crc, hdr.m_checksum);
                if (hdr.m_checksum != crc)
                {
                    // [R16a04 P227] ORIG 0x08647cba `call GetPacketName; mov %eax,%ebx` 先于
                    //   C1 0x08647cdf（实参跨 ctor 保活）⇒ 临时链形。
                    cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)
                        ("[CHECK SUM] check_sum error PACKET : %s",
                         GetPacketName((ENUM_PACKETCLASS)hdr.m_class, hdr.m_id));
                    return false;
                }
            }

            // [M2S1] ORIG 0x08647d05 `mov -0x2c(%ebp),%eax; add $0x8,%eax`
            // = 用前面已解出的 32 位 size 局部（-0x2c），不是重读 16 位 m_size
            // （旧形多一条 movzwl，P0 槽-局部对应）。
            const int streamLen = static_cast<int>(size) + 8;
            CStreamGuard guard(true);
            // 每个分支各自 Set（ORIG 3 处 Set 调用点，P48）。
            if (streamLen <= 0xfff)
                guard.Set(GlobalData::s_small_stream_pool->Acquire("user.cpp", 0x758));
            else if (streamLen > 0x3fff)
                guard.Set(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x760));
            else
                guard.Set(GlobalData::s_stream_pool->Acquire("user.cpp", 0x75c));

            // P48：每次写入均重调 operator*；put_binary 经 operator->。
            (**guard) << 0;
            (**guard) << static_cast<int>(GetUID());
            guard->put_binary(reinterpret_cast<void*>(m_network.m_recvBuf),
                              size);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            m_network.m_bBufOffsetTrace = 0;
        }

        remain -= size;
        m_network.m_recvBuf += size;
        m_network.m_recvOffset = 0;

        if (remain == 0)
        {
            m_network.m_recvBuf = m_network.m_recv;
            break;
        }
        if (!(remain > 0xc))
            break;
    }

    m_network.m_bBufOffsetTrace = 0;

    if (remain > 0)
    {
        if (remain > 0x1000)
        {
            // ORIG：0x793 logFormat（varargs 可抛）在 try 内；catch(...) 记 0x7a1 后返 0。
            try {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x793,
                    "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                    remain);
            } catch (...) {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x7a1,
                    "[PARSING EXCEPTION] memmove : parsinglength = %d", remain);
                return false;
            }
            return false;
        }
        memmove(m_network.m_recv, m_network.m_recvBuf, remain);
        m_network.m_recvOffset = remain;
        m_network.m_recvBuf = m_network.m_recv + remain;
    }
    return true;
}