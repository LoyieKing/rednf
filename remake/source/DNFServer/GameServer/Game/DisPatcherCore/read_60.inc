int DisPatcher_RemoveFriend::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_ADD_FRIEND* pMsg =
        reinterpret_cast<MSG_ADD_FRIEND*>(&msg);   // ORIG -0xc(%ebp)
    if (!packet.get_int(pMsg->m_nameLen))           // +0x0d
    {
        return LineFunc(0x2115,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(pMsg->m_name, 0x1e, pMsg->m_nameLen))
    {
        return LineFunc(0x211c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
