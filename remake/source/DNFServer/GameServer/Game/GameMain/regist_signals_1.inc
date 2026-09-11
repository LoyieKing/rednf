// 15/10/12/13/11/8/4/7/16/23/24/25/31 → signal_handler（13 → SIG_IGN），
// 任一注册失败即返回 0，全部成功返回 1。
// [b28d-28] DIFF | ORIG 0x0829f4c5 | P167 CLEAR（13/13）；N82 槽/寄存器分配差。
// [b28d-solo-GameMain] AE | 131=131 | p167 13/13 CLEAR | 修法：bool ok 槽形（前 3 次
// 直测 + 后 10 次 `mov %al,-0x9` 槽，P198① 预清）+ regist_signal 返回类型 int→bool
// （P248 档①：调用点 `xor $1,%eax` 复现；返回类型不入 mangled）。
bool regist_signals()
{
    // ORIG 0x0829f4cb `movb $0x0,-0x9(%ebp)`：bool 局部预清（P198①）。
    // 前三次注册 ORIG 直接 `call; xor $1; test; je`（无槽往返）；自第 4 次
    // （sig 13）起为 `mov %al,-0x9; movzbl -0x9; xor $1; test; je`（具名 ok 槽）。
    bool ok = false;
    if (!regist_signal(15, signal_handler)) { return 0; }
    if (!regist_signal(10, signal_handler)) { return 0; }
    if (!regist_signal(12, signal_handler)) { return 0; }
    ok = regist_signal(13, (void (*)(int))1);  // SIG_IGN
    if (!ok) { return 0; }
    ok = regist_signal(11, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(8,  signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(4,  signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(7,  signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(16, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(23, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(24, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(25, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(31, signal_handler);
    if (!ok) { return 0; }
    return 1;
}