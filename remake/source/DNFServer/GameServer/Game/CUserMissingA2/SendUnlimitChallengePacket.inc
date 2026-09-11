// ----------------------------------------------------------------------------
// ORIG T 0x08686f58 _ZN5CUser26SendUnlimitChallengePacketEv：
// PacketGuard：header(0,0xa4) + byte(1) + finalize(true) + Send。
// ----------------------------------------------------------------------------
void CUser::SendUnlimitChallengePacket()
{
    PacketGuard pkt;
    pkt.put_header(0, 0xa4);
    pkt.put_byte(1);
    pkt.finalize(true);
    Send(pkt);
}