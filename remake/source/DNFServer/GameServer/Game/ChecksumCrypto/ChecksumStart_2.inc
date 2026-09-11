int CNChecksumSha384::ChecksumStart()
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA384_Init(reinterpret_cast<SHA384_ALG_INFO*>(m_ctx));
    return CKS_OK;
}