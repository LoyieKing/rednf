bool DB_UpdateBreakAwayRuckPoint::dispatch(int userIdx, int slot,
                                           Stream* stream)
{
    // ORIG 0x8438300 槽位：mId(-0x14)/db(-0x10)/dead(-0xc)；+0x2d 处
    // `movl $0x0,-0xc` 后有零槽但后续未再引用（NST 结果直推），
    // 以未用局部复现（-O0 无 DCE）。
    unsigned int mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    mId = 0;
    char* dead = 0;
    (void)dead;
    *stream >> mId;
    db->set_query(
        "upDate churn_member_info set luck_point=0 where m_id=%s",
        NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        return 0;
    return 1;
}