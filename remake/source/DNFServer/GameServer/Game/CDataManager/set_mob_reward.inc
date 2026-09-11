bool CDataManager::set_mob_reward()
{
    short level;
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect level, exp from monster_reward_ref");
    ok = db->exec(true);
    if (ok != 1)
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows < 1)
    {
        return 0;
    }
    int i = 0;
    while (i < rows)
    {
        ok = db->fetch();
        if (ok != 1)
        {
            break;
        }
        ok = db->get_short(0, level);
        if (ok != 1)
        {
            return 0;
        }
        ok = db->get_short(1, m_mobReward[level]);
        if (ok != 1)
        {
            return 0;
        }
        i = i + 1;
    }
    m_mobReward[0] = 0;
    memcpy(m_mobRewardBackup, m_mobReward, 0x324);
    return 1;
}