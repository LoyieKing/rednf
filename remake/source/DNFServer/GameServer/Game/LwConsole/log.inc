void LwConsole::log(char const* file, char const* func, int line, char const* msg)
{
    if (m_sourcePositionVisible)
        printf("%s(%d) %s: ", file, line, func);
    puts(msg);
}