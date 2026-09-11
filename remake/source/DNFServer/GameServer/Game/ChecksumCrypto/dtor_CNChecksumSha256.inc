CNChecksumSha256::~CNChecksumSha256()
{
    if (m_ctx != 0) {
        ::operator delete(m_ctx);      // ORIG 0x0809871c：标量 `_ZdlPv`
        m_ctx = 0;
    }
}