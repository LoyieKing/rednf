int WideString::compare(const WideString& src1, const wchar_t* src2)
{
    return wcscmp(src1.c_str(), src2);
}