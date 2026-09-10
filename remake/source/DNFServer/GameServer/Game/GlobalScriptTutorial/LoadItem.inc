void GlobalScriptTutorial::LoadItem(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int item, count;
    while (true)
    {
        if (!ScanInt(&item) || !ScanInt(&count))
            break;
        m_data[static_cast<int>(job)].addItem(item, count);
    }
