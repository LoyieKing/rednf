void PvP_GuildWar_Log::WriteGuildWarPvPLog(int roomIdx, char mode, CUser** users)
{
    // [L14 round3] N78 突破口（P214③/P253）：ORIG 0x085ddad0 两处清零是**内联
    // `rep stos`**（0x85ddb5b 0x800 字节 / 0x85ddcbd 0x100 字节）而非 `call memset`；
    // 4.4 对 `memset` 恒发 call（p167 EXTRA +2 memset）。改「声明处 `= {'\0'}`」
    // 并把声明下移到原 memset 位置，令 4.4 在同一位置发常量 rep stos。
    int i;
    int j;
    int k;

    for (i = 0; i < 8; ++i)
    {
        if (users[i] != 0)
        {
            int point = users[i]->get_guildwar_point_per_pvpplay();
            unsigned int guildKey =
                ((CUserCharacInfo*)users[i])->get_charac_guildkey();
            InsertPvPGuild(guildKey, point);
        }
    }
    char logBuf[2048] = { '\0' };
    sprintf(logBuf, "Room_Idx:%d\tPvP_Mode:%d\tElasp_Time:%d\tG_Cnt:%d\n",
            roomIdx, (int)mode,
            GlobalData::s_systemTime_.getCurSec() - m_startTime,
            (unsigned char)m_guildCnt);
    if ((unsigned char)m_guildCnt < 9)
    {
        for (int j = 0; j < (unsigned char)m_guildCnt; ++j)
        {
            if (m_guilds[j].m_guildKey != 0)
            {
                sprintf(logBuf, "%s->\t\tG_Key:%d\tPoint:%d\tM_Cnt:%d\n", logBuf,
                        m_guilds[j].m_guildKey, m_guilds[j].m_point,
                        (unsigned char)m_guilds[j].m_count);
            }
        }
        char logPath[256] = { '\0' };
        sprintf(logPath, "./log/%s/GuildWarResult",
                G_CEnvironment()->get_file_name());
        CMyFileLog fileLog(
            "void PvP_GuildWar_Log::WriteGuildWarPvPLog(int, char, CUser**)",
            0x12dc);
        fileLog(logPath, "%s", logBuf);
        Clear();
        for (k = 0; k < 8; ++k)
        {
            if (users[k] != 0)
            {
                users[k]->set_guildwar_point_per_pvpplay(0);
            }
        }
    }
    else
    {
        LogManager::logFormat(
            1, "pvp.cpp",
            __PRETTY_FUNCTION__,
            0x12cc,
            "PvP_GuildWar_Log::WriteGuildWarPvPLog() m_guildCnt(%d) > MAX_PVP_PLAYER(8) ",
            (unsigned char)m_guildCnt);
    }
