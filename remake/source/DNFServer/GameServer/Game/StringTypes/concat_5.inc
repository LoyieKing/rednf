CharString CharString::concat(const char* src1, int src1Len, const CharString& src2)
{
    if (src1Len == 0)
        return CharString(src2);
    if (src2.length() == 0)
        return CharString(src1, src1Len);
    return concat(src1, src1Len, src2.c_str(), src2.length());
