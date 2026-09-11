int CNChecksumSha512::MakeChecksumFromMemory(unsigned char const* data, int len,
                                             unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (GetChecksumSize() > outSize)
        return CKS_ERR_BUFSIZE;
    if (len <= 0)
        return CKS_ERR_DATALEN;
    SHA512_Init(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx));
    SHA512_Update(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx),
                  const_cast<unsigned char*>(data), (unsigned long)len);
    SHA512_Final(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx), out);
    return CKS_OK;
}