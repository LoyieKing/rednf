bool isUtf8StrInUnicodeRange(const char* str)
{
    WideString wide;
    convertFromUtf8(str, (int)strlen(str), &wide);
    return isWideStrInUnicodeRange(wide.c_str());
