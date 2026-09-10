bool CCreature::Die(CUser* user)
{
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketDiedCreature();
    pkt.SendAll();
    return true;
