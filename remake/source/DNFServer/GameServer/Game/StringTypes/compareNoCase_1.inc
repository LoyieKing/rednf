int CharString::compareNoCase(const CharString& src1, const CharString& src2)
{
    return strcasecmp(src1.c_str(), src2.c_str());
}