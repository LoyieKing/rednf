// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=nd 355/356：SAVE_EXPR 提升（我方循环外
// `add $0xc; mov %eax,-0x24` 缓存槽 vs ORIG 每次 `mov -0x34; lea 0xc` 重算 ⇒ 全帧浅 4）+
// P342 实测：ORIG `__copy_move_backward<ILb1ELb0, stSPReturnInfo_t*>`@0x08609b82 与我方同
// ⇒ 无 ILb 缺口 ⇒ 维持（新=DIFF gap=+4, blocks=46）。
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
    // [H10-复核 2026-09-11 · 回滚 H10-SkillSlot 的「守卫早退」改动]
    // 复核结论：①语义**等价**——旧形在 `A != 0 && grade < 0` 时跳过主体后
    // 落到函数尾的 `result = 7; return -3;`（见本函数末尾两行），与新增守卫
    // 路径行为相同，故旧判词「丢失 result/早退」不成立；②codegen **劣化**——
    // ORIG 0x08607974..0x086079aa 正是「两条件各跳主体 + fail 块」形
    // (`test %al,%al; je 主体` / `setge %al; test %al,%al; jne 主体` /
    //  `mov 0x24(%ebp),%eax; movl $0x7,(%eax); mov $0xfffffffd,%ebx`），
    // 与旧形 `A == 0 || grade >= 0` 同构；改后 our_insns 352→347
    // (ORIG 357，gap +5→+10) 反而背离 ORIG ⇒ 复原旧形。
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
        // [R11-02 2026-09-12] 删多余零初始化：ORIG 该槽（-0x34）在
        // `getCurCharacExpertJobType/GetExpertJobScript` 写前无读、无预清
        // （ORIG 0x08607b.. 段仅 `mov %eax,-0x34(%ebp)`）⇒ `= 0` 多一条
        // `movl $0x0`（P348/§16.3）。值随后被赋值覆盖，语义不变。
        void* expertScript;
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
        // [J10GameRest.J10SkillSlot 2026-09-11] 块序轴：ORIG 0x08607d13
        // `call IsSpecialSkill; xor $1,%eax; test %al,%al; je 0x08607d3e`
        // （0x08607d3e 段 = `call get_temp_remain_sfp`）⇒ fall-through 为
        // `get_temp_remain_sp`，原码 = `if(!IsSpecialSkill()){SP}else{SFP}`；
        // 旧写 `== 1` 令 SFP 落 then、4.4 产 `test;je SP` 反序块（同 refund_skill）。
        // [R16-02 2026-09-12] P165 展开：ORIG 0x08607d2d..0x08607d3c 与
        //   0x08607d4c..0x08607d59 是**两份独立**的 `*remain = *remain + returnSP`
        //   （各 6 条 + 分支 1 尾 jmp），非共享尾；旧写公共尾使 4.4 把赋值合到
        //   汇合点（我方 15 条 vs ORIG 21 条 ⇒ gap +5）。按 ORIG 在分支内展开。
        if (!skill->IsSpecialSkill())
        {
            remain = get_temp_remain_sp();
            *remain = *remain + returnSP;
        }
        else
        {
            remain = get_temp_remain_sfp();
            *remain = *remain + returnSP;
        }
        result = 0;
        return slot;
    }
    result = 7;
    return -3;
}