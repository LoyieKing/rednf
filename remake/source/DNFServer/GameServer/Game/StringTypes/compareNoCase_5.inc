int WideString::compareNoCase(const wchar_t* src1, const WideString& src2)
{
    return wcscasecmp(src1, src2.c_str());
}