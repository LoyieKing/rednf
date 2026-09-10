bool CUser::AddDungeonClear(int dungeonIdx, int clearCount)
{
    // ORIG 0x086??：getClearedDungeonDiff 结果 movsbl %al,%eax 原地单段（无 char
    // 槽物化）+ 比较 setl %al;test;je 尾物化岛（技巧 22 register bool al，直折
    // jge 反形）；残差 movswl %ax,%ebx（4.1 arg 截断 sext 直落 ebx）N18a。
    // 二修：getter 结果直用免槽（技巧 37，ORIG call 后 %eax 即 this 实参）、
    // 正向 `if(less)`（`!less` 反产 xor$1 取反）。
    // b28d-solo-CUser AE: ORIG `cmp; setl %al; test %al,%al; je` 无槽；具名 less 落 -0x9
    // 使帧 +0x10 ⇒ 内联条件（probe_bool a6/a7）。
    if (getDungeonClearRefR()->getClearedDungeonDiff(dungeonIdx) < clearCount)
    {
        getDungeonClearRefW()->addClearedDungeon(dungeonIdx, clearCount);
        return true;
    }
    return false;
