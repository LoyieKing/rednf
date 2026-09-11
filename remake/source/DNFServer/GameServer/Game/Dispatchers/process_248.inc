// ORIG 0x081cff94（报告 Dispatcher_UpgradeDisjointMachine/process.md）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cff94 orig/ours=56/63 ndiff-blocks=11
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: sub    $0x28,%esp | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
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
    // [AttackA b28d-r7 P342 试形] result 改 unsigned（与 m_4 异型）⇒ store RHS-first
    unsigned int result = 0;               // ORIG 0x081d0012：调用前 -0xc 预清零
    result = expert_job::CDisjointer::OnUpgradeDisjointMachine(user);
    // ORIG 0x081cffd5 区：`cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`
    //   —— bool 归一岛且 ORIG 无 bool/rv/dst2 具名槽；4.4 对 `result != 0` 直跳
    //   `cmpl; je`（缺 setne/test 两条）。[L7DispG2 负结论] 补物化两形实测：
    //   ①`(bool)result` 被 4.4 折掉、不产物化；②`const bool rv = (result != 0);
    //   if (rv)` 产 setne 但 rv 落槽 -0x9 + `cmpb $0x0,-0x9(%ebp)`（我方多 1 条
    //   且 out 指针槽位移 -0x10→-0x14）⇒ ORIG 的 setne/test 寄存器归一属 4.1 vs
    //   4.4 版本差（N96），源形不可择。
    // [G5·P225] 4.4 对 `(bool)result`/`result != 0` 恒折 `cmpl; je` 直跳（缺 ORIG
    //   setne %al; test %al,%al 2 条）；P225 复现形 = `while (v != 0) { body; break; }`。
    while (result != 0)
    {
        // ORIG: mov -0xc(%ebp),%edx; mov -0x10(%ebp),%eax; mov %edx,0x4(%eax)
        out.m_4 = result;
        return 0;                          // ORIG 双 return 0 岛（技巧7）
    }
    return 0;
}