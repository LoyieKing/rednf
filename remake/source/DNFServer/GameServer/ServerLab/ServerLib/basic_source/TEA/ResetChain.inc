void CTEA::ResetChain()
{
    if (m_bInit != true)
    {
        throw CCryptorException("ResetChain", 0x4d, sm_szErrorMsg1);
    }
    memcpy(m_apchain, m_apchain0, m_blockSize);
