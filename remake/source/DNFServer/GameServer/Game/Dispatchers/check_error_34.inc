unsigned int Dispatcher_MailBox_Extract::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081cceea：!（state==3 && charac!=0）取反早退岛在入口（0x81ccf1f），
    // dead msg 绑定@0x81ccf52（IsBusy call 前、-0xc 槽、帧 0x28）。
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    if (user == 0 || user->CheckInTrade())
    {
        return -1;
    }
    MSG_BASE* pm = &msg;
    (void)pm;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (user->CheckInTrade())
    {
        return 0xda;
    }
    return 0;
