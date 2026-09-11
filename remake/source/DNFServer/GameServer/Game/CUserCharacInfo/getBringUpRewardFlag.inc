bool CUserCharacInfo::getBringUpRewardFlag() const   // [b28d-Main 代落·S350 P292] uchar→bool
{
    return m_selected ? CUR->m_bringUpRewardFlag : 0;
}