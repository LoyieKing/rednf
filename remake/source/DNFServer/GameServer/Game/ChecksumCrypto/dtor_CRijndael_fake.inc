CRijndael_fake::~CRijndael_fake()
{   // ORIG 0x080b82c8：delete(ptr ^ salt)
    ::operator delete(fakeXor(m_ctxObf, m_salt));   // ORIG：标量 `_ZdlPv`
    m_ctxObf = 0;
}