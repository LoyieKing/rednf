int CNChecksumMd5::MakeChecksumFromFile(char const* path, unsigned char* out,
                                        int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    if (path == 0)
        return CKS_ERR_FILESIZE;

    FILE* fp = std::fopen(path, "rb");
    if (fp == 0)
        return CKS_ERR_OPENFILE;

    std::fseek(fp, 0, SEEK_END);
    long fileLen = std::ftell(fp);
    if (fileLen < 0) {
        std::fclose(fp);
        return CKS_ERR_FILESIZE;
    }
    std::fseek(fp, 0, SEEK_SET);

    unsigned char buf[1024];
    md5_starts(reinterpret_cast<md5_context*>(m_ctx));
    for (unsigned long off = 0; off < (unsigned long)fileLen; ) {
        size_t chunk = (unsigned long)fileLen - off;
        if (chunk > sizeof(buf))
            chunk = sizeof(buf);
        std::fread(buf, chunk, 1, fp);
        md5_update(reinterpret_cast<md5_context*>(m_ctx), buf, (int)chunk);
        off += (unsigned long)chunk;
    }
    md5_finish(reinterpret_cast<md5_context*>(m_ctx), out);
    std::fclose(fp);
    return CKS_OK;
