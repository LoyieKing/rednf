void GameWorld::outFromBlueMarble(CUser* user)
{
    if (!check_valid_area((int)user->getCurCharacVill(),
                          user->get_area(false)))
    {
        // ORIG +0x50 mov %eax,%ebx / +0x75 mov %ebx,0x8(%esp)：P37 链式临时提前实参
        cMyTrace(__PRETTY_FUNCTION__, 0xa7f, 0)(
            "user %s - check_valid_area() fail", user->getCurCharacName());
        user->DisConnSig((DISCONN_SIG)3, 1, 0);
        return;
    }

    user->setBlueMarbleIndex(-1);
    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        Area* area = village->getArea(user->get_area(false));
        if (area != NULL)
        {
            area->insert_user(user);
        }
    }
    PacketGuard packet;
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte(0);
    packet.finalize(true);
    send_all(packet, user);
}