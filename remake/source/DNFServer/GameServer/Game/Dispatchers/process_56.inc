int Dispatcher_TitleBookPut::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_TITLEBOOK_PUT& m = reinterpret_cast<MSG_TITLEBOOK_PUT&>(msg);

    if (user->get_state() <= 2)
    {
        return 0;
    }

    // putTitle(user, space=+0xd, slot=+0x11, itemIdx=+0x15, category=+0x19,
    //          titleSlot=+0x1d)
    int ret = CTitleBook::putTitle(
        user,
        (ENUM_ITEMSPACE)m.m_itemSpace,
        m.m_field11,
        m.m_field15,
        (ENUM_TITLE_BOOK_CATEGORY)m.m_field19,
        m.m_field1d);

    if (ret == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x19c);
        packet.put_byte(1);
        packet.put_int(m.m_itemSpace);
        packet.put_int(m.m_field11);
        packet.put_int(m.m_field19);
        packet.put_int(m.m_field1d);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x19c);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.put_int(m.m_itemSpace);
        packet.put_int(m.m_field19);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
