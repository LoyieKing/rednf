BlueMarble::~BlueMarble()
{
    if (m_pRands)
    {
        delete[] m_pRands;
    }
    m_pRands = 0;
