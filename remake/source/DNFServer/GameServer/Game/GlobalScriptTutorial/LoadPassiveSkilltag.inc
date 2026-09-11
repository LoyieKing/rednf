// b28d-20: ORIG 0x08ab94d4。修法：while(true)+`if(!ScanInt×3)break;`（P133/P225）。
// 结果 IDENTICAL_AE（51=51）。
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
}