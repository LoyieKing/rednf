int Stream::put_binary(void* src, int len)
{
    while (!enable_in(len))
    {
        resize(size() * 2);
    }
    memcpy(m_buffer + in_ptr(), src, len);
    in_ptr(len);
    return 1;
