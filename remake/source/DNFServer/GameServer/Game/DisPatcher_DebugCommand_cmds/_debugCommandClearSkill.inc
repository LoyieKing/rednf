// ===========================================================================
// 3. _debugCommandClearSkill —— 0x0858e9c4
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：N93附「尾双 return 块物理序互换」
// 由 P223 goto 尾标签翻正——成功体尾 `goto ok;` + 块外 `return false;` +
// `ok: return true;` 复现 ORIG `je <尾 mov$0 块>` / `mov$1` 后置布局
// （P222 正向包裹的 goto 变体，探针 p3 实测）。P167 callsite CLEAR 10/10。
bool DisPatcher_DebugCommand::_debugCommandClearSkill(CUser* user,
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
                                                      MSG_DEBUG_COMMAND& /*msg*/)
{
    if (!user->isGMUser())                      // +0x08（P168a① 裸形）
        return false;
    DBG_TRACE(__PRETTY_FUNCTION__, 0x8a, "%s:'%s' GM Mode Clear Skill.");
    // 尾块序实测三形（本波）：①goto 尾标签形=布局与 ORIG 全同、仅多 1 条 ok 标签前
    // 对齐 nop（§6.1 nop 豁免 ⇒ TU 口径仍 identical）；②早返形 66 条但失败块前移且
    // 多 `xor $0x1`；③if/else 双 return 形 65 条但失败块落尾、成功块前移 ⇒ 均劣。
    // 保留①（ORIG 0x0858eaa4 失败块 / 0x0858eaab 成功块 后置布局）。
    if (user->ClearSkills())                    // +0x92 ClearSkills
    {
        user->send_skill_info();                // +0xa1
        if (user->get_state() == 5)             // +0xb1 state==5
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
        goto ok;                                // 成功体尾 jmp（ORIG 0x0858eaa2）
    }
    return false;                               // +0xe0 失败块（mov$0）
ok:
    return true;                                // +0xe7 成功块（mov$1）
}