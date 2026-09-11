int Dispatcher_ItemDictionary::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BASE* pm = &msg;   // ORIG：msg→-0x4 死槽 + sub $0x10 + leave（技巧8）
    (void)pm;
    return 0;
}