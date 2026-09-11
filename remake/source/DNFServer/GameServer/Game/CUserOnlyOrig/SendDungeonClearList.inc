void CUser::SendDungeonClearList()
{
    PacketGuard packet;
    packet.put_header(0, 5);
    // ORIG 0x086781e2/0x0867820a：getDungeonClearRefR 调 2 次、结果 %eax 免槽直用
    // （技巧37；具名指针中转变量会给 4.4 多 1 个 spill 槽并平移 packet 槽）
    packet.put_short(static_cast<int>(getDungeonClearRefR()->getDungeonClearSize()));
    getDungeonClearRefR()->enumDungeonClear(this, ::fnEnumDungeonClear, &packet);
    packet.finalize(true);
    Send(packet);
}