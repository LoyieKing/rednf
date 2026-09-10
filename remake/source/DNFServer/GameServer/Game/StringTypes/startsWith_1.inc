bool WideString::startsWith(const wchar_t* pat, bool isIgnoreCase) const
{
    int patLength = static_cast<int>(wcslen(pat));
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return wcsncasecmp(c_str(), pat, patLength) == 0;
    else
        return wcsncmp(c_str(), pat, patLength) == 0;
