void CAchievement::adjust()
{
    // ORIG 0x828c97d：questIdx/achievementIdx 循环外预初始化（movl $0 序列）
    unsigned int questIdx = 0;
    // ORIG 0x828c984：-0x1c 死局部（P171 死写入照抄，仅复现初始化序列）。
    unsigned int unusedAdjust = 0;
    unsigned int achievementIdx = 0;
    // ORIG 0x0828cbb6..0x0828cbc0 实参槽序：&category 落 0x8(%esp)=槽 -0x28、
    // &slot 落 0xc(%esp)=槽 -0x24 ⇒ 声明序 slot 先于 category。
    int slot = 0;
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)0;
    // ORIG 0x828c97d：不缓存 adjustList 局部引用（每次循环重新取），
    // isClearQuest 通过才进入（嵌套 if，非 continue 前置）
    for (unsigned int i = 0; i < GetAdjustList(G_CDataManager()).size(); ++i)
    {
        questIdx = GetAdjustList(G_CDataManager())[i].m_questIdx;
        achievementIdx =
            GetAdjustList(G_CDataManager())[i].m_achievementIdx;
        // ORIG 0x0828ca5x：getCurCharacQuestR() 结果 `add $0x4,%eax` 后直接调
        // WongWork::CQuestClear::isClearedQuest（bool 返回 ⇒ 调用点 test/je 无
        // setne；UserQuest::isClearQuest 包装是另一符号，p167 会记 MISSING）。
        // ORIG 0x0828ca5b：`mov -0x20(%ebp),%ebx` 在 receiver call **之前**物化
        // 并跨 call 保活（P52 callee-saved 独占形）。[2026-09-10 复测] 语句表达式
        // `({ questIdx; })` 形被 4.4 前端折叠（仍 `mov -0x20(%ebp),%edx`）⇒ 无效；
        // 残余 = N7 实参保活寄存器/时刻差。
        if (((WongWork::CQuestClear*)((unsigned int)m_user->getCurCharacQuestR() + 4))
                ->isClearedQuest(questIdx))
        {
            // [真 bug 修复] ORIG 0x0828ca18 压栈的是 -0x18(%ebp)=m_achievementIdx
            // （stAdjust +0x08），不是 isClearedQuest 用的 -0x20(%ebp)=m_questIdx
            // （+0x00）。旧译文用 questIdx 查找成就 quest = 查找键用错。
            Quest* quest =
                G_CDataManager()->find_quest((int)achievementIdx);
            if ((quest == 0) || (quest->m_type != 7))
            {
                continue;
            }
            // ORIG 0x828ca49：stAchievement 临时对象 ctor 清零，get_init 覆盖字段
            stAchievement ach;
            quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                                ach.m_trigger2);
            _setTrigger(achievementIdx, ach.m_trigger0, ach.m_trigger1,
                        ach.m_trigger2);
            // ORIG 0x0828cab5..0x0828cacd：get_title_reward 结果留 %eax 直压
            // 0x4(%esp)（无具名 titleIdx 槽往返，P0 右值直传）。
            CTitleBook::putTitleOnly(m_user, quest->get_title_reward(), category,
                                     slot);
        }
    }

    if (m_user->get_charac_level() == 0x46)
    {
        Quest* quest = G_CDataManager()->find_quest(0x1983);
        if (quest == 0)
        {
            return;
        }
        // ORIG 0x828cb43：stAchievement 临时对象 ctor 清零，get_init 覆盖字段
        stAchievement ach;
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        _setTrigger(0x1983, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
        // ORIG 0x0828cbab..0x0828cbc4：同上，结果直压实参槽。
        CTitleBook::putTitleOnly(m_user, quest->get_title_reward(), category,
                                 slot);
    }
    alter();
