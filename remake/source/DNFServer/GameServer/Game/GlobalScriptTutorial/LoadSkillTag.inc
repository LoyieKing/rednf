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
