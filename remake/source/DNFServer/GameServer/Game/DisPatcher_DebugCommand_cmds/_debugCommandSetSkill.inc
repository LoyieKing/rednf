// ===========================================================================
// 4. _debugCommandSetSkill —— 0x0858eab8
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：SetSkillLevel bool 化
// （P248 档①，Main 代落 CUser.h）+ P223 goto 尾标签翻尾块序（同 ClearSkill）；
// P167 callsite CLEAR 10/10。
bool DisPatcher_DebugCommand::_debugCommandSetSkill(CUser* user, int skillIdx,
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
                                                    int level)
{
    if (!user->isGMUser())                      // +0x08
        return false;
    // ORIG `call SetSkillLevel; test %al,%al; je <尾 mov $0>` ⇒ 正向包裹 + callee bool
    // （P248 档①：int callee 的一切调用点写法恒产 sete 四段）。
    // 同 ClearSkill：三形实测（goto 尾标签 / 早返 / if-else 双 return = 74 / 74 / 73
    // 条）中仅 goto 形与 ORIG 0x0858ebb8（失败块）/0x0858ebbf（成功块）布局全同，
    // 仅多 1 条 ok 标签前对齐 nop（§6.1 豁免）。
    if (user->SetSkillLevel(skillIdx, level))   // +0x38
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa2, 0)(
            "%s:'%s' GM Mode Set Skill(%d) Level(%d).",
            NumberToString(user->get_acc_id(), 0), user->getCurCharacName(),
            skillIdx, level);
        user->send_skill_info();                // +0xbb
        if (user->get_state() == 5)             // +0xd1
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
        goto ok;                                // 成功体尾 jmp（ORIG 0x0858ebb6）
    }
    return false;                               // +0x100 失败块（mov$0）
ok:
    return true;                                // +0x107 成功块（mov$1）
}