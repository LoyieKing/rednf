// ORIG 0x898d37c：m_field0==1 且 m_vec4 非空 → m_vec4[0].first，否则 -1
int DyeInfo::getDyeColorIndex()
{
    if (m_field0 == 1)
    {
        if (m_vec4.empty())
            return -1;
        return m_vec4[0].first;
    }
    return -1;
}