// ORIG 0x0818b6c9：inSert into charac_npc 空关系行。
int CREATE_QUERY::arad_sp_create_npc_relationship(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    if (db->insert("inSert into charac_npc(charac_no,npc_cnt,npc_data) values(%d,0,'')",
                   sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
}