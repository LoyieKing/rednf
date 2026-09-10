void CHandlePremium::handleSetUserPremium(CUser* user, ENUM_PREMIUM_TYPE type,
                                          long start, long end, int flag, bool recalc)
{
    CUserPremium* premium = user->GetPremiumInfoW();
    // [b28d-56 判词·AE] P0 去 iType 具名局部：ORIG 0x086af802 `cmpl $0x5,0xc(%ebp)`
    // 形参直比、AddPremium/RemovePremium 实参处各自 `mov 0xc(%ebp),%eax` 直取，
    // 无 iType 槽；curSec 反之 ORIG 0x086af85d 有槽往返（-0xc）⇒ 补具名局部。
    if ((int)type == 5)
    {
        premium->AddPremium((int)type, (int)start, (int)end, flag);
    }
    else if (start == 0 || end == 0)
    {
        premium->RemovePremium((int)type);
    }
    else
    {
        int curSec = GlobalData::s_systemTime_.getCurSec();
        if (curSec < end)
            premium->AddPremium((int)type, (int)start, (int)end, flag);
    }
    if (recalc)
    {
        premium->ReCalcAdvantage();
        premium->RecalcAdditionalInfo(user);
    }
