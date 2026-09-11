void SimpleEncryption::decryptUINT(unsigned int shift, unsigned int& out)
{
    unsigned int t = 0;
    int u = 0;
    t = m_shift;
    u = (int)m_key;
    out = shift >> t;
    out = (unsigned int)u ^ shift;
}