// b28d-20: ORIG 0x08ab9408。修法：while(true)+`if(!ScanInt×5)break;`（P133/P225）。
// 结果 IDENTICAL_AE（71=72，尾 nop 剥离）。
void GlobalScriptTutorial::LoadSkillTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, hotkey, skill, field4, field8;
    while (true)
    {
        if (!ScanInt(&slot) || !ScanInt(&hotkey) || !ScanInt(&skill) ||
            !ScanInt(&field4) || !ScanInt(&field8))
            break;
        m_data[static_cast<int>(job)].addSkillIndex(slot, hotkey, skill, field4, field8);
    }
}