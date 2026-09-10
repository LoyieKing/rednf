int Dispatcher_UpgradeDisjointMachine::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUpgradeDisjointMachineParam& out = static_cast<stUpgradeDisjointMachineParam&>(param);
    // ORIG 0x081cffa9：call 结果经 `mov %eax,%edx` 保护、param 槽重载 %eax
    // 后 `mov %edx,0x4(%eax)`（register asm 双钉，同 Repair 形）。
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2a89,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    int result = 0;                        // ORIG 0x081d0012：调用前 -0xc 预清零
    result = expert_job::CDisjointer::OnUpgradeDisjointMachine(user);
    // ORIG 0x081cffd5 区：`cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`
    //   —— 源码为 `if (result != 0)`，ORIG 无 bool/rv/dst2 具名槽
    //   （我方三个具名量使帧 0x38 vs ORIG 0x28，多 10 条指令）。
    if (result != 0)
    {
        // ORIG: mov -0xc(%ebp),%edx; mov -0x10(%ebp),%eax; mov %edx,0x4(%eax)
        out.m_4 = result;
        return 0;                          // ORIG 双 return 0 岛（技巧7）
    }
    return 0;
