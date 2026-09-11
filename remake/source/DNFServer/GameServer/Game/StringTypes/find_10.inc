int WideString::find(const wchar_t* pat, int patLength) const
{
    return find(0, pat, patLength);
}