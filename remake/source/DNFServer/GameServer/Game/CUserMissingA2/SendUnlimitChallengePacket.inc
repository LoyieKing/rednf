void CUser::SendUnlimitChallengePacket()
{
    PacketGuard pkt;
    pkt.put_header(0, 0xa4);
    pkt.put_byte(1);
    pkt.finalize(true);
    Send(pkt);
