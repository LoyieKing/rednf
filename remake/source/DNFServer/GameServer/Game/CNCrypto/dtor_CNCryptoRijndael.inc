CNCryptoRijndael::~CNCryptoRijndael()
{
    if (m_pCrypto)
    {
        delete (CRijndael*)m_pCrypto;
        m_pCrypto = 0;
    }
}