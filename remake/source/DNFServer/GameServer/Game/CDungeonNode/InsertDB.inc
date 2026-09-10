bool CDungeonNode::InsertDB(const DungeonKey& key)   // SIGFIX: bool（ORIG 串）
{
    // [b28d-solo-CDungeonNode · P221 · AE] ORIG 0x08485ba4：循环内 cMyTrace 改全表达式
    // 临时链（4.1 迭代器槽 it -0x54 深于 trace -0x4c；具名 trace 时 4.4 反置）⇒ AE。
    // 补 `int monsterIndex = it->first;`、第二段 `CDungeonJobNode* node` / `int job`
    // 具名局部、两处 it++（p167 CLEAR）。指令数 171=171。
    for (std::map<int, Monster>::iterator it = m_monsterMap.begin(); it != m_monsterMap.end(); it++)
    {
        Monster& m = it->second;
        int monsterIndex = it->first;
        if (m.m_count != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_monster(channel_no,dungeon_index,dungeon_diff,"
                "monster_index,damage_total,damage_count,last_time) "
                "values(%d,%d,%d,%d,%d,%d,NOW())",
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                monsterIndex, m.m_damageTotal, m.m_damageCount);
            if (!sql->exec(true))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x13d, 5)("inSert log_dungeon_monster failed ");
                return false;
            }
        }
        m.Reset();
    }

    for (std::map<int, CDungeonJobNode>::iterator itJob = m_jobMap.begin(); itJob != m_jobMap.end(); itJob++)
    {
        CDungeonJobNode* node = &itJob->second;
        int job = itJob->first;
        node->InsertDB(key, job);
    }

    for (int i = 0; i <= 1; ++i)
    {
        m_success[i].InsertDB(key, i != 0);
    }
    return true;
