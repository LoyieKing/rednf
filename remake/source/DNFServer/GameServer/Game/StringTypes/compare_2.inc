int CharString::compare(const char* src1, const CharString& src2)
{
    return strcmp(src1, src2.c_str());
}