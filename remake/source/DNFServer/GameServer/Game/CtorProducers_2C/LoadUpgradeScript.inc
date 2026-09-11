bool CItemUpgrade::LoadUpgradeScript(const char* path)
{
    // ORIG 0x0854919a：尾调 WongWork::CItemUpgradeTable::loadUpgradeScript(this+0x04,
    // path)（W 0x0854b38a，内部即 importUpgradeTable(path, table) 并透传 eax）。
    // 基类纯虚槽1 返回 int，尾透传 import 错误行号（0=成功）。
    return static_cast<CItemUpgradeTable*>(this)->loadUpgradeScript(path);
}