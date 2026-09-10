WideString WideString::concat(const WideString& src1, const wchar_t* src2, int src2Len)
{
    if (src1.length() == 0)
        return WideString(src2, src2Len);
    if (src2Len == 0)
        return WideString(src1);
    return concat(src1.c_str(), src1.length(), src2, src2Len);
