void Dispatcher_PrecheckSoloTeleport::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e1acc：p4!=0 → clear+header(1,0x1d6)+byte 0+byte(char)p4
    stSoloTeleportParam& p = static_cast<stSoloTeleportParam&>(param);
    if (p.m_4 == 0)
    {
        return;
    }
    {
        PacketGuard pkt;
        pkt.clear();
        pkt.put_header(1, 0x1d6);
        pkt.put_byte(0);
        pkt.put_byte((int)(char)p.m_4);
        pkt.finalize(true);
        user->Send(pkt);
    }
}