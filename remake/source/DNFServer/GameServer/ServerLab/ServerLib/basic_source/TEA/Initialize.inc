void CTEA::Initialize(const char* keydata, int keydatalength, const char* chain, int iMode, int iPadding)
{
    if (keydata == NULL)
    {
        throw CCryptorException("Initialize", 0x19, sm_szErrorMsg4);
    }
    if (keydatalength <= 0)
    {
        throw CCryptorException("Initialize", 0x1b, sm_szErrorMsg5);
    }
    if (iMode < 0 || iMode > 2)
    {
        throw CCryptorException("Initialize", 0x1d, sm_szErrorMsg2);
    }
    if (iPadding < 0 || iPadding > 2)
    {
        throw CCryptorException("Initialize", 0x1f, sm_szErrorMsg3);
    }

    m_iMode = iMode;
    m_iPadding = iPadding;

    char key[16];
    int i = 0;
    int pos = 0;
    while (i < m_keylength)
    {
        key[i] = keydata[pos];
        i = i + 1;
        pos = (pos + 1) % keydatalength;
    }

    // ORIG 声明序：bSameKey 先于 bSameChain（槽位 -0x1a/-0x19）。
    char bSameKey = 0;
    char bSameChain = 0;
    if (m_bInit)
    {
        if (memcmp(m_apchain0, chain, m_blockSize) == 0)
        {
            bSameChain = 1;
        }
        if (memcmp(m_apKey, key, m_keylength) == 0)
        {
            bSameKey = 1;
        }
    }
    if (bSameChain)
    {
        memcpy(m_apchain, m_apchain0, m_blockSize);
    }
    else
    {
        memcpy(m_apchain0, chain, m_blockSize);
        memcpy(m_apchain, chain, m_blockSize);
    }
    if (bSameKey == 0)
    {
        memcpy(m_apKey, key, m_keylength);
        BytesToWord((const unsigned char*)key, m_auiKey[0]);
        BytesToWord((const unsigned char*)key + 4, m_auiKey[1]);
        BytesToWord((const unsigned char*)key + 8, m_auiKey[2]);
        BytesToWord((const unsigned char*)key + 12, m_auiKey[3]);
        m_bInit = true;
    }
    return;
