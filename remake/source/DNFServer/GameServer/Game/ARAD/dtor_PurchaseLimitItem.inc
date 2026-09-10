PurchaseLimitItem::~PurchaseLimitItem() {}

// b28d-solo-ARAD verdict=IDENTICAL/AE：Info 提到循环外构造一次（ORIG 0x81a21dc 在
// r=0 之前）+ Item_Index 写入 Info+0x04（make_pair first = `lea -0x2c; add $0x4`，
// 即 info.reserved 槽）+ 具名 insert 结果 res ⇒ 槽位全对齐（script -0x88 / Info -0x2c /
// insert sret -0x34 / .second -0x30）。残余仅 4.4 循环对齐 nop（§6.1 豁免）。
