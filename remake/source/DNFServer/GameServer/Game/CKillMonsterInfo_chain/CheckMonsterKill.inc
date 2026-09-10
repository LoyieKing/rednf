bool CKillMonsterInfo::CheckMonsterKill(
    std::map<unsigned int, unsigned char> const& kills,
    unsigned int mobIdx, unsigned char diff)
{
    std::map<unsigned int, unsigned char>::const_iterator it =
        kills.find(mobIdx);
    // [b28d-7 P167] ORIG 调 const_iterator::operator!=（0x0854d10x），
    // 旧版 == 产生另一实例（调用点差集 MISSING/EXTRA 各 1）。
    if (it != kills.end())
    {
        // ORIG 0x0854d1xx：`and %ebx,%eax; test %al,%al`（字节宽度判定）⇒
        // 结果按 unsigned char 收窄。
        register unsigned char rec = it->second;
        if ((rec & (unsigned char)GetDungeonDifficultyFlag(diff)) != 0)
            return true;
        return false;
    }
    return false;
