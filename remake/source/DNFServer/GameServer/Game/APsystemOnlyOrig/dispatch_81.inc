bool DB_UpdateMemberPlayInfoPcRoom::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId;   // ORIG 无初始化：直接 lea 传引用 >> 填充
    *stream >> mId;
    db->set_query(
        "upDate member_play_info set pcbang_flag=1 where occ_date=now() and m_id=%s",
        NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    return 1;
}