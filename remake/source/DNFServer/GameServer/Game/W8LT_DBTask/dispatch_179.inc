bool DB_BlackipPrivateSecurityValuation::dispatch(int userIdx, int slot,
                                                  Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    // ORIG 0x84354fa：无 `movl $0x0,-0x10` —— 形参未经初始化直接 &mId >>。
    unsigned int mId;
    *stream >> mId;
    db->set_query(
        "upDate member_security_grade set black_ip_try_time=now() where m_id=%s",
        NumberToString(mId, 0));
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into member_security_grade(m_id, black_ip_try_time) values(%s, now())",
            NumberToString(mId, 0));
        if (!db->exec(true))
            return 0;
    }
    return 1;
