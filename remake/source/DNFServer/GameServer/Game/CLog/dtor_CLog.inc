// ORIG CLog::~CLog 0x0854e69c：逐个关非 stdout 句柄（0xf0/0xf4/0xf8/0x10c
// 关后置 0；0xfc/0x100/0x108 仅关不置 0；0x104 漏关——ORIG 行为照抄）；
// m_mutexes 由编译器隐式析构（循环）。
CLog::~CLog()
{
    if (m_logFileNotice != 0)
    {
        if (m_logFileNotice != stdout)
            std::fclose(m_logFileNotice);
        m_logFileNotice = 0;
    }
    if (m_logFileError != 0)
    {
        if (m_logFileError != stdout)
            std::fclose(m_logFileError);
        m_logFileError = 0;
    }
    if (m_logFileDebug != 0)
    {
        if (m_logFileDebug != stdout)
            std::fclose(m_logFileDebug);
        m_logFileDebug = 0;
    }
    if (m_logFileMoney != 0 && m_logFileMoney != stdout)
        std::fclose(m_logFileMoney);
    if (m_logFileHistory != 0 && m_logFileHistory != stdout)
        std::fclose(m_logFileHistory);
    if (m_logFileInit != 0 && m_logFileInit != stdout)
        std::fclose(m_logFileInit);
    if (m_logFileSnap != 0)
    {
        if (m_logFileSnap != stdout)
            std::fclose(m_logFileSnap);
        m_logFileSnap = 0;
    }
}