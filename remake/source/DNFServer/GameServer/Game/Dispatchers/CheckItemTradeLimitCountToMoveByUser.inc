// ORIG T 0x0812032e：
// [b28c-lane24] ORIG 尾 `leave; ret` 无返回值设置 + Ghidra C `void`（P159：
// 以 ORIG 函数尾判定真实返回类型）⇒ void；原 bool + `return ok` 使 4.4 在
// 两出口各读 ok 引用（+3 条）。全部 4 个调用点均为语句调用（不消费返回值）。
void CheckItemTradeLimitCountToMoveByUser(CUser& user, int space, int slot,
                                          bool& ok)
{
    ok = true;
    // b28d-D0: ORIG 0x08120394/0x08120397 两处落点不同（第一守卫 nop+jmp 岛、
    //   第二守卫 nop 直落）⇒ P223/BM8 双守卫版：两条独立 goto + 尾标签串（标签序=守卫序）。
    if (user.getCurCharacR() == 0)
        goto L_early;   // [b28c-lane24] P223 尾空标签岛：ORIG 0x0812034c
                        // `jne +0x66` → `nop; jmp` 中继岛。
    {
        Inven_Item item =
            ((CInventory*)user.getCurCharacInvenR())->GetInvenSlot(space, slot);
        // [b28c-lane24] 清除存量 register asm("eax") 绑定（P0 全域禁绑，N96）。
        // ORIG 0x08120379：`mov -0x43(%ebp),%eax; test %eax,%eax`（typed 字段直读
        // 单级 disp，P0 下为表达式直读形）。
        if (item.m_addInfo == 0)
            goto L_late;    // ORIG 0x0812037e `je 0x08120397`（第二独立 nop 岛）
        CheckTradeLimitItemCountToMove(item, ok);
    }
L_early:;
L_late:;
}