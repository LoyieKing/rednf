// IsBuyableRestrictItem —— ORIG 0x080e0ce2 (T)
// 静态转发：ARAD::Singleton<Manager>::Get()->CheckBuyableProduct(user, ipgNo, itemIdx, type)。
// 返回 CheckBuyableProduct 的结果（1=可购买）；调用方（ItemVendingMachine::
// ProcessBuyRequestEx）取反判定「不可购买」。
// [L6S6 2026-09-11 复核] AE 16=16 无源码改动：ORIG 0x080e0ce2 `sub $0x28,%esp` →
// `call Singleton::Get` → 4 条 `mov 0x14/0x10/0xc/0x8(%ebp),%edx; mov %edx,0x10/0xc/
// 0x8/0x4(%esp)`（栈参数按**逆序**逐个中转，esp 直传 %eax）→ `call
// CheckBuyableProduct` → `leave; ret`（无归一化 ⇒ 返回类型非 bool 转换点）；
// p167 CALLSITE CLEAR（calls 2=2）。
bool Manager::IsBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                   unsigned int itemIdx, int type)
{
    return ARAD::Singleton<CerashopAddRestrict::Manager>::Get()
        ->CheckBuyableProduct(user, ipgNo, itemIdx, type);
}