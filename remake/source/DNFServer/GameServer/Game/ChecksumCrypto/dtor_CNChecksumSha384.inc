CNChecksumSha384::~CNChecksumSha384()
{
    if (m_ctx != 0) {
        ::operator delete(m_ctx);
        m_ctx = 0;
    }
