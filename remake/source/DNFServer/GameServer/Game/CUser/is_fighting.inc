bool CUser::is_fighting()   // CUser.h:2556 已 bool 化（Main 落，ORIG 0x867ed68 尾 mov$0/$1 直返 + caller test %al 无 setne）
{
    register int bit;   // 跨标签 goto 用无初始化 POD（P177）
    // ORIG 0x867ed68：无 state/bit 槽（P0 全寄存器直通）——ja 直跳后置 zero 块、
    // 两 mask 岛（mov$1/mov$0）后置共享（goto 形复现 jcc 方向与块序）；
    // (bit&0xdf)!=0 与 fall 同值返 0 = 死块照抄（P171）。残差：ORIG shl 源 eax
    // 单读直通 + ebx→eax→edx 双中转（N18a/寄存器直通跨版本）。
    // b28d-solo-CUser 复测：`register unsigned int st=m_chState;` 单读缓存形反劣
    // （st 落 %ebx：+mov %eax,%ebx / +mov %ebx,%eax，ours 30→32）⇒ 维持 N7/N18a。
    // b28d-solo-CUser 再测（P231 裸 register 单读缓存）：`register unsigned int st`
    //   实测仍反劣（ndiff 9→11：多 `mov %eax,%ebx` 中转、`and/test` 落 %eax 无 %edx 段）
    //   ⇒ 维持 N7/N18a 封顶，回退。
    if ((unsigned)m_chState > 10)
        goto zero;
    bit = 1 << (int)m_chState;
    if ((bit & 0x720) != 0)
        goto one;
    if ((bit & 0xdf) == 0)
        goto zero;
    return false;
one:
    return true;
zero:
    return false;
