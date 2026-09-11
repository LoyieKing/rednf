int WideString::compareNoCase(const WideString& src1, const WideString& src2)
{
    return wcscasecmp(src1.c_str(), src2.c_str());
}