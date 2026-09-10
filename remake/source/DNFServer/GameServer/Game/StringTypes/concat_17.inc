WideString WideString::concat(wchar_t ch, const WideString& src1)
{
    if (ch == 0)
        return WideString(src1);
    if (src1.length() == 0)
        return WideString(ch);
    WideStringData* newData = WideStringData::createTerminated(src1.length() + 1);
    newData->getBuffer()[0] = ch;
    memcpy(newData->getBuffer() + 1, src1.c_str(), src1.length() * sizeof(wchar_t));
    return WideString(newData);
