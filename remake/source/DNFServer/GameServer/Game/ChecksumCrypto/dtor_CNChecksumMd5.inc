CNChecksumMd5::~CNChecksumMd5()
{
    if (m_ctx != 0) {
        ::operator delete(m_ctx);      // ORIG 0x08098370：标量 `_ZdlPv`
        m_ctx = 0;
    }
}