void CSecialIMonsterDropItemGenRateTable::destroy()
{
    if (m_rand != 0)
    {
        delete m_rand;
        m_rand = 0;
    }
}