void SimpleEncryption::encryptUINT(unsigned int shift, unsigned int& out)
{
    unsigned int t = 0;   // -0x8 槽
    int u = 0;            // -0x4 槽
    t = m_shift;
    u = (int)m_key;
    out = shift << t;
    out = (unsigned int)u ^ shift;
}