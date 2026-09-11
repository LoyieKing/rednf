int DB_MouseRegister::_unregistPassword(unsigned int mid, char* ip)
{
    // ORIG 栈槽：0x8=this 0xc=mid 0x10=ip。
    MySQL* db;      // -0x10
    MySQL* grade;   // -0xc

    db = _getDBHandleForDevideAccountDB();
    if (!_updateHistory(mid, 10, ip))
    {
        return 0x15;
    }
    db->set_query("deLete from member_mousepass where m_id=%s",
                  NumberToString(mid, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        return 0x15;
    }
    grade = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_1, static_cast<ENUM_SERVER_GROUP>(0));
    grade->set_query(
        "upDate member_security_grade set goblin_pass_mod='0000-00-00 00:00:00'"
        " where m_id=%s",
        NumberToString(mid, 0));
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_pass_mod)"
            " values(%s, '0000-00-00 00:00:00')",
            NumberToString(mid, 0));
        if (!grade->exec(true))
        {
            return 0;
        }
    }
    return 0;
}