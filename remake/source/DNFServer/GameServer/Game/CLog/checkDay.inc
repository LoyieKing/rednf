bool CLog::checkDay(TimeLog_ t, TimeLog_* last, FILE*& out, char* ext, int flag)
{
    if (flag != 0)
    {
        if (last->m_day != t.m_day || t.m_hour >= last->m_hour + flag)
        {
            std::memcpy(last, &t, 8);
            std::fclose(out);
            openLogFile(t, out, ext, flag);
        }
    }
    else
    {
        if (last->m_day != t.m_day)
        {
            std::memcpy(last, &t, 8);
            std::fclose(out);
            openLogFile(t, out, ext, flag);
        }
    }
    return true;
