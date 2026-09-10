int CGameManager::getAdvanceAltarIndex(advancealtar::StageControl* control)
{
    int idx = m_stagePool.GetIndex(control);
    return idx;
