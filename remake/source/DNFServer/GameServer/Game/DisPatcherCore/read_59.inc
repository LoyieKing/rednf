int DisPatcher_AddFriend::read(PacketBuf& packet, MSG_BASE& msg)

{
    STFriendReqView* pMsg =
        reinterpret_cast<STFriendReqView*>(&msg);   // ORIG -0xc(%ebp)
    if (!packet.get_int(pMsg->m_nameLen))           // +0x0d
    {
        return LineFunc(0x207b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(pMsg->m_name, 0x1e, pMsg->m_nameLen))
    {
        return LineFunc(0x2082,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
