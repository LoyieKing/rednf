void CUser::init_skill_tree(ENUM_SKILL_TREE_KIND kind)
{
    if (getCurCharacR() == 0)
        return;

    ((SkillSlot*)getCurCharacSkillW())->clear_all_skills(kind);
    ((SkillSlot*)getCurCharacSkillW())->set_parent(this);

    // asm 0822f140=getCurCharacSkillW / 080fdf20=get_charac_job（nm 实证 thunk 序），
    // 反编译为两分支各 addSkillOnCreateCharacter((_Mastered_skill*)(skillW+{0x46|0x1de}), job)。
    if ((int)kind == -1 || (int)kind == 0)
    {
        addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot,
                                  get_charac_job());
    }
    else
    {
        addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot2,
                                  get_charac_job());
    }

    std::vector<std::pair<int,int> > giveVec;
    // ORIG 0x0866bce8：giveVec 后立即构造独立 pair 局部（C1 符号），
    // 循环内 giveVec[i] 逐字段拷入（mov/mov），非引用。
    std::pair<int,int> pr;
    char secondGrow = getCurCharSecondGrowType();
    char firstGrow = getCurCharFirstGrowType();
    // [FL5 2026-09-10 负结论·已回滚] 本处 ORIG 0x0866bce8 段看似无 job 具名槽
    // （`call get_charac_job; mov 0x14(%eax),%edi; call G_CDM; imul $0x7dc; lea (%edi,%eax,1)`），
    // 但把 `int job` 内联为 `m_characs[get_charac_job()]`（P0 方向）实测 **ndiff 146→241 反劣**
    // （our_insns 246 不变）⇒ 原码确为具名 job，ORIG 的 edi 承载是 4.1 分配差异（N7 族）。
    // 维持具名形；勿再试内联。
    int job = get_charac_job();
    // ORIG：*(int*)(mgr+0x14)+job*0x7dc —— mgr->m_characList（CCharacList*）->m_characs[job]
    bool got = G_CDataManager()->m_characList->m_characs[job].
                   get_give_skill((int)firstGrow, (int)secondGrow, giveVec, 0);
    if (!got)
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__, 0x5328,
            "User %s - CUser::init_skill_tree", getCurCharacName());
        return;
    }

    // [A06 2026-09-10 轴B 试行记录·已回退] 地址自校通过（ORIG 0x0866bed9
    // `cmp -0x20(%ebp),%eax; setg` ↔ 我方 0x824260f `cmp -0x24(%ebp),%eax; seta`，上下文同构），
    // 但改 int 后 **delta 56→58 反劣**（seta 2→0 形态虽对齐，4.4 另生中转）⇒ 按「严格改善才留」回退。
    for (unsigned int i = 0; i < giveVec.size(); ++i)
    {
        // ORIG 循环体顺序：先 get_charac_job、再 getCurCharacSkillW，
        // slot->growtype_skill(job, pair.first, pair.second, kind)。
        pr = giveVec[i];
        int j = get_charac_job();
        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second, kind);
    }

    // Expert job 补技能树：GetExpertJobScript(0x0822b5f2 W _ZN12CDataManager18GetExpertJobScriptEi)
    // 返回 STExpertJobScript*（+0x0c = m_growSkillVec）。
    STExpertJobScript* expert =
        (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
    if (expert != 0)
    {
        // [A06 2026-09-10 轴B 试行记录·已回退]（同件第二循环，同上理由）
        for (unsigned int i = 0; i < expert->m_growSkillVec.size(); ++i)
        {
            int exp = GetCurCharacExpertJobExp();
            int lvl = GetCurExpertJobLevel(exp);
            int j = get_charac_job();
            std::pair<int,int>& pr = expert->m_growSkillVec[i];
            ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                               (ENUM_SKILL_TREE_KIND)1);
        }
    }

    WongWork::CSkillChanger changer;
    changer.SkillInitialize(this, ((int)kind == -1 || (int)kind == 0) ? 1 : 2, false);
