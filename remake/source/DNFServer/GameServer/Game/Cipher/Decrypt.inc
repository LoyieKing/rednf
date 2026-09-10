int Cipher::Decrypt(int key, const unsigned char* src, int srcLen,
                    unsigned char* dst, int* dstLen)
{
    // [N7 封顶] 同 Encrypt：m_graphs[type]->Decrypt 虚调用寄存器互换。
    if (srcLen < 1)
    {
        *dstLen = 0;
        return 1;
    }
    int type = GetCryptoType(key);
    int bs = m_graphs[type]->GetBlockSize();
    (void)bs;   // ORIG Decrypt 调 GetBlockSize 但不调整长度
    if (m_graphs[type]->Decrypt(src, srcLen, dst, *dstLen) == 0x6fffffff)
    {
        *dstLen = srcLen;
        return 1;
    }
    return 0;
