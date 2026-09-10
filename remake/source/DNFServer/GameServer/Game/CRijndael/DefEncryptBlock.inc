int CRijndael::DefEncryptBlock(const char* in, char* out)
{
    if (!m_bInit)
        return 0x70000005;
    EncryptCore((const unsigned char*)in, (unsigned char*)out);
    return 0x6fffffff;
