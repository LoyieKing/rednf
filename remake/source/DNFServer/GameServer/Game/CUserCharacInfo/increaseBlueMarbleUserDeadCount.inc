void CUserCharacInfo::increaseBlueMarbleUserDeadCount()
{
    if (m_selected)
    {
    ++CUR->m_blueMarbleUserDeadCount;
    }
}