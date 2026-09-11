// ORIG T 0x0833579e _ZN22CConditionEventManager14IsFreeGoldCardEP5CUserc
// CheckErrorCardForStep 不过 → false；取当前条件事件索引，按 step 收集奖励信息，
// IsFreeGoldCardProcess 判定免费金卡。
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
}