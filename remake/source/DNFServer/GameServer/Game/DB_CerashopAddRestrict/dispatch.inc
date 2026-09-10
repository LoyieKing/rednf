bool DB_SaveCerashopAddRestrictType::dispatch(int /*param1*/, int /*param2*/,
                                              Stream* stream)
{
    unsigned int type = 0;    // -0x14
    unsigned int id = 0;      // -0x18
    unsigned int ipgNo = 0;   // -0x1c
    long nextDate = 0;        // -0x20
    long endDate = 0;         // -0x24
    *stream >> type;
    *stream >> id;
    *stream >> ipgNo;
    *stream >> nextDate;
    *stream >> endDate;

    switch (type) {
    case 0: {
        unsigned int mId = 0;   // -0x28
        *stream >> mId;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)1, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            return false;
        db->set_query(
            "upDate account_cerashop_restrict set count=%u, next_date=%d, "
            "last_access_date=UNIX_TIMESTAMP() where m_id = %u and ipg_no=%u",
            ipgNo, nextDate, mId, id);
        if (!db->exec(true))
            return false;
        if (db->getAffectedRowCount() == 0) {
            db->set_query(
                "inSert into account_cerashop_restrict(m_id, ipg_no, count, "
                "next_date, end_date, last_access_date) values(%u, %u, %u, %d, "
                "%d, UNIX_TIMESTAMP())",
                mId, id, ipgNo, nextDate, endDate);
            if (!db->exec(true))
                return false;
        }
        break;
    }
    case 1: {
        unsigned int characNo = 0;   // -0x2c
        *stream >> characNo;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            return false;
        db->set_query(
            "upDate charac_cerashop_restrict set count=%u, next_date=%d, "
            "last_access_date=UNIX_TIMESTAMP() where charac_no = %u and ipg_no=%u",
            ipgNo, nextDate, characNo, id);
        if (!db->exec(true))
            return false;
        if (db->getAffectedRowCount() == 0) {
            db->set_query(
                "inSert into charac_cerashop_restrict(charac_no, ipg_no, count, "
                "next_date, end_date, last_access_date) values(%u, %u, %u, %d, "
                "%d, UNIX_TIMESTAMP())",
                characNo, id, ipgNo, nextDate, endDate);
            if (!db->exec(true))
                return false;
        }
        break;
    }
    }
    return true;
