bool CSyncScript::insert_exp_level_to_db(unsigned int exp, int level)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into exp_level_ref(exp, lev) values(%u, %d)", exp, level);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}