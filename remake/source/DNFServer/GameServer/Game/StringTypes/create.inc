StringData* StringData::create(int size)
{
    StringData* data = static_cast<StringData*>(malloc(size + 8));
    if (data == 0)
        return 0;
    data->m_refCount = 0;
    data->m_size = size;
    return data;
}