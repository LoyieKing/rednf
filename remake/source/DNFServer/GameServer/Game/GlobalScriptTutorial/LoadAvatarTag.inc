void GlobalScriptTutorial::LoadAvatarTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int type, index;
    while (true)
    {
        if (!ScanInt(&type) || !ScanInt(&index))
            break;
        m_data[static_cast<int>(job)].addAvatarIndex(type, index);
    }
