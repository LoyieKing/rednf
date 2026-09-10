int Dispatcher_VerifyPrivateStoreItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xe740,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x3d);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55, (unsigned char)(secuErr & 0xff));
        return 0;
    }
    if (user->isHackUser() || user->isHackUserTimer())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x195, 0x11);
        return 0;
    }
    short slot = 0;
    int itemIdx = 0;
    private_store::IBuyer* buyer = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(0xe76a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xe76b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (itemIdx < 0)
    {
        return LineFunc(0xe76c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    private_store::CPrivateStoreMgr* mgr = private_store::GetInstancePrivateStoreMgr();
    mgr->GetStoreBuyer(&buyer, user, (CUser*)0);
    buyer->VerifyPrivateStoreItem((int)slot, itemIdx);
    return 0;
