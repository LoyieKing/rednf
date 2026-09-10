int CSHA1::GetHexHash(char* out)
{
    // ORIG 0x0808a410：逐字节 sprintf("%02X")，40 个十六进制字符 + NUL 由调用方持有。
    if (out == 0)
        return 0;
    for (int i = 0; i < 0x14; ++i)
        sprintf(out + i * 2, "%02X", (unsigned int)m_digest[i]);
    return 1;
