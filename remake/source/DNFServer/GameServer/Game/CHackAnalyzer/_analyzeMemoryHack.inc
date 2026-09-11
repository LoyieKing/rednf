void CHackAnalyzer::_analyzeMemoryHack(unsigned int val)
{
    // ORIG 0x080f7dce（_analyzeMemoryHack.md）。
    // ORIG je/jne 双侧汇入统一 leave（P63 共享出口）：if(!=0&&val){body} 形
    // 消除 4.4 jmp/nop 中继（探针 f2 形）。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_MEMORY))
        return;
    if (val != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_MEMORY, (ENUM_HACKTYPE)0x3bc, (int)val);
    }
}