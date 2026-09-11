// ---- ORIG T 0x0813175a _ZN12advancealtar25CharacAdvanceAltarManager10clearStageEi
//      NS_15StageDifficulty1TENS_9StageType1TEiRiRbS6_（clearStage.md 全文；
//      2026-08-27 LfAltarStage 批）。----
bool CharacAdvanceAltarManager::clearStage(int stageIdx, StageDifficulty::T diff,
                                           StageType::T type, int clearRound,
                                           int& starDelta, bool& unlockBuy,
                                           bool& bOut)
{
    unlockBuy = false;   // ORIG 首指令 *param_7=0（其余引用路径不改写）
    AdvanceAltarData* cur = m_currentAdvanceAltarData;   // ORIG 0x813176a mov (%eax) 直读 this+0（无 getter call）
    if (cur == 0)
    {
        return false;
    }
    if (!G_CDataManager()->m_stageMapList.isExist(stageIdx))
    {
        return false;
    }
    std::map<int, _Stage>::iterator it = cur->m_stages.find(stageIdx);
    if (it == cur->m_stages.end())
    {
        return false;
    }
    if (type == StageType::STAGE_SURVIVAL)                  // 生存关：只刷新最高轮数
    {
        if (static_cast<int>(cur->m_maxClearRound) < clearRound)   // ORIG sext(u16) 比较
        {
            cur->m_maxClearRound = static_cast<short>(clearRound);
            bOut = true;
            cur->m_update = 1;
        }
        return true;
    }
    if (it->second.m_field4 == -1)     // 首次记录 → 解锁商店购买项
    {
        unlockBuy = true;
    }
    if (it->second.m_field4 < static_cast<int>(diff))    // 未达该难度 → 晋级发星
    {
        it->second.m_field4 = static_cast<StageField4_t>(
            StageDifficulty::getNextDifficulty(
                static_cast<StageDifficulty::T>(it->second.m_field4)));   // b28d：m_field4 已 enum 化
        starDelta = kRewardStar[diff];
        cur->m_curStar += starDelta;      // +0x0c
        cur->m_starPoint += starDelta;    // +0x14
        cur->m_update = 1;
    }
    return true;
}