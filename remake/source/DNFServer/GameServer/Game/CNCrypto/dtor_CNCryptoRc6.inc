CNCryptoRc6::~CNCryptoRc6()
{
    if (m_pCrypto)
    {
        delete (RC6_ALG_INFO*)m_pCrypto;
        m_pCrypto = 0;
    }
}