void GameWorld::out_from_dungeon(CUser* user)
{
    user->SetRevivalTime(0);
    user->SetUseAPCPotionTime(0);
    // ORIG 0x086c6a16：+0x3c setne（`!= 1` then 在前）；+0x7d xor$1（`!check_valid_area` log 在前）
    if (user->getMoveSpace() != 1)
    {
        if (!check_valid_area((int)user->getCurCharacVill(),
                              user->get_area(false)))
        {
            // ORIG +0x084..+0x0ba：name 直写 0x14(%esp)（单语句无跨调用缓存，勿加 register）
            LogManager::logFormat(
                1, "world.cpp", __PRETTY_FUNCTION__, 0x926,
                "user %s - check_valid_area() fail", user->getCurCharacName());
        }
        else
        {
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
            user->SendFatigue();
            static_cast<XNuclear::CHades*>(user->getHades())->Send_ReturnToVillage();
        }
    }
    else
    {
        user->SendFatigue();
        user->set_state((ch_state)0xb);
        PacketGuard packet;
        packet.put_header(0, 3);
        packet.put_byte(1);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte(7);
        packet.finalize(true);
        user->Send(packet);
    }
}