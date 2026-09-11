// ----------------------------------------------------------------------------
// turnOffSecurityFlag(MySQL*, uint m_id) —— ORIG @ 0x0842af12：与 turnOn 同构，
// 仅 SQL 的 security_flag 目标值相反。
// ----------------------------------------------------------------------------
void turnOffSecurityFlag(MySQL* db, unsigned int m_id)
{
    char table[0x14];
    memset(table, 0, 0x14);
    memcpy(table, "member_login", 0xd);

    db->set_query(
        "upDate %s set security_flag=0 where m_id=%s and security_flag=1",
        table, NumberToString(m_id, 0));
    db->exec(true);
}