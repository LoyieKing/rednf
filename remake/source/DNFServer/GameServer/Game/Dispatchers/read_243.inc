int Dispatcher_Delete_GuildBoard_Text::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG（帧 $0x18）：读 unsigned int(msg+0x89)，无指针物化槽 ⇒ 内联。
    // msg+0x89 = MSG_GUILD_BOARD::m_info(+0xd, pack1) 的 STGuildBoardDBInfo::m_7c(+0x7c)。
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(
            &reinterpret_cast<MSG_GUILD_BOARD&>(msg).m_info.m_7c)))
    {
        return LineFunc(0x3101, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
