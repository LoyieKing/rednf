GenerateRandomUpgrade::~GenerateRandomUpgrade()
{
    if (m_rand)
        delete m_rand;
    m_rand = 0;
}