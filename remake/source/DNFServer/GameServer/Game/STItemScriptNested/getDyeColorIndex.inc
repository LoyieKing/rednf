int DyeInfo::getDyeColorIndex()
{
    if (m_field0 == 1)
    {
        if (m_vec4.empty())
            return -1;
        return m_vec4[0].first;
    }
    return -1;
