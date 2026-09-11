CharStringData* CharStringData::create(int length)
{
    int size = length + 1;
    StringData* data = StringData::create(size);
    if (data == 0)
        return 0;
    return static_cast<CharStringData*>(data);
}