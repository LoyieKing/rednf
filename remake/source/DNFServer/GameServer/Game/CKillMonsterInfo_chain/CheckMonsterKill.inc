// ---- CheckMonsterKill(map const&, mobIdx, diff) —— ORIG T 0x0854d0ea -------
// find(mobIdx)!=end 且 (diffFlag & 记录值) 非 0 → true。
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0854d0ea（49 条）：
// const_iterator::operator!=（ORIG `ne(PKi,RKi)`）、字节宽度判定
// `test %al,%al; setne`（uchar 物化）、共享 false 尾块均对齐（P167 CLEAR）。
// 关键旋钮（P231/P248 档②）：`register unsigned char rec = it->second;` +
// 条件 RHS 侧 `(unsigned char)GetDungeonDifficultyFlag(diff)` —— 使 ORIG 0854d149
// 的 `movzbl 0x4(%eax),%ebx` 单段直落 callee-saved 复现；无 register 时 4.4 恒
// `movzbl …,%eax; mov %eax,%ebx`（50/49），RHS 不加 uchar cast 则丢 setne 段。
// 现 49=49 助记符/槽位逐条全等，残余仅 .o 重定位地址显示（P264）。
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
}