// [b28d-solo-CMailBoxHelper] 修法（DIFF 132→NEAR，156=156 insn）：
// ①`Inven_Item const& pkg` → 按值 `Inven_Item pkg`（ORIG 0x085536fa
// `lea -0x69(%ebp); call 拷贝构造`；引用局部多一枚指针槽 + 每处解引用，-9 条）；
// ②删 SetEventCoin/SetPayCoin/SetCoin 三处冗余 `(int)` cast（P41，-6 条）；
// ③switch case 书写序改 0,1,2（P46 叶块序 = case 序；ORIG 决策链
// `cmp $1;je;cmp $2;je;test;jne default` 且 case0 fall-through，-1 条）。
// [本波 2026-09-10 真 bug 修复] P18 make_pair 键值序：ORIG 0x08553868/0x0855386f
// `movl $0x1,-0x1c(%ebp); movl $0x0,-0x18(%ebp)` 且 0x4(%esp)←&(-0x18)、
// 0x8(%esp)←&(-0x1c)；0x4(%esp) = make_pair 第一实参已由 _DoGetPackageItem
// ORIG 0x8554143（`movl $0x0,-0x24` + `lea -0x74→0x8`，第一实参=0 落 -0x24）
// 交叉证实 ⇒ ORIG 第一实参=0(ENUM_ITEMSPACE)、第二实参=1(INVEN_SLOT)。
// 旧码写 (1,0) 使 pair=(space 1, slot 0) 而 GetPackege 以 it->first 选
// INVEN_TYPE（1→type2）⇒ 客户端显示位置错误。
bool CMailBoxHelper::_DoGetPackageCoin(CUser* user, CMailBox::CMail const* mail,
                                       std::vector<std::pair<int, int> >& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553672
    int coin = user->getCurCharacInvenW()->GetCoin();
    int eventCoin = user->getCurCharacInvenW()->GetEventCoin();
    int payCoin = user->getCurCharacInvenW()->GetPayCoin();
    if (coin + eventCoin + payCoin >= 0x186a1)
    {
        err = (ENUM_ERROR)0x4f;
        return false;
    }
    if (bReset)
        return true;

    // ORIG 0x085536fa：`lea -0x69(%ebp),%eax; call Inven_Item 拷贝构造` ⇒
    // 按值局部（非引用；引用局部会多一枚指针槽 + 每处指针解引用）。
    Inven_Item pkg = mail->GetPackageItem();
    // ORIG 决策链 cmp $1/cmp $2/test（无槽）⇒ switch；amount=pkg.m_addInfo2
    // （Inven_Item +0x07）、coinType=pkg.m_fieldb（+0x0b，ushort movzwl 两段）。
    // ORIG 历史日志第二参均为 getCurCharacInvenR()->GetEventCoin() 重取。
    switch (pkg.m_fieldb)
    {
    case 0:
        user->getCurCharacInvenW()->SetCoin(
            user->getCurCharacInvenR()->GetCoin() + pkg.m_addInfo2);
        user->m_historyLog.CoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)0);
        break;
    case 1:
        user->getCurCharacInvenW()->SetEventCoin(
            user->getCurCharacInvenR()->GetEventCoin() + pkg.m_addInfo2);
        user->m_historyLog.EventCoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)2);
        break;
    case 2:
        user->getCurCharacInvenW()->SetPayCoin(
            user->getCurCharacInvenR()->GetPayCoin() + pkg.m_addInfo2);
        user->m_historyLog.PayCoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)3);
        break;
    }

    items.push_back(
        std::make_pair((ENUM_ITEMSPACE)0, (Inven_Item::INVEN_SLOT)1));
    return true;
}