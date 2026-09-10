int CExpandEquipslot::Send_Equip_Info(CUser* pUser)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__, 0x9a,
            "Fail Send_Equip_Info : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__, 0xa0,
            "Fail Send_Equip_Info : pUser is not valid state");
        return 0x13;
    }
    PacketGuard packetGuard;
    packetGuard.clear();
    packetGuard.put_header(0, 2);
    packetGuard.put_byte(1);
    pUser->make_basic_info((char*)&packetGuard, 1);
    packetGuard.finalize(true);
    pUser->Send(packetGuard);
    return 0;
