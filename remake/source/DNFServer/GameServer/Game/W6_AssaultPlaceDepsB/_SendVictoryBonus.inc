// ORIG T 0x82ecdbc _SendVictoryBonus(CUser*, int, int, int, int)
void CAssaultPlace::_SendVictoryBonus(CUser* user, int a, int b, int c, int d)
{
    PacketGuard packet;
    packet.put_header(0, 0x108);
    packet.put_int(a + 1);
    packet.put_int(b);
    packet.put_int(c);
    packet.put_int(d);
    packet.finalize(true);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
        party->send_to_party(packet);
    else
        user->Send(packet);
}