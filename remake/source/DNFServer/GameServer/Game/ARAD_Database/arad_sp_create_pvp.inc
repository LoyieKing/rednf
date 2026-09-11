// ORIG 0x0818b5f4：inSert into pvp_result。
int CREATE_QUERY::arad_sp_create_pvp(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    // ORIG 0x0818b5fa：函数头 `movl $0x198,-0xc(%ebp)` 死槽写（原码遗留未用局部，
    //   P171 照抄），加死局部复现。
    int queryId = 0x198;
    (void)queryId;
    if (db->insert("inSert into pvp_result (charac_no) values(%u)",
                   sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
}