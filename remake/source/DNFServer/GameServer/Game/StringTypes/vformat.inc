CharString CharString::vformat(const char* formatStr, va_list argList)
{
    // b28d-Main：static sret 按值（ORIG 0x08ad8b9e）。
    int length = vsnprintf(reinterpret_cast<char*>(0), 0, formatStr, argList);
    if (length == 0)
        return CharString();
    CharStringData* newData = CharStringData::create(length);
    ss_vsprintf(newData->getBuffer(), length + 1, formatStr, argList);
    return CharString(newData);
