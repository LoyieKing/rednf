// ORIG _ZN13user_creature9CCreature3DieEP5CUser T 0x8338536（Die.md）：
// CPacketRespondent(user,0,0).MakeNotipacketDiedCreature() + SendAll()，恒返 1。
bool CCreature::Die(CUser* user)
{
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketDiedCreature();
    pkt.SendAll();
    return true;
}