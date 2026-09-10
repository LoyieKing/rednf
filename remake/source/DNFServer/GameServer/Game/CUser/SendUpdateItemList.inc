bool CUser::SendUpdateItemList(eSendTarget target, ENUM_ITEMSPACE space, int slot)   // [b28d-Main 代落·S338 P292] int→bool
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte((int)space);
    ((InterfacePacketBuf*)&packet)->put_short(1);
    // ORIG 0x0867c65a：空间→INVEN_TYPE 映射（1→2、7→3、3→0、其余→1）；
    // 每分支独立重调 getCurCharacInvenR（访问器结果禁缓存，战术 P183/P48）；
    // MakeItemPacket != 1 → 返回 0（不发包），否则 finalize + SendPacket 返回 1。
    // b28d-PoolCUser 重写（2026-09-09，P17/P45）：ORIG 判定为 `cmpl $K,0x10(%ebp)`
    //   形参槽直比 + jne 下一检查的**线性比较链**（1→7→3→else），非 switch 决策树
    //   （4.4 switch 恒产值载入+二分序 3/7/1）⇒ 改 if-else-if 链、书写序按 ORIG。
    //   尾段 ORIG `call SendPacket; mov $0x1,%ebx` —— SendPacket 返回值**被丢弃**
    //   （非 mov %eax,%ebx）⇒ 调用作语句 + `return true`。
    if (space == 1)
    {
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)2, slot, packet) != 1)
            return 0;
    }
    else if (space == 7)
    {
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)3, slot, packet) != 1)
            return 0;
    }
    else if (space == 3)
    {
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)0, slot, packet) != 1)
            return 0;
    }
    else
    {
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet) != 1)
            return 0;
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    SendPacket(target, packet);
    return true;
