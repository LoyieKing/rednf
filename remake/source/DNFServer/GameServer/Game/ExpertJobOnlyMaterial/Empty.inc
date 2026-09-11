// 【objdump】ORIG _ZN10expert_job17ExpertJobMaterial5EmptyEv W @0x0849f286：
// m_item==0 || m_itemIndex==0 || m_user==0 → 1，否则 0。（无独立报告）
bool ExpertJobMaterial::Empty()
{
    return m_item == 0 || m_itemIndex == 0 || m_user == 0;
}