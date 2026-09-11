// ORIG T 0x08335a8a _ZN22CConditionEventManager21CheckErrorCardForStepEP5CUserc
// user 非空、IsProperDungeonUser()、step>=0 三关；_getRequiredParameter(step) 得
// required：==-1 或 < GetProperDungeonClearCount() → false；否则 true。
bool CConditionEventManager::CheckErrorCardForStep(CUser* user, char step)
{
    if (user == NULL)
        return false;
    if (!user->IsProperDungeonUser())
        return false;
    if (step < 0)
        return false;
    std::pair<int, int> required = _getRequiredParameter(step);
    int clearCount = (short)user->GetProperDungeonClearCount();
    if (required.first == -1)
        return false;
    if (required.first < (int)clearCount)
        return false;
    return true;
}