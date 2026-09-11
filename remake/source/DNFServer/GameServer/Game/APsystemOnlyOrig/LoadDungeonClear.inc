// ORIG 0x8419db4：member_dungeon 表按 m_id 取单行 dungeon 串（%s+NumberToString），
// 成功行数必须恰为 1；取回后 m_selected 清零返 1。
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
}