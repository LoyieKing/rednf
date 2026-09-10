int Dispatcher_RepairDisjointMachine::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG：out ptr@-0x10（先声明远槽）、result@-0xc；
    // 头 store = value→%edx 先/地址→%eax 后（Pause 技巧28 变体，ORIG 0x081e?）。
    stRepairDisjointMachineParam* out =
        reinterpret_cast<stRepairDisjointMachineParam*>(&param);
    out->m_4 = check_error(user, msg, param);
    if (out->m_4 > 0)
    {
        return 0;
    }
    if (out->m_4 < 0)
    {
        return LineFunc(0x2a54,
                        __PRETTY_FUNCTION__,
                        out->m_4, user->get_acc_id());
    }
    int result = 0;                                    // ORIG：调用前 -0xc 预清零
    result = expert_job::CDisjointer::OnRepairDisjointMachine(user);
    bool nz;                        // 技巧22：cmpl;setne;test;je
    nz = (result != 0);
    if (nz)
    {
        unsigned int rv = result;           // ORIG 0x081cfeda：value 从 -0xc 槽装 %edx
        stRepairDisjointMachineParam* dst2 = out;  // ORIG 0x081cfedd：地址后装 %eax
        dst2->m_4 = rv;                                // ORIG 0x081cfee0：mov %edx,0x4(%eax)
        return 0;                                      // ORIG 双 return 0 岛（技巧7）
    }
    return 0;
