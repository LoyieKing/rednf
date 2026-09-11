CharString CharString::concat(const CharString& src1, const char* src2, int src2Len)
{
    if (src1.length() == 0)
        return CharString(src2, src2Len);
    if (src2Len == 0)
        return CharString(src1);
    return concat(src1.c_str(), src1.length(), src2, src2Len);
}