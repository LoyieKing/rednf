bool CConditionEventManager::IsDoubleGoldCard(CUser* user, char step)
{
    // [b28c-L15] ORIG 只用一个 bool 局部（CheckErrorCardForStep 与
    //   IsDoubleGoldCardProcess 结果共用 -0xd 槽）⇒ 合并原 check/doubleGoldCard。
    bool doubleGoldCard = CheckErrorCardForStep(user, step);
    if (!doubleGoldCard)
        return false;
    int curIdx = GetCurEventIndex();
    std::vector<stConditionEventRewardInfo> infos;
    GetConditionRewardInfo((char)0, infos, (short)curIdx, step);
    doubleGoldCard = IsDoubleGoldCardProcess(infos);
    if (!doubleGoldCard && user->GetParty() != 0)
    {
        if (((CParty*)user->GetParty())->is_quick_party())
        {
            std::vector<stConditionEventRewardInfo> quickInfos;
            GetConditionRewardInfo((char)1, quickInfos, (short)curIdx, step);
            doubleGoldCard = IsDoubleGoldCardProcess(quickInfos);
        }
    }
    return doubleGoldCard;
