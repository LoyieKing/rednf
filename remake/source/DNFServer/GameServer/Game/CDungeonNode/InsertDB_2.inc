bool CDungeonSuccessNode::InsertDB(const DungeonKey& key, bool flag)   // SIGFIX: bool
{
    // [b28d-solo-CDungeonNode · P221 · AE] ORIG 0x08485f8a：cMyTrace 改全表达式临时链
    // ⇒ AE。两循环各补具名引用 `const JobKey& jobKey` / `const int& partyKey`
    // + `CSuccessJobNode* node`、it++。194=194、p167 CLEAR。
    for (std::map<JobKey, CSuccessJobNode>::iterator itJob = m_jobMap.begin(); itJob != m_jobMap.end(); itJob++)
    {
        const JobKey& jobKey = itJob->first;
        CSuccessJobNode* node = &itJob->second;
        node->InsertDB(key, flag, jobKey);
    }

    for (std::map<int, Party>::iterator it = m_partyMap.begin(); it != m_partyMap.end(); it++)
    {
        const int& partyKey = it->first;
        Party& p = it->second;
        if (p.m_f3c != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_party(clear_time,die_count,hp_consume,"
                "mp_consume,hit_count,hit_per_avg_damage,hp_recovery,mp_recovery,"
                "update_count,channel_no,dungeon_index,dungeon_diff,success,level,"
                "party_user_count,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
                "%d,%d,%d,NOW())",
                p.m_f00, p.m_f04, p.m_f08, p.m_f0c, p.m_f10, p.m_f14, p.m_f18, p.m_f1c,
                p.m_f3c, G_CEnvironment()->get_channel_no(), key.m_dungeonIndex,
                key.m_dungeonDiff, flag, p.m_f24, partyKey);
            if (!sql->exec(true))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x1b9, 5)("inSert log_dungeon_party failed");
                                return 0;
            }
        }
        p.Reset();
    }
    return 1;
