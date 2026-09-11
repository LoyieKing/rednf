void CUserCharacInfo::increaseBlueMarbleEnterCount()
{
    if (m_selected)
    {
    ++CUR->m_blueMarbleEnterCount;
    }
}