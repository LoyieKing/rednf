int Dispatcher_QueryCharacInfoMailbox::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return LineFunc(0xe289, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x08226c3a/0x08226c41：先 nameLen=0（-0xc）再 name 清零（-0x2a）。
    int nameLen = 0;                                 // -0xc
    char name[0x1e] = {0};                           // -0x2a（ORIG 内联慢路，N78）
    if (!packet.get_int(nameLen))
        return LineFunc(0xe28e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(name, 0x1e, nameLen))
        return LineFunc(0xe28f, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x08226d29/0x08226d36/0x08226d47：trim 只调一次且返回值直接作 strncmp 第 1 参
    // （trim 返回 char*，ORIG `call trim; mov %eax,(%esp)`）；curName 先入 %ebx（右→左实参序）。
    if (strncmp(trim(name), user->getCurCharacName(), 0x1d) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x145, 7);
        return 0;
    }
    DB_ReqQueryCharacInfoMailbox::makeRequest((int)user->GetUID(), name);
