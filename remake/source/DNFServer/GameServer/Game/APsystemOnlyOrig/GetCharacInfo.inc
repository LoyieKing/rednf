// ORIG 0x08434a8a：charac db(DB_HANDLE_2) 查 charac_info（name/job/lev/
// grow_type/guild_id），失败返 0；guild_id==0 直接返 1；否则 guild db
// (DB_HANDLE_8) 查 guild_name，失败返 0。server_id 取 env+0x378。
bool DB_LoadPowerWarStatueInfo::GetCharacInfo(
    SIG_LOAD_POWER_WAR_STATUE_INFO* sig, unsigned int characNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    MySQL* guildDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect charac_name,job,lev,grow_type,guild_id from charac_info where charac_no=%d and delete_flag!=1",
        characNo);
    db->exec(true);
    if (!db->exec(true) || !db->fetch())
        return 0;
    if (db->get_n_rows() != 1)
        return 0;
    // 技巧36：删具名 v 中转（ORIG 每次 `mov 0xc(%ebp),%eax` 参数槽直读+
    // add disp，0x08434b62/0x08434b98/…；指针槽会多占 -0x10 使整帧平移）。
    if (!db->get_str(0, sig->m_name, 0x1d))
        return 0;
    if (!db->get_byte(1, sig->m_job))
        return 0;
    if (!db->get_byte(2, sig->m_lev))
        return 0;
    if (!db->get_byte(3, sig->m_growType))
        return 0;
    // ORIG 0x08434c22-0x08434c30：G_CEnvironment+0x378 取 serverId（-0x10）
    // 与死局部 -0xc 清零均发生在第 5 个 get_uint 之前（4.1 声明序）。
    // +0x378 = m_serverEnvir(+0x198).m_serverGroup(+0x1e0)；具名链访
    // ORIG 0x08434c27 `mov 0x378(%eax),%eax` 单 disp 折叠。
    int serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    int deadC = 0;
    (void)deadC;
    if (!db->get_uint(4, sig->m_guildId))
        return 0;
    if (sig->m_guildId == 0)
        return 1;
    guildDb->set_query(
        "seLect guild_name from guild_info where guild_id=%d and server_id=%d",
        sig->m_guildId, serverId);
    if (!guildDb->exec(true) || !guildDb->fetch())
        return 0;
    if (guildDb->get_n_rows() != 1)
        return 0;
    if (!guildDb->get_str(0, sig->m_guildName, 0x28))
        return 0;
    return 1;
}