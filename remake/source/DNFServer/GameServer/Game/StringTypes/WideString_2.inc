WideString::WideString(const wchar_t* src)
{
    int length = static_cast<int>(wcslen(src));
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::create(length);
        memcpy(data->getBuffer(), src, (length + 1) * sizeof(wchar_t));
        attachData(data);
    }
}