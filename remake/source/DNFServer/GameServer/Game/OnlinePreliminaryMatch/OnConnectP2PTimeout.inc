void COnlinePreliminary::OnConnectP2PTimeout(PvP_Room* room)
{
    // b28：ORIG 单 leave 无中继（jne→leave；body 尾 call 直落）。4.4 对
    // guard-return 形在 body 尾产 `jmp leave; nop` 中继（P63）；改正极性
    // if 共享统一出口消岛（DIFF15→NEAR）。
    // [b28 判词 N7] 残差=call *%edx↔%eax 换名 + 实参链寄存器对调，封顶。
    if (m_field04 == 4)
    {
        _SendMatchStatePacket(4, 2);
        OnEndFight(room);                                /* vtable+0x40 虚调实证 */
    }
