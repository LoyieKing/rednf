int DB_MouseRegister::_registNewPassword(unsigned int mid, const char* pwd,
                                         char* ip, int param4)
{
    // ORIG 栈槽：0x8=this 0xc=mid 0x10=pwd 0x14=ip 0x18=param4。
    MySQL* db;      // -0x10
    MySQL* grade;   // -0xc

    db = _getDBHandleForDevideAccountDB();
    db->set_query("seLect enable_flag from member_mousepass where m_id=%s",
                  NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        db->set_query(
            "inSert into member_mousepass(m_id,mousepass,occ_time,version_info,"
            " validity_time, reward_time,enable_flag) values(%s,'%s',now(),'2',"
            " unix_timestamp(now()), 0,'1')",
            NumberToString(mid, 0), pwd);
    }
    else
    {
        db->set_query(
            "upDate member_mousepass set mousepass = '%s', occ_time = now() ,"
            "version_info = '2', validity_time=unix_timestamp(now()),"
            " enable_flag = '1', reward_time = 0 where m_id = %s",
            pwd, NumberToString(mid, 0));
    }
    if (!db->exec(true))
    {
        if (db->getDBError() == 0x426)
        {
            return 0x12;
        }
        return 1;
    }
    DB_CancelRestingUserRestrict::makeRequest(mid, param4);
    if (!_updateHistory(mid, 8, ip))
    {
        return 1;
    }
    grade = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_1, static_cast<ENUM_SERVER_GROUP>(0));
    grade->set_query(
        "upDate member_security_grade set goblin_pass_mod=now() where m_id=%s",
        NumberToString(mid, 0));
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_pass_mod)"
            " values(%s, now())",
            NumberToString(mid, 0));
        if (!grade->exec(true))
        {
            return 1;
        }
    }
    DB_CheckSecuReward::makeRequest(param4, mid, 1);
    return 0;
