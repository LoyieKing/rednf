int DisPatcher_EventDungeon_ClearRoom::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pBuf = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    packet.get_binary(*(&pBuf), 0x14);            // 同上：reload 先于 len 入栈
    return 0;
