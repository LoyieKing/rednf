bool SkillSlot::InitSkill(bool sendFlag, ENUM_SKILL_TREE_KIND kind)
{
    // [b28d-solo-SkillSlot] 判词（N82/P75/P168a 封顶，340 vs 338，p167 CLEAR 51=51）：
    // 已复现：kindArg/changerKind P45 短路链 + 死初值(P171)、PvP 段 secondGrow
    // 具名 int + firstGrow/job/pvpGrade 内联实参、两处 cMyTrace 全表达式临时链
    // (P227)、giveVec 循环 pair 具名局部(P50)、addSkillOnCreateCharacter 双臂。
    // 残余跨版本族：① 4.4 把内联 pvpGrade 落槽而 ORIG 留 %esi（具名 secondGrow
    // 反之），帧 0x13c vs 0x12c（+0x10，N82/P0 求值序族）；
    // ② giveVec 循环 pair 4.4 折临时槽直读（ORIG 具名槽 -0x3c/-0x38 + 拷贝）；
    // ③ `(got^1)!=0` 4.4 折 `cmpb $1`，ORIG 4.1 物化 `movzbl; xor $1; test`；
    // ④ receiver `lea 0xa800(%eax)` vs ORIG `add $0xa800,%eax`（N107 反向）。
    // 全部语义等价（调用点差集为零）。
    if (m_parent == 0)
        return false;

    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        // ORIG 0x0860815d：`movl $0x0` 初值（死写，P171）+ P45 短路链
        // `(kind==-1)||(kind==0) → 1 : 2`。
        int kindArg = 0;
        if (kind == -1 || kind == 0)
            kindArg = 1;
        else
            kindArg = 2;
        m_parent->givePvPSkillTree(0, true, kindArg);
        int secondGrow = m_parent->getCurCharSecondGrowType();
        int sp = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                     m_parent->get_charac_job(),
                     m_parent->getCurCharFirstGrowType(), secondGrow,
                     m_parent->get_pvp_grade(), false);
        set_remain_sp_at_index(sp, kind);
        if (sendFlag)
        {
            m_parent->send_skill_info();
            cMyTrace(__PRETTY_FUNCTION__, 0xb47, 0)(
                "SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                m_parent->getCurCharacName());
        }
        InitSkillHistoryLog(kind, 1);
        return true;
    }

    if (kind == -1 || kind == 0)
    {
        addSkillOnCreateCharacter(&m_skillSlot[0],
                                  m_parent->get_charac_job());
    }
    else
    {
        addSkillOnCreateCharacter(&m_skillSlot2[0],
                                  m_parent->get_charac_job());
    }

    std::vector<std::pair<int, int> > giveVec;
    // [g11-slot 2026-09-10] P0/P183 去具名局部（直证）：ORIG 0x0860832f
    // `movsbl %al,%esi`（secondGrow 直进 %esi）、0x0860833f `movsbl %al,%ebx`
    // （firstGrow 直进 %ebx）、0x08608347 `mov 0x14(%eax),%edi`（chars 直进
    // %edi，随后 `imul $0x7dc` + `lea (%edi,%eax,1),%edx`）——全部无栈槽。
    // 我方旧形三个具名局部各占一槽（-0x42/-0x41/-0x40 的 store+load 往返）。
    // 同上（D06 配对 bool 化）：ORIG 0x0860837f 为 1 字节 bool 槽。
    bool got = ((CCharacter*)((char*)G_CDataManager()->m_characList +
                              m_parent->get_charac_job() * 0x7dc))
                   ->get_give_skill(
                       m_parent->getCurCharFirstGrowType(),
                       m_parent->getCurCharSecondGrowType(), giveVec, 0);
    if (!got)
    {
        return false;
    }

    // [b28d-solo-SkillSlot] ORIG 0x0860830c 段：`pair = giveVec[i]` 具名 pair
    // （-0x3c/-0x38）+ 临时 -0x70/-0x6c（P50 先声明后赋值形）。
    std::pair<int, int> pair;
    // 2026-09-10 试形留痕（B 轴反向 · A17 地址自校 0x86084bd/0x86084c0）：
    //   ORIG 为 `call size; cmp -0x1c(%ebp),%eax; seta`（无符号）而我方 `setg`；
    //   改 unsigned 后 InitSkill insns 340 与 ndiff 代理 466 **均不变** ⇒ 已回滚。
    for (int i = 0; i < (int)giveVec.size(); ++i)
    {
        pair = giveVec[i];
        growtype_skill(m_parent->get_charac_job(), pair.first, pair.second,
                       kind);
    }

    STExpertJobScript* expert = static_cast<STExpertJobScript*>(
        G_CDataManager()->GetExpertJobScript(
            m_parent->GetCurCharacExpertJobType()));
    if (expert != 0)
    {
        for (int i = 0; i < (int)expert->m_growSkillVec.size(); ++i)
        {
            int level =
                m_parent->GetCurExpertJobLevel(m_parent->GetCurCharacExpertJobExp());
            growtype_skill(m_parent->get_charac_job(),
                           expert->m_growSkillVec[i].first, level, kind);
        }
    }

    WongWork::CSkillChanger changer;
    // ORIG：movl $0x0 初值；kind==-1→0、kind==0→1、其他→2（同 kindArg 模式）。
    int changerKind = 0;
    if (kind == -1 || kind == 0)
        changerKind = 1;
    else
        changerKind = 2;
    changer.SkillInitialize(m_parent, changerKind, false);
    if (sendFlag)
    {
        m_parent->send_skill_info();
        cMyTrace(__PRETTY_FUNCTION__, 0xb86, 0)(
            "SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
            m_parent->getCurCharacName());
    }
    InitSkillHistoryLog(kind, 0);
    return true;
