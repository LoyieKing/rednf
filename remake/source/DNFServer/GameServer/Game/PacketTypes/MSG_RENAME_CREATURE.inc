MSG_RENAME_CREATURE::MSG_RENAME_CREATURE()
{
    memset(m_name, 0, sizeof(m_name));  // ORIG C1：memset(this+0x14,0,0xd)（字段化改名）
