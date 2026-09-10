int CREATE_QUERY::arad_sp_create_charac_dungeon(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    // ORIG 0x0818b648：先 `movl $0x0,-0xc`（双槽置 0/P197③）再用具名局部 characNo
    // 经槽传递（mov 0x5348→-0xc 再重读入参），非直读成员。
    int characNo = 0;
    characNo = sig->m_characNo;
    if (db->insert("inSert into charac_dungeon(charac_no, dungeon) values(%d, '')",
                   characNo) != 0)
    {
        return -1;
    }
    return 0;
