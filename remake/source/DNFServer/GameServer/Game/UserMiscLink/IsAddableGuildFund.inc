bool CUser::IsAddableGuildFund(int amount)
{
    // [b28d-solo-UserMiscLink] ORIG 0x0866c21e：两个具名指针局部（-0x8 资金/-0x4 等级，
    // P0 槽往返证据）+ if/else 双分支各自比较；两个 return 0 独立块、return 1 共享尾块。
    unsigned int* pFund = &m_guildDB.m_fundMoney;
    unsigned char* pLevel = &m_guildDB.m_fundLevel;
    if (*pLevel > 0xf)
    {
        if (*pFund + amount > 10000000u)
            return 0;
    }
    else
    {
        if (*pFund + amount > 20000000u)
            return 0;
    }
    return 1;
