int Dispatcher_EnterPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    bool gate;
    short sellerIndex;
    private_store::IBuyer* buyer;
    private_store::CPrivateStoreMgr* mgr;
    bool ok;

    if (user->get_state() == 3)
    {
        if (user->getCurCharacR() != 0)
            gate = false;
        else
            gate = true;
    }
    else
    {
        gate = true;
    }
    if (gate)
    {
        LineFunc(0x98d4,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    sellerIndex = 0;
    buyer = 0;
    ok = packet.get_short(sellerIndex);
    if (!ok)
    {
        LineFunc(0x98d8,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    mgr = private_store::GetInstancePrivateStoreMgr();
    mgr->GetStoreBuyer(&buyer, user, (unsigned short)sellerIndex);
    buyer->EnterPrivateStore();
    return 0;
