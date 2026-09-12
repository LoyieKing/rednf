// ==== Dispatcher_MonstercardBind::~Dispatcher_MonstercardBind (ORIG D1 0x08244b42 / D0 0x08244b72，基类转发) ====
// ==== Dispatcher_MonstercardBind::check_need_slot (ORIG 0x081d83c4 T) ====
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081d83c4 orig/ours=28/28
// [SemB00 2026-09-12] 真语义差异修复：ORIG 0x081d83c4 段为
//   cmpl $0x0,-0x10(%ebp); jns L(+0x5c)=mov $0x1,%eax;
//   cmpl $0x0,-0xc(%ebp); js L(+0x63)=mov $0x0,%eax
//   ⇒ 语义 = (emptyCard >= 0) || (emptyAny >= 0)（任一有空位即 1）。
// 旧写法 `emptyCard < 0 && emptyAny >= 0` 为反演式，与 ORIG 不等价
// （反例 emptyCard=5/emptyAny=-3：ORIG 1 / 旧写 0）。
// A/B（check_tu_routed Dispatchers.cpp）：ae 2567→2568、diff 169→168、
// near 12 不变、missing 813 不变；.o 反汇编与 ORIG 逐条同形（jns +0x5c / js +0x63）。
bool Dispatcher_MonstercardBind::check_need_slot(CUser* user)
{
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    const int emptyCard =
        invenR->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)10);
    const int emptyAny =
        invenR->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)0);
    if (emptyCard >= 0 || emptyAny >= 0)
    {
        return 1;
    }
    return 0;
}