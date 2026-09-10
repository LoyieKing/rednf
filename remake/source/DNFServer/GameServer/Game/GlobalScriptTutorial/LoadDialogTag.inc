void GlobalScriptTutorial::LoadDialogTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    while (true)
    {
        int key, face;
        std::string text;
        if (!ScanInt(&key) || !ScanInt(&face) || !ScanStr(&text))
            break;
        m_data[static_cast<int>(job)].addDialogData(key, face, text.c_str());
    }
