int CSHA1::GetHash(unsigned char* out) const
{
    // ORIG 0x0808a3d8：memcpy digest[20]。
    if (out == 0)
        return 0;
    memcpy(out, m_digest, 0x14);
    return 1;
