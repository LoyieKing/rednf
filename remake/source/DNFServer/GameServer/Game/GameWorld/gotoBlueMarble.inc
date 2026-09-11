void GameWorld::gotoBlueMarble(CUser* user)
{
    if (!check_valid_area((int)user->getCurCharacVill(),
                          user->get_area(false)))
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0xa54,
                              "user %s - check_valid_area() fail",
                              user->getCurCharacName());
        return;
    }

    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        Area* area = village->getArea(user->get_area(false));
        if (area != NULL)
        {
            area->erase_user(user);
        }
    }
    PacketGuard packet;
    packet.put_header(0, 0x17);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte((int)user->getCurCharacVill());
    packet.put_byte(0xfe);
    packet.put_short((unsigned short)user->get_posX());
    packet.put_short((unsigned short)user->get_posY());
    packet.put_byte((int)user->get_direction());
    packet.put_byte((int)user->get_charac_visible_values());
    packet.finalize(true);
    user->Send(packet);
    user->set_state((ch_state)0xb);
    packet.clear();
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte(7);
    packet.finalize(true);
    send_all(packet, user);
}