bool CharString::startsWith(const char* pat, bool isIgnoreCase) const
{
    int patLength = static_cast<int>(strlen(pat));
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return strncasecmp(c_str(), pat, patLength) == 0;
    else
        return strncmp(c_str(), pat, patLength) == 0;
}