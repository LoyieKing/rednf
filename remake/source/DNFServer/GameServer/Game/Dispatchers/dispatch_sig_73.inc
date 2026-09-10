int Dispatcher_CallGuildLevelUp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned int local_2c = 0;           // -0x28：升级价格输出
    unsigned int local_24;               // -0x20：CheckGuildLevelUp 结果
    int local_28;                        // -0x24：Secu Check
    char local_2f;                       // -0x2b：临时 level
    unsigned char local_2e;              // -0x2a：临时 powerSide
    char cVar2;
    bool bVar1;

    int iVar3 = user->get_state();
    if (iVar3 <= 2 || user->getCurCharacR() == 0)
    {
        LineFunc(0x947f, __PRETTY_FUNCTION__, 0, 0);
        return 0;
    }
    cVar2 = (char)((ServiceRestrictManager*)ARAD::Singleton<ServiceRestrictManager>::Get())
                ->isRestricted(user, (RestrictCategory::Enum)1, 0x1d);
    if (cVar2 != 0)
    {
        send_packet_guild_levelup(user, 0xd1);
        return 0;
    }
    local_28 = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)7);
    if (local_28 != 0)
    {
        send_packet_guild_levelup(user, (unsigned char)(local_28 & 0xff));
        return 0;
    }
    local_24 = user->CheckGuildLevelUp(local_2c);
    if (local_24 != 0)
    {
        send_packet_guild_levelup(user, (unsigned char)(local_24 & 0xff));
        return 0;
    }
    send_packet_guild_levelup(user, 0);
    unsigned int uVar9 = local_2c;
    unsigned int uVar6 = user->get_charac_guildkey();
    unsigned int uVar7 = user->getCurCharacNo();
    int iVar3b = (int)user->GetUID();
    ENUM_SERVER_GROUP uVar4 = user->GetServerGroup();
    CGuildServerProxy* pCVar8 =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(uVar4);
    pCVar8->SendGuildLevelup(iVar3b, uVar7, uVar6, uVar9);
    void* local_20 = user->GetGuildDBInfo();
    STGuildProxy local_46;
    STGuildDBInfoOnly* guildDb = reinterpret_cast<STGuildDBInfoOnly*>(local_20);
    local_2f = (char)guildDb->m_fundLevel + 1;
    local_46.m_level = local_2f;
    memcpy(local_46.m_guildName, local_20, 0x16);
    local_2e = guildDb->m_field95;
    local_46.m_powerSide = local_2e;
    unsigned int uVar9b = user->get_charac_guildkey();
    ENUM_SERVER_GROUP uVar4b = user->GetServerGroup();
    CGuildServerProxy* pCVar8b =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(uVar4b);
    pCVar8b->ModifyGuildInfoProxy(uVar9b, local_46);
    return 0;
