void CUser::_GetSaveInvenBase(SIG_SAVE_INVENTORY* out)
{
    out->m_accId = get_acc_id();
    out->m_characNo = getCurCharacNo();
    out->m_money = getCurCharacMoney();
    out->m_coin = getCurCharacCoin();
    out->m_payCoin = getCurCharacPayCoin();
    out->m_eventCoin = getCurCharacEventCoin();
    if (getCurCharacR() == 0)
    {
        out->m_avatarCoin = 0;
    }
    else
    {
        // ORIG 0x0864fd80：getCurCharacInvenR 结果 add $0x658 直落 Get receiver
        //（无 coinOf call；真实成员 CInventory::m_avatarCoin @+0x658）。
        out->m_avatarCoin = getCurCharacInvenR()->m_avatarCoin.Get();
    }
    // ORIG 0x0864fd80 尾：getCurSec 结果落 -0xc 槽再装载（P0 具名局部）。
    // P211 双向开关：形参 long 与实参同型（long 局部）⇒ this(LHS) 先载 =
    // ORIG `mov 0x8(%ebp),%eax; mov -0xc(%ebp),%edx` 序；unsigned 异型产 RHS 先。
    long curSec = GlobalData::s_systemTime_.getCurSec();
    setCurCharacLastPlayTick(curSec);
