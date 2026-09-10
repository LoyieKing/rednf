void CUser::SaveBackupSkillInfo(SIG_LOAD_PREMIUM_LETHE* src,
                                ENUM_SKILL_TREE_KIND kind)
{
    // ORIG 0x0868148c：三段 `test %eax; sete/setne %al; test %al; jcc 岛` = bool 显式
    // 物化形（直接指针判缺 sete/setne 段）；dst 双 call（P48：条件一次、memcpy 实参
    // 再一次，call 结果 %eax 免槽直落 (%esp)，技巧37）；尾 `jmp+nop` 岛×2（技巧24）。
    if (!(static_cast<bool>(getCurCharacR())))
        return;
    if (kind == (ENUM_SKILL_TREE_KIND)-1 || kind == (ENUM_SKILL_TREE_KIND)0)
    {
        if (static_cast<bool>(GetCurCharacBackupSkill()))
            memcpy((void*)GetCurCharacBackupSkill(), src, 0x199);
    }
    else
    {
        if (static_cast<bool>(GetCurCharacBackupSkill2ND()))
            memcpy((void*)GetCurCharacBackupSkill2ND(), src, 0x199);
    }
