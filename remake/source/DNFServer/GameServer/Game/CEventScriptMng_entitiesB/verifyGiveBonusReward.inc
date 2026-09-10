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
