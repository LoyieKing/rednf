WideString WideString::pattern(const wchar_t* pattern, int count)
{
    int patternLen = static_cast<int>(wcslen(pattern));
    if (patternLen == 0 || count < 1)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(patternLen * count);
    wchar_t* buffer = newData->getBuffer();
    for (int i = 0; i < count; ++i)
    {
        memcpy(buffer, pattern, patternLen * sizeof(wchar_t));
        buffer += patternLen;
    }
    return WideString(newData);
