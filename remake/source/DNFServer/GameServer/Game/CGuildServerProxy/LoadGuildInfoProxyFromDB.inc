bool CGuildServerProxy::LoadGuildInfoProxyFromDB(ENUM_SERVER_GROUP group)
{
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    if (group == E_SERVER_GROUP_NONE)
    {
        group = (ENUM_SERVER_GROUP)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    }
    db->set_query("seLect guild_id, guild_name, lev , power_side, guild_agit_flag "
                  "from guild_info where expire_flag = 0 and server_id = %d", group);
    ok = db->exec(true);
    if (ok == false)
    {
        return 0;
    }
    int rows = db->get_n_rows();
    for (int i = 0; i < rows; i++)
    {
        ok = db->fetch();
        if (ok == false)
        {
            break;
        }
        unsigned int guildId;
        STGuildProxy info;
        ok = db->get_uint(0, guildId);
        if (ok == false)
        {
            return 0;
        }
        ok = db->get_str(1, info.m_guildName, 0x16);
        if (ok == false)
        {
            return 0;
        }
        ok = db->get_ubyte(2, info.m_level);
        if (ok == false)
        {
            return 0;
        }
        ok = db->get_ubyte(3, info.m_powerSide);
        if (ok == false)
        {
            return 0;
        }
        ok = db->get_ubyte(4, info.m_agitFlag);
        if (ok == false)
        {
            return 0;
        }
        if (m_guildInfoMap.insert(std::make_pair(guildId, info)).second == false)
        {
            return 0;
        }
    }
    return 1;
