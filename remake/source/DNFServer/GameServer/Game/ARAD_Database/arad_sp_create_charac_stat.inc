int CREATE_QUERY::arad_sp_create_charac_stat(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    if (db->insert(
            "inSert into charac_stat  (charac_no,HP, forbidden_to_play, "
            "forbidden_due_to)  values(%d,100,0,0) ",
            sig->m_characNo) != 0)
    {
        sig->m_resultCode = 2;                                                                                // +0x5344 直写（helper 外联消除）
        return -1;
    }
    return 0;
