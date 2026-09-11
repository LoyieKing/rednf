CharString::CharString(const char* src)
{
    int length = static_cast<int>(strlen(src));
    if (length == 0)
    {
        attachData(s_emptyCharStringData);
    }
    else
    {
        CharStringData* data = CharStringData::create(length);
        memcpy(data->getBuffer(), src, length + 1);
        attachData(data);
    }
}