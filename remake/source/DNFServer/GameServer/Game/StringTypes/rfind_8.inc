int WideString::rfind(const wchar_t* pat) const
{
    int patLength = static_cast<int>(wcslen(pat));
    return rfind(length() - patLength, pat, patLength);
}