void CUser::SendLogoutToPCRoomServer()
{
    char login[0x210];
    // b28d-lane5：同 SendLoginToPCRoomServer——ORIG 0x086817xx 逐字段重发
    // `lea -0x226(%ebp)`（P183），删 lp 指针缓存。
    // 2026-09-10（G3CUser-B 负结论）：本 .inc 只含 `char login[0x210]` 声明，函数的
    //   全部指令在**聚合器 CUser.cpp:3336+** 内联段。实测 diff（ORIG 0x0868170c vs
    //   ours 78 insn）：ORIG `sub $0x1238` 且**不 push %ebx**、每站点重发
    //   `lea -0x216(%ebp),%eax; add $0x2,%eax`；我方 `push %ebx; sub $0x1234` 并把
    //   `lea -0x21c(%ebp)` 缓存进 %ebx（P183 反例，每站点省 1 条 but 帧/寄存器形反）。
    //   ⇒ 修复点在 CUser.cpp 的宏段（`#define LP_LOGOUT` 后的 LP_LOGOUT->… 语句需
    //   改为每次 `((nexon_packet::stLogin*)(login))->…` 直写、不引入 `pLogin` 局部），
    //   非本 .inc 写域；已报 Main。
