SALE_INFO* RETAILER::GetSaleInfo(int itemIdx)
{
    // ORIG T 0x08ab12a0：std::find + ne 比较；迭代器先声明后赋值（P28/P50）。
    std::vector<SALE_INFO>::iterator it;
    it = std::find(m_sales.begin(), m_sales.end(), itemIdx);
    if (it != m_sales.end())
        return &*it;
    return 0;
}