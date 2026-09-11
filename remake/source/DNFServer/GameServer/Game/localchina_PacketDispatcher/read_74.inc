int DisPatcher_EventDungeon_DestoryObject::read(PacketBuf& packet, MSG_BASE& msg)

{
    // ORIG 0x0812bd54：pBuf 局部落栈（-0xc）后 get_binary(pBuf, 0x18)，恒返 0。
    // *(&pBuf) 实参使 reload 紧跟 spill（ORIG +0x0c），复现 4.4.7 求值序。
    char* pBuf = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    packet.get_binary(*(&pBuf), 0x18);
    return 0;
}
