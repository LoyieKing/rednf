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
