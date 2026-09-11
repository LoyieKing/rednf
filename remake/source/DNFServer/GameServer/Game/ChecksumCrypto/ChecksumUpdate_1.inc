int CNChecksumSha256::ChecksumUpdate(unsigned char const* data, int len)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA256_Update(reinterpret_cast<SHA256_ALG_INFO*>(m_ctx),
                  const_cast<unsigned char*>(data), (unsigned long)len);
    return CKS_OK;
}