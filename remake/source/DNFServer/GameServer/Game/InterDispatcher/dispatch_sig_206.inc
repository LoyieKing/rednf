// [b28d-lane2] verdict=DIFF ndiff=100（ORIG 0x084dd69e）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=49（ORIG 0x84dd69e）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_GuildCargoHistory::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoHistoryPktView* pPkt = reinterpret_cast<Inter_GuildCargoHistoryPktView*>(data);
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_characNo)         // msg+0x0e 角色号校验
        return 0;

    // ORIG 084dd6f9 `movl $0x0,-0x18(%ebp)`（在 putCount 的 084dd700 之前、PacketGuard
    //   构造 084dd707 之前）⇒ 原码 countIdx 声明即预清 0，且声明位次在 packet 之前。
    int countIdx = 0;                                // local_18/asm -0x18(%ebp)
    int putCount = 0;                                // local_14/asm -0x10(%ebp)
    PacketGuard packet;
    packet.put_header(1, 0x10a);
    packet.put_byte(1);
    // （asm("ebx") 钉形被 put_int(int&,int) 取址阻断，保持具名槽。）
    countIdx = packet.get_index();             // 计数回填位（ORIG 084dd74b `mov %eax,-0x18(%ebp)`）
    packet.put_int(putCount);
    for (int i = 0; i <= 0x31; ++i)                  // asm cmpl $0x31, setle
    {
        // [G2A-PC 手法1] ORIG 084dd76c `mov -0x14(%ebp),%ecx` 取基址，而 -0x14 即 pPkt 槽
        //   （084dd6a6 `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` 直证）⇒ 原码两处基址
        //   都用 pPkt 具名指针；我方旧写形参 `data + ...` 会发 `add 0x10(%ebp),%eax`
        //   （形参直读，见本 diff N 侧），与 ORIG 084dd792 `add -0x14(%ebp),%eax` 不符。
        if (*reinterpret_cast<int*>(pPkt->m_recs[i].m_blob) != 0)
        {
            packet.put_binary(
                pPkt->m_recs[i].m_blob, 0x30);
            ++putCount;
        }
    }
    packet.put_int(countIdx, putCount);              // put_int(int&,int) 回填
    packet.finalize(true);
    user->Send(packet);
    user->m_historyLog.GuildCargoHistoryResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)4, putCount);
    return 0;
}