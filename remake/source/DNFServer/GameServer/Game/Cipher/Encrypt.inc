int Cipher::Encrypt(int key, const unsigned char* src, int srcLen,
                    unsigned char* dst, int* dstLen)
{
    // [N7 封顶] m_graphs[type]->Encrypt 虚调用：ORIG funcptr→%ecx/实参链→%edx，
    // 4.4 互换（funcptr→%edx/实参→%ecx），寄存器分配跨版本差。
    if (srcLen < 1)
    {
        *dstLen = 0;
        return 1;
    }
    int type = GetCryptoType(key);
    int outCap = *dstLen;   // ORIG 在选图前死读 *dstLen（-0x10 槽，后续未用）
    (void)outCap;
    int bs = m_graphs[type]->GetBlockSize();
    if (srcLen % bs != 0)
        srcLen += bs - srcLen % bs;
    if (m_graphs[type]->Encrypt(src, srcLen, dst, *dstLen) == 0x6fffffff)
    {
        *dstLen = srcLen;
        return 1;
    }
    return 0;
}