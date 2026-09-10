bool DB_LoadDungeonClear::LoadDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig)
{
    sig->m_selected = 1;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect dungeon from member_dungeon where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() != 1)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x4bbc,
                              "Dungeon count error : m_id(%s) dungeon count(%d)",
                              NumberToString(sig->m_accId, 0), db->get_n_rows());
        return 0;
    }
    if (!db->fetch())
        return 0;
    if (!db->get_str(0, sig->m_clearData, 0x17ff))
        return 0;
    sig->m_selected = 0;
    return 1;
