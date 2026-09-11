// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082251f4 orig/ours=79/92 ndiff-blocks=11
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: xor    $0x1,%eax | jne    0822524b <+0x57>
int Dispatcher_Change_Charac_Slot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082251f4：state!=2 直接返 0；连读两个 uint（失败任一 → LineFunc(0xdff5)）；
    // 锁链：isLocked4DataLoad||isLockedCreateCharac 再 ||isLockedDeleteCharac，
    // 全未锁时 ChangeCharacSlot(第一个 uint, 第二个 uint)，返 0。
    if (user->get_state() != 2)
    {
        return 0;
    }
    // ORIG 0x082251f4：[12] 段无 `movl $0x0,-0x10/-0x14` 预清（lea 直取址）⇒
    //   first/second 是裸局部；readOk 非具名（ORIG 仅 `xor $0x1,%eax` 直判，
    //   我方旧形多 8 字节帧 + bool 物化岛）。
    unsigned int first;
    unsigned int second;
    if (!(packet.get_int(first) && packet.get_int(second)))
    {
        return LineFunc(0xdff5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x08225281..0x082252d2 两段短路物化：第一段 `isLocked4DataLoad||isLockedCreateCharac`
    //   经 `mov $1/%eax;jmp /mov $0` 汇聚后 `mov %al,-0x9(%ebp)`（O[52]）；第二段
    //   `if (!locked) locked = isLockedDeleteCharac();` 再写同一槽（O[62]）⇒ 源码是**两段赋值同一具名
    //   bool**（Ghidra 亦为 local_d 两次赋值）；单段三条件 `(a||b||c)` 被 4.4 折成三连直跳、丢 6 条
    //   （DispLaneA 2026-09-11 试形，P214①/P194）。
    //   实测（routed 私有 .o）：our 72→75、gap +6→+3（补回 3 条），blocks 1→2；
    //   残余 ①`cmpb $0x0,-0x9; jne`（ORIG O[53:55]）vs 我方 `movzbl; xor $1; test; je`
    //   = N122（4.4 对 bool 局部取反恒 xor 物化，不复现 4.1 内存直测）；
    //   ②第二段 `locked = (c != 0)` 的 `mov $1/jmp/mov $0` 汇聚被 4.4 折掉 ⇒ 判 N 封顶。
    //   TU 四数三次编译恒为 1010/2553/15/181（零劣化）。
    bool locked = (user->isLocked4DataLoad() != 0
                   || user->isLockedCreateCharac() != 0);
    if (!locked)
    {
        locked = (user->isLockedDeleteCharac() != 0);
    }
    if (!locked)
    {
        user->ChangeCharacSlot(first, second);
    }
    return 0;
}