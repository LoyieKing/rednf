bool DB_UpdateBlueMarbleEnterCount::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    unsigned int characNo = 0;
    unsigned char enterCnt = 0;
    *stream >> characNo;
    *stream >> enterCnt;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_option set blue_marble_enter_count = %u where charac_no = %u",
        (unsigned int)enterCnt, characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_option(charac_no, blue_marble_enter_count) values(%u, %u)",
            characNo, (unsigned int)enterCnt);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}