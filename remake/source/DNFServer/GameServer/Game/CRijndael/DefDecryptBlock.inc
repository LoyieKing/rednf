int CRijndael::DefDecryptBlock(const char* in, char* out)
{
    if (!m_bInit)
        return 0x70000005;
    DecryptCore((const unsigned char*)in, (unsigned char*)out);
    return 0x6fffffff;
