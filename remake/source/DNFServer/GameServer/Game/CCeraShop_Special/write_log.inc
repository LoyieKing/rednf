// ---- write_log —— ORIG 0x0827d2df ----
// type==0x53/0x57：重算+发表情；type==8：组 2 包并广播 basic_info。
// b28c-L20: AE — P17/P46：ORIG 0x827d2e7 为 switch 线性链（`mov 0xc(%ebp),%eax`
// 一次载入 + 三 `cmp $imm,%eax/je` 复用 + 两 je 同目标 + 尾 jmp）；if/else-if
// 在 4.4 生 `cmpl $imm,mem` 直比 + 第二条件 jne 落体（旧 N18a/b 判词作废）。
void premium_helper_function::write_log(CUser* user, int type)
