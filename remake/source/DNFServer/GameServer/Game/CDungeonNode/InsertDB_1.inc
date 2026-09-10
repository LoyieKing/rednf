bool CDungeonJobNode::InsertDB(const DungeonKey& key, int job)   // SIGFIX: bool
{
    // [b28d-solo-CDungeonNode · P221 · AE] ORIG 0x08485e00：cMyTrace 改全表达式临时链
    // （迭代器槽 it -0x40 深于 trace -0x38）⇒ AE。`Skill& s = it->second;`
    // + `int skillIndex = it->first;` 两个具名局部（operator-> 各调一次，槽 -0x24/-0x20）；
    // 日志尾参用 skillIndex 而非重读 it->first；it++（ppEi）。110=110。
    for (std::map<int, Skill>::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
    {
        Skill& s = it->second;
        int skillIndex = it->first;
        if (s.m_useCount != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_skill(skill_cool_time,skill_use,skill_count,"
                "channel_no,dungeon_index,dungeon_diff,charac_job,charac_grow,"
                "skill_index,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
                s.m_coolTime, s.m_skillUse, s.m_skillCount,
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                job, 0, skillIndex);
            if (!sql->exec(true))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x177, 5)("inSert log_dungeon_skill failed");
                return false;
            }
        }
        s.Reset();
    }
    return true;
