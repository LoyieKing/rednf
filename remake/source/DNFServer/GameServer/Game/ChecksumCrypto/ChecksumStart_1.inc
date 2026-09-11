int CNChecksumSha256::ChecksumStart()
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA256_Init(reinterpret_cast<SHA256_ALG_INFO*>(m_ctx));
    return CKS_OK;
}