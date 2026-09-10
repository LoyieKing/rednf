int Inter_Notice::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    struct NoticePacketView
    {
        char m_pad[0x51];
        char m_len;
    };
    char* packet = data;
    G_GameWorld()->send_chat_msg(0, 0, 0, 0, packet + 1,
                                 static_cast<int>(reinterpret_cast<NoticePacketView*>(packet)->m_len), 0, 0,
                                 static_cast<ENUM_NOTIPACKET>(0x0c));
    return 0;
