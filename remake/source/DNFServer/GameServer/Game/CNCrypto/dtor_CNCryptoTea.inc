CNCryptoTea::~CNCryptoTea()
{
    if (m_pCrypto)
    {
        delete (CTEA*)m_pCrypto;
        m_pCrypto = 0;
    }
}