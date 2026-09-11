// ---------------------------------------------------------------------------
// 26) CUser::CalLevelUpItemCheck(unsigned long itemId)
//     报告：docs/class_func_reports/CUser/CalLevelUpItemCheck.md。ORIG 0x08689d06。
// ---------------------------------------------------------------------------
bool CUser::CalLevelUpItemCheck(unsigned long itemId)
{
    int lvl = get_charac_level();
    // ORIG 0x08689d06：4.1 switch 二分树 `cmp 0x28d2bc je 体岛1/ja 右支；cmp 1f71 je 体岛2；
    // 右支 cmp 989a13 je 体岛1、cmp 292090e1 jne ret0-fall 体岛1`。体岛1（三 case 共享，书写
    // 在前）内层假路落 +0x62 `nop;jmp`/+0x65 `nop` 岛、体岛2 假路直落 +0x66 `mov$0`——
    // probe p9.sw3 实证 4.4 嵌套 if+break 逐条同形（技巧24/34；&& 合并形反产双直落）。
    switch (itemId)
    {
        case 0x28d2bc:
        case 0x989a13:
        case 0x292090e1:
            if (lvl > 0) {
                if (lvl < 0x55) return true;
            }
            break;
        case 0x1f71:
            if (lvl >= 0x13) {
                if (lvl <= 0x3b) return true;
            }
            break;
        default:
            break;
    }
    return false;
}