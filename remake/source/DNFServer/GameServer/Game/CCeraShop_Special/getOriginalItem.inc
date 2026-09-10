// ============================================================================
// getOriginalItem —— ORIG 0x08327ef4 (T)：读取记录的原始商品 idx（<=0 返回 0）。
// ============================================================================
int CCeraShop::getOriginalItem()
{
    // ORIG 0x08327ef4：if-return 分支岛形（4.4 三目会折 cmovs，异形）。
    if (m_originalItemIdx > 0)
        return m_originalItemIdx;
    return 0;
}

