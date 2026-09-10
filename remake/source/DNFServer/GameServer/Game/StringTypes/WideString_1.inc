WideString::WideString(const WideString& src)
{
    WideStringData* newData = src.getData();
    attachData(newData);
