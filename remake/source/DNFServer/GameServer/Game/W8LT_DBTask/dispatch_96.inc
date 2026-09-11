// ============================================================================
// DB_StatDungeonFail::dispatch —— ORIG 0x0841b6e0（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x841b6e0：G_GameWorld()->getStatisticsShusiaService() → lock →
// GetDBHandle(DB_HANDLE_4,0) → 遍历 m_dungeonFailure（键=lev，值=occ_count）：
//   每项先 inSert dungeon_fail(occ_date,lev,occ_count)
//   values(cast(now() as date),%d,%d)（lev,count）；exec!=1 → upDate
//   set occ_count=occ_count+count where occ_date=now() and lev=lev
//   （count,lev）；仍失败 → cMyTrace(0x4e8f,5) "upDate dungeon_fail failed
//   at %d"(lev)。
// 再遍历 m_serviceUsage（同形）：inSert re_stamina_shusia(key,value)；成功
//   → cMyTrace(0x4eb3,5) "re_stamina_shusia : %d SHUSIA USAGE %d"；
//   失败 → cMyTrace(0x4ea9,5) "inSert into re_stamina_shusia failed at %d"
//   + upDate re_stamina_shusia set occ_count=occ_count+value where
//   occ_date=now() and lev=key；update 成功 → 同上 0x4eb3 成功日志；
//   update 失败 → cMyTrace(0x4eaf,5) "upDate re_stamina_shusia failed at %d"。
// 两循环结束 unlock → reset → return 1。
// [b28d-solo-2R] NEAR(16)：P221/P227 三处具名 cMyTrace→全表达式临时链后残差 36→16 条；
//   余 lev/count/r 槽深差 4（-0x24/-0x20/-0x1a vs -0x20/-0x1c/-0x16）+ db2 槽方向
//   （ORIG -0x18 浅 vs 4.4 -0x24 深）= N82 分配器差（4.1 不复用/4.4 复用），封顶。ORIG 0x841b6e0
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
}