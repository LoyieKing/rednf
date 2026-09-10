void Dispatcher_Expand_Equipslot_Flag_Update::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d5fae：clear+header(1,0x18e)+byte 1+byte(char)p4
    stExpandEquipslotFlagUpdate& p = static_cast<stExpandEquipslotFlagUpdate&>(param);
    PacketGuard pkt;
    pkt.clear();
    pkt.put_header(1, 0x18e);
    pkt.put_byte(1);
    pkt.put_byte((int)(char)p.m_4);
    pkt.finalize(true);
    user->Send(pkt);
