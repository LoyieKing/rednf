// ---- CUser::_GetSaveInvenBase ORIG @ 0x0864fd80 ----
// [b28d-solo-Arad_AvatarRoulette AE] 61=61 逐条对齐，p167 11/11 CLEAR。
// P211 档③字段侧同型化：ORIG 0x0864fda1 段为 LHS-first 直存
// `mov 0xc(%ebp),%edx; mov %eax,0x4(%edx)`；SIG_SAVE_INVENTORY::m_characNo 为 int 时
// 与 getCurCharacNo()(unsigned int) 异型 ⇒ 4.4 恒产 RHS-first+`mov %eax,%edx` 中转。
// 字段改 unsigned int（SigTypes_E.h:104，宽度/偏移/sizeof 0xad99 不变）后逐条全等；
// 源侧 static_cast<int> 形实测无效（P211 明证，勿再试）。
// 其余 store（+0x00/+0x09/+0x0d/+0x11/+0x15）本就与各自 callee 同型直存。
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
}