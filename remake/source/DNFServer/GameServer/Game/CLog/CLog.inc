// setLogWriterWired()。
CLog::CLog()
{
    m_logFileHistory = m_logFileNotice = m_logFileError = m_logFileDebug =
        m_logFileMoney = m_logFileCri = m_logFileInit = 0;
    m_logFileSnap = 0;
    m_flag = 0;
    std::memset(m_logFile, 0, sizeof(m_logFile));
    std::memset(m_initFile, 0, sizeof(m_initFile));
    setLogWriterWired();
}