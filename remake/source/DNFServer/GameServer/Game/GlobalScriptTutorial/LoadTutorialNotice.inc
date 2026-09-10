void GlobalScriptTutorial::LoadTutorialNotice(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    while (true)
    {
        int key, field8, fieldc;
        std::string path;
        if (!ScanInt(&key) || !ScanInt(&field8) || !ScanInt(&fieldc) || !ScanStr(&path))
            break;
        m_data[static_cast<int>(job)].addTutorialNotice(key, field8, fieldc, path.c_str());
    }
