void SkillSlot::InitSkillHistoryLog(ENUM_SKILL_TREE_KIND kind, int reason)
{
    // b28d-solo-SkillSlot 判词（N 封顶，N7/P75 族；ORIG 0x086085fa 尾段 0x08608675）：
    // ORIG 4.1 求值序 = reason→sfp→sp→level→spKind(-0x20→%edx)→receiver
    // (m_parent+0x79700→%ecx，内联无槽)；4.4 恒 this-first（receiver→%edx 在
    // spKind 之前，spKind 落 %eax），压栈尾部两条互换。三形实测：
    // ① 原形 57=57 DIFF（仅 2 条 mov 寄存器角色/序差）；
    // ② `(0, m_parent->m_historyLog)` 逗号接收者 57=57 DIFF 同形；
    // ③ `(*m_parent).m_historyLog` 解引用形 57=57 DIFF 同形；
    // ④ 具名 `cUserHistoryLog* historyLog=&…; historyLog->InitSkill(...)` 求值序
    //    翻正（this 最后装载）但 receiver 槽化 -0x1c + 槽位重排 ⇒ 59 vs 57 DIFF。
    // receiver 无槽往返（P0 ⇒ 原码无具名局部），4.4 无法在无槽下把 this 求值后置
    // ⇒ 跨版本求值序结构性差，封 N7；语义等价（全部实参/副作用序一致）。
    // [round1-L19 2026-09-09] 第 ⑤ 形复测：`(&m_parent->m_historyLog)->InitSkill(...)`
    // 与链式形逐条同形（56=56、ndiff 12 不变）⇒ 维持 N7 封顶（4.4 恒先算 receiver
    // 落 %edx、spKind 落 %eax；ORIG 0x086085fa 段为 spKind→%edx、receiver→%ecx）。
    if (m_parent == 0)
    {
        return;
    }
    int spKind;
    int sfpKind;
    if (kind == -1 || kind == 0)
    {
        spKind = 0;
        sfpKind = 2;
    }
    else
    {
        spKind = 1;
        sfpKind = 3;
    }
    m_parent->m_historyLog.InitSkill(
        spKind, ((CUserCharacInfo*)m_parent)->get_charac_level(),
        get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)spKind),
        get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)sfpKind),
        (eSkillInitReason)reason);
