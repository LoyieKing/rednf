// ----------------------------------------------------------------------------
// turnOnSecurityFlag(MySQL*, uint m_id) —— ORIG @ 0x0842ae90：
// char buf[0x14] 清零后 memcpy 13 字节 "member_login"（含 \0）；set_query(
// "upDate %s set security_flag=1 where m_id=%s and security_flag=0", buf,
// NumberToString(m_id,0)) → exec(true)。void。
// ----------------------------------------------------------------------------
void turnOnSecurityFlag(MySQL* db, unsigned int m_id)
{
    char table[0x14];
    memset(table, 0, 0x14);
    memcpy(table, "member_login", 0xd);

    db->set_query(
        "upDate %s set security_flag=1 where m_id=%s and security_flag=0",
        table, NumberToString(m_id, 0));
    db->exec(true);
}