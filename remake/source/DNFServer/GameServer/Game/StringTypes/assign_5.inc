void WideString::assign(const wchar_t* src)
{
    assign(src, static_cast<int>(wcslen(src)));
