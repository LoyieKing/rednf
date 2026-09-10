bool CUser::Send(PacketGuard& packet)   // b28c-L5 SIGFIX：ORIG 串 bool；
                                        // 调用点 0x082e6c2b `xor$1;test%al` bool callee 直判
{
    PacketBuf* pBuf = ((InterfacePacketBuf*)&packet)->get();
    if (pBuf->is_finallized() == 0)
    {
        unsigned short packetId = *((unsigned short*)pBuf->get_packet(1));
        const char* name = getCurCharacName();
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__, 0x8a0,
                              "User %s - Packet.is_finallized packet_id(%d) return false",
                              name, (int)packetId);
        return 0;
    }
    // ORIG 0x086486e5/0x086486ea：两个死 PacketBuf::at() 直调（结果丢弃，P184 死直调保留）
    pBuf->at(0);
    pBuf->at(1);
    unsigned short packetId = *((unsigned short*)pBuf->get_packet(1));
    unsigned char area = pBuf->at(0);
    GetPacketName((ENUM_PACKETCLASS)area, packetId);
    unsigned char b = pBuf->at(1);
    if (b != 6)
    {
        // ORIG 块内 at(1)/at(0) 各重调一次，非复用 b/area
        m_pad8cf0c[0] = pBuf->at(1);
        m_pad8cf0c[1] = pBuf->at(0);
    }
    return AntibotSend(*pBuf);
