int Dispatcher_RequestPCRoomPlayerList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        return 0;
    }
    const unsigned int accountId = user != 0 ? user->get_acc_id() : 0;
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xd14e, 5);
    trace("PCROOM : Dispatcher_RequestPCRoomPlayerList => User State : %d, m_id : %s",
          user != 0 ? user->get_state() : 0, NumberToString(accountId, 0));
    return LineFunc(0xd14f,
                    __PRETTY_FUNCTION__,
                    0, 0);
