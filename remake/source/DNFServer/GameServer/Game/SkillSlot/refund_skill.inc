// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N7,N82）→ 试=钥匙④守卫包裹形实测
// `if (get_pvp_grade() >= 0) { } else { result = 7; return -3; }` ⇒ 551（基线 550，
// 劣 +1；`shr $0x1f` 未变 setge）已回滚；nd 兜底 = 全帧槽相位 + setge/shr 跨版本 bool
// 物化（P168a③b）⇒ 维持（新=DIFF gap=-1, blocks=52）。
int SkillSlot::refund_skill(int skillIdx, int level, int job, int firstGrow,
                            int secondGrow, int refundCount, int& result)
{
    // [round1-L19 2026-09-09] 复修（判据 P0/P183/P329 + 判据⑨）：
    //   ① 删 8 处「具名中间局部」（treeKind/treeKind2/treeKind3/treeKind4/
    //      characJob/characJob2/newRemain/giveResult/characNo/accId/str）——
    //      ORIG Ghidra C 用寄存器 uVar3/uVar8/iVar9（无槽）承载，P0 判据；
    //   ② `for(it=begin(); it!=end(); it++)` 专家脚本循环改 `->operator[](i)`
    //      （旧用 at() ⇒ p167 EXTRA `vector::at`）；
    //   ③ 守卫改嵌套 `if (IsPvPSkilTreeChannel()!=0){ if(pvp_grade<0) fail }`，
    //      复现 ORIG `call; test %al,%al; je <body>`（4.4 对 `A==0||B` 恒插
    //      `xor $1` 物化，嵌套正极形才直测）；
    //   ④ skill 预清两句式（P197③：ORIG 0x086069e1 `movl $0x0,-0x50` + 独立赋值）。
    // 读数：549/571 gap=+22 → **547/546 gap=1**、ndiff 680→447（check_tu_routed 18:18）。
    // 残余 = ①第二项 ORIG `cmp $0;setge;test;jne` vs 4.4 `shr $0x1f;test;je`
    //（4.1 短路第二项 bool 物化，4.4 恒符号位直测，P168a③b 族）；
    // ② 全帧槽号平移 0x10（我方 group 落 -0x4c vs ORIG -0x50）+ 1 条 mov 中转。
    // ORIG 证据：asm 0x08606a4a（`test %al,%al; je 0x08606a78`）、
    // Ghidra C `local_50=CSkill::get_group; local_4c=get_skillslot_no(...)`、
    // p167 调用点差集 CLEAR。语义全等（実参/副作用序一致）⇒ N7/N82 封顶。
    // [J10GameRest.J10SkillSlot 2026-09-11 第 2 轮] 块序轴**落地**：ORIG
    // 0x08606de4 `call IsSpecialSkill; xor $1,%eax; test %al,%al; je 0x08606f5e`
    // （je 落 SFP/TP 块）⇒ 原码 = `if(!IsSpecialSkill()){SP 块}else{SFP 块}`；
    // 旧写 `== 1` 令 SFP 落 then、4.4 产 `test;je SP` 反序块。改后
    // insns 549/548(gap+1) → **549/549(gap 0)**、帧 0xa0 一致、TU 三数不变。
    // 残余（N 封顶）：① 守卫第二项 ORIG `cmp $0;setge;test;jne` vs 4.4
    // `shr $0x1f;test;je`（P168a③b 跨版本 bool 物化）；② giveList 循环 ORIG
    // 0x08606c07..c10 的两个 4B 拷贝槽 + `cmp mem,reg` 直读 vs 4.4 单 pair 槽。
    // 负结论（试形留痕，均已回滚）：
    //  ③ 删 tree-index 具名局部（内联两次 GetCurCharacSkillTreeIndex，P0/P48）
    //     ⇒ 545(gap-4)/帧 0x90 劣化；
    //  ④ giveList 循环补 `int giveIdx/giveLevel` 具名局部（对齐 ORIG 的两拷贝槽）
    //     ⇒ 552(gap+3) 劣化（4.4 多 2 条局部 load；ORIG 是 cmp mem 直读）。
    result = 0x13;
    if (m_parent == 0)
    {
        return -4;
    }
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    int group = skill->get_group();
    if (group == 4)
    {
        result = 7;
        return -5;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
    {
        // 2026-09-10 试形留痕（B 轴 · A17 地址自校 0x8606a5b/0x8606a5e）：
        //   ORIG 为 `cmp $0x0,%eax; setge`（有符号）而我方 `seta`；加 `(int)`
        //   强转后 insns 548 与 ndiff 代理 563 **均不变** ⇒ 已回滚（A17 直证留档）。
        if (((CUserCharacInfo*)m_parent)->get_pvp_grade() < 0)
        {
            result = 7;
            return -3;
        }
    }
    {
        // 2026-09-11 试形留痕（J10SkillSlot）：删本件 tree-index 具名局部（内联两次
        // getter，P0/P48 形）⇒ insns 549→545（gap 0→-4）、帧 0xa0→0x90 ⇒ 劣化，已回滚。
        ENUM_SKILL_TREE_KIND kind =
            (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                ->GetCurCharacSkillTreeIndex();
        int slot = get_skillslot_no(skillIdx, group, kind, 1);
        if (slot == -1)
        {
            return -1;
        }
        kind = (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                   ->GetCurCharacSkillTreeIndex();
        _Mastered_skill* buf = get_skillslot_buf(kind);
        if (buf == 0)
        {
            return -4;
        }
        int curLevel = buf[slot].m_level - refundCount;
        if (curLevel < 0)
        {
            return -1;
        }
        void* expertScript =
            G_CDataManager()->GetExpertJobScript(
                ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType());
        if (expertScript != 0)
        {
            // [b28d-solo-SkillSlot] P183：ORIG 每次 `mov -0x34(%ebp),%eax;
            // lea 0xc(%eax),%edx` 重读 expertScript+0xc，禁具名引用缓存。
            // 2026-09-10 试形留痕（B 轴 · A17 线索）：本循环 `unsigned`→`int`+
            //   `(int)` 强转 ⇒ ndiff 代理 563→**569（劣 6）** ⇒ 已回滚（A17 的
            //   setge/jge 直证对应的是本函数 giveList/postList 两循环，非本处）。
            for (unsigned int i = 0;
                 i < ((std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                           0xc))
                         ->size();
                 ++i)
            {
                if (((std::vector<std::pair<int, int> >*)((char*)expertScript +
                                                          0xc))
                        ->operator[](i)
                        .first == skillIdx)
                {
                    result = 7;
                    return -5;
                }
            }
        }
        std::pair<int, int> pair;
        std::vector<std::pair<int, int> > giveList;
        if (get_give_skill(job, firstGrow, secondGrow, giveList) < 0)
        {
            return -1;
        }
        for (int i = 0; i < (int)giveList.size(); ++i)
        {
            pair = giveList[i];
            // 2026-09-11 试形留痕（J10SkillSlot）：本循环按 ORIG 0x08606c07..0x08606c10
            // 的两个 4B 拷贝槽（-0x38/-0x34）补 `int giveIdx/giveLevel` 具名局部
            // ⇒ insns 549→552（gap 0→+3）劣化，已回滚（4.4 对局部比较多 2 条 load，
            // ORIG 是 `cmp mem,reg` 直读槽）。
            if (buf[slot].m_index == pair.first)
            {
                if (curLevel < pair.second)
                {
                    result = 7;
                    return -1;
                }
                break;
            }
        }
        std::vector<std::pair<int, int> > postList;
        G_CDataManager()->m_skillList->get_post_learning_skill(
            ((CUserCharacInfo*)m_parent)->get_charac_job(), skillIdx,
            postList);
        for (int i = 0; i < (int)postList.size(); ++i)
        {
            int preSkillIdx = postList[i].first;
            int preLevel = postList[i].second;
            for (int s = 0; s < 204; ++s)
            {
                if (buf[s].m_index == preSkillIdx)
                {
                    if (curLevel < preLevel)
                    {
                        result = 0x14;
                        return -1;
                    }
                    break;
                }
            }
        }
        int returnSP = skill->get_need_sp_sfp_for_level(
            curLevel, buf[slot].m_level);
        if (skill->IsStealable(firstGrow, secondGrow) != 0)
        {
            returnSP =
                getSkillSpendSPOnSteal(returnSP, skill->GetStealSPPenalty());
        }
        buf[slot].m_level = (char)(buf[slot].m_level - refundCount);
        if (buf[slot].m_level == 0)
        {
            buf[slot].m_index = 0;
        }
        // [J10GameRest.J10SkillSlot 2026-09-11] 块序轴：ORIG 0x08606de4
        // `call IsSpecialSkill; xor $1,%eax; test %al,%al; je 0x08606f5e`（je 落
        // SFP/TP 块），fall-through = SP 块 ⇒ 原码 = `if(!IsSpecialSkill()){SP}else{SFP}`
        // （旧写 `== 1` 使 SFP 块落 then、4.4 产 `test;je SP` 反向块序）。
        if (!skill->IsSpecialSkill())
        {
            int remain = get_remain_sp_at_index(
                (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                    ->GetCurCharacSkillTreeIndex());
            set_remain_sp_at_index(
                remain + returnSP,
                (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
                    ->GetCurCharacSkillTreeIndex());
            (&m_parent->m_historyLog)
                ->SkillDel(
                    ((CUserCharacInfo*)m_parent)->get_charac_job(),
                    ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex(),
                    skillIdx, refundCount, (eSkillDelReason)1);
            (&m_parent->m_historyLog)
                ->SPAdd(
                    ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex(),
                    remain + returnSP, returnSP, (eSPAddReason)2);
            if (0 < refundCount && 1000 < returnSP / refundCount)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    __PRETTY_FUNCTION__,
                    0x921,
                    "Wrong refund SP skill mId:%s char_no:%d rSkillIdx:%d rSp:%d rCount:%d",
                    NumberToString(m_parent->get_acc_id(), 0),
                    ((CUserCharacInfo*)m_parent)->getCurCharacNo(), skillIdx,
                    returnSP, refundCount);
            }
        }
        else
        {
            char kindChar = 0;
            if (((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() ==
                    -1 ||
                ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() ==
                    0)
            {
                kindChar = 2;
            }
            else
            {
                kindChar = 3;
            }
            int remain = get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)kindChar);
            set_remain_sfp_at_index(remain + returnSP,
                                    (ENUM_SKILL_TREE_KIND)kindChar);
            (&m_parent->m_historyLog)
                ->SkillDel(
                    ((CUserCharacInfo*)m_parent)->get_charac_job(),
                    ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex(),
                    skillIdx, refundCount, (eSkillDelReason)1);
            (&m_parent->m_historyLog)
                ->SFPAdd((int)kindChar, remain + returnSP, returnSP,
                         (eSPAddReason)2);
            if (0 < refundCount && 10 < returnSP / refundCount)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    __PRETTY_FUNCTION__,
                    0x93b,
                    "Wrong refund TP skill mId:%s char_no:%d rSkillIdx:%d rSp:%d rCount:%d",
                    NumberToString(m_parent->get_acc_id(), 0),
                    ((CUserCharacInfo*)m_parent)->getCurCharacNo(), skillIdx,
                    returnSP, refundCount);
            }
        }
        result = 0;
        return slot;
    }
}