int CharString::compareNoCase(const CharString& src1, const char* src2)
{
    return strcasecmp(src1.c_str(), src2);
}