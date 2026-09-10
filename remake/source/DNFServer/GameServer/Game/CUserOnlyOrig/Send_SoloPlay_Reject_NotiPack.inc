void CUser::Send_SoloPlay_Reject_NotiPack(const char* msg, ENUM_SOLO_PLAY_REJECT_MODE mode,
                                          ENUM_SOLO_PLAY_REJECT_MSG code)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x17d);
    // ORIG 0x0868e0ee：`cmpl $0,0xc; je <put0 岛>` ⇒ strlen 体 fall-through 在前（技巧17）
    if (msg != 0) {
        packet.put_int(strlen(msg));
        packet.put_str(msg, strlen(msg));
    } else {
        packet.put_int(0);
    }
    packet.put_int(mode);
    packet.put_int(code);
    packet.finalize(true);
    Send(packet);
