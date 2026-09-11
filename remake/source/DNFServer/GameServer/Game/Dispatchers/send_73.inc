void Dispatcher_GrowthWeaponEventUseMaterial::send(CUser* user, ParamBase& param)
{
    stGrowthEquipment& out = static_cast<stGrowthEquipment&>(param);
    if (out.m_4 == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x1fa);
        packet.put_byte(1);
        packet.put_int(out.m_8);
        packet.put_int(out.m_c);
        packet.put_short((int)out.m_10);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fa,
                                 (unsigned char)(out.m_4 & 0xff));
    }
}