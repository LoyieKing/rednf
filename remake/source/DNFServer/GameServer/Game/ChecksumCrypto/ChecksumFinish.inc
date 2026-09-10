int CNChecksumMd5::ChecksumFinish(unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    md5_finish(reinterpret_cast<md5_context*>(m_ctx), out);
    return CKS_OK;
