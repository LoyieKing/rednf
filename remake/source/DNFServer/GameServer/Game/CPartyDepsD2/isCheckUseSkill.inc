bool CParty::isCheckUseSkill(int job, int skillIdx)
{
    // [共享岛单链] ORIG 0x085bb5b0：9 条件合一 || 短路链，各 jcc 跳共享
    // return false 岛（9 条独立 if 语句 4.4 会各出 mov0;jmp 岛 +16 行）。
    if (skillIdx > 0xa8 ||
        (job == 0 && skillIdx == 0x2e) ||
        (job == 1 && skillIdx == 0x5) ||
        (job == 2 && skillIdx == 0x4) ||
        (job == 3 && skillIdx == 0xc) ||
        (job == 4 && skillIdx == 0x1) ||
        (job == 5 && skillIdx == 0x4) ||
        (job == 6 && skillIdx == 0x1) ||
        (job == 7 && skillIdx == 0x5))
    {
        return false;
    }
    return true;
