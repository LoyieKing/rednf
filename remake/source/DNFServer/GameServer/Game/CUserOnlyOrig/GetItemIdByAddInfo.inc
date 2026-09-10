int GetItemIdByAddInfo(Inven_Item* items, int space, int addInfo);   // W 0x084e91a4（owner：Item 域批次）

// [b28d-22 N82] ORIG 0x08690226：p167=CLEAR，gap=6；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08690226：N82 封顶——帧差 0x10（sub $0x34 vs
// $0x44）+ 槽序（p167 CLEAR）。
