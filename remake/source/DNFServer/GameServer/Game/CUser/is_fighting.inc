// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867ed68 | _ZN5CUser11is_fightingEv
bool CUser::is_fighting()   // CUser.h:2556 已 bool 化（Main 落，ORIG 0x867ed68 尾 mov$0/$1 直返 + caller test %al 无 setne）
{
    register int bit;   // 无初始化 POD 免槽（P177/P231）
    // ORIG 0x867ed68：无 state/bit 槽（P0 全寄存器直通）——ja 直落后置 zero 块、
    // 两 mask 岛（mov$1/mov$0）后置共享；(bit&0xdf)==0 与 fall 同值返 0 = 照抄（P171）。
    // [G5] 去 goto 标签 + 改正向 `<=10` 包裹形：4.4 对 goto 目标标签恒插 1B nop 填充
    //   （CheckMoney 探针 nl11_probe1/2 实证；if/else return 形无 nop），且 goto 到
    //   zero 会多出一道蹦床块 ⇒ 原 3×nop + 1×jmp（+4 条）全消；ja/je 仍共享尾 return
    //   false（jcc 方向与块序与 ORIG 一致）。
    // 残差（N 封顶）：ORIG `mov 0x8cfc4(%eax),%eax` 单读跨 ja 复用为 shl 计数
    //   （4.1 register 缓存免中转），4.4 恒二读；`register unsigned int st=m_chState`
    //   两形均反劣（st 落 %ebx：+mov %eax,%ebx/+mov %ebx,%eax）⇒ N7/N18a；掩码段
    //   ebx→eax→edx 双中转同源（N18b 族）。
    if ((unsigned)m_chState <= 10)
    {
        bit = 1 << (int)m_chState;
        if ((bit & 0x720) != 0)
            return true;
        if ((bit & 0xdf) == 0)
            return false;
    }
    return false;
}