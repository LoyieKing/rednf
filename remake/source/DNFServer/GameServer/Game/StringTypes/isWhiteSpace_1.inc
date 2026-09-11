bool WideString::isWhiteSpace() const
{
    const wchar_t* buf = buffer_;
    while (*buf != 0)
    {
        if (!isuspace(*buf))
            return false;
        ++buf;
    }
    return true;
}