int DB_Login::GetSchoolName(int schoolId, char* name)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10, E_SERVER_GROUP_NONE);
    db->set_query("seLect school_name from school_info where school_id=%d", schoolId);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_str(0, name, 0x28))
    {
        return 0;
    }
    return 1;
}