void IMethod::SetPadding(int iPadding)
{
    if (m_bInit != true)
    {
        throw CCryptorException("SetPadding", 0x35, sm_szErrorMsg1);
    }
    if (iPadding < 0 || iPadding >= 3)
    {
        throw CCryptorException("SetPadding", 0x37, sm_szErrorMsg3);
    }
    m_iPadding = iPadding;
