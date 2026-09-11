// ORIG T 0x08335566 _ZN22CConditionEventManager18ProcessCheckStepUpEP5CUsers
// 事件 0x71 进行中且 (short)step == m_conditionEvent.m_field0 == 2 时推进晋级流程。
void CConditionEventManager::ProcessCheckStepUp(CUser* user, short step)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return;
    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if ((int)step != field0)
        return;
    // b28d-52：ORIG 0x083355be movswl -0x1c(%ebp),%eax; cmp $0x2,%eax（寄存器
    // 比较链）；P286 单 case switch 复现该形（if 形 4.4 恒折 cmpw mem）。
    switch ((int)step)
    {
    case 2:
        _processProperDungeonClearEvent(user, step);
        break;
    default:
        break;
    }
}