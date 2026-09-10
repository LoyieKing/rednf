int PUNISH_CHECK::punish_check_by_SYS(SIG_LOGIN_DATA* data, long arg, int flag)
{
    // ORIG `jne +0x30`：return-1 作岛、body 作 fall-through。`if (flag != 1) return 1;`
    // 会被 GCC 反成 `je body`+return-1 fall-through（P196 极性）。用 ==1 正形。
    if (flag == 1)
    {
        data->m_errCode = 4;
        if (arg == 0)
        {
            data->m_errCode = 0x3ea;
        }
        return 0;
    }
    return 1;
