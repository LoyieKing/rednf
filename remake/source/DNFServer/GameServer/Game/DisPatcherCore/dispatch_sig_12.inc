int DisPatcher_Lottery::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        PacketGuard guard;
        guard.put_header(1, 0x1d);
        guard.put_byte(0);
        guard.put_byte(0xda);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    short idx;                                 // ORIG msg 视图未定（-0x1f 基址，见上）
    if (!packet.get_short(idx))
    {
        return LineFunc(0x861c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int result = user->lottery_item((int)idx);
    if (result < 1)
    {
        if (user->getLastLotteryTime() != 0)
        {
            int last = user->getLastLotteryTime();
            unsigned int diff = GlobalData::s_systemTime_.getCurSec() - last;
            if (diff <= 2)
            {
                WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
                hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x334, 1, 0, 0);
            }
        }
        user->setLastLotteryTime(GlobalData::s_systemTime_.getCurSec());
        return 0;
    }
    PacketGuard guard;
    guard.put_header(1, 0x1d);
    guard.put_byte(0);
    guard.put_byte(result);
    guard.finalize(true);
    user->Send(guard);
    return 0;
