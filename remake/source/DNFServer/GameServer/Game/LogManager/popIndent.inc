void LogManager::popIndent()
{
    if (s_indent > 0)
    {
        --s_indent;
    }
}