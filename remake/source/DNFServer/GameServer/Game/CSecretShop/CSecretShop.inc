secretshop::CSecretShop::CSecretShop()
{
    // [b28d-solo-CSecretShop 判词·N] ORIG 124 条 vs 我们 126 条（gap -2）：
    // 残差全部为两处 `pair<eBuyRule,CBuyItembyGold*>` 等 sret 调用后的
    // `sub $0x4,%esp`（4.4 outgoing 预留，ORIG 4.1 无）+ lea 目标寄存器
    // %eax↔%edx 分配差（N7/N115 族）；调用点集合与槽位集逐项一致，语义完成。
    // ORIG：map.insert(pair)（非 operator[]）；m_pRand = new CMTRand(time(0))
    // （PvPTypes.h 权威 CMTRand 的 const unsigned long& 种子 ctor，
    //  符号 _ZN7CMTRandC1ERKm @0x080cba2a；ORIG 无 CMTRand(time_t) 重载，
    //  time(0) 经临时 unsigned long 绑定 const 引用）。
    // [L32 组8] P192/P18：ORIG 无 make_pair —— p167 差集实证 ORIG 调
    //   `_ZNSt4pairI8eBuyRulePN10secretshop8IBuyRuleEEC2IS0_PNS1_14CBuyItembyGoldEEEOT_OT0_`
    //   /…16CBuyItembyRecipe…（两实参模板 ctor，右值实参）+ 随后 const 版 converting ctor
    //   `…C2IS0_S4_EEOS_IT_T0_E`×2；我方原用 make_pair 产 EXTRA `_ZSt9make_pair…`×2
    //   ⇒ 改显式 `std::pair<eBuyRule, IBuyRule*>(右值, 右值)` 直构（两实参皆为右值，
    //   选模板 ctor，与 ORIG 同形；C1/C2 名差属 4.1↔4.4 版本族）。
    m_rules.insert(std::pair<eBuyRule, IBuyRule*>((eBuyRule)0, new CBuyItembyGold(m_statistic)));
    m_rules.insert(std::pair<eBuyRule, IBuyRule*>((eBuyRule)1, new CBuyItembyRecipe(m_statistic)));
    m_pRand = new CMTRand(time(0));
}