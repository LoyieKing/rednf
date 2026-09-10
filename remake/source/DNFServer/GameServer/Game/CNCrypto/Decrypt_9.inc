int CNCryptoRc6::Decrypt(const unsigned char* in, int inLen,
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    if (inLen > outCap)
        return 0x7000000a;
    int count = inLen / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        rc6_block_decrypt((const RC6_ALG_INFO*)m_pCrypto,
                          (const unsigned int*)in, (unsigned int*)out);
        in += GetBlockSize();
        out += GetBlockSize();
    }
    return 0x6fffffff;
