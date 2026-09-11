int CBattle_Field::GetCurrentMapIndex()
{
    if (m_pMap != 0)
    {
        return m_pMap->get_index();
    }
    return 0;
}