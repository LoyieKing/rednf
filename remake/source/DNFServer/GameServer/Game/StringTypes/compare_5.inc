int WideString::compare(const wchar_t* src1, const WideString& src2)
{
    return wcscmp(src1, src2.c_str());
