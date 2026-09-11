// ORIG W 0x08985d1c：基类 → string+0x8 → set+0xc → vector<pair<Rule,mail>>+0x24
// → string+0x8 clear（无 mail.reset，忠实保留）。
PurchaseCashItemBonusScript::PurchaseCashItemBonusScript()
{
    m_dbTableName.clear();
}