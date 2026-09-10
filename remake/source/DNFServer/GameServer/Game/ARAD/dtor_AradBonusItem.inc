AradBonusItem::~AradBonusItem() {}

// b28d-solo-ARAD verdict=IDENTICAL/AE：ORIG 0x81a23f8 在 r=0 之前**循环外**构造
// 一次 Data（-0x38），循环内复用；make_pair 实例 `_ZSt9make_pairIRjRN13AradBonusItem4DataEE`
// 的 first = &Data+0x04（即 unsigned int itemId 字段），无独立 itemIdx 局部
//（ORIG 0x81a2426 `call GetInt; mov %eax,-0x34` 直存字段）。
