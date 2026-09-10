int Dispatcher_WriteGuildMemberMemo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821e748: state>2 且角色非空
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xd55c, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x068: 无公会门
    unsigned int guildKey = static_cast<unsigned int>(user->get_charac_guildkey());
    if (guildKey == 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xe3), 0x22);
        return 0;
    }

    // +0x09b: char memo[0x15] 聚合初始化（ORIG 内联展开，非 call memset）
    char memo[21] = {0};

    // +0x110: 读 memo 长度 + 内容
    int memoLen = 0;
    if (!packet.get_int(memoLen))
    {
        return LineFunc(0xd56a, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_str(memo, 0x15, memoLen))
    {
        return LineFunc(0xd56d, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x1a9: 敏感词/特殊字符过滤（ORIG `||` 布尔合并）
    if (G_CDataManager()->hasPreventString(memo) ||
        G_CDataManager()->hasSpecialCharacterExceptSpace(memo))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xe3), 0x6c);
        return 0;
    }

    // +0x20d: 发公会备忘录（ORIG 链式调用不缓存 proxy）
    int characNo = user->get_charac_no(-1);
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendMemoToGuild(guildKey, characNo, memoLen, memo);

    // +0x25c: 0xe3 成功回执
    PacketGuard pg;
    pg.put_header(1, 0xe3);
    pg.put_byte(1);
    pg.finalize(true);
    user->Send(pg);
    return 0;
