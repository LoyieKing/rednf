std::vector<pieceQuestRewardStatus> QuestParameterScript::getRewardStatus(
    int level)
{
    std::vector<pieceQuestRewardStatus> result;
    int rewardLevel = getRewardLevel(level);
    if (rewardLevel == -1)
        return result;

    for (std::map<int, std::vector<pieceQuestRewardStatus> >::iterator it =
             m_map54.begin(); it != m_map54.end(); it++)
    {
        if (rewardLevel < it->first)
            break;

        // ORIG 0x8a6bee3：`operator->(it) → +4 → vector C1ERKS(-0x54, node->second)`
        // ⇒ 原码按值拷贝 section（p167 MISSING C1ERKS×1 + D1Ev×2 实证）。
        std::vector<pieceQuestRewardStatus> section = it->second;
        for (std::vector<pieceQuestRewardStatus>::iterator sit =
                 section.begin(); sit != section.end(); sit++)
        {
            Statuepredicate pred(*sit);
            std::vector<pieceQuestRewardStatus>::iterator found =
                std::find_if(result.begin(), result.end(), pred);
            if (found != result.end())
            {
                // P167：ORIG 发 operator*（deEv）而非 operator->（ptEv）
                // ⇒ 写 (*it).member 形。
                (*found).m_value += (*sit).m_value;
            }
            else
            {
                result.push_back(*sit);
            }
        }
    }
    return result;
