// ORIG 0x081cfe3c（报告 Dispatcher_RepairDisjointMachine/process.md）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cfe3c orig/ours=56/59 ndiff-blocks=10
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: sub    $0x28,%esp | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
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
    // [AttackA b28d-r7 P342 试形] result 改 unsigned（与 m_4 异型）⇒ store RHS-first
    unsigned int result = 0;                           // ORIG：调用前 -0xc 预清零
    result = expert_job::CDisjointer::OnRepairDisjointMachine(user);
    // ORIG 0x081cfecf: cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je
    //   —— 源码就是 `if (result != 0)`，ORIG 无 bool 具名槽；
    //   我方曾用 bool nz / unsigned rv / dst2 三个具名量，多占 3 槽（帧 0x38 vs 0x28）+10 条指令。
    // [G5·P225] 4.4 对 `if (result != 0)` 恒折 `cmpl; je` 直跳（缺 ORIG setne/test 2 条）；
    //   P225 实证复现形 = `while (v != 0) { body; break; }`（4.1 槽比零 bool 物化）。
    while (result != 0)
    {
        // ORIG 0x081cfeda: mov -0xc(%ebp),%edx; mov -0x10(%ebp),%eax; mov %edx,0x4(%eax)
        out->m_4 = result;
        return 0;                                      // ORIG 双 return 0 岛（技巧7）
    }
    return 0;
}