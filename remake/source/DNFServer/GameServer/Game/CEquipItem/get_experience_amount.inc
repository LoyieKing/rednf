float CEquipItem::get_experience_amount() const
{
    // [b28c-21] ORIG `mov 0x158(%eax),%eax` 单条；float 位视图成员直读。
    return m_randomTable.m_experienceAmount;
