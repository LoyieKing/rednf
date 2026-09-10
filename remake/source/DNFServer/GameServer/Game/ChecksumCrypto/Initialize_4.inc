int CRijndael_fake::Initialize(char const* key, int keyLen,
                               char const* /*chain*/, int /*chainLen*/,
                               int /*blockSize*/, int /*mode*/, int /*padding*/)
{
    if (keyLen <= 15)                          // ORIG cmpl $0xf
        return CKS_ERR_OPENFILE;               // 0x70000002
    symmetric_key* skey =
        reinterpret_cast<symmetric_key*>(fakeXor(m_ctxObf, m_salt));
    int rc = anubis_setup(reinterpret_cast<unsigned char const*>(key), 16, 12,
                          skey);
    if (rc == 0)
        return CKS_OK;                         // 0x6fffffff
    if (rc == CRYPT_INVALID_ROUNDS)
        return (int)0x7000001a;
    if (rc == CRYPT_INVALID_KEYSIZE)
        return CKS_ERR_OPENFILE;
    return CKS_ERR_NOSTATE;                    // ORIG 其它分支返 0x70000000
