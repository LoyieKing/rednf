int CFairMatch::CheckJoinable(PvP_Room* room, CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 1;
    // ORIG +0x2e：pc 直落 callee-saved %ebx 跨 GetPlayCountPerOneTeam 调用。
    // [b28d-PoolMisc2 P231 反例复验] 删 register ⇒ 值落栈槽 -0x10/-0xc、帧
    // 0x14→0x24、ndiff 4→9（更差）⇒ 本件 register 是**正例**，维持 register。
    register const int pc = room->get_player_count();
    // b28d-196/211：ORIG `cmp %eax,%ebx; setge %al; test %al,%al; je`（ebx=pc
    // 为左操作数）⇒ 试 P225 X3 形（while+单 return 复现 4.1 的 setcc 四段物化）。
    // [b28d-solo-CFairMatchRooms-2] S1 一次性旋钮实测**无效**（反向书写 `X <= pc`
    //   与 `pc >= X` 产物逐条相同：4.4 恒规范化成 `cmp %ebx,%eax; setle`）⇒
    //   属 P196 折痕（同语义不同 compare 方向），判 N，已恢复直证形态。
    while (pc >= static_cast<int>(
               static_cast<unsigned char>(GetPlayCountPerOneTeam())) * 2) {
        return 4;
    }
    register const int total = m_teams[0].size() + m_teams[1].size();
    while (total > static_cast<int>(
               static_cast<unsigned char>(GetPlayCountPerOneTeam())) * 2) {
        return 4;
    }
    // ORIG +0x8d..0x99 跳表（cmp $5/ja/jmp *table）⇒ switch（P17）；
    // 叶块排布序 = case 书写序（P46）：0 → 3 → 1/4/5 与 default。
    switch (m_field04) {
    case 0:
        if (!_CheckSameTeam(room, user))
            return 4;
        // b28d-168a①：ORIG +0xd6 段 `call _CheckObserver; xor $0x1,%eax; test %al,%al;
        // je <mov $0>` ⇒ 源码为否定早返 `if (!f()) return 4; return 0;`（正极性写法
        // 少 xor 物化）。
        if (!_CheckObserver(room, user))
            return 4;
        return 0;
    case 3:
        return 0;
    case 1:
    case 4:
    case 5:
        return 4;
    case 2:
    default:
        // ORIG 跳表 case2/越界共用 +0xfe 块（P46 叶块排布）。
        return 4;
    }
