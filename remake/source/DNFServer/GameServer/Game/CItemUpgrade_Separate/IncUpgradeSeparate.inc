void UpgradeSeparateInfo::IncUpgradeSeparate()
{
    // [b28d-solo-CItemUpgrade_Separate] AE：位域前置自增 `++m_bits.m_lo` 复现 ORIG
    // 0x0811ed32 的「截断掩码就地施于 %eax + 再掩码写回」形（探针 /tmp/b28d_51/p6.cpp
    // 形 b 逐条全等；`= m_lo + 1` 与 `+= 1` 恒多一条 `mov %eax,%edx` 中转）。
    ++m_bits.m_lo;
