CharString CharString::concat(const CharString& src1, const char* src2)
{
    return concat(src1, src2, static_cast<int>(strlen(src2)));
