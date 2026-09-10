void CharString::assign(const char* src, int srcLength)
{
    if (srcLength == 0)
    {
        clear();
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(srcLength);
        memcpy(newData->getBuffer(), src, srcLength);
        replaceData(newData);
    }
