// ---- CPrivateStore（ORIG C1 0x85c4d10 / D1 0x85c4ecc）----
CPrivateStore::CPrivateStore()
{
    // ORIG 语句顺序：m_pParent → m_state → items reserve/resize → name reserve
    // → m_dollCreated → m_dollItemID（O0 下按源码顺序生成指令；异常路径只含
    // string/vector/list 析构 + Unwind_Resume，无基类析构——基类 trivial）。
    m_pParent = 0;
    m_state = 0;
    // ORIG ctor 不初始化 m_pSeller/m_pBuyer（开店/购买流程经 SetSeller/SetBuyer 写入）。
    m_items.reserve(0xe);
    m_items.resize(0xe, PrivateStoreItem());
    m_name.reserve(0x28);
    m_dollCreated = 0;
    m_dollItemID = 0;
}