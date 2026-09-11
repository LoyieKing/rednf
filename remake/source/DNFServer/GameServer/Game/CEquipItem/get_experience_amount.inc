// ORIG W 0x833eea6：flds [this+0x158]，即 m_randomTable.m_field10 的 float 位模式。
float CEquipItem::get_experience_amount() const
{
    // [b28c-21] ORIG `mov 0x158(%eax),%eax` 单条；float 位视图成员直读。
    return m_randomTable.m_experienceAmount;
}