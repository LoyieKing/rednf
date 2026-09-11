// ============================================================================
// CSuccessJobNode::InsertDB(DungeonKey const&, bool, JobKey const&) —— ORIG T 0x08486234
// m_charac.m_f3c!=0 时写 log_dungeon_charac（失败即返 0）；
// 再遍历 m_jobPartyMap：m_count!=0 时写 log_dungeon_party_job（失败即返 0），随后 Reset。
// ============================================================================
bool CSuccessJobNode::InsertDB(const DungeonKey& key, bool flag, const JobKey& jobKey)   // SIGFIX: bool
{
    // [b28d-solo-CDungeonNode · P221 · AE] ORIG 0x08486234：两处 cMyTrace 改全表达式
    // 临时链（4.1 迭代器槽 -0x54 深于 trace，具名 trace 时 4.4 反置）⇒ AE。
    // 补缺失的 `m_charac.Reset()`（真语义缺口，p167 归零）+ 具名引用
    // `const int& jobIndex = it->first;` + it++。226=226、p167 CLEAR。
    if (m_charac.m_f3c != 0)
    {
        MySQL* sql = GetMySqLInstance();
        sql->set_query(
            "inSert into log_dungeon_charac(clear_time,die_count,hp_consume,mp_consume,"
            "hit_count,hit_per_avg_damage,hp_recovery,mp_recovery,update_count,channel_no,"
            "dungeon_index,dungeon_diff,success,charac_job,level,charac_grow,last_time,"
            "exp_avg) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW(),%d)",
            m_charac.m_f00, m_charac.m_f04, m_charac.m_f08, m_charac.m_f0c,
            m_charac.m_f10, m_charac.m_f14, m_charac.m_f18, m_charac.m_f1c,
            m_charac.m_f3c, G_CEnvironment()->get_channel_no(), key.m_dungeonIndex,
            key.m_dungeonDiff, flag, jobKey.m_job, m_charac.m_f24,
            (int)(char)jobKey.m_sub, m_charac.m_f28);
        if (!sql->exec(true))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1f2, 5)("inSert log_dungeon_charac failed");
            return 0;
        }
    }

    m_charac.Reset();

    for (std::map<int, JobParty>::iterator it = m_jobPartyMap.begin(); it != m_jobPartyMap.end(); it++)
    {
        const int& jobIndex = it->first;
        JobParty& jp = it->second;
        if (jp.m_count != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_party_job(channel_no,dungeon_index,dungeon_diff,"
                "charac_job,charac_grow,party_user_count,success,job_count,last_time) "
                "values(%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                jobKey.m_job, (int)(char)jobKey.m_sub, jobIndex, flag, jp.m_count);
            if (!sql->exec(true))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x215, 5)("inSert log_dungeon_party_job failed");
                return 0;
            }
        }
        jp.Reset();
    }
    return 1;
}