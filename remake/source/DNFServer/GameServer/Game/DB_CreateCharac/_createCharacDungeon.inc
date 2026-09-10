bool DB_CreateCharac::_createCharacDungeon(unsigned int accId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);

    db->set_query("seLect * from member_dungeon where m_id=%s",
                  NumberToString(accId, 0));
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into member_dungeon (m_id,dungeon) values (%s,'')",
                      NumberToString(accId, 0));
    }
    // P168a① 裸形（N96 清理）。
    if (!db->exec(true))
        return false;
    return true;
