int IMethod::Pad(char* in, int iLength)
{
    if (m_bInit != true)
    {
        throw CCryptorException("Pad", 0x5c, sm_szErrorMsg1);
    }
    int iRes = iLength % m_blockSize;
    if (iRes != 0)
    {
        int iPadded = m_blockSize - iRes;
        char* pin = in + iLength;
        switch (m_iPadding)
        {
        case 0:
            // ORIG：while 形，体内 i++ 先于 pin++。
            for (int i = 0; i < iPadded; )
            {
                *pin = '\0';
                i++;
                pin = pin + 1;
            }
            break;
        case 1:
            for (int i = 0; i < iPadded; )
            {
                *pin = ' ';
                i++;
                pin = pin + 1;
            }
            break;
        case 2:
            for (int i = 0; i < iPadded; )
            {
                *pin = (char)iPadded;
                i++;
                pin = pin + 1;
            }
            break;
        }
        return iLength + iPadded;
    }
    return iLength;
