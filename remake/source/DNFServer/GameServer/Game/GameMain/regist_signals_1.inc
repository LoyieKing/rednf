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
