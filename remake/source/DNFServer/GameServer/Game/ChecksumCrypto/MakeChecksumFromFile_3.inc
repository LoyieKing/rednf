int CNChecksumSha512::MakeChecksumFromFile(char const* path, unsigned char* out,
                                           int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (GetChecksumSize() > outSize)
        return CKS_ERR_BUFSIZE;
    if (path == 0)
        return CKS_ERR_FILESIZE;

    SHA512_Init(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx));

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
    for (unsigned long off = 0; off < (unsigned long)fileLen; ) {
        size_t chunk = (unsigned long)fileLen - off;
        if (chunk > sizeof(buf))
            chunk = sizeof(buf);
        std::fread(buf, chunk, 1, fp);
        SHA512_Update(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx), buf,
                      (unsigned long)chunk);
        off += (unsigned long)chunk;
    }
    SHA512_Final(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx), out);
    std::fclose(fp);
    return CKS_OK;
