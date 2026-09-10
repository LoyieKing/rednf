CharString CharString::concat(const char* src1, const CharString& src2)
{
    return concat(src1, static_cast<int>(strlen(src1)), src2);
