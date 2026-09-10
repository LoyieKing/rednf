void CLog::initiate(char* name, char* path, int flag)
{
    struct stat st;
    if (stat(name, &st) < 0)
    {
        if (mkdir(name, 0x1fd) < 0)
        {
            std::printf("Fail To Make Directory, PATH[%s]\n", name);
            return;
        }
    }
    m_flag = flag;
    m_initFlag = 1;
    if (path != 0)
        std::memcpy(m_logFile, path, 0x100);
    else
        std::memcpy(m_logFile, "Log", 4);
    m_logTime[0].setTime();
    m_logTime[1].setTime();
    m_logTime[2].setTime();
    m_logTime[3].setTime();
    m_logTime[4].setTime();
    m_logTime[5].setTime();
    m_logTime[6].setTime();
    m_logTime[7].setTime();
    if (name == 0)
    {
        m_logFileNotice = stdout;
        m_logFileError = stdout;
        m_logFileDebug = stdout;
        m_logFileMoney = stdout;
        m_logFileHistory = stdout;
        m_logFileInit = stdout;
        m_logFileSnap = stdout;
    }
    else
    {
        std::strcpy(m_initFile, name);
        openLogFile(m_logTime[0], m_logFileNotice, ".log", m_flag);
        openLogFile(m_logTime[1], m_logFileError, ".error", m_flag);
        openLogFile(m_logTime[2], m_logFileDebug, ".debug", m_flag);
        openLogFile(m_logTime[3], m_logFileMoney, ".money", m_flag);
        openLogFile(m_logTime[4], m_logFileHistory, ".history", m_initFlag);
        openLogFile(m_logTime[6], m_logFileInit, ".init", m_flag);
        openLogFile(m_logTime[5], m_logFileCri, ".cri", m_flag);
        openLogFile(m_logTime[7], m_logFileSnap, ".snap", m_flag);
    }
