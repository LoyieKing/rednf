bool PvP_Room::IsQuickJoinablePVPRoom(CUser* user)
{
    // P24：ORIG 内联实参（0x085d6c1c），getCurCharacNo 结果直传不落栈槽。
    // ORIG 0x085dc18 逐段：layer1 `call check_joinable; test%eax; jne L0(mov$0)`；
    // layer2 `call find…; xor$1; test%al; je L0`；真值路 mov$1，尾 `test%al;
    // je; mov$1/jmp; mov$0` 双归一岛（&&-结果岛 + return 归一岛）。P0：结果全程
    // %eax 无槽 ⇒ 源形 = 短路 && 条件 + if-return 1/0（无具名局部，P56 形）。
    if (check_joinable(user) == 0 &&
        !user->find_pvp_masterid_walkingout_me(m_seats[7]->getCurCharacNo()))
        return 1;
    return 0;
}