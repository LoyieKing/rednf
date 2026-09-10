int WideString::compareNoCase(const WideString& src1, const wchar_t* src2)
{
    return wcscasecmp(src1.c_str(), src2);
