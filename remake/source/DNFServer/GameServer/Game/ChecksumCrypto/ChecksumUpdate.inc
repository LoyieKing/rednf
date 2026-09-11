int CNChecksumMd5::ChecksumUpdate(unsigned char const* data, int len)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    md5_update(reinterpret_cast<md5_context*>(m_ctx),
               const_cast<unsigned char*>(data), len);
    return CKS_OK;
}