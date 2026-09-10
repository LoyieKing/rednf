int Village::get_gate_area()
{
    for (int i = 0; i < m_areaCount; ++i)
    {
        if (m_pAreas[i].m_areaType == 1)
            return i;
    }
    return 1;
