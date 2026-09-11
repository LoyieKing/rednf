void CUser::LogHistory(const char* fmt, ...)
{
    // ORIG 0x0867c8d8：日志前缀（Hangame 服 'X' 否则 'D'）+ 固定头（账号/角色/
    // 时间/成长/职业/等级/IP/端口/频道）+ 变参 fmt 追加，经 cHistoryTrace 输出。
    // 布局对齐（ORIG 0x867c8fe 帧 0x4bc）：typeFlag@-0x19、history@-0x1a、
    // timeLog@-0x22、accIdBuf[24]@-0x3a、ap@-0x40、buf[1024]@-0x440，
    // sprintf 实参右→左求值，temp 槽自 -0x470 起。
    char typeFlag;
    if (isHangameUser())
        typeFlag = 'X';
    else
        typeFlag = 'D';
    cHistoryTrace history;
    TimeLog_ timeLog;
    timeLog.setTime();
    char accIdBuf[24];
    memset(accIdBuf, 0, 4);
    strcpy(accIdBuf, NumberToString(get_acc_id(), 7));
    va_list ap;
    char buf[1024];
    sprintf(buf,
        "\"%s\",%s,%c,%02d%02d%02d,\"%s\",%d,%d,%d,%d,%s,%s,%d,%d, ",
        m_accName, NumberToString(get_acc_id(), 0), typeFlag,
        (int)(char)(unsigned char)timeLog.m_hour,
        (int)(char)(unsigned char)timeLog.m_min,
        (int)(char)(unsigned char)timeLog.m_sec,
        getCurCharacR() ? getCurCharacName() : "",
        getCurCharacNo(), get_charac_level(), get_charac_job(),
        (int)getCurCharFirstGrowType(),
        getWebAddress(),
        m_network.GetPeerIP2(reinterpret_cast<char*>(this) + 0x6eef0, 0x10),
        (int)m_network.get_port(),
        G_CEnvironment()->m_serverEnvir.m_channelNo);
    va_start(ap, fmt);
    vsprintf(buf + strlen(buf), fmt, ap);
    va_end(ap);
    history(buf);
}