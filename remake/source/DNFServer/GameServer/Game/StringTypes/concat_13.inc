WideString WideString::concat(const wchar_t* src1, const WideString& src2)
{
    return concat(src1, static_cast<int>(wcslen(src1)), src2);
