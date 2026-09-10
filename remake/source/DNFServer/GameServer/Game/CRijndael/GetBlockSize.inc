int IMethod::GetBlockSize(unsigned int* outError)
{
    // ORIG _ZN7IMethod12GetBlockSizeEPj @0x080b4416（0x2f）
    if (!m_bInit)
    {
        if (outError != NULL)
        {
            *outError = 0x70000005;
        }
        return 0;
    }
    return m_blockSize;
