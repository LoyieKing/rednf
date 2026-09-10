
// ============================================================================
// checkBuyingCondition —— ORIG 0x08323f2a (T)
// 仅转发到 _CheckBuyingCondition(CUser*, uint, ulong, uint, ushort, uchar, ushort)。
// ============================================================================
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P292/P159：返回类型 int→bool
// 消 `movzbl %al,%eax`（ORIG 尾为 call;leave;ret 直透 bool 结果）
bool CCeraShop::checkBuyingCondition(CUser* user, unsigned int a, unsigned long itemIdx,
                                     unsigned int b, unsigned short c, unsigned char d,
                                     unsigned short e)
{
    return _CheckBuyingCondition(user, a, itemIdx, b, c, d, e);
}

