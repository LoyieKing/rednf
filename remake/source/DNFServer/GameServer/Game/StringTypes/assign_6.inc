void WideString::assign(const wchar_t* src, int srcLength)
{
    if (srcLength == 0)
    {
        clear();
    }
    else
    {
        WideStringData* newData = WideStringData::createTerminated(srcLength);
        memcpy(newData->getBuffer(), src, srcLength * sizeof(wchar_t));
        replaceData(newData);
    }
}