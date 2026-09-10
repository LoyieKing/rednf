bool CSyncScript::insert_charac_quest_category(unsigned int origin,
                                               unsigned int mapped)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into charac_quest_ref(origin_idx, mapped_idx) values(%d, %d)",
                  origin, mapped);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
