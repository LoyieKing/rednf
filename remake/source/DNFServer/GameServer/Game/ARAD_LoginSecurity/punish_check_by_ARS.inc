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
