void WideString::attachData(WideStringData* newData)
{
    newData->incRef();
    buffer_ = newData->getBuffer();
}