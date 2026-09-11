// ORIG T 0x083354f0 _ZN22CConditionEventManager16GetMaxRewardStepEs
// 事件 0x71 进行中且 (short)step == m_conditionEvent.m_field0 时返回
// m_conditionEvent.m_map8.size()（+0x4da0 map 的奖励步数上限）；否则 0。
int CConditionEventManager::GetMaxRewardStep(short step)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return 0;
    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if ((int)step != field0)
        return 0;
    return G_CDataManager()->m_conditionEvent.m_map8.size();
}