// getStreamName（ORIG 0x8ace8a0）：tstring 返回（按 DNFLexWrapper.cpp
// getLastTokenDataAsString 的 by-value std::string 惯例）：lexType_ != 0 时
// 返回 "NONE"；NORMAL 时委托 DNFLex::getStreamName。
// b28d-21 + E06 复测: NEAR 封顶 — 29=29 助记符序列全同，唯一残差为 lexType_
// 载入寄存器（ORIG `mov 0x14(%eax),%edx; test %edx,%edx` vs 4.4.x `%ecx`）。
// E06 实测矩阵：①本 TU 路由 c6446r -O2 → %ecx；②c6root 4.4.7 全旗标 -O2（无
// 路由改写）→ %ecx；③源码形穷举 `lexType_ != 0` / Yoda `NORMAL != lexType_` /
// 三目条件式（此处反劣为 DIFF，已回退）→ 均 %ecx。⇒ 4.4 分配器恒选 %ecx，
// 属 N7 跨版本寄存器角色差（P321-T5：寄存器角色不改类型/源码），无源码旋钮。
// ORIG 0x8ace8a0。
std::string DNFLexWrapper::getStreamName()
{
    // [P6S3 round6 · §6.5 串审计命中复核 → 判**工具归因假阳性**，非语义缺口]
    // strall.py 报本函数 `O-only[1] NONE`；但 `strings -a /tmp/tu_DNFLexWrapperStream.o`
    // 直证我方 .o 首串即 `NONE`，且本函数体内确有
    // `movl $0x14,0x4(%esp) + R_386_32 .rodata.str1.1` + `call _ZNSsC1EPKcRKSaIcE`
    // （= `std::string("NONE")`，与 ORIG 0x8ace9xx 同形）⇒ 串未缺，属逐函数归因偏差。
    // 残余真差仅 1 处寄存器：ORIG `mov 0x14(%eax),%edx; test %edx,%edx` vs 我方
    // `%ecx`（N22 族「后求值者作 dst / 首槽寄存器择取」，源码不可翻）。
    if (lexType_ != LEX_TYPE_NORMAL)
        return std::string("NONE");
    return dnfLex_.getStreamName();
}