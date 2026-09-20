int Dispatcher_MailBox_Extract::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MAILBOX_EXTRACT_ITEM& req = *reinterpret_cast<MSG_MAILBOX_EXTRACT_ITEM*>(&msg);
    if (!packet.get_int(req.m_mailIdx))
    {
        return LineFunc(0x244c, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
