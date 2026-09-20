int Dispatcher_UpdateContractOfCubeInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_UPDATE_CONTRACT_OF_CUBE_INFO& req = *reinterpret_cast<MSG_UPDATE_CONTRACT_OF_CUBE_INFO*>(&msg);
    if (!packet.get_byte(req.m_useFlag))
        return LineFunc(0x95e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(req.m_selectIdx))
        return LineFunc(0x961, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
