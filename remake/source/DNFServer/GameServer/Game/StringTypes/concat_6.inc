CharString CharString::concat(const CharString& src1, const CharString& src2)
{
    if (src1.length() == 0)
        return CharString(src2);
    if (src2.length() == 0)
        return CharString(src1);
    return concat(src1.c_str(), src1.length(), src2.c_str(), src2.length());
