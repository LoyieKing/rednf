void GameWorld::destroy()
{
    if (m_pVill != NULL)
    {
        delete[] m_pVill;
        m_pVill = NULL;
    }
