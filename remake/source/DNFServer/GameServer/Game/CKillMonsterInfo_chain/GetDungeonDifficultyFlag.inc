// ---- GetDungeonDifficultyFlag —— ORIG T 0x0854d21a -------------------------
// diff<=4 ? (1<<diff) : 0（shl %cl 位型实证；返回 int，写 map 值时按 byte 截断）。
int CKillMonsterInfo::GetDungeonDifficultyFlag(unsigned char diff)
{
    if (diff > 4)
        return 0;
    return 1 << diff;
}