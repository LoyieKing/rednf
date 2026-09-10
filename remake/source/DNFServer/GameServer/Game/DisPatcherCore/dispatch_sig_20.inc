int DisPatcher_EnterPVP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ed6c8 T（报告 DisPatcher_EnterPVP/dispatch_sig.md 全文转写）：
    // get_short(pvpIdx) → 清 flag/seat/buf[9] → get_byte(flag)，flag==1 时
    // get_int(seat)（>0xa→LineFunc）+ get_binary(buf,seat)；
    // GetPvp(idx,user,0)==0 → 错误包(1,0x36)+0+0x16；否则 EnterPVPRoom(user,pvp,0x36,buf)。
    short pvpIdx;
    if (!packet.get_short(pvpIdx))
    {
        return LineFunc(0x6fc7, __PRETTY_FUNCTION__, 0, 0);
    }
    char flag = 0;              // ORIG movb $0x0,-0x1c
    int seat = 0;               // ORIG movl $0x0,-0x20
    char buf[9] = { 0 };        // ORIG movl/movl/movb 内联清零（-0x29..-0x21）
    if (!packet.get_byte(flag))
    {
        return LineFunc(0x6fce, __PRETTY_FUNCTION__, 0, 0);
    }
    if (flag == 1)
    {
        if (!packet.get_int(seat))
        {
            return LineFunc(0x6fd2, __PRETTY_FUNCTION__, 0, 0);
        }
        if (seat > 0xa)         // ORIG cmp $0xa; jle 通过
        {
            return LineFunc(0x6fd4, __PRETTY_FUNCTION__, 0, 0);
        }
        if (!packet.get_binary(buf, seat))
        {
            return LineFunc(0x6fd7, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    PvP_Room* pvp = G_CGameManager()->GetPvp((int)pvpIdx, user, 0);
    if (pvp == 0)
    {
        PacketGuard guard;
        guard.put_header(1, 0x36);
        guard.put_byte(0);
        guard.put_byte(0x16);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    EnterPVPRoom(user, pvp, (ENUM_CMDPACKET)0x36, buf);
    return 0;
