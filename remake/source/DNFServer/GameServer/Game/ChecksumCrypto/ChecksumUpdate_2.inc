int CNChecksumSha384::ChecksumUpdate(unsigned char const* data, int len)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA384_Update(reinterpret_cast<SHA384_ALG_INFO*>(m_ctx),
                  const_cast<unsigned char*>(data), (unsigned long)len);
    return CKS_OK;
}