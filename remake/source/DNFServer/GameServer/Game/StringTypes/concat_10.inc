WideString WideString::concat(const wchar_t* src1, int src1Len,
                              const wchar_t* src2, int src2Len)
{
    if (src1Len + src2Len == 0)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(src1Len + src2Len);
    memcpy(newData->getBuffer(), src1, src1Len * sizeof(wchar_t));
    memcpy(newData->getBuffer() + src1Len, src2, src2Len * sizeof(wchar_t));
    return WideString(newData);
