// ---- ORIG T 0x0813234e（sendAchievementReward.md 全文）：奖励领奖入口。----
// [b28d-solo-advancealtar_Manager AE] 本次修：items 引用缓存先于双 vector 声明
// （ORIG +0xcb）+ dead int 局部（ORIG +0x0ea `movl $0,-0xc` 后无读点，P171/P232
// 撑帧）+ 循环体嵌套 if 语义修正（见下）+ 内层 `== 0` 反极性（ORIG sete）+
// rit++（ppEi）⇒ 222=222 逐条全等。真语义 bug：原 `onEvent && type!=0 ? event :
// normal` 在 `!onEvent && type!=0` 误推 normalItems，ORIG 该分支无 push。
int CharacAdvanceAltarManager::sendAchievementReward(
    CUser* user, int achievementId,
    const std::vector<_RewardItem>** outItems)
{
    if (user == 0 || m_currentAdvanceAltarData == 0)
    {
        return 2;
    }
    // [b28d-solo-advancealtar_Manager] ORIG 每处重读 m_currentAdvanceAltarData
    // （P183，无 cur/rewards 具名槽）。
    std::map<int, _Reward>::iterator it =
        m_currentAdvanceAltarData->m_rewards.find(achievementId);
    if (it == m_currentAdvanceAltarData->m_rewards.end())
    {
        return 2;
    }
    if (it->second.m_byte4)              // 已领取标记（_Reward+4，bool 裸判 P300）
    {
        return 3;
    }
    AchievementRewardData* reward =
        G_CDataManager()->m_altarReward.getAchievementRewardData(achievementId);
    if (reward == 0)
    {
        return 3;
    }

    // [b28d-solo-advancealtar_Manager] ORIG +0xcb `add $0x10; mov %eax,-0x10`
    // ⇒ 具名引用缓存 &reward->m_rewardItems，且先于两 vector 构造（声明序）。
    const std::vector<_RewardItem>& items = reward->m_rewardItems;
    std::vector<_RewardItem> normalItems;      // ORIG -0x30（+0x0d4 先构造）
    std::vector<_RewardItem> eventItems;       // ORIG -0x3c（+0x0df 后构造）
    // [b28d-solo-advancealtar_Manager] ORIG +0x0ea `movl $0x0,-0xc(%ebp)` 后无
    // 读取点 ⇒ 原码一枚未使用 int 局部（P171/P232 照抄，占最浅槽撑帧）。
    int deadCount = 0;
    (void)deadCount;
    for (std::vector<_RewardItem>::const_iterator rit = items.begin();
         rit != items.end(); rit++)   // ORIG +0x192 ppEi（后置，P28 增量侧）
    {
        // [b28d-solo-advancealtar_Manager 真语义 bug] ORIG +0x10b..+0x192 嵌套 if：
        //   onEvent && m_type!=0 → eventItems；onEvent && m_type==0 → normalItems；
        //   !onEvent && m_type==0 → normalItems；!onEvent && m_type!=0 → 丢弃。
        // 原 `onEvent && type!=0 ? event : normal` 在 !onEvent && type!=0 误推
        // normalItems（ORIG 该分支无 push）。
        if (advancealtar::isOnEvent())
        {
            // [b28d-solo-advancealtar_Manager] ORIG +0x121 `sete %al; test; je`
            // ⇒ 原码写 `== 0`（正极性比较物化），normalItems 走 fall-through。
            if (rit->m_type == 0)
            {
                normalItems.push_back(*rit);
            }
            else
            {
                eventItems.push_back(*rit);
            }
        }
        else
        {
            if (rit->m_type == 0)
            {
                normalItems.push_back(*rit);
            }
        }
    }
    // [b28d-solo-advancealtar_Manager] ORIG 实参内联 c_str()（P44，无指针局部）
    // + `size()!=0` 物化（P197④；ORIG `test %eax,%eax; setne`）。
    if (normalItems.size() != 0)
    {
        ProcStage::sendMailRewardItem(user, normalItems,
                                      reward->m_postalTitle.c_str(),
                                      reward->m_postalLetter.c_str());
    }
    if (eventItems.size() != 0)
    {
        ProcStage::sendMailRewardItem(user, eventItems,
                                      "chn_game_server_msg_86",
                                      reward->m_postalLetter.c_str());
    }
    *outItems = &reward->m_rewardItems;
    it->second.m_byte4 = 1;                                     // 置已领取
    m_currentAdvanceAltarData->m_update = 1;
    return 0;
}