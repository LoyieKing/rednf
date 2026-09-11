void GameWorld::goto_dungeon(CUser* user)
{
    if (user == NULL)
    {
        return;
    }
    if (user->getMoveSpace() != 1)
    {
        Village* village = getUserVillage(user);
        if (village == NULL)
        {
            // ORIG +0x051 movsbl→%ebx / +0x077 mov %ebx,0x8(%esp)：P37 链式临时提前实参
            cMyTrace(__PRETTY_FUNCTION__, 0x8d1, 5)("%d", (int)user->getCurCharacVill());
            return;
        }
        Area* area = village->getArea(user->get_area(false));
        if (area == NULL)
        {
            // ORIG +0x0d1 mov %eax,%ebx / +0x0f6 mov %ebx,0x8(%esp)
            cMyTrace(__PRETTY_FUNCTION__, 0x8d8, 5)("%d", user->get_area(false));
            return;
        }
        area->erase_user(user);

        PacketGuard packet;
        packet.put_header(0, 0x17);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte((int)user->getCurCharacVill());
        packet.put_byte(0xff);
        packet.put_short((unsigned short)user->get_posX());
        packet.put_short((unsigned short)user->get_posY());
        packet.put_byte((int)user->get_direction());
        packet.put_byte((int)user->get_charac_visible_values());
        packet.finalize(true);
        user->Send(packet);
    }
    user->set_state((ch_state)5);
}