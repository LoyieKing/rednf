// ORIG T 0x08116212 _ZN13CEventManager30IsExistOverlappedExpBonusEventEv
// 事件 0x66（经验加成类）进行中，或全局效果管理器效果 2 生效 → 视为存在
// 重叠经验加成。
bool CEventManager::IsExistOverlappedExpBonusEvent()
{
    bool eventExp = GlobalData::s_event_manager->GetRepeatEvent(0x66)->IsEventing(0);
    bool effect2 = GlobalData::s_globalEffectManager_->isAffectedEffect(2);
    if (eventExp || effect2)
        return true;
    return false;
}