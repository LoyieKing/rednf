bool CItemUpgrade::Load3rdChronicleUpgradeScript(const char* path)  // [P330] 与 CDataManager.h 声明同步
{
    // ORIG 0x085491b8：与 LoadUpgradeScript 同体（同一尾调用目标 0x0854b38a），
    // 仅符号不同；chronicle.etc 表与 Upgrade.etc 表共用表类型。
    return static_cast<CItemUpgradeTable*>(this)->loadUpgradeScript(path);
}