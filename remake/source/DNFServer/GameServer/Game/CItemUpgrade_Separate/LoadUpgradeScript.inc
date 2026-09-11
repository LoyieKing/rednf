// ORIG T 0x0811eca8：import 尾透传（int 返回零开销透传 eax；0=成功）。
bool CItemUpgrade_Separate::LoadUpgradeScript(const char* path)
{
    return importGenuineUpgradeTable(path, &m_table);
}