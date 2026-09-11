// ---- DisJoint::getPredictResultItemTypeCount（ORIG T 0x08473148，
//      docs/class_func_reports/DisJoint/getPredictResultItemTypeCount.md）：
//      rarity ∉ [0,5] → 1；否则读 m_additional[rarity].m_count
//      （this+0x5c8+rarity*0x10，P8：Ghidra 的 (param+0x5c)*0x10+8 即 entry
//      起点 m_count，非 +8 字段），非 0 → 2，否则 → 1。 ----
int DisJoint::getPredictResultItemTypeCount(int rarity) const
{
    if (rarity < 0 || rarity > 5)
        return 1;
    if (m_additional[rarity].m_count != 0)
        return 2;
    return 1;
}