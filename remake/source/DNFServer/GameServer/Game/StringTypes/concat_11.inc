WideString WideString::concat(const WideString& src1, const wchar_t* src2)
{
    return concat(src1, src2, static_cast<int>(wcslen(src2)));
