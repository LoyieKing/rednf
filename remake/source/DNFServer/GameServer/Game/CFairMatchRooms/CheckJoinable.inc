// CheckJoinable ORIG T 0x08562a6e 报告 CheckJoinable.md
// b28d-solo-CFairMatchRooms 判词：N196——两处 `pc>=X`/`total>X` 被 4.4 折成反向
// 比较（cmp %ebx,%eax; setle/setl vs ORIG cmp %eax,%ebx; setge/setg，同语义不同
// compare 方向）；两向源码写法（`pc>=X` / `X<=pc`）实测同形 ⇒ 源形不可择。
// [b28f-SweepCFairMatch 复核] 探针 /tmp/fm_p1.cpp f1..f6（while/for/if/负极性/
// X*2 显式 cast/register 局部）**六形逐条同为 `cmp %ebx,%eax; setle`** ⇒ 4.4
// 恒把 call 结果作左操作数，跨版本 compare 折叠（P196 折痕）封顶。
// 语义轴核对：ORIG 跳表 @0x08ca235c 六项实测 = case0→0x8562b09（_CheckSameTeam/
// _CheckObserver 链）、case1/4/5→0x8562b65（return 4）、case2 与越界→0x8562b6c
// （return 4）、case3→0x8562b5e（return 0）——与本实现 case 0/3/1/4/5/2+default
// 逐项一致；`pc >= X*2`（setge）/`total > X*2`（setg）差一亦一致。
// 余 1 条跳表重定位伪差。call 差集 8=8 CLEAR。fresh ndiff=4（85=85）。
// [GW20Run 2026-09-11 复核] fresh 85/85；ORIG 0x08562a6e 段 `cmp %eax,%ebx; setge`
// /`setg` 与报告逐条一致、掩码/立即数无差；p167 8=8 CLEAR、enc_kr_scan 0 ⇒ 维持 N196。
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
}