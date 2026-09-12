// ORIG 0x081ce97c（报告 Dispatcher_SaveCharacterOption/process.md）：
// param+4=check_error；>0 → 0；<0 LineFunc(0x283d)；==0 setSaveCharacOption(true)
// + SetCurCharacOption(msg+0x11)。
int Dispatcher_SaveCharacterOption::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ce97c 序言：`mov 0x14(%ebp),%eax; mov %eax,-0x10(%ebp)` ⇒ param 指针
    // **函数头即落槽**（P0 槽往返），其后所有条件读回的都是 `param+4`（无独立 err 槽：
    // call 结果经 `mov %eax,%edx` 直存 disp）。裸偏移 +4 改具名 pack(1) 视图（§4.3）。
    stSaveCharacOption& p = static_cast<stSaveCharacOption&>(param);
    p.m_4 = check_error(user, msg);
    if (p.m_4 > 0)
    {
        return 0;
    }
    // ORIG 0x081ce9xx：err<0 块在前（`test; jns` 越过），两臂汇入**共享尾 return**
    // （4.4 对独立早返恒就地物化 `mov $0,%eax` ⇒ 用 if/else 而非二次 return）。
    // [SemB06 s2_06] ORIG 0x081ce9f1 `jmp <+0xaa=leave>`（**跳过**尾 `mov $0x0,%eax`）
    // ⇒ 该分支源码是 `return LineFunc(...)`（返回 LineFunc 的 int 值），非
    // `LineFunc(...);` + 共享 `return 0;`（后者多一跳落到 mov $0x0 ⇒ 返回值被改写）。
    if (p.m_4 < 0)
    {
        return LineFunc(0x283d,
                        __PRETTY_FUNCTION__,
                        p.m_4, (unsigned int)user->get_acc_id());
    }
    else
    {
        // ORIG 0x081ce9xx：msg 指针槽 -0xc 在 setSaveCharacOption **之前**物化
        // ⇒ req 引用声明前置（4.4 在绑定处落槽）。
        MSG_SAVE_CHARACTER_OPTION& req =
            reinterpret_cast<MSG_SAVE_CHARACTER_OPTION&>(msg);
        user->setSaveCharacOption(true);
        user->SetCurCharacOption(req.m_option);        // +0x11 CharacterOption
    }
    return 0;
}