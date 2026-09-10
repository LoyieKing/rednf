bool DB_LoadEtc::LoadProperDungeonClearCount(SIG_LOAD_ETC* sig)
{
    // ORIG 槽序：db=-0x10（先声明→深）、nRows=-0xc（后声明→浅）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect clear_cnt from event_dungeon_clear where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    int nRows = db->get_n_rows();
    if (nRows != 1)
    {
        sig->m_properDungeonClearCount = 0;          // +0x40cc word 清零
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_short(0,
                       *reinterpret_cast<short*>(&sig->m_properDungeonClearCount)))
    {
        return false;
    }
    return true;
