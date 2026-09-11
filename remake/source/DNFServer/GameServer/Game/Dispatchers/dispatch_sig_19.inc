int Dispatcher_MoveToGate::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8203350：state!=3 或 getCurCharacR()==0 时 LineFunc(0x9a0d)；
    // 无 user==0/manager==0/seller==0 防御检查（ORIG 直接调用）。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9a0d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [b28c-30] P0：ORIG 0x0820338b 段 `movl $0x0,-0xc(%ebp); call
    // GetInstancePrivateStoreMgr; …` —— mgr 无槽往返（值经 %eax 直传），
    // 原码是内联表达式；具名 manager 局部反产 -0xc 槽+store（原 44 insn）。
    private_store::ISeller* seller = 0;
    private_store::GetInstancePrivateStoreMgr()->GetStoreSeller(&seller, user);
    seller->MoveToGate();
    return 0;
}