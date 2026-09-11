// ORIG W 0x0854b382 _ZNK8WongWork17CItemUpgradeTable19getUpgradeInfoTableEv：
// 仅返回 this（mov 0x8(%ebp),%eax; ret）。
const upgrade_table_t* CItemUpgradeTable::getUpgradeInfoTable() const
{
    return this;
}