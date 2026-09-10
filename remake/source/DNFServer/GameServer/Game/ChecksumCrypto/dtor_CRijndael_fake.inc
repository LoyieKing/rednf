CRijndael_fake::~CRijndael_fake()
{   // ORIG 0x080b82c8：delete(ptr ^ salt)
    delete[] fakeXor(m_ctxObf, m_salt);
    m_ctxObf = 0;
