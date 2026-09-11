// ORIG 0x0864dcf2 _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864dcf2 | _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
bool CUser::GetSkill(SIG_LOAD_SKILL* skill, SIG_COMBO_SKILL* combo) const
{
    // b28c-L5 判词（N30/N18a 族·lea-vs-add 跨版本折叠差，语义零差）：ORIG 对
    //   `&skill->m_remainSpTree{1,2}/m_remainSfpTree{1,2}` 三个 callee-saved 实参
    //   恒走「地址算术在 %eax 上做 → 再 mov 到 %edi/%esi/%ebx」两条
    //   （`mov 0xc(%ebp),%eax; add $0x340,%eax; mov %eax,%edi`），而 4.4-O0 折叠为
    //   单条 `lea 0x340(%eax),%edi`（我方 -3 insn）。同函数内**非 callee-saved**
    //   的那处 `mov 0xc(%ebp),%eax; add $0x1a8,%eax`（memcpy 目标）两侧完全一致，
    //   证明差异只在「目标寄存器是否 callee-saved」这一 lea 折叠决策上，与源形无关
    //   （cast/具名局部/语句表达式三形实测均维持 lea 单条）⇒ P30 尾判：
    //   「穷举后 add-vs-lea 与寄存器名仍不齐 → 结构性伪影不强求」。
    //   本波已翻正项：get_skill_command_buf 的具名结果指针 + 判空（原缺，
    //   ORIG `mov %eax,-0x1c(%ebp); cmpl $0x0,-0x1c(%ebp); je` 实证 P0 有槽往返）
    //   ⇒ 同时把 cmds 数组顶到 ORIG 的 -0xe4（原 -0xe0 帧差 4 即此槽缺失所致）。
    skill->m_field04 = 0;
    // ORIG：getCurCharacSkillR 在 get_remain_sp/get_skill_slot/getComboSkillSaveData
    // 三处各自重调——访问器结果禁缓存（战术 P183/P48）。
    ((SkillSlot*)getCurCharacSkillR())
        ->get_remain_sp(&skill->m_remainSpTree1, &skill->m_remainSpTree2,
                        &skill->m_remainSfpTree1, &skill->m_remainSfpTree2);
    ((SkillSlot*)getCurCharacSkillR())
        ->get_skill_slot((char*)skill->m_skillSlotTree1,
                         (char*)skill->m_skillSlotTree2, 0x198);
    STSkillCommandData::STCommand cmds[50];
    memset(cmds, 0, sizeof(cmds));
    // b28c-L5：ORIG 0x0864ddxx 在 call 之后另有一次「cmds 基址落槽 + 判空」：
    //   `call get_skill_command_buf / lea -0xe4(%ebp),%eax / mov %eax,-0x1c(%ebp) /
    //    cmpl $0x0,-0x1c(%ebp) / je +0x127 / …mov -0x1c(%ebp),%eax → memcpy 源`
    //   ⇒ 原码有具名指针局部（P0：该值有槽往返），且声明点在 call **之后**
    //   （4.1 初始化前置族 N88 的反向：此处落槽动作确实发生在 call 之后）。
    //   该具名槽同时把 cmds 数组顶到 -0xe4（我方原形 -0xe0 的 4 字节差即此槽缺失，
    //   非 N95 帧基差）。memcpy 源改用该指针（ORIG 从 -0x1c 回读）。
    get_skill_command_buf(cmds);
    STSkillCommandData::STCommand* pCmdBuf = cmds;
    if (pCmdBuf != 0)
    {
        memcpy(skill->m_commands, pCmdBuf, sizeof(cmds));
    }
    // ORIG 0x0864de1e `movzbl 0xa850(%eax)`（unsigned char 宽实证）
    skill->m_field411 = G_CDataManager()->m_pvpSkillTree.m_field50;
    ((SkillSlot*)getCurCharacSkillR())->getComboSkillSaveData(combo);
    return 1;
}