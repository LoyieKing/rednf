CNCryptoBlowFish::~CNCryptoBlowFish()
{
    if (m_pCrypto)
    {
        delete (symmetric_key*)m_pCrypto;
        m_pCrypto = 0;
    }
