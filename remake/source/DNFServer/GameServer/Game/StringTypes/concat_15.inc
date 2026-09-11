WideString WideString::concat(const WideString& src1, const WideString& src2)
{
    if (src1.length() == 0)
        return WideString(src2);
    if (src2.length() == 0)
        return WideString(src1);
    return concat(src1.c_str(), src1.length(), src2.c_str(), src2.length());
}