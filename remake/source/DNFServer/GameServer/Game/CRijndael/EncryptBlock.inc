int CRijndael::EncryptBlock(const char* in, char* out)
{
    if (!m_bInit)
        return 0x70000005;
    if (m_blockSize == 0x10)
        return DefEncryptBlock(in, out);
    EncryptCore((const unsigned char*)in, (unsigned char*)out);
    return 0x6fffffff;
}