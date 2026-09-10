bool Stream::get_binary(void* dst, int len)
{
    if (!enable_out(len))
    {
        return 0;
    }
    memcpy(dst, m_buffer + out_ptr(), len);
    out_ptr(len);
    return 1;
