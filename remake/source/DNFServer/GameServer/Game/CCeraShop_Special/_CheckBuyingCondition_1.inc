// ============================================================================
// _CheckBuyingCondition —— ORIG 0x08324810 (T)
//   CCeraShopGoods 重载。groupIdx==1 且 param6==0：按金币家庭（GetCoin+GetPayCoin+
//   GetEventCoin）+stackCount 判定金币上限（<0x186a1 通过，否则 outCode=0x4f）。
//   其余 groupIdx 委托 uint 重载 _CheckBuyingCondition(CUser*, uint, ulong, uint,
//   ushort, uchar, ushort)；子返回非 1 则 outCode=8。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08324810 orig/ours=102/97 ndiff=92。
// 帧/槽号与寄存器分配差（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P0 内联三实参 + P292 uint 重载 bool 化 + 块序翻转
bool CCeraShop::_CheckBuyingCondition(CUser* user, const CCeraShopGoods* goods,
                                      unsigned int param3, unsigned char param4,
                                      unsigned short param5, char param6, int& outCode)
{
    if (goods->GetGroupIdx() == 1 && param6 == 0)
    {
        // ORIG：三取值各自现场 getCurCharacInvenR（P48/P183 禁缓存）。
        unsigned int coin = user->getCurCharacInvenR()->GetCoin() +
                            user->getCurCharacInvenR()->GetPayCoin() +
                            user->getCurCharacInvenR()->GetEventCoin();
        unsigned int stackCount = (unsigned int)goods->GetStackCount();
        if ((int)(coin + stackCount) > 0x186a0)
        {
            outCode = 0x4f;
            return false;
        }
        outCode = 0;
        return true;
    }
    // P0：ORIG 委托段三实参无槽往返（esi/ebx/eax 直传）⇒ 内联实参。
    bool ok = false;
    ok = _CheckBuyingCondition(user, param3, (unsigned long)goods->GetItemIdx(),
                               (unsigned int)goods->GetStackCount(),
                               (unsigned short)goods->GetExpiryDate(), param4,
                               param5);
    if (!ok)
        outCode = 8;
    return ok;
}