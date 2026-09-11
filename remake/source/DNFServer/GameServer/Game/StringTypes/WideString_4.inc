WideString::WideString(const std::wstring& src)
{
    int length = static_cast<int>(src.size());
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::create(length);
        memcpy(data->getBuffer(), src.c_str(), (length + 1) * sizeof(wchar_t));
        attachData(data);
    }
}