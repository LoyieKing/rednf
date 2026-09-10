int CRijndael_fake::Decrypt(char const* in, char* out, unsigned int len)
{
    symmetric_key* skey =
        reinterpret_cast<symmetric_key*>(fakeXor(m_ctxObf, m_salt));
    unsigned int blocks = len >> 4;
    for (unsigned int i = 0; i < blocks; ++i) {
        anubis_ecb_decrypt(reinterpret_cast<unsigned char const*>(in),
                           reinterpret_cast<unsigned char*>(out), skey);
        in += 16;
        out += 16;
    }
    return CKS_OK;
