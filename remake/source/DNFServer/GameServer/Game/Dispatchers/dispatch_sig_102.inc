// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08202bb6 orig/ours=83/84 ndiff-blocks=16
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x20,%esp | jne    08202be4 <+0x2e> | lea    -0x18(%ebp),%eax
int Dispatcher_RemovePrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08202bb0 直证（objdump + Ghidra C 双向核对）：
    //   `cmp $2,%eax; jle L_true` / `test %eax,%eax; jne L_false`
    //   / `L_true: mov $1,%eax; jmp T; L_false: mov $0,%eax; T: test %al,%al; je <seller 段>`
    //   ⇒ bVar1 = (state <= 2) || (getCurCharacR() == 0)（P258 德摩根 or 形），
    //   发包分支在 fall-through。旧形 `state > 2 && r != 0` 与之**相反**（真语义 bug）。
    //   ORIG 亦无 user/manager/seller 判空（GetStoreSeller 假定有效）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        PacketGuard response;
        response.put_header(1, 0x59);
        response.put_byte(0);
        response.put_byte(0x15);
        response.finalize(true);
        user->Send(response);
        return 0;
    }
    private_store::ISeller* seller = 0;
    // ORIG 0x08202bb0 段：`movl $0x0,-0xc(%ebp)`（seller 预清）后 `lea -0xc(%ebp),%edx`
    //   —— 无 manager 具名指针槽（我方具名使帧 0x20→0x30）。
    private_store::GetInstancePrivateStoreMgr()->GetStoreSeller(&seller, user);
    seller->RemovePrivateStore(false);
    return 0;
}