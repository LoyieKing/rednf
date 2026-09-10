int IMethod::GetPadding()
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetPadding", 0x54, sm_szErrorMsg1);
    }
    return m_iPadding;
