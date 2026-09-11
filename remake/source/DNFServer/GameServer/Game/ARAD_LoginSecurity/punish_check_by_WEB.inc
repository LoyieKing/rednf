// punish_check_by_WEB @ 0x08196d1a（arg==0 → 0x3e9）
int PUNISH_CHECK::punish_check_by_WEB(SIG_LOGIN_DATA* data, long arg, int flag)
{
    // ORIG `jne +0x30`：return-1 作岛、body 作 fall-through（P196 极性，同 SYS）。
    if (flag == 1)
    {
        data->m_errCode = 4;
        if (arg == 0)
        {
            data->m_errCode = 0x3e9;
        }
        return 0;
    }
    return 1;
}