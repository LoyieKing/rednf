// ----------------------------------------------------------------------------
// saveSecurityCardHistory(MySQL*, uint m_id, ENUM_SECURITYCARD_MOD_TYPE mod)
// ORIG @ 0x0842ae3f：set_query("inSert into member_security_card_history(
// occ_time,m_id,modify_type) values(now(),%s,%d)", NumberToString(m_id,0), mod)
// → exec(true)。void。
// ----------------------------------------------------------------------------
void saveSecurityCardHistory(MySQL* db, unsigned int m_id,
                             ENUM_SECURITYCARD_MOD_TYPE mod)
{
    db->set_query(
        "inSert into member_security_card_history(occ_time,m_id,modify_type) "
        "values(now(),%s,%d)",
        NumberToString(m_id, 0), static_cast<int>(mod));
    db->exec(true);
}