bool CSimpleLog::CreateLogFile(DateTime& dt)
{
    char buf[1024] = {0};
    sprintf(buf, "%s%04d%02d%02d.%s", m_path, (int)dt.m_year, (int)dt.m_month, (int)dt.m_day);
    return OpenFile(buf);
