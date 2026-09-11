void SkillUseItem::clear()
{
    m_index = -1;
    for (int i = 0; i <= 1; ++i)
        m_useLevel[i] = 0;
}