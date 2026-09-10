APSystem::CActionPointEtcParameter* CDataManager::GetActionPointEtcParameter(bool bGM)
{
    return bGM ? &m_actionPoint2 : &m_actionPoint1;
