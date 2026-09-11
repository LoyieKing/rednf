bool CSyncSlangFilter::AddSlangList()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slang from slang_list");
    if (db->exec(true) != 1)
        return 0;
    unsigned i = 0;
    for (; i < (unsigned)db->get_n_rows(); ++i)
    {
        if (db->fetch() != 1)
            return 0;
        char buf[0x33] = {0};
        if (db->get_str(0, buf, 0x33) != 1)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0xbc, 5);
            t("CSyncSlangFilter::AddSlangList");
            return 0;
        }
        {
            std::string tmp(buf, std::allocator<char>());
            AddSlang(tmp);
        }
    }
    return 1;
}