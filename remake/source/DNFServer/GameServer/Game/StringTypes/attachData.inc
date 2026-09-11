void CharString::attachData(CharStringData* newData)
{
    newData->incRef();
    buffer_ = newData->getBuffer();
}