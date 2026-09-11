// punish_check_by_ARS @ 0x08196d51（arg==0 → 0x3eb）
int PUNISH_CHECK::punish_check_by_ARS(SIG_LOGIN_DATA* data, long arg, int flag)
{
    if (flag == 1)
    {
        data->m_errCode = 4;
        if (arg == 0)
            data->m_errCode = 0x3eb;
        return 0;
    }
    return 1;
}