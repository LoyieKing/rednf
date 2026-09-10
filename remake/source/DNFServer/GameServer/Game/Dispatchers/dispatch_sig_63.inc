int Dispatcher_DisguiseRequest::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 3)
    {
        return 0;
    }
    private_store::CPrivateStoreMgr* manager =
        private_store::GetInstancePrivateStoreMgr();
    if (manager != 0 && manager->IsBusyPrivateStore(user))
    {
        return 0;
    }
    if (user->IsCurCharacGhost() != 0 || user->GetAssaultPlace() != 0)
    {
        return 0;
    }
    unsigned char kind = 0;
    if (!packet.get_byte(kind))
    {
        return LineFunc(0xcf00,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned short index = 0;
    if (!packet.get_short(index))
    {
        return LineFunc(0xcf02,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->isDisguiseCharac() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd6, 0x12);
        return 0;
    }
    user->setDisguiseCharac(true, kind, index);
    PacketGuard notification;
    notification.put_header(0, 2);
    notification.put_byte(0);
    notification.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
    notification.finalize(true);
    G_GameWorld()->send_all(notification, user);
    return 0;
