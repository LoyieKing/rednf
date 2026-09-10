bool DB_SaveKillMonsterInfo::dispatch(int /*userIdx*/, int /*slot*/,
                                      Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_LOAD_KILL_MONSTER_INFO* sig =
        stream->GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    if (sig == 0)
        return 0;
    char* bossInfo = 0;    // -0x4c
    char* namedInfo = 0;   // -0x50
    char* apcBossInfo = 0; // -0x54
    bool ok = false;       // -0x9
    ok = put_compressed_blob_data(db, 0, (char*)sig, 0x1d4c, &bossInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa010, 5);
        trace("DB_SaveKillMonsterInfo::dispatch boss_info compress_zip(%u) ERROR charac_no=%u",
              0, sig->m_characNo);
        return 0;
    }
    ok = put_compressed_blob_data(db, 1, (char*)sig->m_named, 30000,
                                  &namedInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa01a, 5);
        trace("DB_SaveKillMonsterInfo::dispatch named_info compress_zip(%u) ERROR charac_no=%u",
              1, sig->m_characNo);
        return 0;
    }
    ok = put_compressed_blob_data(db, 2, (char*)sig->m_apc, 0x1d4c,
                                  &apcBossInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa024, 5);
        trace("DB_SaveKillMonsterInfo::dispatch apc_boss_info compress_zip(%u) ERROR charac_no=%u",
              2, sig->m_characNo);
        return 0;
    }
    db->set_query(
        "upDate charac_kill_monster_info set boss_info='%s', named_info='%s', apc_boss_info='%s' where charac_no=%u",
        bossInfo, namedInfo, apcBossInfo, sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_kill_monster_info(charac_no, boss_info, named_info, apc_boss_info) values(%u, '%s', '%s', '%s')",
            sig->m_characNo, bossInfo, namedInfo, apcBossInfo);
        if (!db->exec(true))
            return 0;
    }
    return 1;
