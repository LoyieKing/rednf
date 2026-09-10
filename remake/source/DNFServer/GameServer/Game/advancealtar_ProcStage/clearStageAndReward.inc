void ProcStage::clearStageAndReward()
{
    if (m_dungeon == 0)
    {
        return;
    }
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return;
    }
    int starCount = 0;
    bool unlockBuy = false;
    bool bFlag = false;
    // b28d-solo-ProcStage：P0/P183——ORIG 无 cam 引用槽（0x812e464 每次
    //   `mov -0x1c(%ebp),%edx; lea 0x8df60(%edx),%ecx` 内联）⇒ 全部实参内联。
    if (!user->m_advanceAltar.clearStage(
            getStageIndex(),
            static_cast<StageDifficulty::T>(m_difficulty),
            static_cast<StageType::T>(getStageType()),
            getClearSurvivalRoundNo(),
            starCount, unlockBuy, bFlag))
    {
        return;
    }

    std::map<int, _Reward> rewardMap;
    addAchievementReward(rewardMap);
    std::vector<_RewardItem> rewardItems;
    rewardClearStage(rewardItems);
    user->m_advanceAltar.setNextStage(getStageIndex());

    // b28d-solo-ProcStage：P48——ORIG 0x812e4ff/0x812e50e 两次独立 getStageType()
    //   （差集 MISSING ×1；旧具名 stageType 缓存少一条 call）；
    //   P197④——ORIG 0x812e51e `call size; test; je` ⇒ 原码 `size() != 0` 非 empty()；
    //   P168a 档①——ORIG 0x812e547 `call decTicket; xor $1; test; je` ⇒ bool 返回 + 裸 `!`。
    // ORIG 0x812e533 `test %al; je 主流程` + 0x812e54c `xor $1; test; je 主流程`
    //   ⇒ 单条 `if ((A||B) && !C) return;`（A/B 物化为 mov$1/mov$0 汇合）。
    //   P226 条件向语句表达式（内不含具名变量）复现 A||B 的零槽短路物化岛。
    if (({ getStageType() == 0 ||
          (getStageType() == 1 && rewardItems.size() != 0); }) &&
        !user->m_advanceAltar.decTicket())
    {
        return;
    }
    std::vector<_BuyItem> buyItems;
    if (unlockBuy)
    {
        user->m_advanceAltar.setUnlockBuyItem(
            static_cast<AdvanceAltarShopType::T>(0), getStageIndex(), &buyItems);
        user->m_advanceAltar.setUnlockBuyItem(
            static_cast<AdvanceAltarShopType::T>(1), getStageIndex(), &buyItems);
        user->m_advanceAltar.setUnlockBuyItem(
            static_cast<AdvanceAltarShopType::T>(2), getStageIndex(), &buyItems);
    }
    sendClearStage(rewardItems, buyItems, rewardMap, starCount, bFlag);
    if (0 < starCount)
    {
        HistoryLog::starAdd(*user, AddStarType::ADD_BY_CLEAR,
                            getStageIndex(), starCount);
    }
