// CharString::vformat 同形（ORIG T 0x08adbc88）。
// [b28d-solo-StringTypes] verdict=N83 封顶（ORIG 0x08adbc88）：同 CharString::vformat
// （4.4 placement operator new 恒发 `_ZnwjPv` call + 判空，无合法源形可内联）。
WideString WideString::vformat(const wchar_t* formatStr, va_list argList)
{
    // b28d-Main：static sret 按值（ORIG 0x08adbc88）。
    int length = vswprintf(reinterpret_cast<wchar_t*>(0), 0, formatStr, argList);
    if (length == 0)
        return WideString();
    WideStringData* newData = WideStringData::create(length);
    ss_vswprintf(newData->getBuffer(), length + 1, formatStr, argList);
    return WideString(newData);
}