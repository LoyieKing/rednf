void LwServerWriter::log(char const* /*file*/, char const* func, int line,
                         char const* msg)
{
    char buf[0x1000];
    if (func != 0)
        snprintf(buf, 0xfff, "%s(%d): %s", func, line, msg);
    else
        snprintf(buf, 0xfff, "%s", msg);

    Guard<Mutex> guard(&CLog::instance()->m_mutexes[m_type]);
    switch (m_type) {
    case ENUM_LOG_TYPE_1:
        CLog::instance()->logError(buf);
        break;
    case ENUM_LOG_TYPE_2:
        CLog::instance()->logDebug(buf);
        break;
    }
