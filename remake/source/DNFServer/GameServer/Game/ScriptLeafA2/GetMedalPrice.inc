// ============================================================================
// CItem::GetMedalPrice —— ORIG W 0x0822c862
// 【报告缺失】无专属函数报告；已先读调用者报告
// DisPatcher_RentAvatar/dispatch_sig.md 与 DisPatcher_BuyItem/get_data.md
// （与 GetCashPrice/get_price 并列的价格访问器）后按 nm 地址 objdump 转写：
//   mov 0x8(%ebp),%eax ; mov 0x14(%eax),%eax ; ret —— 返回 this+0x14 int。
// ============================================================================
int CItem::GetMedalPrice() const
{
    return m_field14;   // +0x14
}