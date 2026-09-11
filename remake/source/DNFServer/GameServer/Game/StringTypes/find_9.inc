int WideString::find(int pos, const wchar_t* pat) const
{
    return find(pos, pat, static_cast<int>(wcslen(pat)));
}