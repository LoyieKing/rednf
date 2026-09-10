CharString CharString::pattern(const char* pattern, int count)
{
    int patternLen = static_cast<int>(strlen(pattern));
    if (patternLen == 0 || count < 1)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(patternLen * count);
    char* buffer = newData->getBuffer();
    for (int i = 0; i < count; ++i)
    {
        memcpy(buffer, pattern, patternLen);
        buffer += patternLen;
    }
    return CharString(newData);
