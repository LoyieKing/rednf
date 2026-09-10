WideStringData* WideStringData::create(int length)
{
    int size = (length + 1) * 4;
    StringData* data = StringData::create(size);
    if (data == 0)
        return 0;
    return static_cast<WideStringData*>(data);
