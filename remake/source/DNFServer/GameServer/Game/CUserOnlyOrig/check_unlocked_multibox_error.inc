int CUser::check_unlocked_multibox_error(const CItem* stack, int addInfo)
{
    if (stack == 0) return 0x11;
    if (!stack->is_stackable()) return 0x11;
    // [b28d-22 修法 P0/P198②] ORIG 0x0868c57x：同虚槽 call 两次且结果无栈槽
    // （`call *%edx; cmp $0x1a,%eax; je <ok>` + `cmp $0x1b,%eax`）⇒ 原码是内联双调用
    // 表达式；具名 t/t2 令 4.4 槽化 -0x14/-0x10 并改比较形。
    if (!(stack->GetItemType() == 0x1a || stack->GetItemType() == 0x1b))
        return 0x11;
    if (addInfo != 0xffff)
        return 0x11;
    // ORIG 0x0868c5xx：`movl $0x2,-0x10(%ebp); mov -0x10(%ebp),%eax; mov %eax,0x4(%esp)`
    // ⇒ 具名 int 局部直传（非立即数内联）。
    int mode = 2;
    int err = check_multibox_error(mode);
    // ORIG 0x0868c5xx 尾 `cmpl $0x0,-0xc(%ebp); je <mov$0 岛>; mov -0xc(%ebp),%eax;
    // jmp <end>; <岛>: mov $0x0,%eax`。三目形被 4.4 折成 `mov -0xc,%eax; leave; ret`
    // （==0 时 0==err 恒等）；if-return 形反产 jne 岛 ⇒ 用正向包裹（P222）。
    if (err != 0)
        return err;
    return 0;
