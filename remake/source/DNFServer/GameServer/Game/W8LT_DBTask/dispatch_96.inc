bool DB_StatDungeonFail::dispatch(int userIdx, int slot, Stream* stream)
{
    G_GameWorld()->getStatisticsShusiaService()->lock();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    std::map<int, int>::iterator it =
        G_GameWorld()->getStatisticsShusiaService()->m_dungeonFailure.begin();
    std::map<int, int>::iterator end =
        G_GameWorld()->getStatisticsShusiaService()->m_dungeonFailure.end();
    while (it != end)
    {
        // [b28d-solo] P167：ORIG 调 `_Rb_tree_iterator::operator*`（deEv），
        // 我方 `it->` 选中 ptEv（差集 -4/+4）⇒ 写 `(*it).`。
        int lev = (*it).first;
        int count = (*it).second;
        db->set_query(
            "inSert into dungeon_fail(occ_date,lev,occ_count) values(cast(now() as date),%d,%d)",
            lev, count);
        bool r = db->exec(true);
        if (!r)
        {
            db->set_query(
                "upDate dungeon_fail set occ_count = occ_count + %d where occ_date = now() and lev = %d",
                count, lev);
            bool r2 = db->exec(true);
            if (!r2)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x4e8f, 5)
                    ("upDate dungeon_fail failed at %d", lev);
            }
        }
        ++it;
    }
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    std::map<int, int>::iterator it2 =
        G_GameWorld()->getStatisticsShusiaService()->m_serviceUsage.begin();
    std::map<int, int>::iterator end2 =
        G_GameWorld()->getStatisticsShusiaService()->m_serviceUsage.end();
    while (it2 != end2)
    {
        int key = (*it2).first;
        int value = (*it2).second;
        db2->set_query(
            "inSert into re_stamina_shusia(occ_date,lev,occ_count) values(cast(now() as date),%d,%d)",
            key, value);
        bool r = db2->exec(true);
        if (!r)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4ea9, 5)
                ("inSert into re_stamina_shusia failed at %d", key);
            db2->set_query(
                "upDate re_stamina_shusia set occ_count = occ_count + %d where occ_date = now() and lev = %d",
                value, key);
            bool r2 = db2->exec(true);
            if (!r2)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x4eaf, 5)
                    ("upDate re_stamina_shusia failed at %d", key);
                goto loop_end;
            }
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x4eb3, 5)
            ("re_stamina_shusia : %d SHUSIA USAGE %d", key, value);
loop_end:
        ++it2;
    }
    G_GameWorld()->getStatisticsShusiaService()->unlock();
    G_GameWorld()->getStatisticsShusiaService()->reset();
    return 1;
