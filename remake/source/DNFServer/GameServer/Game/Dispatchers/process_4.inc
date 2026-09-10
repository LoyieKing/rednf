int Dispatcher_StartGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x813f8ce（报告 advancealtar__Dispatcher_StartGame/process.md 全文已读）：
    // msg@-0x10、param@-0xc 两局部槽；check_error 结果直存 p+4（call 后回读槽，
    // 不 hoist %ebx）；(p+4)!=0 → 真体 fall-through return 0（je → call 段）。
    // [b28d-solo-S162] P211 档③：字段与 RHS(int) 异型 ⇒ 4.4 翻 RHS-first+%edx 中转
    // （ORIG 0x0813f8d8 `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,0x4(%eax)`）。
    struct _P4 { int m0; unsigned int m4; };
    MSG_BASE& m = msg;                // ORIG -0x10 死存储（调用直传形参 msg）
    _P4* const p = reinterpret_cast<_P4*>(&param);   // ORIG -0xc（指针形：
                                                    // store 时 base 从 -0xc 载 %eax、
                                                    // 值留 %edx → ORIG `mov %edx,0x4(%eax)`）
    (void)m;
    // ORIG 0x813f8ce：call → mov %eax,%edx（value 先落 %edx）→ `mov -0xc(%ebp),%eax`
    // （地址从 p 的局部槽 reload，非形参槽 0x14）→ `mov %edx,0x4(%eax)`。
    // [b28d-solo-S162] 原具名 ec + dst 两局部各占一槽（帧 +8、槽整体漂移）。
    // ORIG 值是 call 结果右值直传（P0 无槽往返）⇒ 单语句内联实参形。
    p->m4 = check_error(user, msg);
    if (p->m4 != 0)
    {
        return 0;
    }
    {
        user->m_advanceAltar.checkAndCreateDefaultRidable();  // ORIG this=user+0x8df60
    }
    return 0;
