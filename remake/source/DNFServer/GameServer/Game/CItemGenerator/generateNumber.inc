int CGenerateRandomNumber::generateNumber(int idx, int range)
{
    if (m_count < idx)
        return get_rand_int(range);
    return m_table[idx].randInt((unsigned long)range);
