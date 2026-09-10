// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::IsQuickJoinablePVPRoom（原 TU 第 656 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

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
