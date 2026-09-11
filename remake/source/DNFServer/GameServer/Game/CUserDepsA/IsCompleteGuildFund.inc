// ----------------------------------------------------------------------------
// 3. ORIG T 0x0866c1ca sz 0x53。
// m_guildDB.m_fundLevel（+0x8d03b，无符号字节）>0xf 且 m_guildDB.m_fundMoney
// （+0x8d0c0，uint）>=10000000 即完整；否则 fundMoney >=20000000 判完整
// （汇编 jbe $0xf / jbe $0x98967f / jbe $0x1312cff，全部无符号比较）。
// ----------------------------------------------------------------------------
bool CUser::IsCompleteGuildFund()
{
    unsigned int* money = &m_guildDB.m_fundMoney;
    unsigned char* level = &m_guildDB.m_fundLevel;
    if (*level > 15 && *money >= 10000000)
        return true;
    if (*money > 19999999)
        return true;
    return false;
}