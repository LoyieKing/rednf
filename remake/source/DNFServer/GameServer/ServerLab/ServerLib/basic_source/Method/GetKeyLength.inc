int IMethod::GetKeyLength()
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetKeyLength", 0x3f, sm_szErrorMsg1);
    }
    return m_keylength;
