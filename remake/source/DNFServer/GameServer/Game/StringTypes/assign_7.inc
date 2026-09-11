void WideString::assign(wchar_t src)
{
    if (src == 0)
    {
        clear();
    }
    else
    {
        WideStringData* newData = WideStringData::createTerminated(1);
        newData->getBuffer()[0] = src;
        replaceData(newData);
    }
}