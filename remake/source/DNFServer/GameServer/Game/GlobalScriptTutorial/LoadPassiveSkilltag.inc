void GlobalScriptTutorial::LoadPassiveSkilltag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, skill, field4;
    while (true)
    {
        if (!ScanInt(&slot) || !ScanInt(&skill) || !ScanInt(&field4))
            break;
        m_data[static_cast<int>(job)].addPassiveSkillData(slot, skill, field4);
    }
