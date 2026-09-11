bool DB_SaveEventLevelupSupport::dispatch(int userIdx, int slot,
                                          Stream* stream)
{
    int characNo = 0;
    int level = 0;
    int state = 0;
    *stream >> characNo;
    *stream >> level;
    *stream >> state;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    db->set_query(
        "upDate event_levelup_support set state = %d where charac_no = %d and level = %d",
        state, characNo, level);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_levelup_support(charac_no, level, state) values(%d, %d, %d)",
            characNo, level, state);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}