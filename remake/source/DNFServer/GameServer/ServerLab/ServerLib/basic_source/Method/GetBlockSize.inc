int IMethod::GetBlockSize()
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetBlockSize", 0x46, sm_szErrorMsg1);
    }
    return m_blockSize;
