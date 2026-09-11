WideString WideString::concat(const wchar_t* src1, int src1Len, const WideString& src2)
{
    if (src1Len == 0)
        return WideString(src2);
    if (src2.length() == 0)
        return WideString(src1, src1Len);
    return concat(src1, src1Len, src2.c_str(), src2.length());
}