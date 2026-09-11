// ORIG T 0x083355e6 _ZN22CConditionEventManager31_processProperDungeonClearEventEP5CUsers
// 取当前条件事件步 curStep；_getRequiredParameter(step+1) 得 (required,-)；
// required != -1 且 required <= GetProperDungeonClearCount() → 步进 + 发送条件事件信息；
// 无条件发送清剿计数。ORIG 中 PacketGuard 局部对象构造后未使用（照抄保留）。
void CConditionEventManager::_processProperDungeonClearEvent(CUser* user, short step)
{
    int curStep = (short)user->GetCurConditionEventStep();
    std::pair<int, int> required = _getRequiredParameter((short)(curStep + 1));
    int clearCount = (short)user->GetProperDungeonClearCount();
    PacketGuard packet;
    if (required.first != -1 && required.first <= clearCount)
    {
        user->IncCurConditionEventStep();
        user->SendConditionEventInfo();
    }
    user->SendProperDungeonClearCount();
}