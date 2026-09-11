void GameWorld::make_packet_chat_msg_hyper_link(PacketGuard& packet, CUser* user,
                                                MSG_ITEM_HYPER_LINK_MESSAGE& msg,
                                                ENUM_NOTIPACKET notify)
{
    packet.clear();
    // ORIG 86cf…：对 msg 的每次读取均为 `mov 0x14(%ebp),%eax` 直用来参槽
    // ⇒ 4.4 中“引用参无局部别名绑定”时不落独立槽；显式 `T& msg = linkMsg;` 会物化 -0x10 槽
    packet.put_header(0, (int)notify);
    packet.put_byte(msg.m_typeD);
    if (user == 0)
    {
        packet.put_short(0);
        packet.put_byte(0);
    }
    else
    {
        packet.put_short(user->get_unique_id());
        packet.put_byte(user->GetServerGroup());
    }
    packet.put_int(msg.m_textLen14);
    packet.put_str(msg.m_text18, msg.m_textLen14);
    packet.put_byte(msg.m_linkCount13b);
    for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
    {
        packet.put_binary((char*)msg.m_links13c[i], 0x68);
    }
    packet.finalize(true);
}