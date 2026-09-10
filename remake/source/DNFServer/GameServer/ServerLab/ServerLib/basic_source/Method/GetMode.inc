int IMethod::GetMode()
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetMode", 0x4d, sm_szErrorMsg1);
    }
    return m_iMode;
