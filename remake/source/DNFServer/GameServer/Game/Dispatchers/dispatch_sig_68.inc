int Dispatcher_ReportAbuseUser::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xd7a1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG：mgr 无槽（GetInstanceAssaultMgr 返回值直传 %eax 作 this），bool 返回 ! 形 xor
    if (!pvp_assault::GetInstanceAssaultMgr()->OnAbuseUser(user))
    {
        return LineFunc(0xd7b1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
