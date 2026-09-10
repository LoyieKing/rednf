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
