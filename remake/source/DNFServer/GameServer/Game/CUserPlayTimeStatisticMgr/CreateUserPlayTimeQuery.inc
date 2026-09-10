int CreateUserPlayTimeQuery(std::string& query, SIG_USER_PLAY_TIME_STATISTIC* sig)
{
    // [b28d-62] P214③：ORIG 0x084255d7 `rep stos`（0x100 dword）⇒ 源形
    //   `char buf[N] = {0}`；显式 memset 会退化为 call。
    char buf[0x400] = {0};
    for (int i = 0; i < sig->m_count; ++i)
    {
        sprintf(buf, "(%s,%d,%d,now())",
                NumberToString(sig->m_entries[i].m_accId, 0),
                G_CEnvironment()->get_channel_no(),
                sig->m_entries[i].m_playSec);
        query += buf;
        if (i < sig->m_count - 1)
        {
            query += ",";
        }
    }
    return 1;
