void CUserCharacInfo::increaseBlueMarbleUserDeadCount()
{
    if (m_selected)
    {
    ++m_selected->m_blueMarbleUserDeadCount;
    }
}