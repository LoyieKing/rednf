void CSwitchLog::operator()(const char* fmt, ...)
{
    if (log_write_[m_flagIndex]) {
        char buf[4096];
        sprintf(buf, "%s(%d): ", m_name, m_line);
        va_list ap;
        va_start(ap, fmt);
        int len = strlen(buf);
        vsnprintf(buf + len, 0xfff - len, fmt, ap);
        // [L23-r4 P0/P183] ORIG 槽表无 log 具名槽（我方多 `mov %eax,-0x14(%ebp)`
        //   使 buf 基址由 -0x1014 漂到 -0x1018）⇒ instance() 结果实参位直传。
        Guard<Mutex> g((Mutex*)((char*)CLog::instance() + m_level * 0x18));
        switch (m_level) {
        case 0:
            CLog::instance()->logNotice(buf);
            break;
        case 1:
            CLog::instance()->logError(buf);
            break;
        case 2:
            CLog::instance()->logDebug(buf);
            break;
        case 3:
            CLog::instance()->logConsole(buf);
            break;
        case 4:
            CLog::instance()->logMoney(buf);
            break;
        case 5:
            CLog::instance()->logCritical(buf);
            break;
        }
    }
