void CharString::replaceData(CharStringData* newData)
{
    CharStringData* oldData = getData();
    newData->incRef();
    buffer_ = newData->getBuffer();
    oldData->decRef();
