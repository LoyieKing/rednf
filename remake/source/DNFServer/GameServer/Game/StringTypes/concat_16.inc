WideString WideString::concat(const WideString& src1, wchar_t ch)
{
    if (src1.length() == 0)
        return WideString(ch);
    if (ch == 0)
        return WideString(src1);
    WideStringData* newData = WideStringData::createTerminated(src1.length() + 1);
    memcpy(newData->getBuffer(), src1.c_str(), src1.length() * sizeof(wchar_t));
    newData->getBuffer()[src1.length()] = ch;
    return WideString(newData);
