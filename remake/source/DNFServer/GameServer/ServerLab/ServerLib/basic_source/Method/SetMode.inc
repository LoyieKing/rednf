void IMethod::SetMode(int iMode)
{
    if (m_bInit != true)
    {
        throw CCryptorException("SetMode", 0x2b, sm_szErrorMsg1);
    }
    if (iMode < 0 || iMode >= 3)
    {
        throw CCryptorException("SetMode", 0x2d, sm_szErrorMsg2);
    }
    m_iMode = iMode;
