// ORIG checkDay 0x0854f594：flag==0 仅比较日；flag!=0 且 (last->m_day !=
// t.m_day || (char)last->m_hour + flag <= (char)t.m_hour) 时 memcpy 8 字节、
// ORIG checkDay 0x0854f594：flag!=0 且 (last->m_day != t.m_day ||
// t.m_hour >= last->m_hour + flag) 时 memcpy 8 字节、fclose、openLogFile 重开；
// flag==0 仅比较日。恒返回 true。
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
}