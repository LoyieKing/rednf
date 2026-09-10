void CSetGuildFund::execute()
{
    CUser* user = GetUser();
    register int amount = m_fund;
    register int guildKey = user->get_charac_guildkey();
    register int characNo = user->get_charac_no(-1);
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendAddGuildFund(characNo, guildKey, amount,
                           Packet_Add_Guild_Fund::eFundType_GM);
