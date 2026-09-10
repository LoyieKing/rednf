bool CConditionEventManager::IsFreeGoldCard(CUser* user, char step)
{
    // b28d-52：ORIG 0x0833579e check 与 freeGoldCard 复用同一槽 -0xb
    // （P0 槽-局部对应律 ⇒ 原码单变量复用），合并为 result。
    bool result = CheckErrorCardForStep(user, step);
    if (!result)
        return false;
    short curIdx = (short)GetCurEventIndex();
    std::vector<stConditionEventRewardInfo> infos;
    GetConditionRewardInfo((char)0, infos, curIdx, step);
    result = IsFreeGoldCardProcess(infos);
    return result;
