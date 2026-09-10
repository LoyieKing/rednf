void CItemGenRateTable::destroy()
{
    // ORIG 0x08534638。
    if (m_rand != 0)
    {
        delete m_rand;
        m_rand = 0;
    }
