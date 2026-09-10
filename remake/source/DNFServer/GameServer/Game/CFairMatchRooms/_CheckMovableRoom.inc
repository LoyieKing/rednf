int CFairMatch::_CheckMovableRoom(IMatch* other, PvP_Room* /*room*/,
                                  PvP_Room** out)
{
    // ORIG 0x8563ede 实证 other==0 直接 return 0（无 trace；0xc5d trace 仅在
    // 循环后 member==0 分支，见下）
    if (other == 0) {
        return 0;
    }
    CUser* member = 0;
    for (int i = 0; i < 4; ++i) {
        if ((member = other->GetUser(static_cast<unsigned>(i))) != 0) break;
    }
    // b28d-solo-CFairMatchRooms：P222 正向早返 —— ORIG `cmpl $0,-0x10(%ebp);
    // jne <then>`（member==0 的 trace 0xc5d 块直落、成功路径在尾部）。
    if (member == 0) {
        // P221 全表达式临时链（翻互斥块 trace 槽方向，ORIG c5d@-0x30）。
        cMyTrace(__PRETTY_FUNCTION__, 0xc5d, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0xc5d);
        return 0;
    }
    // ORIG `mov 0x14(%ebp),%edx; mov %eax,(%edx); mov 0x14(%ebp),%eax;
    // mov (%eax),%eax; jne` ⇒ 两句式（store 后重读）；目标与 RHS 同型 ⇒
    // P211 档① LHS-first 直存。
    *out = member->GetPVPRoom();
    // ORIG `test %eax,%eax; jne <mov $1 尾块>` ⇒ 失败块直落、return 1 在尾
    // （P191/P222 正极性尾块）。
    if (*out == 0) {
        // P221 全表达式临时链（ORIG c64@-0x20）。
        cMyTrace(__PRETTY_FUNCTION__, 0xc64, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0xc64);
        return 0;
    }
    return 1;
