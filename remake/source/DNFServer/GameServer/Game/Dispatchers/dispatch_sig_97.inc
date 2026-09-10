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
    // ORIG：[52] `mov %al,-0x9(%ebp)` / [62] `xor $0x1,%eax` ⇒ 单一 `locked` 具名
    //   （char 槽 -0x9）+ `!locked` 判；旧形两段赋值多一个 char 槽。
    bool locked = (user->isLocked4DataLoad() != 0
                   || user->isLockedCreateCharac() != 0
                   || user->isLockedDeleteCharac() != 0);
    if (!locked)
    {
        user->ChangeCharacSlot(first, second);
    }
    return 0;
