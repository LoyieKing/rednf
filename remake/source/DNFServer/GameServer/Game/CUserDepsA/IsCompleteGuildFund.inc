bool CUser::IsCompleteGuildFund()
{
    unsigned int* money = &m_guildDB.m_fundMoney;
    unsigned char* level = &m_guildDB.m_fundLevel;
    if (*level > 15 && *money >= 10000000)
        return true;
    if (*money > 19999999)
        return true;
    return false;
