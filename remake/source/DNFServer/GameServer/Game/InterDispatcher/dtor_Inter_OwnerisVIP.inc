Inter_OwnerisVIP::~Inter_OwnerisVIP() {}
// ORIG 0x084e7c90 T dispatch_sig —— 点数拍卖 VIP 购买：计费/组包/预付登记。
// 报告 docs/class_func_reports/Inter_OwnerisVIP/dispatch_sig.md 全文转写。
// 实参口径（Ghidra 同证）：第二槽=CUser*，第三槽=int 位置实际传 msg 指针。
// ORIG PCK_AUCTION_BIDDING_GP（nsl::PACKET_HEADER 派生 pack(1)，
// C1/C2 W 0x084e9a98 sz0x67）。ctor 反汇编：nsl::PACKET_HEADER(this)；
// memset(this, 0, 0x33)；setCategory(0x12)；setPacketID(5)；setSize(0x33)
// —— 与 PACKET_CTOR_BODY(0x12, 5, 0x33) 宏语义逐指令一致。
