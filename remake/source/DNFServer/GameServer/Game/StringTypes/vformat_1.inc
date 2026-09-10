WideString WideString::vformat(const wchar_t* formatStr, va_list argList)
{
    // b28d-Main：static sret 按值（ORIG 0x08adbc88）。
    int length = vswprintf(reinterpret_cast<wchar_t*>(0), 0, formatStr, argList);
    if (length == 0)
        return WideString();
    WideStringData* newData = WideStringData::create(length);
    ss_vswprintf(newData->getBuffer(), length + 1, formatStr, argList);
    return WideString(newData);
