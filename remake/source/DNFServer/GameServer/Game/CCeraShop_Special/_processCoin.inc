// ============================================================================
// _processCoin —— ORIG 0x08325a88 (T)
// 代币购买入账：PayCoin += value → historyLog.PayCoinAdd(总额, 增量, reason 3)
// → SaveMoneyCoin → SendCoin。第 3 参 flag 在 ORIG 中未参与逻辑（签名保留）。
// ============================================================================
void CCeraShop::_processCoin(CUser* user, unsigned short value, bool)
{
    user->getCurCharacInvenW()->SetPayCoin(
        user->getCurCharacInvenR()->GetPayCoin() + value);
    user->m_historyLog.PayCoinAdd(
        (int)user->getCurCharacInvenR()->GetPayCoin(), (int)value,
        (eCoinAddReason)3);
    user->SaveMoneyCoin();
    user->SendCoin();
}