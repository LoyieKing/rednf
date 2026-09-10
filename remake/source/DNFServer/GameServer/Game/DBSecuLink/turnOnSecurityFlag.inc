void turnOnSecurityFlag(MySQL* db, unsigned int m_id)
{
    char table[0x14];
    memset(table, 0, 0x14);
    memcpy(table, "member_login", 0xd);

    db->set_query(
        "upDate %s set security_flag=1 where m_id=%s and security_flag=0",
        table, NumberToString(m_id, 0));
    db->exec(true);
