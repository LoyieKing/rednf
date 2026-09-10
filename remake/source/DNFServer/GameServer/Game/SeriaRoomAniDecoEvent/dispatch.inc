bool DB_UpdateSeriaRoomAnimationDeco::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int accId = 0;
    short serverGroup = 0;
    int characNo = 0;
    short field1 = 0;
    short field2 = 0;
    *stream >> accId;
    *stream >> serverGroup;
    *stream >> characNo;
    *stream >> field1;
    *stream >> field2;
    db->set_query(
        "upDate event_1208_seriaroom_anideco set receive_flow = %d, deco_flow = %d where m_id = %s and server_id = %d and charac_no = %d",
        field1, field2, NumberToString(accId, 0), (int)serverGroup, characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1208_seriaroom_anideco(m_id ,server_id, charac_no, receive_flow, deco_flow, occ_date) values(%s, %d, %d, %d, %d, now())",
            NumberToString(accId, 0), (int)serverGroup, characNo, field1,
            field2);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}
