// ORIG W 0x0854b38a _ZN8WongWork17CItemUpgradeTable17loadUpgradeScriptEPKc：
// 尾调 importUpgradeTable(path, this) 并透传返回值（错误行号，0=成功）。
bool CItemUpgradeTable::loadUpgradeScript(const char* path)
{
    return importUpgradeTable(path, &m_table);   // [R18rti · B3] 原 `this`（基类转换）
}