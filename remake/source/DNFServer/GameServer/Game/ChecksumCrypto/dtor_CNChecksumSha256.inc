CNChecksumSha256::~CNChecksumSha256()
{
    if (m_ctx != 0) {
        delete[] m_ctx;
        m_ctx = 0;
    }
