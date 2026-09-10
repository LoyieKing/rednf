CharString CharString::concat(const CharString& src1, char ch)
{
    if (src1.length() == 0)
        return CharString(ch);
    if (ch == '\0')
        return CharString(src1);
    CharStringData* newData = CharStringData::createTerminated(src1.length() + 1);
    memcpy(newData->getBuffer(), src1.c_str(), src1.length());
    newData->getBuffer()[src1.length()] = ch;
    return CharString(newData);
