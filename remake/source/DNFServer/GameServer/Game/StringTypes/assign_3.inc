void CharString::assign(char src)
{
    if (src == '\0')
    {
        clear();
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(1);
        newData->getBuffer()[0] = src;
        replaceData(newData);
    }
