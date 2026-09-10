int WideString::compare(const WideString& src1, const WideString& src2)
{
    return wcscmp(src1.c_str(), src2.c_str());
