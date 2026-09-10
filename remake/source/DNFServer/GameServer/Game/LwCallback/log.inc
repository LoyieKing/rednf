void LwCallback::log(char const* file, char const* func, int line, char const* msg)
{
    if (m_callback)
        m_callback(file, func, line, msg);
