// ORIG 0x0821eed2（报告 Dispatcher_GiveupExpertJob/dispatch_sig.md）：
// state<=2 或 curCharac==0 → LineFunc 0xd797；否则 s_ExpertJobMgr->OnGiveup。
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
}