bool DB_LoadEtc::LoadVillageAttackInfo(SIG_LOAD_ETC* sig)
{
    if (sig->m_characNo == 0)
    {
        return false;
    }

    long cur = GlobalData::s_systemTime_.getCurSec();        // ORIG localtime 单线程域
    struct tm* ptm = localtime(&cur);
    time_t baseDay;                                          // ORIG 槽 -0x18 位次
    int fatigueHour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // +0x37c
    if (ptm->tm_hour < fatigueHour)
    {
        ptm = 0;
        cur -= 0x15180;
        ptm = localtime(&cur);
    }
    baseDay = mktime(ptm);
    (void)baseDay;

    char occDate[12];
    memset(occDate, 0, sizeof(occDate));
    sprintf(occDate, "%04d-%02d-%02d",
            ptm->tm_year + 0x76c, ptm->tm_mon + 1, ptm->tm_mday);

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
    {
        return false;
    }
    db->set_query("seLect attack_count, revenge_dungeon from"
                  " village_attack_dungeon \t\t\t\t\t\t   where occ_date = '%s'"
                  " and charac_no = %u", occDate, sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        sig->m_curVillageAttackCount = 0;                    // +0xc9ac
        sig->m_curRevengeDungeonCount = 0;                   // +0xc9ad
        return false;
    }
    int fetched = db->fetch();                              // ORIG movzbl %al → int 槽
    if (fetched == 0)
    {
        return false;
    }
    if (!db->get_ubyte(0, sig->m_curVillageAttackCount))
    {
        return false;
    }
    if (!db->get_ubyte(1, sig->m_curRevengeDungeonCount))
    {
        return false;
    }
    return true;
