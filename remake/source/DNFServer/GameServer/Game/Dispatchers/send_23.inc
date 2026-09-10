void Dispatcher_Equipslot_Switch::send(CUser* user, ParamBase& param)
{
    stEquipslotSwitch& out = static_cast<stEquipslotSwitch&>(param);
    if (out.m_result == 0)
    {
        CExpandEquipslot* expand =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        expand->Send_Equip_Info(user);
        expand->Send_Expand_Equip_Info(user);
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x18d);
    // ORIG 0x0822?: `movzbl 0x4(%eax),%eax; test %al,%al; je OK岛`——非零段 fall 在前（技巧17）。
    if (out.m_result != 0)
    {
        packet.put_byte(0);
        packet.put_byte((char)out.m_result);
    }
    else
    {
        packet.put_byte(1);
    }
    packet.finalize(true);
    user->Send(packet);
