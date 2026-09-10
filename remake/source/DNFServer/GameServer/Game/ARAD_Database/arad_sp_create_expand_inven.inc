int CREATE_QUERY::arad_sp_create_expand_inven(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    if (db->insert(" inSert into charac_inven_expand  "
                   "(charac_no,cargo,cargo_capacity, jewel)  values(%u,'',0,'')",
                   sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
