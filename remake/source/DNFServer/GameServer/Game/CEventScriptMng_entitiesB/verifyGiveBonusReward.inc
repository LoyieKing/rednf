// ORIG 0x0810fae0：档位规则校验（count=本次比较基准购买数）。
// count < start 不发；end 非 0 且 count > end 不发；(count-start)%step!=0 不发。
bool CPurchaseCashItemBonus::verifyGiveBonusReward(
    PurchaseCashItemBonusScript::PurchaseBonusRule& rule, int count)
{
    if (count >= rule.m_purchaseStartCount)
    {
        if (rule.m_purchaseEndCount >= count || rule.m_purchaseEndCount == 0)
        {
            if ((count - rule.m_purchaseStartCount) %
                    rule.m_purchaseMutipleNumber == 0)
                return true;
        }
    }
    return false;
}