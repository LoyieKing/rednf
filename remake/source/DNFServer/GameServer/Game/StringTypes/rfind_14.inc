int WideString::rfind(wchar_t pat) const
{
    return rfind(length() - 1, pat);
}