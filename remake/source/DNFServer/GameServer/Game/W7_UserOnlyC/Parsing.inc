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
#if !remake_verify
            cMyTrace(__PRETTY_FUNCTION__, 0x7f0, 5)(
                "[DNF-DIAG PARSING-RET] site=1 size-too-large size=%d remain=%d id=%d",
                static_cast<int>(size), remain, static_cast<int>(hdr.m_id));
#endif
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
            {
#if !remake_verify
                cMyTrace(__PRETTY_FUNCTION__, 0x7f1, 5)(
                    "[DNF-DIAG PARSING-RET] site=2 verifier verdict=%d id=%d seq=%d",
                    verdict, static_cast<int>(hdr.m_id), static_cast<int>(hdr.m_seq));
#endif
                return false;
            }
        }

        if (verdict == 0)
        {
            _NS_PI_2ND_RecvHash_CS(m_network.m_recvBuf, this,
                                   size);

            if (hdr.m_id != 0x25c)
            {
                unsigned char decrypted[0x578];
                int outLen = 0x578;
#if !remake_verify
                unsigned char rawSave[64];   // [DNF-DIAG] 解密前的密文（前 64B）
                for (int k = 0; k < 64; ++k)
                    rawSave[k] = (13 + k < size) ? m_network.m_recvBuf[13 + k] : 0;
#endif
                G_Cipher()->Decrypt(
                    static_cast<int>(hdr.m_id),
                    reinterpret_cast<const unsigned char*>(
                        m_network.m_recvBuf + 13),
                    size - 13, decrypted, &outLen);
                if (outLen > 0)
                    std::memcpy(m_network.m_recvBuf + 13, decrypted, outLen);

#if !remake_verify
                {
                    // [DNF-DIAG 2026-09-19] 整包原始字节（含我方解密结果）
                    const int n = size < 64 ? size : 64;
                    char hx[3 * 64 + 1];
                    hx[0] = 0;
                    for (int k = 0; k < n; ++k)
                    {
                        char b[4];
                        b[0] = "0123456789abcdef"[(m_network.m_recvBuf[k] >> 4) & 0xf];
                        b[1] = "0123456789abcdef"[m_network.m_recvBuf[k] & 0xf];
                        b[2] = ' ';
                        b[3] = 0;
                        for (int j = 0; j < 3; ++j) hx[k * 3 + j] = b[j];
                    }
                    hx[n * 3] = 0;
                    char hxr[3 * 64 + 1];
                    hxr[0] = 0;
                    {
                        int rn = (size - 13) < 64 ? (size - 13) : 64;
                        if (rn < 0) rn = 0;
                        for (int k = 0; k < rn; ++k)
                        {
                            char b[4];
                            b[0] = "0123456789abcdef"[(rawSave[k] >> 4) & 0xf];
                            b[1] = "0123456789abcdef"[rawSave[k] & 0xf];
                            b[2] = ' ';
                            b[3] = 0;
                            for (int j = 0; j < 3; ++j) hxr[k * 3 + j] = b[j];
                        }
                        hxr[rn * 3] = 0;
                    }
                    cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
                        "[DNF-DIAG PKT] id=%d size=%d hdr_crc=%u raw=[13..]=%s dec=%s",
                        static_cast<int>(hdr.m_id), static_cast<int>(size),
                        static_cast<unsigned int>(hdr.m_checksum), hxr, hx);
                }
#endif
                unsigned int crc = Encryption::crc32N(
                    reinterpret_cast<void*>(m_network.m_recvBuf + 11),
                    size - 11);
                _NS_PI_2ND_MakeChecksumTo1Byte2(crc, hdr.m_checksum);
                if (hdr.m_checksum != crc)
                {
#if !remake_verify
                    {
                        // [DNF-DIAG TRY2] 折叠后低字节比较（上一版拿 32 位比 1 字节 ⇒ 无效）
                        const unsigned int want =
                            static_cast<unsigned int>(hdr.m_checksum) & 0xFF;
                        for (int tt = -1; tt < 14; ++tt)
                        {
                            unsigned char tmp[0x600];
                            int rawLen = size - 11;
                            for (int k = 0; k < rawLen; ++k)
                                tmp[k] = static_cast<unsigned char>(m_network.m_recvBuf[11 + k]);
                            int outLen = 0x600;
                            int ok = -1;
                            if (tt >= 0)
                            {
                                ok = G_Cipher()->Decrypt(tt,
                                        reinterpret_cast<const unsigned char*>(tmp) + 2,
                                        rawLen - 2, tmp + 2, &outLen) ? 1 : 0;
                            }
                            unsigned int c = Encryption::crc32N(tmp, rawLen) & 0xFF;
                            cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
                                "[DNF-DIAG TRY2] t=%d ok=%d outLen=%d fold=%u want=%u %s",
                                tt, ok, outLen, c, want, (c == want) ? "<<< MATCH" : "");
                        }
                    }
#endif
#if !remake_verify
                    // [DNF-DIAG 2026-09-19] 临时诊断（只在失败时打印）：
                    //   size = 我们用来算校验和的长度；hdr.m_size = 包头自带长度；
                    //   crc = 我们算出的值；hdr.m_checksum = 客户端带来的值；
                    //   再打前 16 字节便于判断"是否解密后才算"。
                    cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
                        "[DNF-DIAG CHECKSUM] id=%d size=%d hdr_size=%d crc=%u hdr_crc=%u "
                        "b=%02x%02x%02x%02x%02x%02x%02x%02x",
                        static_cast<int>(hdr.m_id), static_cast<int>(size),
                        static_cast<int>(hdr.m_size),
                        static_cast<unsigned int>(crc),
                        static_cast<unsigned int>(hdr.m_checksum),
                        static_cast<unsigned char>(m_network.m_recvBuf[0]),
                        static_cast<unsigned char>(m_network.m_recvBuf[1]),
                        static_cast<unsigned char>(m_network.m_recvBuf[2]),
                        static_cast<unsigned char>(m_network.m_recvBuf[3]),
                        static_cast<unsigned char>(m_network.m_recvBuf[4]),
                        static_cast<unsigned char>(m_network.m_recvBuf[5]),
                        static_cast<unsigned char>(m_network.m_recvBuf[6]),
                        static_cast<unsigned char>(m_network.m_recvBuf[7]));
                    cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
                        "[DNF-DIAG BODY] 解密后 [11..18]=%02x%02x%02x%02x%02x%02x%02x%02x "
                        "size=%d 参与CRC字节数=%d",
                        static_cast<unsigned char>(m_network.m_recvBuf[11]),
                        static_cast<unsigned char>(m_network.m_recvBuf[12]),
                        static_cast<unsigned char>(m_network.m_recvBuf[13]),
                        static_cast<unsigned char>(m_network.m_recvBuf[14]),
                        static_cast<unsigned char>(m_network.m_recvBuf[15]),
                        static_cast<unsigned char>(m_network.m_recvBuf[16]),
                        static_cast<unsigned char>(m_network.m_recvBuf[17]),
                        static_cast<unsigned char>(m_network.m_recvBuf[18]),
                        static_cast<int>(size), static_cast<int>(size) - 11);
#endif
                    // [R16a04 P227] ORIG 0x08647cba `call GetPacketName; mov %eax,%ebx` 先于
                    //   C1 0x08647cdf（实参跨 ctor 保活）⇒ 临时链形。

                    cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)
                        ("[CHECK SUM] check_sum error PACKET : %s",
                         GetPacketName((ENUM_PACKETCLASS)hdr.m_class, hdr.m_id));
#if !remake_verify
                    // [DNF-FIX 2026-09-19 · 进频道即被踢] 仅 OFF 路线：本机重建的
                    //   Cipher::Decrypt 与客户端存在差异 ⇒ 解密后正文的 CRC 与客户端不符，
                    //   ORIG 会 return false 丢包，随后连接被断（实测：进频道后第一个
                    //   GET_USERINFO 即触发）。私有服不依赖该包校验做防护 ⇒ 降级为
                    //   记日志继续。（若要严格对齐 ORIG：修好 Decrypt 后删除本 #if 分支。）
                    // 注意：不执行 return false，继续往下走。
#else
                    return false;
#endif
                }
#if !remake_verify
                else
                {
                    // [DNF-DIAG 2026-09-19] 正向证据：该包校验和与客户端一致。
                    //   RC6 修复（DNF-FIX-RC6-2）后 id%14==3 的包应全部走这一支。
                    cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
                        "[DNF-DIAG CHECKSUM-OK] id=%d size=%d crc=%u",
                        static_cast<int>(hdr.m_id), static_cast<int>(size),
                        static_cast<unsigned int>(crc));
                }
#endif
            }

            // [M2S1] ORIG 0x08647d05 `mov -0x2c(%ebp),%eax; add $0x8,%eax`
            // = 用前面已解出的 32 位 size 局部（-0x2c），不是重读 16 位 m_size
            // （旧形多一条 movzwl，P0 槽-局部对应）。
            const int streamLen = static_cast<int>(size) + 8;
            CStreamGuard guard(true);
            // 每个分支各自 Set（ORIG 3 处 Set 调用点，P48）。
            // [R26a08 §32.2/§15-A 形态修 2026-09-13] ORIG 0x08647d5e 第二档入口保留
            // 4.1 的**冗余重测**：`cmpl $0xfff,-0x1c(%ebp); jle <Big段>` —— 首档已排除
            // <=0xfff ⇒ 该条恒不成立（dead），4.1 不消除短路链中已知条件。4.4 会复用
            // 前次比较结果，故条件必须写成 `&&` 全链才产出同款冗余 cmp/jle（先例：
            // DispatchServerPacket/dispatchPacket.inc、ExchangeServer 池选择族）。
            if (streamLen <= 0xfff)
                guard.Set(GlobalData::s_small_stream_pool->Acquire("user.cpp", 0x758));
            else if (streamLen > 0xfff && streamLen <= 0x3fff)
                guard.Set(GlobalData::s_stream_pool->Acquire("user.cpp", 0x75c));
            else
                guard.Set(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x760));

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
#if !remake_verify
            cMyTrace(__PRETTY_FUNCTION__, 0x7f2, 5)(
                "[DNF-DIAG PARSING-RET] site=3 parsing-length-exception remain=%d",
                remain);
#endif
            return false;
        }
        memmove(m_network.m_recv, m_network.m_recvBuf, remain);
        m_network.m_recvOffset = remain;
        m_network.m_recvBuf = m_network.m_recv + remain;
    }
    return true;
}