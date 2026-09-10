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
    // ORIG 0x081d0012：result 槽 -0xc 调用前预清零；!=0 走 bool 物化
    // `cmpl;setne%al;test;je` 尾岛（技巧22）；store `mov -0xc,%edx;
    // mov -0x10,%eax; mov %edx,0x4(%eax)`（value→%edx、地址→%eax，
    // register asm 钉双寄存器，同 RepairDisjointMachine 形）。
    bool nz;
    nz = (result != 0);
    if (nz)
    {
        unsigned int rv = result;
        stUpgradeDisjointMachineParam* dst2 = &out;
        dst2->m_4 = rv;
        return 0;                          // ORIG 双 return 0 岛（技巧7）
    }
    return 0;
