// ORIG 0x0812268c W：bGM=false→m_actionPoint1（Chn_ActionPointSystem.etc），
// true→m_actionPoint2（_GM.etc）。见 CDataManager/init.md 0x6079 上下文。
APSystem::CActionPointEtcParameter* CDataManager::GetActionPointEtcParameter(bool bGM)
{
    return bGM ? &m_actionPoint2 : &m_actionPoint1;
}