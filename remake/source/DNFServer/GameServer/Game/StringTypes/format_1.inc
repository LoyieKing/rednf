WideString WideString::format(const wchar_t* formatStr, ...)
{
    // b28d-Main：static sret 按值（ORIG 0x08adbbe4）。
    va_list argList;
    va_start(argList, formatStr);
    // [b28d-solo-StringTypes] verdict=N83 封顶（ORIG 0x08adbbe4）：同 CharString::format——
    // ORIG 体是 vformat 逻辑的独立副本（无 call vformat），展开完整逻辑；
    // 余下仅 N83（placement operator new）封顶。
    int length = vswprintf(reinterpret_cast<wchar_t*>(0), 0, formatStr, argList);
    if (length == 0)
    {
        va_end(argList);
        return WideString();
    }
    WideStringData* newData = WideStringData::create(length);
    ss_vswprintf(newData->getBuffer(), length + 1, formatStr, argList);
    va_end(argList);
    return WideString(newData);
