CStreamGuard::CStreamGuard(Stream* stream, bool bOwn)
{
    m_stream = stream;
    m_bOwn = bOwn;
