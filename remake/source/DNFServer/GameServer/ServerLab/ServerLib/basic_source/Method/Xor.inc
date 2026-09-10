void IMethod::Xor(char* buff, const char* chain)
{
    if (m_bInit != true)
    {
        throw CCryptorException("Xor", 0x22, sm_szErrorMsg1);
    }
    for (int i = 0; i < m_blockSize; i++)
    {
        *buff = *buff ^ *chain;
        buff = buff + 1;
        chain = chain + 1;
    }
