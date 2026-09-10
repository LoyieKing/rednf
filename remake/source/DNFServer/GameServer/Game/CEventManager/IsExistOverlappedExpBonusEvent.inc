bool CEventManager::IsExistOverlappedExpBonusEvent()
{
    bool eventExp = GlobalData::s_event_manager->GetRepeatEvent(0x66)->IsEventing(0);
    bool effect2 = GlobalData::s_globalEffectManager_->isAffectedEffect(2);
    if (eventExp || effect2)
        return true;
    return false;
