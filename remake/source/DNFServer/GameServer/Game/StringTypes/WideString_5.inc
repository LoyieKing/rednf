WideString::WideString(wchar_t src)
{
    WideStringData* data = WideStringData::createTerminated(1);
    data->getBuffer()[0] = src;
    attachData(data);
