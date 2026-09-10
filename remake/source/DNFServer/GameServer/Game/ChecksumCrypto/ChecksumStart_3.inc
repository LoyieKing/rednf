int CNChecksumSha512::ChecksumStart()
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA512_Init(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx));
    return CKS_OK;
