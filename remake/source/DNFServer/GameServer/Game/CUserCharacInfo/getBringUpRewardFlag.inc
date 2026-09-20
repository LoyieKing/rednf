bool CUserCharacInfo::getBringUpRewardFlag() const   // [b28d-Main 代落·S350 P292] uchar→bool
{
    return m_selected ? m_selected->m_field1278 : 0;
}