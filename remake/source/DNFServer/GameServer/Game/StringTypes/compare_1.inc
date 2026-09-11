int CharString::compare(const CharString& src1, const CharString& src2)
{
    return strcmp(src1.c_str(), src2.c_str());
}