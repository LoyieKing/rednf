int Dispatcher_GiveupExpertJob::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xd797,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    GlobalData::s_ExpertJobMgr->OnGiveup(user);
    return 0;
