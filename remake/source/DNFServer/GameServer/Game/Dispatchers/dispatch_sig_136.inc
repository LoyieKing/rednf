int Dispatcher_OpenGuildCreateWindow::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 ||
        user->getCurCharacR() == 0)
    {
        return LineFunc(0xa3e3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    user->resetGuildCreateFlag();
    user->resetTempGuildNameAddr();
    return 0;
}