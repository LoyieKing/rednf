// ----------------------------------------------------------------------------
// CUser::init_skill_tree —— ORIG T 0x0866bce8 _ZN5CUser15init_skill_treeE20ENUM_SKILL_TREE_KIND
// 报告 docs/class_func_reports/CUser/init_skill_tree.md 全文转写：
//   getCurCharacR==0 → return；
//   clear_all_skills(kind) → set_parent(this)；
//   kind==-1/0 → addSkillOnCreateCharacter(skillW+0x46, job)，否则 skillW+0x1de；
//   get_give_skill(first,second,giveVec,0) 成功→ growtype_skill×giveVec；
//     expert: GetExpertJobScript(GetCurCharacExpertJobType()) 非空→对 m_growSkillVec
//     每个 (skillIdx) 以 GetCurExpertJobLevel(Exp) 作 level 调 growtype_skill 两树；
//     CSkillChanger SkillInitialize(this, kind==-1/0?1:2, false)；
//   失败→ LogManager::logFormat(1,"user.cpp",PRETTY,0x5328,"User %s - ...")。
// ----------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0866bce8：p167=CLEAR，gap=6；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0866bce8：N82 封顶——帧/槽差（p167 CLEAR，gap 6）。
// [A06 2026-09-10 必配对律消费侧获益] 3 处 `get_give_skill` 消费点之一（8665）在本件：
// D06 将 `CCharacter::get_give_skill` char→bool 落地后，我方 `bool got = …; if(!got)`
// 与该 bool callee **成对** ⇒ 复现 ORIG `call; mov %al,槽; movzbl; xor $1; test; je`
// （无 setne）⇒ 本件 247→**245** 条（gap −6→**−4**），源码零改动。
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
    // [G6Sub1 单旋钮·P200] ORIG 0x0866bda7..0x0866bdd3 为
    // `call getCurCharSecondGrowType; movsbl %al,%esi` +
    // `call getCurCharFirstGrowType; movsbl %al,%ebx` —— 拓宽即目标寄存器（2 操作数
    // movsbl 直入 callee-saved），说明原码把短整表达式**作普通实参直传**。
    // 旧形 `char secondGrow = ...; char firstGrow = ...;`（具名局部）退化为
    // `mov %al,-0x32/-0x31` + `movsbl -0x32,-0x31,%esi/%ebx`（4 条，本件 gap 主项）。
    // 下面实参位内联后 4.4 右→左求值：second（arg2）先入 %esi、first（arg1）后入 %ebx。
    // [FL5 2026-09-10 负结论·已回滚] 本处 ORIG 0x0866bce8 段看似无 job 具名槽
    // （`call get_charac_job; mov 0x14(%eax),%edi; call G_CDM; imul $0x7dc; lea (%edi,%eax,1)`），
    // 但把 `int job` 内联为 `m_characs[get_charac_job()]`（P0 方向）实测 **ndiff 146→241 反劣**
    // （our_insns 246 不变）⇒ 原码确为具名 job，ORIG 的 edi 承载是 4.1 分配差异（N7 族）。
    // 维持具名形；勿再试内联。
    int job = get_charac_job();
    // ORIG：*(int*)(mgr+0x14)+job*0x7dc —— mgr->m_characList（CCharacList*）->m_characs[job]
    bool got = G_CDataManager()->m_characList->m_characs[job].
                   get_give_skill((int)(signed char)getCurCharFirstGrowType(),
                                  (int)(signed char)getCurCharSecondGrowType(),
                                  giveVec, 0);
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
    // [R4a15 2026-09-12 复刻·Main 裁定] 按 ORIG setg（有符号）语义复刻 `int i` 形；
    //   受控 A/B（同工具同时点，RB=unsigned vs INT）：setg 对齐 ✓（原 seta）；
    //   eq2 结构差异段 22→**21**；align 差异块 11→11；TU 四数 497/979/1/40/284
    //   逐值不变 ⇒ **不劣化（略优）⇒ 保留 int 形**（A06 旧读数经复测不复现）。
    for (int i = 0; i < (int)giveVec.size(); ++i)
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
            // [K7 2026-09-11 单旋钮·P0 去具名槽] ORIG 0x0866bf1a `call GetCurCharacExpertJobExp`
            // → 0x0866bf1f `mov %eax,0x4(%esp)` 直入实参（**无 exp 具名槽**；我方 -0x18 槽
            // 每点多 store+load 一对 = 区间 ORIG 4 条 vs 我方 6 条）⇒ 内联右值。
            int lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
            int j = get_charac_job();
            std::pair<int,int>& pr = expert->m_growSkillVec[i];
            ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                               (ENUM_SKILL_TREE_KIND)1);
        }
    }

    WongWork::CSkillChanger changer;
    // [H6 2026-09-11 负结论·已回滚] 试 `int mode = 0; if (...) mode = 1; else mode = 2;`
    // 具名槽（欲复现 ORIG 0x0866bfa9 dead init + 0x0866bfbf/c8 两具名 store）：
    // our 244→**246**（gap -3→-5）劣化 ⇒ 回滚；4.4 对该 if/else 赋值仍产直传形。
    changer.SkillInitialize(this, ((int)kind == -1 || (int)kind == 0) ? 1 : 2, false);
}