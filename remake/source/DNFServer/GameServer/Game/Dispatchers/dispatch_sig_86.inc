// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x082025ae orig/ours=63/72 ndiff-blocks=15
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: push   %ebx | sub    $0x24,%esp | jne    082025d4 <+0x26>
int Dispatcher_EnterPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082025ae：`state!=3 || CurCharacR==0` 单表达式直判（`||` 短路物化）。
    //   铁证（ORIG 0x082025c3 区）：`cmp $0x3,%eax; jne +0x26`，+0x26 为 `mov $0x1,%eax`
    //   （err=1 → 0x34 `je` 不跳 → 落 LineFunc(0x98d4) 报错块）⇒ ORIG 在 state!=3 时
    //   **直接报错返回**；旧形 `state==3 && CurCharacR==0` 生成 `jne +0x2d`（err=0 继续处理），
    //   两者在 state!=3 时行为相反（真语义差异，2026-09-12 修正）。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        // [G1-Dispatchers] 两处 LineFunc 早退均走 `jmp <epilogue>` 复用 %eax（ORIG 无独立
        // `mov $0x0,%eax`；我方旧形多 2 条）⇒ `return LineFunc(...);` 共享出口。
        return LineFunc(0x98d4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    short sellerIndex = 0;
    private_store::IBuyer* buyer = 0;
    if (!packet.get_short(sellerIndex))
    {
        return LineFunc(0x98d8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    private_store::GetInstancePrivateStoreMgr()->GetStoreBuyer(
        &buyer, user, (unsigned short)sellerIndex);
    buyer->EnterPrivateStore();
    return 0;
}