bool DB_LoadDungeonClear::AradLoadMemberDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect dungeon from member_dungeon where m_id=%u", sig->m_accId);
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() == 0)
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_str(0, sig->m_clearData, 0x17ff))
        return 0;
    size_t len = strlen(sig->m_clearData);
    if (len == 0)
        return 0;
    sig->m_pad8[0] = 0;
    sig->m_selected = 0;
    return 1;
