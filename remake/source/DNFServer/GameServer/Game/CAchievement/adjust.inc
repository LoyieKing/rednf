// [2026-09-09 复测] 真 bug 已修（find_quest 实参 questIdx→achievementIdx，
// ORIG 0x0828ca18 压 -0x18=m_achievementIdx）；titleIdx 内联进 putTitleOnly
// 实参位（P0，ORIG 结果留 %eax 直压）；补 -0x1c 死局部（P171）；声明序改
// questIdx/unused/achievementIdx/slot/category 后 168=168 且 6 条初始化与
// ORIG 槽值/发射序全等（questIdx@-0x20、死@-0x1c、achievementIdx@-0x18、
// slot@-0x24、category@-0x28、i@-0x14；&category/&slot 实参槽序经 0x0828cbb6
// 段核对）。余下残差仅 N7 寄存器角色（isClearedQuest/find_quest 的 ebx↔edx
// 缓存互换、movzwl 目标寄存器）+ N107 取址形。
// [L5cBattleMonAch 2026-09-11 复测] 168=168 保持。两处机器差经 ORIG 地址复核：
//  ①循环内 isClearedQuest 段 ORIG 0x0828c9eb `mov -0x20(%ebp),%ebx` 在 receiver
//    call **之前**（P52 callee-saved 独占），4.4 恒后读入 %edx；语句表达式形已试
//    无效（前端折叠）。
//  ②三段 movzwl（ORIG 0x0828ca77..0x0828ca89 目标 %ebx/%ecx/%edx）4.4 为
//    %ecx/%edx/%eax（同一台档位偏 1，因循环内 %ebx 占用情形不同）。
//  ⇒ N7 寄存器角色 + P75 实参装载时刻，N 封顶（语义等价；《_Z41__static_init》为
//  工具跨 TU 错配假缺口：本 TU 自身实例已用 objdump 按 ORIG 0x830e3db 核实 = 纯
//  ioinit 形，零改动）。
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
}