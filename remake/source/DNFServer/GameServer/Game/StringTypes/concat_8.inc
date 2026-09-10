CharString CharString::concat(char ch, const CharString& src1)
{
    if (ch == '\0')
        return CharString(src1);
    if (src1.length() == 0)
        return CharString(ch);
    CharStringData* newData = CharStringData::createTerminated(src1.length() + 1);
    newData->getBuffer()[0] = ch;
    memcpy(newData->getBuffer() + 1, src1.c_str(), src1.length());
    return CharString(newData);
