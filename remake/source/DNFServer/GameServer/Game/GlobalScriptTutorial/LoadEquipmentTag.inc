void GlobalScriptTutorial::LoadEquipmentTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, type, index;
    while (true)
    {
        if (!ScanInt(&slot) || !ScanInt(&type) || !ScanInt(&index))
            break;
        m_data[static_cast<int>(job)].addEquipmentIndex(type, index, slot);
    }
