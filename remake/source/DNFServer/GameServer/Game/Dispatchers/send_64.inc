void Dispatcher_UpdateContractOfCubeInfo::send(CUser* user, ParamBase& param)
{
    // ORIG 0x0825f9ae：header(1,0x20e)+byte(p4==0)+byte p8+byte p9
    stUpdateContractOfCubeInfo& p = static_cast<stUpdateContractOfCubeInfo&>(param);
    PacketGuard pkt;
    pkt.put_header(1, 0x20e);
    pkt.put_byte(p.m_4 == 0 ? 1 : 0);
    pkt.put_byte(p.m_8);
    pkt.put_byte(p.m_9);
    pkt.finalize(true);
    user->Send(pkt);
}