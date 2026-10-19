// ---- 发包族（PacketGuard 局部 + InterfacePacketBuf） ----
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086485ba | _ZN5CUser4SendER11PacketGuard
bool CUser::Send(PacketGuard& packet)   // b28c-L5 SIGFIX：ORIG 串 bool；
                                        // 调用点 0x082e6c2b `xor$1;test%al` bool callee 直判
{
    // 2026-09-10 gA（A 组）P0/P87/P183 三轴：
    //  · ORIG 0x086485da `test %al,%al; je 086486ab`（**无 xor$1**，成功块 fall-through、
    //    失败块在 0x086486ab）⇒ 源码是正向 if/else，非 `if (!f())`（P168a③）。
    //  · 失败块 ORIG 0x086486be `movzwl (%eax),%eax; movzwl %ax,%ebx`（packetId 无槽、
    //    跨 getCurCharacName 保活 %ebx）、name 直留 %eax ⇒ 删具名 packetId/name。
    //  · 成功块 ORIG 0x0864861e packetId 保活 %ebx、0x08648634 `movzbl %al,%eax`（area
    //    直传）、0x08648658 `cmp $0x6,%al; setne %al; test %al,%al; je`（b 直判）
    //    ⇒ 删具名 area/b，全部实参内联。
    PacketBuf* pBuf = ((InterfacePacketBuf*)&packet)->get();
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG  // remake-only diag; off unless -DDNF_DIAG=1
    if (pBuf->is_finallized())
    {
        const unsigned char* hp =
            reinterpret_cast<const unsigned char*>(pBuf->get_packet(0));
        {
            const int dn = 80;
            const int n = pBuf->get_len() < dn ? pBuf->get_len() : dn;
            char hx[3 * 80 + 1];
            hx[0] = 0;
            for (int k = 0; k < n; ++k)
            {
                char b[4];
                b[0] = "0123456789abcdef"[(hp[k] >> 4) & 0xf];
                b[1] = "0123456789abcdef"[hp[k] & 0xf];
                b[2] = ' ';
                b[3] = 0;
                for (int j = 0; j < 3; ++j) hx[k * 3 + j] = b[j];
            }
            hx[n * 3] = 0;
            cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
                "[DNF-DIAG OUT] class=%d id=%u size=%u len=%d ra=%p hex=%s",
                (int)hp[0],
                (unsigned int)(unsigned short)(hp[1] | (hp[2] << 8)),
                (unsigned int)(unsigned short)(hp[3] | (hp[4] << 8)),
                (int)pBuf->get_len(),
                __builtin_return_address(0),
                hx);
        }
    }
#endif
    if (pBuf->is_finallized())
    {
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG
        const unsigned char* hp = reinterpret_cast<const unsigned char*>(pBuf->get_packet(0));
        const int sendResult = AntibotSend(*pBuf);
        cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
            "[PACKET_DIAG] send acc=%u cls=%u cmd=%u len=%d result=%d",
            (unsigned)m_accId, (unsigned)hp[0],
            (unsigned)(unsigned short)(hp[1] | (hp[2] << 8)),
            pBuf->get_len(), sendResult);
        return sendResult;
#else
        // Original finalized packet send.
        return AntibotSend(*pBuf);
#endif
    }
    LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__, 0x8a0,
                          "User %s - Packet.is_finallized packet_id(%d) return false",
                          getCurCharacName(),
                          (int)*((unsigned short*)pBuf->get_packet(1)));
    return 0;
}