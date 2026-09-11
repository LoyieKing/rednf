int WideString::rfind(int pos, const wchar_t* pat) const
{
    return rfind(pos, pat, static_cast<int>(wcslen(pat)));
}