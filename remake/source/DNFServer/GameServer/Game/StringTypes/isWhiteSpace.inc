bool CharString::isWhiteSpace() const
{
    const char* buf = buffer_;
    while (*buf != '\0')
    {
        if (isspace(static_cast<int>(*buf)) == 0)
            return false;
        ++buf;
    }
    return true;
