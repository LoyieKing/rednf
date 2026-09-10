void CItemLimitEdition::updateCurSellNum(unsigned int count)
{
    // P95：ORIG 0x084e970e 先载 this 再载值；4.4 直成员赋值先备值。具名视图复现。
    ((CItemLimitEditionView*)this)->m_curSellNum = (int)count;
