// [b28d-32 AE] ORIG 0x080d9c08 尾岛 `jmp +0x1bf; nop`：P223 fC 条件 goto 前置
// （`if(!checkThrowDice) goto DONE; body; DONE:;`）复现 128=128 逐条全等；
// 旧 body 尾 goto 恒被 4.4 jump-thread（少 jmp+nop 2 条）。
void BlueMarble::movePosition(CUser* user, int pos)
{
    int seat = getUserSlot(user);
    if (seat == -1)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x4f4,
                              "BlueMarble::movePosition slot(%d)", seat);
    }
    // ORIG 0x80d9ab9 xor $0x1,%eax; test; jne 出口（=false 走体）——用已验证的
    // `(int)!boolcall()` 物化形（handleStageCommand 0x84661a3 同构）。
    // [b28d-32] P223 fC 形：条件 goto 置于 body 之前，复现 ORIG 0x080d9bfd
    // 尾 `jmp +0x1bf; nop` 岛（body 尾 goto 恒被 4.4 jump-thread）。
    else
    {
        if ((int)!checkThrowDice(seat))
            goto DONE;
        PacketGuard packet;
        packet.put_header(0, 0x1af);
        packet.put_byte(seat);
        packet.put_byte(pos - getMovePosition(seat));
        packet.finalize(true);
        sendToBlueMarble(packet);
        resetIndexInfo(seat);
        setMovePosition(seat, pos - getMovePosition(seat));
        intoBoardZone(seat);
        sendBoardZoneActionInfo(seat, getDungeonIndex(seat),
                                (unsigned int)getItemIndex(seat));
    }
DONE:
    ;
}