int CharString::compare(const CharString& src1, const char* src2)
{
    return strcmp(src1.c_str(), src2);
