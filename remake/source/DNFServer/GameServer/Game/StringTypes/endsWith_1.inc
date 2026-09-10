bool WideString::endsWith(const wchar_t* pat, bool isIgnoreCase) const
{
    int patLength = static_cast<int>(wcslen(pat));
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad9df8）：同 CharString::endsWith，
    // 左结合 `c_str() + length() - patLength` 复现 4.1 六段物化（P205 拆形族）。
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return wcsncasecmp(c_str() + length() - patLength, pat, patLength) == 0;
    return wcsncmp(c_str() + length() - patLength, pat, patLength) == 0;
