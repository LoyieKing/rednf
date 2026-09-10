int CNChecksumSha256::ChecksumFinish(unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    SHA256_Final(reinterpret_cast<SHA256_ALG_INFO*>(m_ctx), out);
    return CKS_OK;
