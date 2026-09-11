WideString::WideString(const wchar_t* src, int length)
{
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::createTerminated(length);
        memcpy(data->getBuffer(), src, length * sizeof(wchar_t));
        attachData(data);
    }
}