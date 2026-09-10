void IMethod::Xor(char* buff, const char* chain)
{
    for (int i = 0; i < m_blockSize; ++i)
    {
        buff[i] = (char)((unsigned char)buff[i] ^ (unsigned char)chain[i]);
    }
