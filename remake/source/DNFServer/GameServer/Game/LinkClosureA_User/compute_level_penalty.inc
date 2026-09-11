// ---------------------------------------------------------------------------
// CUser::compute_level_penalty（报告全文已读）
// m_questParameter(+0x8cc8).m_field18(+0x18 → +0x8ce0) /
// m_field1c(+0x1c → +0x8ce4)。
// ---------------------------------------------------------------------------
int CUser::compute_level_penalty(int levelDelta)
{
    // P196：ORIG `cmpl $6; jle / cmpl $11; jg / call 0x8ce0 / cmpl $11; jle / call 0x8ce4 / mov $100`
    if (levelDelta > 6 && levelDelta <= 11)
        return G_CDataManager()->m_questParameter.m_field18;   // [0x8ce0]
    else if (levelDelta > 11)
    {
        return G_CDataManager()->m_questParameter.m_field1c;   // [0x8ce4]
    }
    return 100;
}