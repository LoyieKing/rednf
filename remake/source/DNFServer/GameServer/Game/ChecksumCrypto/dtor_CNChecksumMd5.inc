CNChecksumMd5::~CNChecksumMd5()
{
    if (m_ctx != 0) {
        delete[] m_ctx;
        m_ctx = 0;
    }
