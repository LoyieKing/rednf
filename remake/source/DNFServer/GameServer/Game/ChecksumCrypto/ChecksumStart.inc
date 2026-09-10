int CNChecksumMd5::ChecksumStart()
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    md5_starts(reinterpret_cast<md5_context*>(m_ctx));
    return CKS_OK;
