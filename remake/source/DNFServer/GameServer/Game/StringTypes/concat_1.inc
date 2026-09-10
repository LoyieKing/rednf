CharString CharString::concat(const char* src1, int src1Len,
                              const char* src2, int src2Len)
{
    if (src1Len + src2Len == 0)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(src1Len + src2Len);
    memcpy(newData->getBuffer(), src1, src1Len);
    memcpy(newData->getBuffer() + src1Len, src2, src2Len);
    return CharString(newData);
