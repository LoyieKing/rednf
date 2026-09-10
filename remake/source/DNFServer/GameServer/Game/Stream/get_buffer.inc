char* Stream::get_buffer()
{
    return m_buffer + out_ptr();
