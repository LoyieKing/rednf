int Dispatcher_UseRenameCard::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e7652（报告 Dispatcher_UseRenameCard/check_error.md 全文已读）：
    // channelType@-0x14（G_GameWorld() 链式单表达式初始化，无 world 局部）；
    // charac@-0x10；成功路径尾部 msg 死存储@-0xc。
    if (user->get_state() != 3)
    {
        return 0x13;
    }

    if (user->CheckInTrade())
    {
        return 0x13;
    }

    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return 0x13;
    }

    const _Charac_info* charac = user->getCurCharacR();
    if (charac == 0)
    {
        return 0x13;
    }

    if (!user->IsWaitingStateCharac((unsigned int)charac->m_characNo))
    {
        return 0x12;
    }
    const MSG_BASE* pReq = &msg;   // ORIG 死存储 -0xc(%ebp)
    (void)pReq;
    return 0;
}