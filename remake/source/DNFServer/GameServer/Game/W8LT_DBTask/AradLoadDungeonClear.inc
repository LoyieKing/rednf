bool DB_LoadDungeonClear::AradLoadDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig)
{
    if (AradLoadMemberDungeonClear(sig))
        return 1;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("select B.dungeon from charac_info A Inner Join charac_dungeon B where A.m_id = %d and B.charac_no = A.charac_no and A.delete_flag = 0",
                  sig->m_accId);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x4b72,
                              "Dungeon share query error : m_id(%d) dungeon count(%d)",
                              sig->m_accId, db->get_n_rows());
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x4b79,
                              "Dungeon count error : m_id(%d) dungeon count(%d)",
                              sig->m_accId, db->get_n_rows());
        return 1;
    }
    char buf[428] = { '\0' };
    char* dst = sig->m_clearData;
    int i = 0;
    while (i < db->get_n_rows())
    {
        if (!db->fetch())
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x4b89,
                                  "Dungeon fetch error : m_id(%d) dungeon count(%d)",
                                  sig->m_accId, db->get_n_rows());
            return 0;
        }
        if (!db->get_str(0, buf, 0x1ab))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x4b8f,
                                  "Dungeon Parsing error : m_id(%d) dungeon count(%d)",
                                  sig->m_accId, db->get_n_rows());
            return 0;
        }
        if (strlen(sig->m_clearData) + strlen(buf) > 0x1800)
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x4b95,
                                  "Dungeon Total Data Error : m_id(%d) dungeon count(%d)",
                                  sig->m_accId, db->get_n_rows());
            return 0;
        }
        memcpy(dst, buf, 0x1ac);
        dst += strlen(dst);
        {
            // ORIG 0x08419…164：`mov -0xc,%ebx` 前置缓存 i，跨 get_n_rows
            // 调用后 `cmp %eax,%ebx`——块作用域 register 局部直落 %ebx。
            register int curI = i;
            // [b28d-solo-4] N22 翻案（P302/P319 族）：比较两侧 unsigned 化后
            // 4.4 产 `cmp %eax,%ebx`（dst=curI，与 ORIG 0x08419d64 全等）；
            // 纯 int 形恒产 `cmp %ebx,%eax`（dst=call 结果）。!= 对符号不敏感，
            // curI>=0 且 rows==0 时两形均判不等 ⇒ 语义等价。
            if ((unsigned int)curI != (unsigned int)db->get_n_rows() - 1)
            {
                *dst = ',';
                ++dst;
            }
        }
        ++i;
    }
    sig->m_pad8[0] = 1;
    sig->m_selected = 0;
    return 1;
