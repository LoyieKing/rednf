COpensslRSA::~COpensslRSA()
{
    if (m_rsa != 0)
    {
        RSA_free(m_rsa);
        m_rsa = 0;
    }
