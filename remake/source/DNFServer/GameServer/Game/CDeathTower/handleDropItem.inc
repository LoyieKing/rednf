int CDeathTower::handleDropItem(CUser* user, int a2, int a3, char a4, int a5, int a6)
{
    if (a6 < 0)
    {
        return 0;
    }

    map_item item;
    CPacketHandler packet;

    if (!_makeDropItem(user, a4, a5, a6, item))
    {
        packet.makeDropItemError((ENUM_CMDPACKET)0x32, (ENUM_ERROR)0x11, a4);
        user->Send(packet);
        return 0;
    }

    int dropIdx = m_stage.dropItem(item);
    if (dropIdx == -1)
    {
        packet.makeDropItemError((ENUM_CMDPACKET)0x32, (ENUM_ERROR)0x16, a4);
        user->Send(packet);
        return 0;
    }

    // [b28d-solo-CDeathTower] P0：ORIG 0x08465a.. 为 `mov -0xc(%ebp),%eax;
    // movzwl %ax,%ebx`（dropIdx 槽直读截断，无 idx16 具名槽）⇒ 内联实参。
    packet.makeDropItemNotify((unsigned short)user->get_unique_id(), a2, a3,
                              (unsigned short)dropIdx, item);
    m_party->send_to_party(packet);

    packet.clear();
    packet.makeDropItem(a4, a5, a6);
    user->Send(packet);
    return 1;
