// [b28d-solo-CDataManagerScripts] NEAR：p167 CLEAR（28/28）—— 本轮修两处：
// ①section 按值拷贝（ORIG 0x8a6bee3 `operator->(it)→+4→vector C1ERKS(-0x54)` 实证，
//   补 MISSING C1ERKS×1 + D1Ev×2）；②`(*found).m_value += (*sit).m_value` 的
//   operator*（deEv）形（P167：MISSING deEv×2 / EXTRA ptEv×2）。帧 0x8c=ORIG；
//   残余仅槽号文本差（-0x3c vs ORIG -0x48 族，N86/N93 分配策略）；ORIG 0x08a6be66。
// [GW20Run 2026-09-11 复核] fresh 195/195，助记符多重集**完全相等**（纯槽号/寄存器
// 文本差）+ 1 条尾 nop（§6.1 豁免）；p167 28=28 全为 C1/C2·D1/D2 别名 ⇒ 维持 N126/N95。
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
}