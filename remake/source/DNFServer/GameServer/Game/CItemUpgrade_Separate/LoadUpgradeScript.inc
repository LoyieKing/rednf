bool CItemUpgrade_Separate::LoadUpgradeScript(const char* path)
{
    return importGenuineUpgradeTable(path, &m_table);
