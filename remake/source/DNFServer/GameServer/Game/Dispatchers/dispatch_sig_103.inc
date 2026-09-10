int Dispatcher_RequestAddPvPBuddy::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 0x08224928：name 清零（-0x36，4.1 内联慢路）先于 type/nameLen 初始化。
    char name[0x1e] = {0};                           // -0x36（ORIG 清零 0x1e 字节）
    char type = 0;                                   // -0x37
    int nameLen = 0;                                 // -0x3c
    if (!packet.get_byte(type))
        return LineFunc(0xdf3c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(nameLen))
        return LineFunc(0xdf3f, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(name, 0x1e, nameLen))
        return LineFunc(0xdf42, __PRETTY_FUNCTION__, 0, 0);
    GlobalData::s_community_proxy->SendReqAddPvPBuddy(
        user->get_acc_id(),
        (unsigned int)user->getCurCharacNo(),
        type,
        name,
        nameLen);
    return 0;
