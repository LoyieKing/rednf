// ORIG 0x084354da sz 0xef：GetDBHandle(1,0) → *stream>>mId(uint) →
// upDate member_security_grade set black_ip_try_time=now() where m_id=%s；
// exec 失败或 affected==0 → inSert ... values(%s, now())（exec 失败返 0）；返 1。
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
}