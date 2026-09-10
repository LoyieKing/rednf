bool DB_DeleteAccountCargo::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    unsigned int mId = 0;
    MySQL* db;
    *stream >> mId;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("deLete from account_cargo where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    return 1;
