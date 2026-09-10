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
