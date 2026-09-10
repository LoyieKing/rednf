int WideString::find(const wchar_t* pat) const
{
    return find(0, pat, static_cast<int>(wcslen(pat)));
