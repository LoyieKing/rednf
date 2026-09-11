int CExpandEquipslot::Send_Expand_Equip_Info(CUser* pUser)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__, 0x21b,
            "Fail Send_Expand_Equip_Info : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__, 0x221,
            "Fail Send_Expand_Equip_Info : pUser is NULL");
        return 0x13;
    }
    PacketGuard packetGuard;
    packetGuard.clear();
    packetGuard.put_header(0, 0x153);
    packetGuard.put_short(pUser->get_unique_id());
    for (unsigned char d = 1; d < 4; ++d)
    {
        if (d == 1)
        {
            if (pUser->getCurChannelEquipslotSwitch() == 1)
            {
                packetGuard.put_byte(0);
            }
            else
            {
                packetGuard.put_byte((int)d);
            }
        }
        else
        {
            packetGuard.put_byte((int)d);
        }
        pUser->getCurCharacInvenR()->MakeEquipList(
            &packetGuard, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)d);
    }
    packetGuard.finalize(true);
    pUser->Send(packetGuard);
    return 0;
}