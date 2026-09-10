int SkillSlot::try_refund_skill(int skillIdx, int level, int job,
                                int firstGrow, int secondGrow,
                                int refundCount, int& result)
{
    // [round1-L19 2026-09-09] 复修（P0/P183/P329）：删 `expertList` 引用（改为
    // 每次重推导 `((vector*)((char*)expertScript+0xc))->operator[](i)`，P183/P157）、
    // `giveResult`/`characJob` 具名局部（ORIG 用 iVar9/uVar8 寄存器）⇒
    // 355/352 gap=3 → **355/350 gap=5**、ndiff 319→215。
    // 剩余 = 全帧槽号平移 0x10（我方 -0x2c.. vs ORIG -0x3c..）+ `cmp $0;setge` /
    // `js/seta` 符号性 lowering 差（P196/P209，jcc 落点已逐位复核一致）。
    // [b28d-solo-SkillSlot] 判词（N82/P75 封顶，355 vs 356，p167 CLEAR 36=36）：
    // 已复现：skill 预清(P198①)、G_GameWorld 内联、`get_pvp_grade() >= 0`
    // 等价形、expertScript 视图去具名引用、两处 0xcc 扫描、EH cleanup 段全等。
    // 残余 2 类跨版本差异：① 4.4 对 `(vector*)((char*)expertScript+0xc)` 的
    // 循环条件/体重复表达式发 SAVE_EXPR 提升（循环外 `add $0xc; mov %eax,-0x24`
    // 缓存槽），ORIG 4.1 每次 `mov -0x34; lea 0xc` 重算 ⇒ 全帧槽号浅 4
    // （skill -0x48 vs -0x44 等，N82 族；P183 源码侧已无缓存）；
    // ② 短路条件 bool 物化：ORIG `test %al; je` + `cmp $0; setge %al; test; jne`，
    // 4.4 恒 `xor $1; test; jne` + `js; mov$1/mov$0; test; je`（P168a③b 跨版本
    // bool 归一，无源码旋钮）。
    // 真语义 bug 修正见下方 GetExpertJobScript 段。
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
    if (G_GameWorld()->IsPvPSkilTreeChannel() == 0 ||
        ((CUserCharacInfo*)m_parent)->get_pvp_grade() >= 0)
    {
        int slot = get_skillslot_no(m_tempSkillSlot, skillIdx, group, true);
        if (slot == -1)
        {
            return -1;
        }
        int curLevel = m_tempSkillSlot[slot].m_level - refundCount;
        if (curLevel < 0)
        {
            return -1;
        }
        void* expertScript = 0;
        // [b28d-solo-SkillSlot] 真语义 bug 修正：ORIG 0x08607a5e 段
        // `call GetCurCharacExpertJobType; mov %eax,%ebx; call G_CDataManager;
        //  mov %ebx,0x4(%esp); call GetExpertJobScript` ⇒ 原码把职业类型
        // 作为 GetExpertJobScript 实参（旧实现丢弃返回值传 0 = 取错脚本）。
        expertScript = G_CDataManager()->GetExpertJobScript(
            ((CUserCharacInfo*)m_parent)->GetCurCharacExpertJobType());
        if (expertScript != 0)
        {
            // 2026-09-10 试形留痕（B 轴）：同 refund_skill ⇒ ndiff 代理 359→365
            //   （劣 6）⇒ 已回滚；保留 giveList/postList 两循环的 int 化（那两处实测改善）。
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
        // 2026-09-10（C05② 反向/符号性旋钮）：ORIG 0x08607b35 段为
        //   `mov -0x38(%ebp),%eax; cmp -0x10(%ebp),%eax; setg`（**有符号**序）
        //   ⇒ ORIG 索引局部是 int；我方 unsigned 产 `seta` 无符号序 ⇒ 改 int
        //   对齐（P196 同向：ORIG 本身即 setg 有符号）。
        for (int i = 0; i < (int)giveList.size(); ++i)
        {
            pair = giveList[i];
            if (m_tempSkillSlot[slot].m_index == pair.first)
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
        // 同上（ORIG 0x08607bxx 循环条件 `cmp -0x18(%ebp),%eax; setg`）。
        for (int i = 0; i < (int)postList.size(); ++i)
        {
            int preSkillIdx = postList[i].first;
            int preLevel = postList[i].second;
            for (int s = 0; s < 204; ++s)
            {
                if (m_tempSkillSlot[s].m_index == preSkillIdx)
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
            curLevel, m_tempSkillSlot[slot].m_level);
        if (skill->IsStealable(firstGrow, secondGrow) != 0)
        {
            returnSP =
                getSkillSpendSPOnSteal(returnSP, skill->GetStealSPPenalty());
        }
        m_tempSkillSlot[slot].m_level =
            (char)(m_tempSkillSlot[slot].m_level - refundCount);
        if (m_tempSkillSlot[slot].m_level == 0)
        {
            m_tempSkillSlot[slot].m_index = 0;
        }
        int* remain;
        if (skill->IsSpecialSkill() == 1)
        {
            remain = get_temp_remain_sfp();
        }
        else
        {
            remain = get_temp_remain_sp();
        }
        *remain = *remain + returnSP;
        result = 0;
        return slot;
    }
    result = 7;
    return -3;
