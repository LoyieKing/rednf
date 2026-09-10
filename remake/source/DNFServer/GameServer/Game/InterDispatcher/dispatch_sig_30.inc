int Inter_GenCeraTicket::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0;

    int mid;
    memcpy(&mid, data, 4);
    char name[32];
    // 窗口指针形态保留（具名视图会使 strncpy 实参经 pPkt 局部多一代指令，
    // 破坏本函数 AE 对齐；ORIG 084ceea2 直接 disp(%reg) 传址）。
    strncpy(name, data + 4, 0x20);

    PacketGuard packet;
    packet.put_header(1, 0x44);
    packet.put_byte(1);
    packet.put_int(strlen(name));
    packet.put_str(name, strlen(name));
    packet.put_int(mid);
    packet.finalize(true);
    user->Send(packet);
    return 0;
