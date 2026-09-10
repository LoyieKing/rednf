void StrikerScript::importParentLevelPenlaty()
{
    m_parentLevelPenalty.clear();
    bool ok = true;
    while (true)
    {
        ParentLevelPenalty entry;                 // 局部 -0x1c 起
        entry.m_field00 = ScanInt(&ok);           // -0x1c
        if (!ok)
            break;
        entry.m_field04 = ScanInt((bool*)0);      // -0x18
        entry.m_rate08 = ScanFloat((bool*)0);     // -0x14 (fstps)
        entry.m_rate0c = ScanFloat((bool*)0);     // -0x10 (fstps)
        m_parentLevelPenalty.push_back(entry);
    }
