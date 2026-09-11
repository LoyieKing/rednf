// 0x0865db6c
// ORIG：按物品空间号生成并发送对应列表包：
//   0/1 → CInventory::MakeItemList(1/2)、2 → CCargo::MakeItemList、
//   7 → 生物列表 + MakeItemList(3)、0xc → CAccountCargo::SendItemList（不存在时返回 1）。
// b28d-lane5 N封顶: P0 清绑连带——同 TU callee（IsExistAccountCargo 等）清绑后帧变化，
// 4.4 按 P210 精确继承 outgoing ⇒ 本函数帧 0x20→0x30、PacketGuard 槽 -0x14→-0x1c；
// 另 v^=1 段清绑后落栈槽物化（P168a④）。| ORIG 0x0865db6c | _ZN5CUser14send_itemspaceEi
bool CUser::send_itemspace(int space)   // 返回形随 CUser.h:2843 bool 化（P22 调用点直测形）
{
    // b28d-G3C 2026-09-10（P183 去具名槽）：ORIG 每个 case 的形态是
    //   `call MakeItemList; xor $0x1,%eax; test %al,%al; je <继续>`
    //   （0x0865dbb9/+0x4d、0x0865dbf0/+0x84、0x0865dc1f/+0xb3、0x0865dc4f/+0xe3、
    //    0x0865dc82/+0x116）——**无任何暂存槽**。我方原形用 5 个具名
    //   `unsigned char vN` 各占 1 字节槽（-0xd/-0xc/-0xb/-0xa/-0x9），每处多
    //   `mov %al,-0xN; xorb $1,-0xN; cmpb $0,-0xN` 三条，并把 PacketGuard 从
    //   -0x14 顶到 -0x1c（帧 0x20→0x30）。改为内联表达式：内层 `^1` 保 int 域
    //   32 位 xor，外层 (unsigned char) 截断保 `test %al,%al` 八位判零。
    PacketGuard packet;
    switch (space)
    {
    case 0:
    {
        if ((unsigned char)(getCurCharacInvenR()->MakeItemList((INVEN_TYPE)1,
                                                              &packet) ^ 1))
            return 0;
        break;
    }
    case 1:
    {
        if ((unsigned char)(getCurCharacInvenR()->MakeItemList((INVEN_TYPE)2,
                                                              &packet) ^ 1))
            return 0;
        break;
    }
    case 2:
    {
        if (!getCurCharacCargoR()->MakeItemList(&packet))
            return 0;
        break;
    }
    case 7:
    {
        if ((unsigned char)(getCurCharacInvenR()->GetCreatureMgrR()
                                ->SendCreatureItemList() ^ 1))
            return 0;
        if ((unsigned char)(getCurCharacInvenR()->MakeItemList((INVEN_TYPE)3,
                                                              &packet) ^ 1))
            return 0;
        break;
    }
    case 0xc:
        // ORIG 0x865dc90 段：`call IsExistAccountCargo; test %al,%al; je <ret1岛>`——
        // 真支 fall-through 直取成员对象地址（`mov 0x8(%ebp),%eax; add $0x6effc,%eax`）
        // 调 m_accountCargo.SendItemList()，⇒ 原码是成员对象直调而非 GetAccountCargo()
        // （此处 4.1 头文件内联的 getter 已在我们 TU 外联，多出一次 call）。
        if (IsExistAccountCargo())
            return m_accountCargo.SendItemList();
        return 1;
    default:
        return 0;
    }
    Send(packet);
    return 1;
}