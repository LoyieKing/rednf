Stream::~Stream()
{
    delete[] m_buffer;
