int Dispatcher_ResponseCreature::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9acf,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    user->getCurCharacInvenW()->GetCreatureMgrW()->ResponseCreature();
    return 0;
