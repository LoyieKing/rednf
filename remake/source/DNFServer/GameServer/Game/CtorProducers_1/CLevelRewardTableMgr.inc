CLevelRewardTableMgr::CLevelRewardTableMgr()
{
    // ORIG 0x08687710：vec@+0x08 自动构造；flag=0；m_table 先置 0 再 new(0x6088)。
    m_flag = 0;
    m_table = 0;
    m_table = new CLevelRewardTable;
