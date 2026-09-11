// checkTotalTradeRuleOver —— ORIG W 0x08531fd2 (0x63) const：
//  type==0 且 totalValue==0          -> false
//  totalTradeType > tradeType        -> false
//  value 低 32 位 > total 低 32 位    -> false
//  低相等时高位 > total 高位          -> false
//  其余                               -> true
// （即记录的总量阈值未被当前 total 超越即视为 over。）
// ----------------------------------------------------------------------------
bool CAutoPunishRuleHackTypeMgr::checkTotalTradeRuleOver(unsigned int tradeType,
                                                         unsigned long long total) const
{
    // b28d-solo-checkTotalTradeRuleOver v3: ORIG 0x08531fd2 = **64 位成员整体比较**
    // `m_totalTradeValue <= total` 的正向包裹形（探针 fF 逐条命中）：`ja ret0`（type>）、
    // `cmp hi; ja ret0; cmp hi; jb ret1; cmp lo; ja ret0` 是 4.1 对 `<=` 的展开；
    // 旧 lo/hi 具名手写比较（成员 hi 重读 3 次）与 ORIG 的 hi 单读复用不符。
    if (m_totalTradeType == 0 && m_totalTradeValue == 0)
    {
        return false;
    }
    if (m_totalTradeType <= tradeType)
    {
        if (m_totalTradeValue <= total)
        {
            return true;
        }
    }
    return false;
}