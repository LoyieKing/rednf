int CNChecksumMd5::MakeChecksumFromMemory(unsigned char const* data, int len,
                                          unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    md5_starts(reinterpret_cast<md5_context*>(m_ctx));
    md5_update(reinterpret_cast<md5_context*>(m_ctx),
               const_cast<unsigned char*>(data), len);
    md5_finish(reinterpret_cast<md5_context*>(m_ctx), out);
    return CKS_OK;
}