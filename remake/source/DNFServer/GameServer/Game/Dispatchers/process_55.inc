int Dispatcher_TitleBookGet::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* pMsg = (char*)&msg;

    if (user->get_state() <= 2)
    {
        return 0;
    }

    // getTitle(user, category=+0x19, slot=+0x1d, itemIdx=+0x15, space=+0xd,
    //          outSlot=&+0x11)
    int ret = CTitleBook::getTitle(
        user,
        (ENUM_TITLE_BOOK_CATEGORY)*(int*)(pMsg + 0x19),
        *(int*)(pMsg + 0x1d),
        *(int*)(pMsg + 0x15),
        (ENUM_ITEMSPACE)*(int*)(pMsg + 0x0d),
        *(int*)(pMsg + 0x11));

    if (ret == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x19d);
        packet.put_byte(1);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x11));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.put_int(*(int*)(pMsg + 0x1d));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x19d);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
