int CharString::compareNoCase(const char* src1, const CharString& src2)
{
    return strcasecmp(src1, src2.c_str());
}