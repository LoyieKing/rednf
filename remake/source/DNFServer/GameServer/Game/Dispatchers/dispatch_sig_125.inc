int Dispatcher_CallGuildCreateRight::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ffcc2..0x08200125 (0x464 字节)。packet 未读取（ORIG 不消费包）。

    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return LineFunc(0x93f4, __PRETTY_FUNCTION__, 0, 0);

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x1c))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0xd1);
        return 0;
    }

    const unsigned int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)6);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, (unsigned char)(secuErr & 0xff));
        return 0;
    }

    // 公会名（getCreateGuildName 返回 char*，构造局部 std::string）
    std::string guildName(user->getCreateGuildName());

    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(guildName.c_str()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }
    if (CSyncSlangFilter::GetInstance()->HasSlangName(guildName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }

    bool badName = false;
    if (CSyncSlangFilter::GetInstance()->HasSlang(guildName))
    {
        badName = true;
    }
    else if (G_CDataManager()->hasSpecialCharacter(user->getCreateGuildName()))
    {
        badName = true;
    }
    if (badName)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }

    if (user->get_charac_guildkey() != 0 || user->get_guild_create_right() == 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x20);
        return 0;
    }

    if (!user->isSetGuildCreateFlag(2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x6d);
        return 0;
    }
    if (!user->isSetGuildCreateFlag(4))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x71);
        return 0;
    }

    const int money = user->getCurCharacInvenR()->get_money();
    const int cost = G_CDataManager()->m_guildParameter.m_field0;   // ORIG dm+0xa50c
    if (money < cost)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x1f);
        return 0;
    }

    user->getCurCharacInvenW()->use_money(cost, (eMoneySubReason)0xb, true);
    send_packet_guildcreateright(user, 1);
    user->set_guild_create_right(1);
    user->UpdateData();

    const char* guildAddr = user->getCreateGuildAddr();
    const char* name = user->getCreateGuildName();
    const unsigned int characNo = (unsigned int)user->getCurCharacNo();
    const ENUM_SERVER_GROUP group = user->GetServerGroup();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
    proxy->SendGuildCreate(characNo, const_cast<char*>(name), const_cast<char*>(guildAddr));

    user->resetGuildCreateFlag();
    user->resetTempGuildNameAddr();
    return 0;
