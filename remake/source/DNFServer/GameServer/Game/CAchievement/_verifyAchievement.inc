void CAchievement::_verifyAchievement(unsigned int self)
{
    // ORIG 0x828d09e：不缓存 elements 引用，begin 一次、end 每次循环重新取
    std::map<int, std::vector<stTitleElement> >::iterator it =
        GetTitleElements(G_CDataManager()).begin();
    unsigned int achievementIdx = 0;
    unsigned int clearQuestIdx = 0;
    // ORIG 0x0828d0bd..0x0828d0d2：-0x28/-0x24/-0x20 = 0,0,4 三个死局部（后续无读，
    // P171 死写入照抄；仅复现帧槽布局与 5 条初始化序列）。
    unsigned int unusedA = 0;
    unsigned int unusedB = 0;
    unsigned int unusedC = 4;
    for (; it != GetTitleElements(G_CDataManager()).end(); ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            achievementIdx = (unsigned int)it->second[i].m_achievementIdx;
            // ORIG 0x0828d111 `je +0x139` 直接落循环尾 addl（无岛）；另两个
            // continue 才各带独立 nop 岛（+0x135/+0x138）。平铺三 continue 在
            // 4.4 恒岛化三个 ⇒ 首条件改正向包裹，让 4.4 直落循环尾（P223）。
            if (achievementIdx != 0xffffffff)
            {
                const Quest* quest =
                    G_CDataManager()->find_quest((int)achievementIdx);
                if (quest == 0)
                {
                    continue;
                }
                if (!((quest->m_type == 7) && (quest->m_triggerType == 0xc) &&
                      (quest->m_triggerData.size() != 0)))
                {
                    continue;
                }
                clearQuestIdx = (unsigned int)quest->m_triggerData[0];
                // ORIG 0x0828d1ca：getCurCharacQuestR() + 4 为 WongWork::CQuestClear
                // 子对象（UserQuest 容器 +0 m_user / +4 CQuestClear），调 isClearedQuest。
                // [b28d-solo N95/N82 封顶] ORIG 实参先于 this 装载（`mov -0x2c(%ebp),%ebx;
                // call getCurCharacQuestR; add $0x4,%eax; mov %ebx,0x4(%esp)`）+ 槽基比
                // 我方浅 8（it -0x38 vs -0x40）；P277 具名 receiver 形实测更劣
                //（槽再深 4、内层 i 由 -0x14 漂到 -0x18），已回退。
                if (((WongWork::CQuestClear*)((unsigned int)m_user
                                                  ->getCurCharacQuestR() +
                                              4))
                        ->isClearedQuest(clearQuestIdx))
                {
                    _forceClear(achievementIdx);
                }
            }
        }
    }

    if (m_user->get_charac_level() <= 0x45)
    {
        const unsigned int resetIds[2] = { 0x1983, 0x198b };
        for (int i = 0; i <= 1; ++i)
        {
            _forceReset(resetIds[i]);
        }
    }
    else if (m_user->get_charac_level() > 0x45)
    {
        _forceClear(0x1983);
    }

    int missionA = 0x83;
    int missionB = 0x87;
    unsigned int clearId = 0x19c0;
    CMissionList_Charac* missionList = (CMissionList_Charac*)
        m_user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (missionList->isClearMission(missionA) ||
        missionList->isClearMission(missionB))
    {
        _forceClear(clearId);
    }
